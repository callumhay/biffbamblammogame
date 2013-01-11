/**
 * BallDeathState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "BallDeathState.h"
#include "GameOverState.h"
#include "BallOnPaddleState.h"
#include "GameModel.h"
#include "GameBall.h"
#include "GameEventManager.h"

const double BallDeathState::SPIRAL_ANIMATION_TIME_TOTAL    = 1.25;	// Total time of the spiral animation in seconds
const double BallDeathState::EXPLOSION_ANIMATION_TIME_TOTAL	= 0.75;	// Total time of the explosion animation in seconds

BallDeathState::BallDeathState(GameBall* lastBallToBeAlive, GameModel* gm) : 
GameState(gm), spiralRadius(0.0f), lastBallToBeAlive(lastBallToBeAlive), timeElapsed(0.0),
currAnimationState(BallDeathState::SpiralingToDeath) {
	assert(lastBallToBeAlive != NULL);

	// EVENT: Last ball is about to die
	GameEventManager::Instance()->ActionLastBallAboutToDie(*this->lastBallToBeAlive);

	this->initialBallPosition    = this->lastBallToBeAlive->GetBounds().Center();
	this->initialBallVelocityDir = this->lastBallToBeAlive->GetDirection();
	this->initialBallVelocityMag = this->lastBallToBeAlive->GetSpeed();

	// Disable any effects currently on the ball...
	this->lastBallToBeAlive->RemoveAllBallTypes();

	// Completely pause the paddle since the ball death is a non-controllable happening
	this->gameModel->SetPause(GameModel::PausePaddle);
	// Move the camera to follow the ball as it dies
	this->gameModel->GetTransformInfo()->SetBallDeathCamera(true);
	// Make sure the ball is visible
	this->lastBallToBeAlive->SetAlpha(1.0f);

	// Setup any animations for the ball
	this->spiralAnimation = AnimationLerp<float>(&this->spiralRadius);
	this->spiralAnimation.SetRepeat(false);
	this->spiralAnimation.SetLerp(0.0, BallDeathState::SPIRAL_ANIMATION_TIME_TOTAL, 0.5, 3.0);

	// Clear all the falling items out of the level - makes sure all their sounds/effects
	// are eliminated so the player only sees and hears his/her death animation
	this->gameModel->ClearLiveItems();
}

BallDeathState::~BallDeathState() {
	// Place the camera back in its default position
	this->gameModel->GetTransformInfo()->SetBallDeathCamera(false);	
	
	// If we are exiting being in play then clear all items and timers
	this->gameModel->ClearBeams();
	this->gameModel->ClearActiveTimers();
	
    // Remove any rockets attached to the paddle
    this->gameModel->GetPlayerPaddle()->RemovePaddleType(PlayerPaddle::RocketPaddle);

    // Only clear projectiles if it's game over
    if (this->gameModel->IsGameOver()) {
        this->gameModel->ClearProjectiles();
    }

	// Allow the player to control the paddle again
	this->gameModel->UnsetPause(GameModel::PausePaddle);
}

/**
 * Follow the ball down into the abyss and then have it spiral and explodinate.
 */
void BallDeathState::Tick(double seconds) {
	// Animate the ball based on its current animation state... starts to spiral and shake then explode...
	switch (this->currAnimationState) {

		case BallDeathState::SpiralingToDeath:
			this->ExecuteSpiralingToDeathState(seconds);
			return;

		case BallDeathState::Exploding:
			// We need to just wait for the explosion...
			this->timeElapsed += seconds;
			if (this->timeElapsed >= BallDeathState::EXPLOSION_ANIMATION_TIME_TOTAL) {
				this->currAnimationState = BallDeathState::Dead;
			}
			return;

		case BallDeathState::Dead: {
				bool stateChanged = false;
				this->gameModel->BallDied(this->lastBallToBeAlive, stateChanged);
				assert(stateChanged);
                // Place the ball off camera for the time being
                this->lastBallToBeAlive->SetCenterPosition(Point2D(99999,99999));
			}
			return;
		
		default:
			assert(false);
			break;
	}
}

/**
 * Private helper function for executing the spiral to death animation state.
 */
void BallDeathState::ExecuteSpiralingToDeathState(double dT) {
	// Grab the distance along the circle of the spiral
	float circleDistTravelled = this->initialBallVelocityMag * this->timeElapsed;

	// Figure out which way to make it spiral
	float spiralXMultiplier	= -NumberFuncs::SignOf(this->initialBallVelocityDir[0]);
	float modifiedRadius    = spiralXMultiplier * this->spiralRadius;

	// To make the ball spiral we use a parameteric equation:
	Vector2D ballTranslation(modifiedRadius * cos(circleDistTravelled) - modifiedRadius, 
        (-this->spiralRadius * sin(circleDistTravelled)));

	Point2D newBallPos = this->initialBallPosition + ballTranslation + 
        (0.85f * this->initialBallVelocityMag * this->timeElapsed * this->initialBallVelocityDir);
	this->lastBallToBeAlive->SetCenterPosition(newBallPos);
	
	// Check to see if the spiral animation is completed... if it has then we move onto the next state
	bool isFinished = this->spiralAnimation.Tick(dT);
	if (isFinished) {
		this->timeElapsed = 0.0;
		this->currAnimationState = BallDeathState::Exploding;
		
		// EVENT: Ball just exploded...
		GameEventManager::Instance()->ActionLastBallExploded(*this->lastBallToBeAlive);

		// Make the ball disappear as it explodes and also (sortof) stop moving
		this->lastBallToBeAlive->SetAlpha(0.0f);
        this->lastBallToBeAlive->SetSpeed(GameBall::GetSlowestSpeed());
	}
	else {
		timeElapsed += dT;
	}
}