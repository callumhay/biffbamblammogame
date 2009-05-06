#include "ESPParticleBatchMesh.h"
#include "ESPParticle.h"

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/CgFxEffect.h"

ESPParticleBatchMesh::ESPParticleBatchMesh() {
	this->vertexBufferObjs.resize(4);
	glGenBuffers(4, &this->vertexBufferObjs[0]);

	assert(this->vertexBufferObjs[0] != 0);
	assert(this->vertexBufferObjs[1] != 0);
	assert(this->vertexBufferObjs[2] != 0);
	assert(this->vertexBufferObjs[3] != 0);

	debug_opengl_state();
}

ESPParticleBatchMesh::~ESPParticleBatchMesh() {
	// Free VBO objects that exist
	glDeleteBuffers(this->vertexBufferObjs.size(), &this->vertexBufferObjs[0]);
	this->vertexBufferObjs.clear();
}

void ESPParticleBatchMesh::DrawParticleBatch(const Camera& camera, const ESP::ESPAlignment alignment, std::list<ESPParticle*> aliveParticles) {
	// Make sure there's data...
	if (aliveParticles.size() == 0) {
		return;
	}
	
	// Initialize data arrays for holding particle geometry
	std::vector<GLfloat>  vertexData;
	std::vector<GLfloat>  normalData;
	std::vector<GLfloat>  texCoordData;
	std::vector<GLfloat>  colourData;

	const unsigned int NUM_COMPONENTS_PER_PARTICLE = 4;
	unsigned int numVerts = 3 * aliveParticles.size() * NUM_COMPONENTS_PER_PARTICLE;
	vertexData.reserve(numVerts);
	normalData.reserve(numVerts);
	
	unsigned int numTexCoords = 2 * aliveParticles.size() * NUM_COMPONENTS_PER_PARTICLE;
	texCoordData.reserve(numTexCoords);

	unsigned int numColours = 4 * aliveParticles.size() * NUM_COMPONENTS_PER_PARTICLE;
	colourData.reserve(numColours);

	// Store the transformed vertex geometry and other attributes for all the particles
	for (std::list<ESPParticle*>::iterator particleIter = aliveParticles.begin(); particleIter != aliveParticles.end(); particleIter++) {
		
		ESPParticle* currParticle = *particleIter;
		assert(currParticle != NULL);

		// Grab the transform info for the particle
		Point3D particlePos = currParticle->GetPosition();
		float particleRotationInDegs = currParticle->GetRotation();
		Vector2D particleScale = currParticle->GetScale();

		Matrix4x4 particleAlignXF = currParticle->GetPersonalAlignmentTransform(camera, alignment);
		Matrix4x4 particleTranslationXF = Matrix4x4::translationMatrix(Vector3D(particlePos[0], particlePos[1], particlePos[2]));
		Matrix4x4 particleRotationXF = Matrix4x4::rotationMatrix('z', -particleRotationInDegs, true);
		Matrix4x4 particleScaleXF = Matrix4x4::scaleMatrix(Vector3D(particleScale[0], particleScale[1], 1.0f));
		
		// Compose the entire transform on the particle into one matrix
		Matrix4x4 particleFullXF = particleTranslationXF * particleAlignXF * particleRotationXF * particleScaleXF;
		//Matrix4x4 particleFullXF = particleScaleXF * particleRotationXF * particleAlignXF * particleTranslationXF;

		// Grab colour info for the particle
		Colour4D particleColour = currParticle->GetColour();

		// Typical points and vectors for an untransformed particle
		std::vector<Point3D> particleVertices;
		particleVertices.reserve(NUM_COMPONENTS_PER_PARTICLE);
		particleVertices.push_back(Point3D(-0.5f, -0.5f, 0.0f));
		particleVertices.push_back(Point3D( 0.5f, -0.5f, 0.0f));
		particleVertices.push_back(Point3D( 0.5f,  0.5f, 0.0f));
		particleVertices.push_back(Point3D(-0.5f,  0.5f, 0.0f));
		
		// Transform the vertices
		for (unsigned int i = 0; i < particleVertices.size(); i++) {
			particleVertices[i] = particleFullXF * particleVertices[i];
		}

		std::vector<GLfloat> particleVertComponents(3 * particleVertices.size());
		for (unsigned int i = 0; i < particleVertices.size(); i++) {
			for (unsigned int j = 0; j < 3; j++) {
				particleVertComponents[(3 * i) + j] = particleVertices[i][j];
			}
		}

		// TODO: optimize since this never changes...
		std::vector<Vector3D> particleNormals(NUM_COMPONENTS_PER_PARTICLE, ESPParticle::PARTICLE_NORMAL_VEC);

		// Transform the normals
		for (unsigned int i = 0; i < particleVertices.size(); i++) {
			particleNormals[i] = particleFullXF * particleNormals[i];
		}

		std::vector<GLfloat> particleNormalComponents(3 * particleNormals.size());
		for (unsigned int i = 0; i < particleNormals.size(); i++) {
			for (unsigned int j = 0; j < 3; j++) {
				particleNormalComponents[(3 * i) + j] = particleNormals[i][j];
			}
		}

		// TODO: optimize since this never changes...
		std::vector<Point2D> particleTexCoords;
		particleTexCoords.reserve(NUM_COMPONENTS_PER_PARTICLE);
		particleTexCoords.push_back(Point2D(0,0));
		particleTexCoords.push_back(Point2D(1,0));
		particleTexCoords.push_back(Point2D(1,1));
		particleTexCoords.push_back(Point2D(0,1));
		
		std::vector<GLfloat> particleTexComponents(2 * particleTexCoords.size());
		for (unsigned int i = 0; i < particleTexCoords.size(); i++) {
			for (unsigned int j = 0; j < 2; j++) {
				particleTexComponents[(2 * i) + j] = particleTexCoords[i][j];
			}
		}

		// TODO: optimize this?
		std::vector<GLfloat> particleColourComponents(4 * NUM_COMPONENTS_PER_PARTICLE);
		for (unsigned int i = 0; i < NUM_COMPONENTS_PER_PARTICLE; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				particleColourComponents[(4 * i) + j]	= particleColour.rgba[j];
			}
		}

		// Add the gathered particle geometry data to the various buffers
		vertexData.insert(vertexData.end(), particleVertComponents.begin(), particleVertComponents.end());
		normalData.insert(normalData.end(), particleNormalComponents.begin(), particleNormalComponents.end());
		texCoordData.insert(texCoordData.end(), particleTexComponents.begin(), particleTexComponents.end());
		colourData.insert(colourData.end(), particleColourComponents.begin(), particleColourComponents.end());
	}

	// Bind vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjs[VertexData]);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size()*sizeof(GLfloat), &vertexData[0], GL_STREAM_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
	
	// Bind normals
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjs[NormalData]);
	glBufferData(GL_ARRAY_BUFFER, normalData.size()*sizeof(GLfloat), &normalData[0], GL_STREAM_DRAW);
	glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));
	
	// Bind texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjs[TexCoordData]);
	glBufferData(GL_ARRAY_BUFFER, texCoordData.size()*sizeof(GLfloat), &texCoordData[0], GL_STREAM_DRAW);
	glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));
	
	// Bind vertex colour
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjs[ColourData]);
	glBufferData(GL_ARRAY_BUFFER, colourData.size()*sizeof(GLfloat), &colourData[0], GL_STREAM_DRAW);
	glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(0));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);	

	// Draw the mesh in all its glory
	glDrawArrays(GL_QUADS, 0, vertexData.size());

	// Disable the appropriate client arrays...
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	debug_opengl_state();
}

//
///**
// * Clear out all pre-existing loaded geometry for this particle batch mesh.
// */
//void ESPParticleBatchMesh::Flush() {
//	// Clear all the data arrays
//	this->vertexData.clear();
//	this->normalData.clear();
//	this->texCoordData.clear();
//	this->colourData.clear();
//	this->recentlyKilled.clear();
//	this->recentlyRevived.clear();
//	this->particleIndices.clear();
//
//	this->texture = NULL;
//	this->effect  = NULL;
//}
//
///**
// * Set the particle geometry for the given set of particles all assigned to have
// * the given textures. This will flush out all previously loaded geometry.
// */
//void ESPParticleBatchMesh::SetParticleGeometry(const std::list<ESPParticle*>& allParticles, Texture2D* texture) {
//	assert(texture != NULL);
//	assert(allParticles.size() > 0);
//
//	// Clear out all previous geometry data
//	this->Flush();
//
//	// Set up all the required arrays
//	this->vertexData.reserve(allParticles.size());
//	this->normalData.reserve(allParticles.size());
//	this->texCoordData.reserve(allParticles.size());
//	this->colourData.reserve(allParticles.size());
//
//	// Load the geometry and its attributes into proper VBOs for each particle that is alive
//	for (std::list<ESPParticle*>::const_iterator particleIter = allParticles.begin(); particleIter != allParticles.end(); particleIter++) {
//		
//		const ESPParticle* currParticle = *particleIter;
//		if (!currParticle->IsDead()) {
//			this->ReviveParticleGeometry(currParticle);
//		}
//	}
//
//
//}
//
//void ESPParticleBatchMesh::SetParticleGeometry(const std::list<ESPParticle*>& allParticles, CgFxEffectBase* effect) {
//	assert(effect != NULL);
//	assert(allParticles.size() > 0);
//
//	// Clear out all previous geometry data
//	this->Flush();
//
//}
//	
//// TODO: 
////void ESPParticleBatchMesh::SetParticleGeometry(const std::list<ESPParticle*>& allParticles, onomotopoiea???);
//
///**
// * Queues up killed particle geometry, to be removed from VBOs at draw time.
// */
//void ESPParticleBatchMesh::KillParticleGeometry(const ESPParticle* particle) {
//	assert(particle != NULL);
//
//	// Make sure the particle is actually a part of this batch mesh
//	if (this->particleIndices.find(particle) != this->particleIndices.end()) {
//		assert(false);
//		return;
//	}
//
//	this->recentlyKilled.push_back(particle);
//}
//
///**
// * Queues up revived geometry, to be added to VBOs at draw time.
// */
//void ESPParticleBatchMesh::ReviveParticleGeometry(const ESPParticle* particle) {
//	assert(particle != NULL);
//	this->recentlyRevived.push_back(particle);
//}
//
///**
// * Will add geometry to the VBOs for revived particles and will remove geometry
// * from VBOs for killed particles.
// */
//void ESPParticleBatchMesh::UpdateVBOData() {
//	// First thing we do is kill off all the geometry 
//
//}
//
//
///**
// * Draw this entire mesh using VBOs.
// */
//void ESPParticleBatchMesh::Draw() {
//	// Revive geometry that needs reviving and kill geometry that needs killing
//	this->UpdateVBOData();
//	
//	// If there's no data to draw just get out now
//	if (this->vertexData.size() < 3) {
//		return;
//	}
//
//	// Bind vertices
//	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjs[VertexData]);
//	//glBufferData(GL_ARRAY_BUFFER, this->vertexData.size(), &this->vertexData[0], this->vertexHint);
//	glVertexPointer(3, GL_FLOAT, sizeof(Point3D), BUFFER_OFFSET(0));
//	glEnableClientState(GL_VERTEX_ARRAY);
//
//	
//	// Bind normals
//	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjs[NormalData]);
//	//glBufferData(GL_ARRAY_BUFFER, this->normalData.size(), &this->normalData[0], this->normalHint);
//	glNormalPointer(GL_FLOAT, sizeof(Vector3D), BUFFER_OFFSET(0));
//	glEnableClientState(GL_NORMAL_ARRAY);	
//
//
//	// Bind texture coordinates
//	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjs[TexCoordData]);
//	//glBufferData(GL_ARRAY_BUFFER, this->texCoordData.size(), &this->texCoordData[0], this->texCoordHint);
//	glTexCoordPointer(2, GL_FLOAT, sizeof(Point2D), BUFFER_OFFSET(0));
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);	
//
//
//	// Bind vertex colour
//	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjs[ColourData]);
//	//glBufferData(GL_ARRAY_BUFFER, this->colourData.size(), &this->colourData[0], this->colourHint);
//	glColorPointer(4, GL_FLOAT, sizeof(Colour4D), BUFFER_OFFSET(0));
//	glEnableClientState(GL_COLOR_ARRAY);	
//
//	// Draw the mesh in all its glory
//	glDrawArrays(GL_TRIANGLES, 0, this->vertexData.size());
//
//	// Disable the appropriate client arrays...
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
//
//	debug_opengl_state();
//}