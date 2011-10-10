/**
 * GameBall.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameBall.h"
#include "NormalBallState.h"
#include "InCannonBallState.h"
#include "CannonBlock.h"

const float GameBall::DEFAULT_NORMAL_SPEED = 15.33f;

float GameBall::ZeroSpeed    = 0.0f;
float GameBall::NormalSpeed  = GameBall::DEFAULT_NORMAL_SPEED;

float GameBall::SlowestSpeed          = GameBall::NormalSpeed - 5.5f;
float GameBall::SlowSpeed             = GameBall::NormalSpeed - 2.5f;
float GameBall::FastSpeed             = GameBall::NormalSpeed + 5.0f;
float GameBall::FastestSpeed          = GameBall::NormalSpeed + 9.5f;
float GameBall::FastestSpeedWithBoost = GameBall::NormalSpeed + 14.0f;

// Default radius of the ball - for defining its boundries
const float GameBall::DEFAULT_BALL_RADIUS = 0.5f;

// Minimum angle a ball can be to the normal when it comes off something
const float GameBall::MIN_BALL_ANGLE_IN_DEGS = 8.0f;
const float GameBall::MIN_BALL_ANGLE_IN_RADS = Trig::degreesToRadians(MIN_BALL_ANGLE_IN_DEGS);

// Ball size change related constants
const float GameBall::SECONDS_TO_CHANGE_SIZE = 0.5f;	
const float GameBall::RADIUS_DIFF_PER_SIZE	 = GameBall::DEFAULT_BALL_RADIUS / 3.5f;

// Maximum rotation speed of the ball
const float GameBall::MAX_ROATATION_SPEED	= 70;

// Typical initial velocity for the ball when released from the player paddle
const Vector2D GameBall::STD_INIT_VEL_DIR = Vector2D(0, GameBall::NormalSpeed);

// Temporary speed increase gained by boosting the ball
const float GameBall::BOOST_TEMP_SPD_INCREASE_AMT = 7.0f;

// Acceleration of the ball towards the ground when gravity ball is activated
const float GameBall::GRAVITY_ACCELERATION  = 8.0f;
const float GameBall::BOOST_DECCELERATION   = BOOST_TEMP_SPD_INCREASE_AMT;

GameBall* GameBall::currBallCamBall = NULL;

void GameBall::SetNormalSpeed(float speed) {
    NormalSpeed            = speed;
    SlowestSpeed           = GameBall::NormalSpeed - 5.5f;
    SlowSpeed              = GameBall::NormalSpeed - 2.5f;
    FastSpeed              = GameBall::NormalSpeed + 5.0f;
    FastestSpeed           = GameBall::NormalSpeed + 9.5f;
    FastestSpeedWithBoost  = GameBall::NormalSpeed + 14.0f;
}

GameBall::GameBall() : bounds(Point2D(0.0f, 0.0f), DEFAULT_BALL_RADIUS), currDir(Vector2D(0.0f, 0.0f)), currSpeed(GameBall::ZeroSpeed),
currType(GameBall::NormalBall), rotationInDegs(0.0f, 0.0f, 0.0f), currScaleFactor(1), currSize(NormalSize), ballballCollisionsDisabledTimer(0.0),
lastThingCollidedWith(NULL), zCenterPos(0.0), currState(NULL), timeSinceLastCollision(0.0f), 
boostSpdDecreaseCounter(BOOST_TEMP_SPD_INCREASE_AMT), impulseAmount(0), 
impulseDeceleration(0), impulseSpdDecreaseCounter(0)  {
	this->ResetBallAttributes();
}

GameBall::GameBall(const GameBall& gameBall) : bounds(gameBall.bounds), currDir(gameBall.currDir), currSpeed(gameBall.currSpeed), 
currType(gameBall.currType), currSize(gameBall.currSize), currScaleFactor(gameBall.currScaleFactor), 
rotationInDegs(gameBall.rotationInDegs), ballballCollisionsDisabledTimer(0.0), lastThingCollidedWith(gameBall.lastThingCollidedWith),
zCenterPos(gameBall.zCenterPos), contributingGravityColour(gameBall.contributingGravityColour), 
contributingCrazyColour(gameBall.contributingCrazyColour), contributingIceColour(gameBall.contributingIceColour),
timeSinceLastCollision(gameBall.timeSinceLastCollision), boostSpdDecreaseCounter(gameBall.boostSpdDecreaseCounter),
blockCollisionsDisabled(false),
paddleCollisionsDisabled(false), impulseAmount(gameBall.impulseAmount), 
impulseDeceleration(gameBall.impulseDeceleration), 
impulseSpdDecreaseCounter(gameBall.impulseSpdDecreaseCounter),
currState(NULL) {

	this->SetColour(ColourRGBA(1,1,1,1));
	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);
	this->SetDimensions(gameBall.GetBallSize());

	// Make sure we aren't copying special states across to the new ball...
	if (gameBall.currState->GetBallStateType() == BallState::InCannonBallState) {
		// In the case of the cannon state (i.e., the ball is inside a cannon block) we don't copy that
		// (since only one ball can be inside the cannon block at once), and we just take the previous state
		// and copy that instead...
		InCannonBallState* cannonState = static_cast<InCannonBallState*>(gameBall.currState);
		assert(cannonState != NULL);
		BallState* newState = cannonState->GetPreviousState();
		assert(newState != NULL);
		this->SetBallState(newState->Clone(this), true);
	}
	else {
		this->SetBallState(gameBall.currState->Clone(this), true);
	}
}

GameBall::~GameBall() {
	// Delete the ball state...
	this->SetBallState(NULL, true);

	// If the ball camera ball dies then we better set it to NULL since it will no longer exist after this
	if (this == GameBall::currBallCamBall) {
		GameBall::currBallCamBall = NULL;
	}
}

/**
 * Reset all the special attributes of the ball so that
 * the ball is in its default state. (For example if the ball dies
 * then it should be reset).
 */
void GameBall::ResetBallAttributes() {
    this->TurnOffImpulse();
	this->SetSpeed(NormalSpeed);
	this->currType  = NormalBall;
	this->SetBallSize(NormalSize);
	this->SetDimensions(NormalSize);
	this->lastThingCollidedWith = NULL;
	this->SetColour(ColourRGBA(1, 1, 1, 1));
	this->contributingGravityColour = Colour(1.0f, 1.0f, 1.0f);
	this->contributingCrazyColour   = Colour(1.0f, 1.0f, 1.0f);
	this->contributingIceColour     = Colour(1.0f, 1.0f, 1.0f);
	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);

	this->blockCollisionsDisabled = false;
	this->paddleCollisionsDisabled = false;
	this->ballballCollisionsDisabledTimer = 0.0;
	this->timeSinceLastCollision = 0.0;
    this->boostSpdDecreaseCounter = BOOST_TEMP_SPD_INCREASE_AMT;
    
	// Set the ball state back to its typical state (how it normally interacts with the world)
	this->SetBallState(new NormalBallState(this), true);
}

/**
 * Adds an animation to the ball that fades it in or out based on the
 * given parameter over the given amount of time.
 */
void GameBall::AnimateFade(bool fadeOut, double duration) {
	ColourRGBA finalColour = this->colour;
	if (fadeOut) {
		finalColour[3] = 0.0f;
	}
	else {
		finalColour[3] = 1.0f;
	}

	this->colourAnimation.SetLerp(duration, finalColour);
}

/**
 * Set the dimensions of the ball based on an enumerated ball size given.
 * This will change the scale factor and bounds of the ball.
 */
void GameBall::SetDimensions(GameBall::BallSize size) {
	int diffFromNormalSize = static_cast<int>(size) - static_cast<int>(NormalSize);
	this->SetDimensions((DEFAULT_BALL_RADIUS + diffFromNormalSize * RADIUS_DIFF_PER_SIZE) / DEFAULT_BALL_RADIUS);
}

/**
 * Set the dimensions of the ball based on a scale factor given.
 * This will change the scale factor and bounds of the ball.
 */
void GameBall::SetDimensions(float newScaleFactor) {
	this->currScaleFactor = newScaleFactor;
	assert(this->currScaleFactor > 0.0f);

	float newRadius = this->currScaleFactor * DEFAULT_BALL_RADIUS;
	this->bounds.SetRadius(newRadius);
}

/**
 * Sets what the 'future' ball size will be - this function does not immediately
 * change the size of the ball but will cause the ball to 'grow'/'shrink' to that
 * size as the tick function is called.
 */
void GameBall::SetBallSize(GameBall::BallSize size) {
	// If the current size is already the size being set then just exit
	if (this->currSize == size) {
		return;
	}
	this->currSize = size;
}

/**
 * Set the ball's current state to the given state and remove any previous state.
 */ 
void GameBall::SetBallState(BallState* state, bool deletePrevState) {
	if (this->currState != NULL && deletePrevState) {
		delete this->currState;
	}
	this->currState = state;
}

Onomatoplex::Extremeness GameBall::GetOnomatoplexExtremeness() const {
	Onomatoplex::Extremeness result;

	if ((this->GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
		if (this->GetSpeed() <= GameBall::SlowSpeed) {
			result = Onomatoplex::GOOD;
		}
		else if (this->GetSpeed() <= GameBall::NormalSpeed) {
			result = Onomatoplex::AWESOME;
		}
		else if (this->GetSpeed() <= GameBall::FastSpeed) {
			result =  Onomatoplex::SUPER_AWESOME;
		}
		else {
			result =  Onomatoplex::UBER;
		}	
	}
	else {

		if (this->GetSpeed() <= GameBall::SlowSpeed) {
			result = Onomatoplex::WEAK;
		}
		else if (this->GetSpeed() <= GameBall::NormalSpeed) {
			result = Onomatoplex::GOOD;
		}
		else if (this->GetSpeed() <= GameBall::FastSpeed) {
			result =  Onomatoplex::AWESOME;
		}
		else {
			result =  Onomatoplex::SUPER_AWESOME;
		}
	}

	return result;
}

void GameBall::Tick(double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel) {
	this->currState->Tick(seconds, worldSpaceGravityDir, gameModel);
	this->timeSinceLastCollision += seconds;
}

void GameBall::Animate(double seconds) {
	// Tick any ball-related animations
	this->colourAnimation.Tick(seconds);
}

/**
 * Loads the ball into the given cannon block - this will cause the ball to enter
 * the proper state for this, where it will spin around and eventually fire out
 * of the cannon block and then reenter its previous state.
 */
void GameBall::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);
	assert(this->currState->GetBallStateType() != BallState::InCannonBallState);

	// Make sure that we don't delete the previous state since
	// we cache it in the "InCannonBallState" and it will change back once
	// that state is complete
	this->SetBallState(new InCannonBallState(this, cannonBlock, this->currState), false);
}

void GameBall::ApplyImpulseForce(float impulseAmt, float deceleration) {
    if (this->impulseSpdDecreaseCounter < this->impulseAmount) {
        // Ignore the impulse if there already is one
        return;
    }

    this->SetSpeed(this->GetSpeed() + impulseAmt);
    this->impulseAmount = impulseAmt;
    this->impulseDeceleration = fabs(deceleration); // keep it positive when memoized
    this->impulseSpdDecreaseCounter = 0.0f;
}