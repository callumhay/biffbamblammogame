

#include "TeslaBlockMesh.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"
#include "../ESPEngine/ESPPointEmitter.h"
#include "../GameModel/TeslaBlock.h"

const float TeslaBlockMesh::COIL_ROTATION_SPEED_DEGSPERSEC = 270;

TeslaBlockMesh::TeslaBlockMesh() : teslaBaseMesh(NULL), teslaCoilMesh(NULL), flarePulse(0,0), /*sparkGravity(Vector3D(0, -9.8f, 0)), sparkTex(NULL), teslaSparks(NULL),*/
teslaCenterFlare(NULL), flareTex(NULL){
	this->LoadMesh();
	
	// Grab the flare texture
	this->flareTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BRIGHT_FLARE, 
																																																  Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->flareTex != NULL);

	// Set up the pulse effector for the flare emitter
	ScaleEffect flarePulseSettings;
	flarePulseSettings.pulseGrowthScale = 1.25f;
	flarePulseSettings.pulseRate        = 5.0f;
	this->flarePulse = ESPParticleScaleEffector(flarePulseSettings);

	// Setup the flare emitter
	this->teslaCenterFlare = new ESPPointEmitter();
	this->teslaCenterFlare->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	this->teslaCenterFlare->SetInitialSpd(ESPInterval(0));
	this->teslaCenterFlare->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
	this->teslaCenterFlare->SetEmitAngleInDegrees(0);
	this->teslaCenterFlare->SetParticleAlignment(ESP::ScreenAligned);
	this->teslaCenterFlare->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->teslaCenterFlare->SetEmitPosition(Point3D(0, 0, 0));
	this->teslaCenterFlare->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH));
	this->teslaCenterFlare->SetParticleColour(ESPInterval(0.9f), ESPInterval(0.79f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->teslaCenterFlare->AddEffector(&this->flarePulse);
	
	bool result = this->teslaCenterFlare->SetParticles(1, this->flareTex);
	assert(result);

	/*
	// DECIDED NOT TO DO SPARKS SINCE EACH TESLA BLOCK WOULD NEED ITS OWN AND THEY WOULD NEED TO BE RESET
	// EVERY TIME A TESLA BLOCK TURNS OFF.. NOT WORTH IT
	// Grab the spark texture
	this->sparkTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, 
		Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->sparkTex != NULL);
	
	this->teslaSparks = new ESPPointEmitter();
	// ...
	*/
}

TeslaBlockMesh::~TeslaBlockMesh() {
	ResourceManager::GetInstance()->ReleaseMeshResource(this->teslaBaseMesh);
	ResourceManager::GetInstance()->ReleaseMeshResource(this->teslaCoilMesh);
	ResourceManager::GetInstance()->ReleaseTextureResource(this->flareTex);
	delete this->teslaCenterFlare;
	this->teslaCenterFlare = NULL;
}

void TeslaBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
													const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
	
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	float rotationAmt = dT * COIL_ROTATION_SPEED_DEGSPERSEC;

	// Go through each tesla block and transform their center's appropriately
	for (std::set<const TeslaBlock*>::const_iterator iter = this->teslaBlocks.begin(); iter != this->teslaBlocks.end(); ++iter) {
		const TeslaBlock* currTeslaBlock = *iter;
		assert(currTeslaBlock != NULL);
		
		std::map<const TeslaBlock*, std::pair<Vector3D, float>>::iterator findIter = this->rotations.find(currTeslaBlock);

		const Vector3D& currRotationAxis = findIter->second.first;
		float& currRotatationAmt = findIter->second.second;
		const Point2D& blockCenter = currTeslaBlock->GetCenter();

		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
		glRotatef(currRotatationAmt, currRotationAxis[0], currRotationAxis[1], currRotationAxis[2]);	
		this->teslaCoilMesh->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();

		// Only draw the flare in the middle and only rotate the center if the tesla block is active
		if (currTeslaBlock->GetIsElectricityActive()) {
			glPushMatrix();
			glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
			this->teslaCenterFlare->Draw(camera, Vector3D(0,0,0), true);
			glPopMatrix();

			currRotatationAmt += rotationAmt;
		}

	}
	glPopAttrib();

	this->teslaCenterFlare->Tick(dT);
}

void TeslaBlockMesh::LoadMesh() {
	assert(this->teslaBaseMesh == NULL);
	assert(this->teslaCoilMesh == NULL);
	assert(this->materialGroups.size() == 0);
	
	this->teslaBaseMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TESLA_BLOCK_BASE_MESH);
	assert(this->teslaBaseMesh != NULL);
	const std::map<std::string, MaterialGroup*>&  baseMatGrps = this->teslaBaseMesh->GetMaterialGroups();

	// We don't add the coil's material groups, we will draw those manually for each tesla block
	// so that we can move them around
	this->teslaCoilMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TESLA_BLOCK_COIL_MESH);
	assert(this->teslaCoilMesh != NULL);
	
	this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());
	
	assert(this->materialGroups.size() > 0);
}