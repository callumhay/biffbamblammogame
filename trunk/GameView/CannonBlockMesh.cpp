/**
 * CannonBlockMesh.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#include "CannonBlockMesh.h"
#include "GameViewConstants.h"

#include "../ESPEngine/ESPPointEmitter.h"
#include "../GameModel/CannonBlock.h"
#include "../ResourceManager.h"

CannonBlockMesh::CannonBlockMesh() : AbstractCannonBlockMesh(), cannonBlockBaseGeometry(NULL),
cannonBlockBarrelGeometry(NULL) {

	this->LoadMesh();
}

CannonBlockMesh::~CannonBlockMesh() {
	bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->cannonBlockBarrelGeometry);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->cannonBlockBaseGeometry);
	assert(success);
    UNUSED_VARIABLE(success);
}

void CannonBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                           const BasicPointLight& fillLight, const BasicPointLight& ballLight) const {

	bool doCannonActiveEffectTick = false;

    // If the ball camera is active then highlight all of the cannon blocks...
    if (GameBall::GetIsBallCameraOn()) {
        bool doFlareTick = false;
        for (std::set<const CannonBlock*>::const_iterator iter = this->cannonBlocks.begin();
             iter != this->cannonBlocks.end(); ++iter) {

            const CannonBlock* currCannonBlock = *iter;
            const GameBall* loadedBall = currCannonBlock->GetLoadedBall();
            if (loadedBall != NULL && loadedBall->GetIsBallCameraOn()) {
                continue;
            }

            glPushMatrix();
            // Translate to the piece location in the game model...
            const Point2D& blockCenter = currCannonBlock->GetCenter();
            glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
            this->ballCamAttentionEffectEmitter->Draw(camera);
            doFlareTick = true;
            glPopMatrix();
        }

        if (doFlareTick) {
            this->ballCamAttentionEffectEmitter->Tick(dT);
        }
    }

	// Go through each of the cannon blocks and draw them, each with their proper,
	// respective barrel orientation
	float cannonRotationInDegs;
	for (std::set<const CannonBlock*>::const_iterator iter = this->cannonBlocks.begin();
         iter != this->cannonBlocks.end(); ++iter) {

		const CannonBlock* currCannonBlock = *iter;
		const GameBall* loadedBall = currCannonBlock->GetLoadedBall();

		// If the current cannon block has a ball loaded in it with the ball camera 
		// on then don't draw any of it
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
		
		// If the cannon is loaded then we highlight it with an effect to draw attention to it
		if (currCannonBlock->GetIsLoaded()) {
			this->activeCannonEffectEmitter->Draw(camera);
			doCannonActiveEffectTick = true;
		}

		// If the lights are out and a ball is inside the current cannon block 
		// then we illuminate the cannon block
		if (currCannonBlock->GetLoadedBall() != NULL) {
			BasicPointLight newKeyLight(Point3D(blockCenter[0], blockCenter[1], 10), Colour(1, 1, 1), 0.0f);
            this->cannonBlockBaseGeometry->Draw(camera, newKeyLight, fillLight, ballLight);
            
            // Rotate the barrel to its current direction
            glRotatef(cannonRotationInDegs, 0, 0, 1);
			this->cannonBlockBarrelGeometry->Draw(camera, newKeyLight, fillLight, ballLight);
		}
		else {
            this->cannonBlockBaseGeometry->Draw(camera, keyLight, fillLight, ballLight);

            // Rotate the barrel to its current direction
            glRotatef(cannonRotationInDegs, 0, 0, 1);
			this->cannonBlockBarrelGeometry->Draw(camera, keyLight, fillLight, ballLight);
		}
		glPopMatrix();
	}
	
	if (doCannonActiveEffectTick) {
		this->activeCannonEffectEmitter->Tick(dT);
	}
}

void CannonBlockMesh::LoadMesh() {
	assert(this->cannonBlockBaseGeometry == NULL);
	assert(this->cannonBlockBarrelGeometry == NULL);
	
	this->cannonBlockBaseGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CANNON_BLOCK_BASE_MESH);
	this->cannonBlockBarrelGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CANNON_BLOCK_BARREL_MESH);

    assert(this->cannonBlockBaseGeometry != NULL);
	assert(this->cannonBlockBarrelGeometry != NULL);
}