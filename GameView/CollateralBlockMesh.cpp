

#include "CollateralBlockMesh.h"
#include "GameViewConstants.h"
#include "../GameModel/CollateralBlock.h"

CollateralBlockMesh::CollateralBlockMesh() : collateralBlockGeometry(NULL) {
	this->LoadMesh();
}

CollateralBlockMesh::~CollateralBlockMesh() {
	ResourceManager::GetInstance()->ReleaseMeshResource(this->collateralBlockGeometry);
}

void CollateralBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
															 const BasicPointLight& fillLight, const BasicPointLight& ballLight) const {
	static double SHAKE_TIME_COUNT = 0.0;
	SHAKE_TIME_COUNT += dT;

	float currRotation;
	Vector3D shake;
	Colour colour;
	static const float SHAKE_DIST = 0.15f * std::min<float>(LevelPiece::PIECE_HEIGHT, LevelPiece::PIECE_WIDTH);

	for (std::set<const CollateralBlock*>::const_iterator iter = this->collateralBlocks.begin(); iter != this->collateralBlocks.end(); ++iter) {
		const CollateralBlock* collateralBlock = *iter;
		assert(collateralBlock != NULL);

		const Point2D& blockCenter = collateralBlock->GetCenter();
		currRotation = collateralBlock->GetCurrentRotation();

		glPushMatrix();
		// Translate to the piece location in the game model...
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

		// Based on whether it's in warning mode, change its colour and position to indicate that
		// it's about to 'go off'
		// TODO
		if (collateralBlock->GetState() == CollateralBlock::WarningState && SHAKE_TIME_COUNT >= 0.016) {
			// Randomly shake the block around...
			shake[0] = Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_DIST;
			shake[1] = Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_DIST;
			shake[2] = Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_DIST;
			glTranslatef(shake[0], shake[1], shake[2]);

			// Randomly flash colour on the block
			colour[0] = 0.5f + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.5f;
			colour[1] = Randomizer::GetInstance()->RandomNumZeroToOne();
			colour[2] = 0.0f;
			glColor4f(colour[0], colour[1], colour[2], 1);

			SHAKE_TIME_COUNT = 0.0;
		}
		else {
			glColor4f(1, 1, 1, 1);
		}

		// Rotate it based on its current rotation...
		glRotatef(currRotation, 1, 0, 0);

		this->collateralBlockGeometry->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}
}

void CollateralBlockMesh::LoadMesh() {
	assert(this->collateralBlockGeometry == NULL);
	this->collateralBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->COLLATERAL_BLOCK_MESH);
	assert(this->collateralBlockGeometry != NULL);
}