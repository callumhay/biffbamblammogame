/**
 * Mesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Mesh.h"

// Copy constructor
PolygonGroup::PolygonGroup(const PolygonGroup& other) {
	this->polygroupArray = new float[other.polygroupArrayLength];
	for (unsigned int i = 0; i < other.polygroupArrayLength; i++) {
		this->polygroupArray[i] = other.polygroupArray[i];
	}
	this->polygroupArrayLength = other.polygroupArrayLength;
	this->numIndices = other.numIndices;
	this->polyType   = other.polyType;
}

PolygonGroup::PolygonGroup(const PolyGrpIndexer& faceIndexer, const std::vector<Point3D>& vertexStream, 
													 const std::vector<Vector3D>& normalStream, const std::vector<Point2D>& texCoordStream) {
	
	assert(faceIndexer.normalIndices.size() == faceIndexer.texCoordIndices.size());
	assert(faceIndexer.normalIndices.size() == faceIndexer.vertexIndices.size());
	assert(faceIndexer.vertexIndices.size() == faceIndexer.texCoordIndices.size());

	this->polyType	 = faceIndexer.polyType;
	this->numIndices = faceIndexer.vertexIndices.size();
	this->polygroupArrayLength = numIndices * INTERLEAVED_MULTIPLIER;
	this->polygroupArray = new float[this->polygroupArrayLength];
	
	// Go through each possible vertex and add it to the polygon group array
	for (unsigned int i = 0, j = 0; i < this->polygroupArrayLength; i += INTERLEAVED_MULTIPLIER, j++) {
		const Point3D& vertex		= vertexStream[faceIndexer.vertexIndices[j]];
		const Vector3D& normal	= normalStream[faceIndexer.normalIndices[j]];
		const Point2D& texCoord = texCoordStream[faceIndexer.texCoordIndices[j]];

		this->polygroupArray[i]			= texCoord[0];
		this->polygroupArray[i + 1]	= texCoord[1];
		this->polygroupArray[i + 2]	= normal[0];
		this->polygroupArray[i + 3]	= normal[1];
		this->polygroupArray[i + 4]	= normal[2];
		this->polygroupArray[i + 5]	= vertex[0];
		this->polygroupArray[i + 6]	= vertex[1];
		this->polygroupArray[i + 7]	= vertex[2];
	}
}

/**
 * Translate this polygon group by the given vector.
 */
void PolygonGroup::Translate(const Vector3D& t) {
	// Only need to transform the vertices
	for (unsigned int i = 0; i < this->polygroupArrayLength; i += INTERLEAVED_MULTIPLIER) {
		this->polygroupArray[i + 5] += t[0];
		this->polygroupArray[i + 6] += t[1];
		this->polygroupArray[i + 7] += t[2];
	}
}

void PolygonGroup::Transform(const Matrix4x4& m) {
	// Only need to transform the vertices
	for (unsigned int i = 0; i < this->polygroupArrayLength; i += INTERLEAVED_MULTIPLIER) {
		Point3D tempVert(this->polygroupArray[i + 5], this->polygroupArray[i + 6], this->polygroupArray[i + 7]);
		Point3D resultVert = m * tempVert;
		Vector3D tempNorm(this->polygroupArray[i + 2], this->polygroupArray[i + 3], this->polygroupArray[i + 4]);
		Vector3D resultNorm = m * tempNorm;
		this->polygroupArray[i + 2] = resultNorm[0];
		this->polygroupArray[i + 3] = resultNorm[1];
		this->polygroupArray[i + 4] = resultNorm[2];
		this->polygroupArray[i + 5] = resultVert[0];
		this->polygroupArray[i + 6] = resultVert[1];
		this->polygroupArray[i + 7] = resultVert[2];
	}
}

/**
 * Obtain arrays of the vertex, normal and texture coordinate data for this polygon group.
 * Returns: The number of vertices in the data.
 */
unsigned int PolygonGroup::GetDataArrays(std::vector<float>& vertexArray, std::vector<float>& normalArray, std::vector<float>& texCoordArray) const {
	const float numVerts = static_cast<float>(this->polygroupArrayLength) / static_cast<float>(INTERLEAVED_MULTIPLIER);

	vertexArray.reserve(3 * numVerts);
	normalArray.reserve(3 * numVerts);
	texCoordArray.reserve(2 * numVerts);

	for (unsigned int i = 0; i < this->polygroupArrayLength; i += INTERLEAVED_MULTIPLIER) {
		
		texCoordArray.push_back(this->polygroupArray[i]);
		texCoordArray.push_back(this->polygroupArray[i + 1]);

		normalArray.push_back(this->polygroupArray[i + 2]);
		normalArray.push_back(this->polygroupArray[i + 3]);
		normalArray.push_back(this->polygroupArray[i + 4]);

		vertexArray.push_back(this->polygroupArray[i + 5]);
		vertexArray.push_back(this->polygroupArray[i + 6]);
		vertexArray.push_back(this->polygroupArray[i + 7]);
	}

	return numVerts;
}

void MaterialGroup::AddFaces(const PolyGrpIndexer& indexer, 
														 const std::vector<Point3D>& vertexStream, 
														 const std::vector<Vector3D>& normalStream,
														 const std::vector<Point2D>& texCoordStream) {

	this->polyGrp = new PolygonGroup(indexer, vertexStream, normalStream, texCoordStream);

	// Create a display list for the polygon group
	this->displayListID = glGenLists(1);
	glNewList(this->displayListID, GL_COMPILE);
	this->polyGrp->Draw();
	glEndList();
}

Mesh::Mesh(const std::string name, const std::map<std::string, MaterialGroup*> &matGrps):
name(name), matGrps(matGrps) {
}

Mesh::~Mesh(){
	this->Flush();
}

/**
 * Clean out all the stuff that currently makes up this mesh.
 */
void Mesh::Flush() {
	// Delete the material groups
	std::map<std::string, MaterialGroup*>::iterator matGrpIter = this->matGrps.begin();
	for (; matGrpIter != this->matGrps.end(); matGrpIter++) {
		delete matGrpIter->second;
		matGrpIter->second = NULL;
	}
	this->matGrps.clear();
}

/**
 * Set the overall colour of this mesh.
 */
void Mesh::SetColour(const Colour& c) {
	
	// Go through each of the material groups in this mesh and
	// change their diffuse colour.
	std::map<std::string, MaterialGroup*>::iterator matGrpIter = this->matGrps.begin();
	for (; matGrpIter != matGrps.end(); matGrpIter++) {
		CgFxMaterialEffect* currMaterial = matGrpIter->second->GetMaterial();
		MaterialProperties* currMatProps = currMaterial->GetProperties();
		currMatProps->diffuse = c;
	}
}

/**
 * Set the texture for the material group of the given name.
 */
void Mesh::SetTextureForMaterial(const std::string& matGrpName, Texture2D* texToSet) {

	// Make sure the material group exists
	std::map<std::string, MaterialGroup*>::iterator foundMatGrpIter = this->matGrps.find(matGrpName);
	assert(foundMatGrpIter != this->matGrps.end());

	// Change the texture for the found material group
	MaterialGroup* foundMatGrp = foundMatGrpIter->second;
	assert(foundMatGrp != NULL);
	CgFxMaterialEffect* matEffect = foundMatGrp->GetMaterial();
	MaterialProperties* matProps = matEffect->GetProperties();
	matProps->diffuseTexture = texToSet;
}

/**
 * Set the colour for the specified material group of the given name.
 */
void Mesh::SetColourForMaterial(const std::string& matGrpName, const Colour& c) {
	// Make sure the material group exists
	std::map<std::string, MaterialGroup*>::iterator foundMatGrpIter = this->matGrps.find(matGrpName);
	assert(foundMatGrpIter != this->matGrps.end());

	// Change the texture for the found material group
	MaterialGroup* foundMatGrp = foundMatGrpIter->second;
	assert(foundMatGrp != NULL);

	CgFxMaterialEffect* matEffect = foundMatGrp->GetMaterial();
	MaterialProperties* matProps = matEffect->GetProperties();
	matProps->diffuse = c;
}

/**
 * Replace the one and only material group in this mesh with the given material group.
 */
void Mesh::ReplaceMaterial(CgFxMaterialEffect* replacementMat) {
	assert(replacementMat != NULL);
	assert(this->matGrps.size() == 1);
	this->matGrps.begin()->second->ReplaceMaterialEffect(replacementMat);
}