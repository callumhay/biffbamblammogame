/**
 * GameState.cpp
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

#include "GameState.h"
#include "GameModel.h"

#include "BallDeathState.h"
#include "BallInPlayState.h"
#include "BallOnPaddleState.h"
#include "BallWormholeState.h"
#include "GameCompleteState.h"
#include "GameOverState.h"
#include "LevelStartState.h"
#include "LevelCompleteState.h"
#include "WorldCompleteState.h"


GameState* GameState::Build(const GameState::GameStateType& stateType, GameModel* gameModel) {
    switch (stateType) {

        case BallDeathStateType:
            return new BallDeathState(gameModel->GetGameBalls().front(), gameModel);
        case BallInPlayStateType:
            return new BallInPlayState(gameModel);
        case BallOnPaddleStateType:
            return new BallOnPaddleState(gameModel);
        case BallWormholeStateType:
            return new BallWormholeState(gameModel);
        case GameCompleteStateType:
            return new GameCompleteState(gameModel);
        case GameOverStateType:
            return new GameOverState(gameModel);
        case LevelStartStateType:
            return new LevelStartState(gameModel);
        case LevelCompleteStateType:
            return new LevelCompleteState(gameModel);
        case WorldCompleteStateType:
            return new WorldCompleteState(gameModel);

        case NULLStateType:
        default:
            assert(false);
            return NULL;
    }
}

bool GameState::IsGameInPlayState(const GameModel& gameModel) {
    return gameModel.GetCurrentStateType() == GameState::BallInPlayStateType ||
        gameModel.GetCurrentStateType() == GameState::BallOnPaddleStateType ||
        gameModel.GetCurrentStateType() == GameState::BallWormholeStateType &&
        (gameModel.GetPauseState() & GameModel::PauseGame) != 0x0;
}

void GameState::MoveKeyPressedForPaddle(int dir, float magnitudePercent) {

    if ((this->gameModel->GetPauseState() & (GameModel::PausePaddle | GameModel::PausePaddleControls)) == 0x0) {

        // Default action here is to just move the paddle around
        this->gameModel->GetPlayerPaddle()->ControlPaddleMovement(
            static_cast<PlayerPaddle::PaddleMovement>(dir), magnitudePercent);
    }
}

/**
 * This should only be called when the state allows the player to control the paddle...
 * This function updates the paddle boundaries so that it hits blocks around it.
 */
bool GameState::DoUpdateToPaddleBoundriesAndCollisions(double dT, bool doAttachedBallCollision) {
	UNUSED_PARAMETER(dT);

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();

    // If the paddle is not in the game then we shouldn't be updating anything
    if (paddle->HasBeenPausedAndRemovedFromGame(this->gameModel->GetPauseState()) ||
        !paddle->GetIsLevelBoundsCheckingOn()) {
        return false;
    }

	GameLevel* currentLevel = this->gameModel->GetCurrentLevel();
	assert(paddle != NULL);
	assert(currentLevel != NULL);

	// Check to see if the paddle collided with any blocks...
	bool didCollideWithCurrentPiece = false;
    bool didCollideWithAnyPiece = false;
	std::set<LevelPiece*> collisionCandidatePieces = currentLevel->GetLevelPieceCollisionCandidates(*paddle, doAttachedBallCollision);
	for (std::set<LevelPiece*>::iterator iter = collisionCandidatePieces.begin(); iter != collisionCandidatePieces.end(); ++iter) {
		LevelPiece* currPiece = *iter;
        if (currPiece->IsNoBoundsPieceType() && currPiece->GetType() != LevelPiece::Portal) {
			continue;
		}

		// First check to see if the paddle actually collides with the piece...
		didCollideWithCurrentPiece = paddle->CollisionCheck(currPiece->GetBounds(), doAttachedBallCollision);
		if (didCollideWithCurrentPiece) {
            
            if (!currPiece->IsNoBoundsPieceType()) {
			    paddle->UpdateBoundsByPieceCollision(*currPiece, doAttachedBallCollision);
            }

            LevelPiece* resultingPiece = currPiece->CollisionOccurred(this->gameModel, *paddle);
            if (resultingPiece != currPiece) {
                this->gameModel->PerformLevelCompletionChecks();
            }
            didCollideWithAnyPiece = true;
		}
	}

    if (!didCollideWithAnyPiece) {
	    // Set the paddle boundaries to be the innermost solid blocks on the paddle level of blocks
	    paddle->UpdatePaddleBounds(
            currentLevel->GetPaddleMinXBound(paddle->GetCenterPosition()[0], paddle->GetDefaultYPosition()), 
            currentLevel->GetPaddleMaxXBound(paddle->GetCenterPosition()[0], paddle->GetDefaultYPosition()));
    }

	return false;
}