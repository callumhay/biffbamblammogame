/**
 * NormalBallState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */


#include "NormalBallState.h"
#include "../BlammoEngine/Algebra.h"

NormalBallState::NormalBallState(GameBall* ball) : BallState(ball) {
}

NormalBallState::~NormalBallState() {
}

// Clone this object and return it as a new object, the caller takes ownership
BallState* NormalBallState::Clone(GameBall* newBall) const {
	return new NormalBallState(newBall);
}

void NormalBallState::Tick(double seconds, const Vector2D& worldSpaceGravityDir) {
	// Update the position of the ball based on its velocity (and if applicable, acceleration)
	Vector2D currVelocity;

	if ((this->gameBall->GetBallType() & GameBall::GraviBall) == GameBall::GraviBall) {
		// Keep track of the last gravity speed calculated based on the gravity pulling the ball down
		// The gravitySpeed variable is basically a mirror of currSpeed but it tracks 
		// the speed of the ball as it gets pulled down by gravity - currSpeed does not do this
		Vector2D currGravityVelocity = (this->gameBall->gravitySpeed * this->gameBall->currDir);
		
		// If the ball ever exceeds the current ball speed projected on the gravity vector then slow it down along that axis
		// Figure out the projected gravity vector and speed
		Vector2D nGravityDir = Vector2D::Normalize(worldSpaceGravityDir);
		float projectedGravitySpeed  = Vector2D::Dot(currGravityVelocity, nGravityDir);
		Vector2D projectedGravityVec = projectedGravitySpeed * nGravityDir;

		if (projectedGravitySpeed > this->gameBall->currSpeed) {
			currVelocity = currGravityVelocity - projectedGravityVec + (this->gameBall->currSpeed * nGravityDir);
		}
		else {
			currVelocity = currGravityVelocity + (seconds * GameBall::GRAVITY_ACCELERATION * nGravityDir);
		}

		float newVelocityMag    = Vector2D::Magnitude(currVelocity);
		Vector2D newVelocityDir = (currVelocity / newVelocityMag);
		this->gameBall->SetGravityVelocity(newVelocityMag, newVelocityDir);
	}
	else {
		currVelocity = this->gameBall->currSpeed * this->gameBall->currDir;
		this->gameBall->gravitySpeed = this->gameBall->currSpeed;
	}

	// Crazy ball manipulates the direction and acceleration of the ball...
	if ((this->gameBall->GetBallType() & GameBall::CrazyBall) == GameBall::CrazyBall) {
		this->ApplyCrazyBallVelocityChange(seconds, currVelocity);
	}

	Vector2D dDist = (static_cast<float>(seconds) * currVelocity);
	this->gameBall->bounds.SetCenter(this->gameBall->bounds.Center() + dDist);

	// Update the rotation of the ball
	float dRotSpd = GameBall::MAX_ROATATION_SPEED * static_cast<float>(seconds);
	this->gameBall->rotationInDegs = this->gameBall->rotationInDegs + Vector3D(dRotSpd, dRotSpd, dRotSpd);

	// Change the size gradually (lerp based on some constant time) if need be...
	int diffFromNormalSize = static_cast<int>(this->gameBall->currSize) - static_cast<int>(GameBall::NormalSize);
	float targetScaleFactor = (GameBall::DEFAULT_BALL_RADIUS + diffFromNormalSize * GameBall::RADIUS_DIFF_PER_SIZE) / GameBall::DEFAULT_BALL_RADIUS;
	float scaleFactorDiff = targetScaleFactor - this->gameBall->currScaleFactor;
	if (scaleFactorDiff != 0.0f) {
		this->gameBall->SetDimensions(this->gameBall->currScaleFactor + ((scaleFactorDiff * seconds) / GameBall::SECONDS_TO_CHANGE_SIZE));
	}

	// Decrement the collision disabled timer if necessary
	if (this->gameBall->ballballCollisionsDisabledTimer >= EPSILON) {
		this->gameBall->ballballCollisionsDisabledTimer -= seconds;
	}
	else {
		this->gameBall->ballballCollisionsDisabledTimer = 0.0;
	}
}

// Apply the crazy ball item's effect to the velocity of the ball by changing it somewhat randomly
// and strangely to make it difficult to track where the ball will go
void NormalBallState::ApplyCrazyBallVelocityChange(double dT, Vector2D& currVelocity) {
	static double TIME_TRACKER = 0.0;
	static double NEXT_TIME    = 0.0;
	static const double WAIT_TIME_BETWEEN_COLLISIONS = 0.75;

	TIME_TRACKER += dT;

	// After some accumlated interval of time, do a random velocity change
	if (TIME_TRACKER >= NEXT_TIME && this->gameBall->GetTimeSinceLastCollision() > WAIT_TIME_BETWEEN_COLLISIONS) {
		TIME_TRACKER = 0.0;
		NEXT_TIME = 1.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 2.0;

		float randomRotation  = Randomizer::GetInstance()->RandomNegativeOrPositive() * (45 + Randomizer::GetInstance()->RandomNumZeroToOne() * 135);
		float currVelMag = Vector2D::Magnitude(currVelocity);
		Vector2D newVelocity = Vector2D::Normalize(Vector2D::Rotate(randomRotation, currVelocity));
		this->gameBall->SetVelocity(currVelMag, newVelocity);
	}
}