
#include "CannonBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/CannonBlock.h"

CannonBlockMesh::CannonBlockMesh() : cannonBlockBaseGeometry(NULL),
cannonBlockBarrelGeometry(NULL) {
	this->LoadMesh();
}

CannonBlockMesh::~CannonBlockMesh() {
	ResourceManager::GetInstance()->ReleaseMeshResource(this->cannonBlockBarrelGeometry);
	ResourceManager::GetInstance()->ReleaseMeshResource(this->cannonBlockBaseGeometry);
}


void CannonBlockMesh::Draw(const Camera& camera, const BasicPointLight& keyLight, 
													 const BasicPointLight& fillLight, const BasicPointLight& ballLight, bool lightsAreOff) const {

	// Go through each of the cannon blocks and draw them, each with their proper,
	// respective barrel orientation
	float cannonRotationInDegs;
	for (std::set<const CannonBlock*>::const_iterator iter = this->cannonBlocks.begin(); iter != this->cannonBlocks.end(); ++iter) {
		const CannonBlock* currCannonBlock = *iter;
		const GameBall* loadedBall = currCannonBlock->GetLoadedBall();

		// If the current cannon block has a ball loaded in it with the ball camera 
		// on then don't draw the barrel
		if (loadedBall != NULL && loadedBall->GetIsBallCameraOn()) {
			continue;
		}

		cannonRotationInDegs = currCannonBlock->GetCurrentCannonAngleInDegs();
		const Point2D& blockCenter = currCannonBlock->GetCenter();
		const Colour& cannonColour = currCannonBlock->GetColour();

		// Make sure the proper colour is set for the barrel
		glColor4f(cannonColour.R(), cannonColour.G(), cannonColour.B(), 1.0f);

		glPushMatrix();
		// Translate to the piece location in the game model...
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
		// Rotate the barrel to its current direction
		glRotatef(cannonRotationInDegs, 0, 0, 1);

		// If the lights are out and a ball is inside the current cannon block 
		// then we illuminate the cannon block
		if (lightsAreOff && currCannonBlock->GetLoadedBall() != NULL) {
			BasicPointLight newKeyLight(Point3D(0, 0, -10), Colour(1, 1, 1), 0.01f);
			this->cannonBlockBarrelGeometry->Draw(camera, newKeyLight, fillLight, ballLight);
		}
		else {
			this->cannonBlockBarrelGeometry->Draw(camera, keyLight, fillLight, ballLight);
		}
		glPopMatrix();
	}
}

void CannonBlockMesh::SetAlphaMultiplier(float alpha) {
	const std::map<std::string, MaterialGroup*>&  barrelMatGrps = this->cannonBlockBarrelGeometry->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = barrelMatGrps.begin(); iter != barrelMatGrps.end(); ++iter) {
		MaterialGroup* matGrp = iter->second;
		matGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
	}
}

void CannonBlockMesh::LoadMesh() {
	assert(this->cannonBlockBaseGeometry == NULL);
	assert(this->cannonBlockBarrelGeometry == NULL);
	assert(this->materialGroups.size() == 0);
	
	this->cannonBlockBaseGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CANNON_BLOCK_BASE_MESH);
	assert(this->cannonBlockBaseGeometry != NULL);
	const std::map<std::string, MaterialGroup*>&  baseMatGrps = this->cannonBlockBaseGeometry->GetMaterialGroups();

	// We don't add the barrel's material groups, we will draw those manually for each cannon block
	// so that we can orient the barrels according to each block (see CannonBlockMesh::Draw).
	this->cannonBlockBarrelGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CANNON_BLOCK_BARREL_MESH);
	assert(this->cannonBlockBarrelGeometry != NULL);
	
	this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());
	
	assert(this->materialGroups.size() > 0);
}