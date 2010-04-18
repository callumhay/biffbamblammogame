
#include "CannonBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/CannonBlock.h"

CannonBlockMesh::CannonBlockMesh() : cannonBlockBaseGeometry(NULL),
cannonBlockBarrelGeometry(NULL), currWorldTranslation(0, 0, 0) {
	this->LoadMesh();
}

CannonBlockMesh::~CannonBlockMesh() {
}


void CannonBlockMesh::Draw(const Camera& camera, const PointLight& keyLight, 
													 const PointLight& fillLight, const PointLight& ballLight) const {

	glPushMatrix();

	// Translate to world space of the current game level...
	glTranslatef(this->currWorldTranslation[0], this->currWorldTranslation[1], this->currWorldTranslation[2]);

	// Go through each of the cannon blocks and draw them, each with their proper,
	// respective barrel orientation
	float cannonRotationInDegs;
	for (std::list<const CannonBlock*>::const_iterator iter = this->cannonBlocks.begin(); iter != this->cannonBlocks.end(); ++iter) {
		const CannonBlock* currCannonBlock = *iter;
		cannonRotationInDegs = currCannonBlock->GetCurrentCannonAngleInDegs();
		const Point2D& blockCenter = currCannonBlock->GetCenter();

		glPushMatrix();
		// Translate to the piece location in the game model...
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
		// Rotate the barrel to its current direction
		glRotatef(cannonRotationInDegs, 0, 0, 1);
		this->cannonBlockBarrelGeometry->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}

	glPopMatrix();
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