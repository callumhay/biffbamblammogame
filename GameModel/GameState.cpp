/**
 * GameState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameState.h"
#include "GameModel.h"
#include "LevelCompleteState.h"

/**
 * Default action here is to just move the paddle around.
 */
void GameState::MovePaddleKeyPressed(const PlayerPaddle::PaddleMovement& paddleMovement) {
	this->gameModel->GetPlayerPaddle()->ControlPaddleMovement(paddleMovement);
}

/**
 * This should only be called when the state allows the player to control the paddle...
 * This function updates the paddle boundries so that it hits blocks around it.
 */
bool GameState::DoUpdateToPaddleBoundriesAndCollisions(double dT, bool doAttachedBallCollision) {
	UNUSED_PARAMETER(dT);

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	GameLevel* currentLevel = this->gameModel->GetCurrentLevel();
	assert(paddle != NULL);
	assert(currentLevel != NULL);

	// Start by setting the paddle boundries to be the innermost solid blocks on the paddle level of blocks
	paddle->UpdatePaddleBounds(currentLevel->GetPaddleMinBound(), currentLevel->GetPaddleMaxBound());

	// Check to see if the paddle collided with any blocks...
	bool didCollideWithPiece = false;
	std::set<LevelPiece*> collisionCandidatePieces = currentLevel->GetLevelPieceCollisionCandidates(*paddle, doAttachedBallCollision);
	for (std::set<LevelPiece*>::iterator iter = collisionCandidatePieces.begin(); iter != collisionCandidatePieces.end(); ++iter) {
		LevelPiece* currPiece = *iter;
		if (currPiece->IsNoBoundsPieceType()) {
			continue;
		}

		// First check to see if the paddle actually collides with the piece...
		didCollideWithPiece = paddle->CollisionCheck(currPiece->GetBounds(), doAttachedBallCollision);
		if (didCollideWithPiece) {
			paddle->UpdateBoundsByPieceCollision(*currPiece, doAttachedBallCollision);
            LevelPiece* resultingPiece = currPiece->CollisionOccurred(this->gameModel, *paddle);
            assert(resultingPiece == currPiece);

			/*
			if ((paddle->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
				LevelPiece* resultingPiece = currPiece->TickPaddleShieldCollision(dT, *paddle, this->gameModel);
				// Check to see if the level is done
				if (currentLevel->IsLevelComplete()) {
					// The level was completed, move to the level completed state
					this->gameModel->SetNextState(new LevelCompleteState(this->gameModel));
					// We signify to the caller that the state has changed...
					return true;
				}
			}
			*/

		}
	}

	return false;
}