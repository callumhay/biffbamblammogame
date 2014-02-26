/**
 * InCannonBallState.cpp
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

#include "InCannonBallState.h"
#include "GameBall.h"
#include "CannonBlock.h"
#include "GameEventManager.h"

InCannonBallState::InCannonBallState(GameBall* ball, CannonBlock* cannonBlock, BallState* prevState) : 
BallState(ball), cannonBlock(cannonBlock), previousState(prevState) {
	assert(cannonBlock != NULL);
	assert(prevState != NULL);

	// Set the ball to be perfectly centered inside the cannon
	this->gameBall->SetCenterPosition(cannonBlock->GetCenter());

	// Make sure the ball is invisible (it's supposed to be "inside" the cannon now)
	// TODO: BE CAREFUL HERE!!!

	// The ball also should be ignoring collisions until it's fired back out of the cannon
	this->gameBall->SetBallBallCollisionsDisabled();
	this->gameBall->SetBallBlockAndBossCollisionsDisabled();

}

InCannonBallState::~InCannonBallState() {	
	// Special case: The game was closed while a ball was still inside a cannon block,
	// we need to delete the previous state as well...
	if (this->previousState != NULL) {
		delete this->previousState;
		this->previousState = NULL;
	}

}

BallState* InCannonBallState::Clone(GameBall* newBall) const {
	return new InCannonBallState(newBall, this->cannonBlock, this->previousState->Clone(newBall));
}

void InCannonBallState::Tick(bool simulateMovement, double seconds, const Vector2D& worldSpaceGravityDir, 
                             GameModel* gameModel) {

    UNUSED_PARAMETER(simulateMovement);
    UNUSED_PARAMETER(gameModel);
	UNUSED_PARAMETER(worldSpaceGravityDir);

    // Start by doing a test to see if the cannon will actually fire...
    Vector2D cannonFireDir;
    bool isBallCameraActive = this->gameBall->HasBallCameraActive();
    bool willCannonFire = this->cannonBlock->TestRotateAndFire(seconds, isBallCameraActive, cannonFireDir);
    if (willCannonFire) {
        
        // Set the velocity in the direction the cannon has fired in
        this->gameBall->SetVelocity(this->gameBall->GetSpeed(), cannonFireDir);

        // Move the ball so that it's outside the barrel of the cannon entirely - it will
        // currently be perfectly centered inside the cannon, so just move it along the cannon
        // firing vector by the correct amount...
        //const Collision::Circle2D& ballBounds = this->gameBall->GetBounds();
        this->gameBall->SetCenterPosition(this->cannonBlock->GetCenter() + 
            CannonBlock::HALF_CANNON_BARREL_LENGTH * cannonFireDir);

        // Restore ball state information that was changed during this state's operations
        this->gameBall->SetBallBallCollisionsEnabled();
        this->gameBall->SetBallBlockAndBossCollisionsEnabled();

        // EVENT: Ball has officially been fired from the cannon
        GameEventManager::Instance()->ActionBallFiredFromCannon(*this->gameBall, *this->cannonBlock);
    }

    // 'Tick' the cannon to spin the rocket around inside it... eventually the function will say
    // it has fired the rocket
    // WARNING: This function can destroy the cannon!
    std::pair<LevelPiece*, bool> cannonFiredPair = this->cannonBlock->RotateAndFire(seconds, gameModel, isBallCameraActive);
    UNUSED_VARIABLE(cannonFiredPair);
	if (willCannonFire) {
        assert(cannonFiredPair.second);

		// We use this to ensure that the destructor knows not to delete the previous state as well
		BallState* prevStateTemp = this->previousState;
		this->previousState = NULL;

		// Change the state of the ball back to what it was before this state...
		this->gameBall->SetBallState(prevStateTemp, true);	// WARNING: THIS WILL DELETE THIS OBJECT, SO GET OUT NOW

		return;
	}
}