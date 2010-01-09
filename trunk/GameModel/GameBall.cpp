/**
 * GameBall.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameBall.h"

// Default radius of the ball - for defining its boundries
const float GameBall::DEFAULT_BALL_RADIUS = 0.5f;

// Minimum angle a ball can be to the normal when it comes off something
const float GameBall::MIN_BALL_ANGLE_IN_DEGS = 15.0f;
const float GameBall::MIN_BALL_ANGLE_IN_RADS = Trig::degreesToRadians(MIN_BALL_ANGLE_IN_DEGS);
// TODO: have a max angle as well?? ... YES PROBABLY!

// Ball size change related constants
const float GameBall::SECONDS_TO_CHANGE_SIZE = 0.5f;	
const float GameBall::RADIUS_DIFF_PER_SIZE	 = GameBall::DEFAULT_BALL_RADIUS/3;

// Maximum rotation speed of the ball
const float GameBall::MAX_ROATATION_SPEED	= 70;

// Typical initial velocity for the ball when released from the player paddle
const Vector2D GameBall::STD_INIT_VEL_DIR = Vector2D(0, GameBall::NormalSpeed);

GameBall* GameBall::currBallCamBall = NULL;

GameBall::GameBall() : bounds(Point2D(0.0f, 0.0f), DEFAULT_BALL_RADIUS), currDir(Vector2D(0.0f, 0.0f)), currSpeed(GameBall::ZeroSpeed),
currType(GameBall::NormalBall), rotationInDegs(0.0f, 0.0f, 0.0f), currScaleFactor(1), currSize(NormalSize), ballCollisionsDisabledTimer(0.0),
lastPieceCollidedWith(NULL), zCenterPos(0.0) {
	this->ResetBallAttributes();
}

GameBall::GameBall(const GameBall& gameBall) : bounds(gameBall.bounds), currDir(gameBall.currDir), currSpeed(gameBall.currSpeed), 
currType(gameBall.currType), currSize(gameBall.currSize), currScaleFactor(gameBall.currScaleFactor), 
rotationInDegs(gameBall.rotationInDegs), ballCollisionsDisabledTimer(0.0), lastPieceCollidedWith(gameBall.lastPieceCollidedWith),
colour(1, 1, 1, 1), zCenterPos(gameBall.zCenterPos) {

	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);
}

GameBall::~GameBall() {
}

/**
 * Reset all the special attributes of the ball so that
 * the ball is in its default state. (For example if the ball dies
 * then it should be reset).
 */
void GameBall::ResetBallAttributes() {
	this->currSpeed = NormalSpeed;
	this->currType  = NormalBall;
	this->SetBallSize(NormalSize);
	this->SetDimensions(NormalSize);
	this->lastPieceCollidedWith = NULL;
	this->colour = ColourRGBA(1, 1, 1, 1);
	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);
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

Onomatoplex::Extremeness GameBall::GetOnomatoplexExtremeness() const {
	Onomatoplex::Extremeness result;

	if ((this->GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
		switch (this->GetSpeed()) {
			case GameBall::SlowSpeed :
				result = Onomatoplex::AWESOME;
				break;
			case GameBall::NormalSpeed :
				result =  Onomatoplex::SUPER_AWESOME;
				break;
			case GameBall::FastSpeed :
				result =  Onomatoplex::UBER;
				break;
			default :
				assert(false);
				result = Onomatoplex::AWESOME;
				break;
		}	
	}
	else {
		switch (this->GetSpeed()) {
			case GameBall::SlowSpeed :
				result = Onomatoplex::WEAK;
				break;
			case GameBall::NormalSpeed :
				result = Onomatoplex::GOOD;
				break;
			case GameBall::FastSpeed :
				result = Onomatoplex::AWESOME;
				break;
			default :
				assert(false);
				result = Onomatoplex::WEAK;
				break;
		}
	}

	return result;
}

void GameBall::Tick(double seconds) {
	// Update the position of the ball based on its velocity
	Vector2D dDist = (static_cast<float>(seconds) * static_cast<float>(this->currSpeed) *this->currDir);
	this->bounds.SetCenter(this->bounds.Center() + dDist);

	// Update the rotation of the ball
	float dRotSpd = GameBall::MAX_ROATATION_SPEED * static_cast<float>(seconds);
	this->rotationInDegs = this->rotationInDegs + Vector3D(dRotSpd, dRotSpd, dRotSpd);

	// Change the size gradually (lerp based on some constant time) if need be...
	int diffFromNormalSize = static_cast<int>(this->currSize) - static_cast<int>(GameBall::NormalSize);
	float targetScaleFactor = (DEFAULT_BALL_RADIUS + diffFromNormalSize * RADIUS_DIFF_PER_SIZE) / DEFAULT_BALL_RADIUS;
	float scaleFactorDiff = targetScaleFactor - this->currScaleFactor;
	if (scaleFactorDiff != 0.0f) {
		this->SetDimensions(this->currScaleFactor + ((scaleFactorDiff * seconds) / SECONDS_TO_CHANGE_SIZE));
	}

	// Decrement the collision disabled timer if necessary
	if (this->ballCollisionsDisabledTimer >= EPSILON) {
		this->ballCollisionsDisabledTimer -= seconds;
	}
	if (this->ballCollisionsDisabledTimer < EPSILON) {
		this->ballCollisionsDisabledTimer = 0.0;
	}

}

void GameBall::Animate(double seconds) {
	// Tick any ball-related animations
	this->colourAnimation.Tick(seconds);
}