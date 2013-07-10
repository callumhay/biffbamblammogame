/**
 * GameState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

void GameState::MoveKeyPressed(int dir, float magnitudePercent) {
    assert(dir <= 1 && dir >= -1);
    // Default action here is to just move the paddle around
    this->gameModel->GetPlayerPaddle()->ControlPaddleMovement(static_cast<PlayerPaddle::PaddleMovement>(dir), magnitudePercent);
}

/**
 * This should only be called when the state allows the player to control the paddle...
 * This function updates the paddle boundries so that it hits blocks around it.
 */
bool GameState::DoUpdateToPaddleBoundriesAndCollisions(double dT, bool doAttachedBallCollision) {
	UNUSED_PARAMETER(dT);

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();

    // If the paddle is not in the game then we shouldn't be updating anything
    if (paddle->HasBeenPausedAndRemovedFromGame(this->gameModel->GetPauseState())) {
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
		if (currPiece->IsNoBoundsPieceType()) {
			continue;
		}

		// First check to see if the paddle actually collides with the piece...
		didCollideWithCurrentPiece = paddle->CollisionCheck(currPiece->GetBounds(), doAttachedBallCollision);
		if (didCollideWithCurrentPiece) {
			paddle->UpdateBoundsByPieceCollision(*currPiece, doAttachedBallCollision);
            LevelPiece* resultingPiece = currPiece->CollisionOccurred(this->gameModel, *paddle);
            UNUSED_VARIABLE(resultingPiece);
            assert(resultingPiece == currPiece);

            didCollideWithAnyPiece = true;
		}
	}

    if (!didCollideWithAnyPiece) {
	    // Set the paddle boundries to be the innermost solid blocks on the paddle level of blocks
	    paddle->UpdatePaddleBounds(currentLevel->GetPaddleMinBound(), currentLevel->GetPaddleMaxBound());
    }

	return false;
}