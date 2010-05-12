

#include "CollateralBlockMesh.h"
#include "GameViewConstants.h"
#include "../GameModel/CollateralBlock.h"

CollateralBlockMesh::CollateralBlockMesh() : collateralBlockGeometry(NULL) {
	this->LoadMesh();
}

CollateralBlockMesh::~CollateralBlockMesh() {
	ResourceManager::GetInstance()->ReleaseMeshResource(this->collateralBlockGeometry);
}

void CollateralBlockMesh::Draw(const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) const {
	glPushMatrix();

	// Translate to world space of the current game level...
	glTranslatef(this->currWorldTranslation[0], this->currWorldTranslation[1], this->currWorldTranslation[2]);

	float currRotation;
	for (std::set<const CollateralBlock*>::const_iterator iter = this->collateralBlocks.begin(); iter != this->collateralBlocks.end(); ++iter) {
		const CollateralBlock* collateralBlock = *iter;
		assert(collateralBlock != NULL);

		const Point2D& blockCenter = collateralBlock->GetCenter();
		currRotation = collateralBlock->GetCurrentRotation();

		glPushMatrix();
		// Translate to the piece location in the game model...
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
		// Rotate it based on its current rotation...
		glRotatef(currRotation, 1, 0, 0);

		this->collateralBlockGeometry->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}

	glPopMatrix();
}

void CollateralBlockMesh::LoadMesh() {
	assert(this->collateralBlockGeometry == NULL);
	this->collateralBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->COLLATERAL_BLOCK_MESH);
	assert(this->collateralBlockGeometry != NULL);
}