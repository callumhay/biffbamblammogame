/**
 * CollateralBlockMesh.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CollateralBlockMesh.h"
#include "GameViewConstants.h"
#include "../GameModel/CollateralBlock.h"

CollateralBlockMesh::CollateralBlockMesh() : collateralBlockGeometry(NULL) {
	this->LoadMesh();

    std::vector<double> timeVals;
    timeVals.reserve(3);
    timeVals.push_back(0.0);
    timeVals.push_back(0.5);
    timeVals.push_back(1.0);

    std::vector<float> alphaVals;
    alphaVals.reserve(timeVals.size());
    alphaVals.push_back(1.0f);
    alphaVals.push_back(0.5f);
    alphaVals.push_back(1.0f);

    this->alphaFlashAnim.ClearLerp();
    this->alphaFlashAnim.SetLerp(timeVals, alphaVals);
    this->alphaFlashAnim.SetRepeat(true);
}

CollateralBlockMesh::~CollateralBlockMesh() {
	bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->collateralBlockGeometry);
    assert(success);
    UNUSED_VARIABLE(success);
}

void CollateralBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                               const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    UNUSED_PARAMETER(dT);

	float currRotation;
	Vector3D shake;
	Colour colour;
	static const float SHAKE_DIST = 0.15f * std::min<float>(LevelPiece::PIECE_HEIGHT, LevelPiece::PIECE_WIDTH);

	for (std::set<const CollateralBlock*>::const_iterator iter = this->collateralBlocks.begin();
         iter != this->collateralBlocks.end(); ++iter) {

		const CollateralBlock* collateralBlock = *iter;
		assert(collateralBlock != NULL);

		const Point2D& blockCenter = collateralBlock->GetCenter();
		currRotation = collateralBlock->GetCurrentRotation();

		glPushMatrix();
		// Translate to the piece location in the game model...
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

		// Based on whether it's in warning mode, change its colour and position to indicate that
		// it's about to 'go off'
        switch (collateralBlock->GetState()) {
            case CollateralBlock::WarningState:
			    // Randomly shake the block around...
			    shake[0] = Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_DIST;
			    shake[1] = Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_DIST;
			    shake[2] = Randomizer::GetInstance()->RandomNumNegOneToOne() * SHAKE_DIST;
			    glTranslatef(shake[0], shake[1], shake[2]);

			    // Randomly flash colour on the block
			    colour[0] = 0.5f + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.5f;
			    colour[1] = Randomizer::GetInstance()->RandomNumZeroToOne();
			    colour[2] = 0.0f;
    			
                this->alphaFlashAnim.Tick(dT);
                glColor4f(colour[0], colour[1], colour[2], this->alphaFlashAnim.GetInterpolantValue());
                break;

            default:
			    glColor4f(1, 1, 1, 1);
                break;
        }

		// Rotate it based on its current rotation...
		glRotatef(currRotation, 1, 0, 0);

		this->collateralBlockGeometry->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}
}

void CollateralBlockMesh::SetAlphaMultiplier(float alpha) {
	const std::map<std::string, MaterialGroup*>&  matGrps = this->collateralBlockGeometry->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = matGrps.begin(); iter != matGrps.end(); ++iter) {
		MaterialGroup* matGrp = iter->second;
		matGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
	}
}

void CollateralBlockMesh::LoadMesh() {
	assert(this->collateralBlockGeometry == NULL);
	this->collateralBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->COLLATERAL_BLOCK_MESH);
	assert(this->collateralBlockGeometry != NULL);
}