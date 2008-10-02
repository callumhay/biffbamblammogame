#include "BallInPlayState.h"
#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "GameItem.h"
#include "GameItemFactory.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"
#include "../Utils/Point.h"

BallInPlayState::BallInPlayState(GameModel* gm) : GameState(gm) {
}

BallInPlayState::~BallInPlayState() {
	// Destory any left-over game items
	for(std::vector<GameItem*>::iterator iter = this->currLiveItems.begin(); iter < this->currLiveItems.end(); iter++) {
		GameItem* currItem = *iter;
		delete currItem;
		currItem = NULL;
	}
	this->currLiveItems.clear();
}

/**
 * Since the ball is now in play this function will be doing A LOT, including:
 * - updating the ball's location and checking for collisions/death
 * - ... TBA
 */
void BallInPlayState::Tick(double seconds) {
	// The paddle can move while the ball is in play, of course
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->Tick(seconds);

	// Update the ball's current position
	GameBall* ball = this->gameModel->GetGameBall();
	ball->Tick(seconds);

	// Update any items that may have been created
	std::vector<unsigned int> removeIndices;
	for(std::vector<GameItem*>::iterator itemIter = this->currLiveItems.begin(); itemIter < this->currLiveItems.end();) {
		GameItem *currItem = *itemIter;
		currItem->Tick(seconds);
		
		// Check to see if any have left the playing area - if so destroy them
		if (currItem->GetCenter()[1] <= GameLevel::Y_COORD_OF_DEATH) {
			delete currItem;
			currItem = NULL;
			this->currLiveItems.erase(itemIter);
		}
		else {
			itemIter++;
		}
	}

	// Check for ball collisions with the pieces of the level and the paddle
	const GameLevel *currLevel = this->gameModel->GetCurrentLevel();
	const std::vector<std::vector<LevelPiece*>> &levelPieces = currLevel->GetCurrentLevelLayout();
	
	// First check for ball-paddle collision
	Vector2D n;
	float d;
	bool didCollide = paddle->CollisionCheck(ball->GetBounds(), n, d);

	if (didCollide) {
		// Do ball-paddle collision
		this->DoBallCollision(*ball, n, d);
		// Tell the model that a ball collision occurred with the paddle
		this->gameModel->BallPaddleCollisionOccurred();
	}
	else {	// No paddle collision with the ball

		// Check for death
		if (ball->GetBounds().Center()[1] <= GameLevel::Y_COORD_OF_DEATH) {
			this->gameModel->PlayerDied();
			return;
		}

		// Check for block collisions
		for (size_t h = 0; h < levelPieces.size(); h++) {
			for (size_t w = 0; w < levelPieces[h].size(); w++) {
				
				LevelPiece *currPiece = levelPieces[h][w];
				didCollide = currPiece->CollisionCheck(ball->GetBounds(), n, d);
				if (didCollide) {
					this->DoBallCollision(*ball, n, d);
					
					// Figure out whether we want to drop an item...
					if (currPiece->CanDropItem()) {
						// Now we will drop an item based on a combination of probablility
						// and the number of consecutive blocks that have been hit on this set of ball bounces

						int numBlocksAlreadyHit = this->gameModel->GetNumConsecutiveBlocksHit() + 1;
						double itemDropProb = min(1.0, GameConstants::PROB_OF_ITEM_DROP * numBlocksAlreadyHit);
						double randomNum = Randomizer::RandomNumZeroToOne();

						if (randomNum <= itemDropProb) {
							// Drop an item - create a random item and add it to the list...
							// TODO: this->currLiveItems.push_back(GameItemFactory::CreateRandomItem());
						}
					}

					// Tell the model that a ball collision occurred with currPiece
					this->gameModel->BallPieceCollisionOccurred(currPiece);
					break;
				}
			}
			
			if (didCollide) { 
				// Now, get out of the loop in case of a collision
				break;
			}
		}
	}

	// Check for item-paddle collisions
	this->DoItemCollision();
}

/**
 * Private helper function for checking to see if an item collided with 
 * the player paddle and carrying out the necessary activity if one did.
 */
void BallInPlayState::DoItemCollision() {
	// Check to see if the paddle hit any items, if so, activate those items
	for(std::vector<GameItem*>::iterator itemIter = this->currLiveItems.begin(); itemIter < this->currLiveItems.end();) {
		GameItem *currItem = *itemIter;
		
		if (currItem->CollisionCheck(*this->gameModel->GetPlayerPaddle())) {
			// There was a collision with the item and the player paddle: activate the item
			// and then delete it.
			currItem->Activate();
			delete currItem;
			currItem = NULL;
			this->currLiveItems.erase(itemIter);
		}
		else {
			itemIter++;
		}
	}
}

// n must be normalized
// d is the distance from the center of the ball to the line that was collided with
// when d is negative the ball is inside the line, when positive it is outside
void BallInPlayState::DoBallCollision(GameBall& b, const Vector2D& n, float d) {
	// Position the ball so that it is against the collision line, exactly
	if (fabs(d) > EPSILON) {
		int signDist = NumberFuncs::SignOf(d);
		if (signDist == -1) {
			// The ball is on the other side of the collision line, fix its position so that
			// it is exactly against the line
			b.SetCenterPosition(b.GetBounds().Center() + (-d + b.GetBounds().Radius()) * n);
		}
		else {
			// The ball is still on the out facing normal side of the line, but fix its position so that
			// it is exactly against the line
			b.SetCenterPosition(b.GetBounds().Center() + (b.GetBounds().Radius() - d) * n);
		}
	}

	// Figure out the reflection vector and speed
	Vector2D reflVecHat					= Vector2D::Normalize(Reflect(b.GetVelocity(), n));
	GameBall::BallSpeed reflSpd	= b.GetSpeed();
	
	// This should NEVER happen
	assert(reflSpd != GameBall::ZeroSpeed);
	if (reflSpd == GameBall::ZeroSpeed) {
		return;
	}

	// Figure out the angle between the normal and the reflection...
	float angleOfReflInDegs = Trig::radiansToDegrees(acosf(Vector2D::Dot(n, reflVecHat)));
	float diffAngleInDegs		= GameBall::MIN_BALL_ANGLE_IN_DEGS - angleOfReflInDegs;

	// Make sure the reflection is big enough to not cause 
	// an annoying slow down in the game
	if (diffAngleInDegs > EPSILON) {

		// We need to figure out which way to rotate the velocity
		// to ensure it's at least the MIN_BALL_ANGLE_IN_RADS
		Vector2D newReflVelHat = Rotate(diffAngleInDegs, reflVecHat);

		// Check to see if it's still the case, if it is then we rotated the wrong way
		float newAngleInDegs =  Trig::radiansToDegrees(acosf(Vector2D::Dot(n, newReflVelHat)));
		if (newAngleInDegs < GameBall::MIN_BALL_ANGLE_IN_DEGS) {
			newReflVelHat = Rotate(-diffAngleInDegs, reflVecHat);
		}
		
		reflVecHat = newReflVelHat;
	}

	// Reflect the ball off the normal
	b.SetVelocity(reflSpd, reflVecHat);
}