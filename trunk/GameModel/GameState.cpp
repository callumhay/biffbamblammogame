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
void GameState::DoUpdateToPaddleBoundries(bool doAttachedBallCollision) {
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	GameLevel* currLevel = this->gameModel->GetCurrentLevel();
	assert(paddle != NULL);
	assert(currLevel != NULL);

	// Start by setting the paddle boundries to be the innermost solid blocks on the paddle level of blocks
	paddle->UpdatePaddleBounds(currLevel->GetPaddleMinBound(), currLevel->GetPaddleMaxBound());

	// Check to see if the paddle collided with any blocks...
	bool didCollideWithPiece = false;
	std::set<LevelPiece*> collisionCandidatePieces = currLevel->GetLevelPieceCollisionCandidates(*paddle, doAttachedBallCollision);
	for (std::set<LevelPiece*>::iterator iter = collisionCandidatePieces.begin(); iter != collisionCandidatePieces.end(); ++iter) {
		LevelPiece* currPiece = *iter;
		if (currPiece->IsNoBoundsPieceType()) {
			continue;
		}

		// First check to see if the paddle actually collides with the piece...
		didCollideWithPiece = paddle->CollisionCheck(currPiece->GetBounds(), doAttachedBallCollision);
		if (didCollideWithPiece) {
			paddle->UpdateBoundsByPieceCollision(*currPiece, doAttachedBallCollision);
			//this->gameModel->CollisionOccurred(currProjectile, currPiece);
		}
	}
}