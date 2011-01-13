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
#include "Projectile.h"
#include "GameModel.h"

#include "../BlammoEngine/Algebra.h"

const double NormalBallState::OMNI_BULLET_WAIT_TIME_IN_SECS = 0.5;
const int NormalBallState::MAX_BULLETS_AT_A_TIME            = 5;

NormalBallState::NormalBallState(GameBall* ball) : BallState(ball), timeSinceLastOmniBullets(0.0) {
}

NormalBallState::~NormalBallState() {
}

// Clone this object and return it as a new object, the caller takes ownership
BallState* NormalBallState::Clone(GameBall* newBall) const {
	return new NormalBallState(newBall);
}

void NormalBallState::Tick(double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel) {
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

	// Crazy ball manipulates the direction and acceleration of the ball... *unless it's attached to a paddle
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

    // Omni Laser ball has a chance of shooting laser bullets in some random direction...
    if ((this->gameBall->GetBallType() & GameBall::OmniLaserBulletBall) == GameBall::OmniLaserBulletBall) {
        this->AttemptFireOfOmniBullets(seconds, gameModel);
    }
}

// Apply the crazy ball item's effect to the velocity of the ball by changing it somewhat randomly
// and strangely to make it difficult to track where the ball will go
void NormalBallState::ApplyCrazyBallVelocityChange(double dT, Vector2D& currVelocity) {
	// If the ball has no velocity then just exit, we're not going to be able to change it...
	if (currVelocity == Vector2D(0.0f, 0.0f)) {
		return;
	}

	static double TIME_TRACKER = 0.0;
	static double NEXT_TIME    = 0.0;
	static const double WAIT_TIME_BETWEEN_COLLISIONS = 1.00;

	TIME_TRACKER += dT;

	// After some accumlated interval of time, do a random velocity change
	if (TIME_TRACKER >= NEXT_TIME && this->gameBall->GetTimeSinceLastCollision() > WAIT_TIME_BETWEEN_COLLISIONS) {
		TIME_TRACKER = 0.0;
		NEXT_TIME = 1.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 2.0;

		float randomRotation  = Randomizer::GetInstance()->RandomNegativeOrPositive() * (45 + Randomizer::GetInstance()->RandomNumZeroToOne() * 115);
		float currVelMag = Vector2D::Magnitude(currVelocity);
		Vector2D newVelocity = Vector2D::Normalize(Vector2D::Rotate(randomRotation, currVelocity));
		this->gameBall->SetVelocity(currVelMag, newVelocity);
	}
}

// When the ball has the omni laser effect we try to fire lasers whenever possible outwards
// in all directions from the ball - this method does that.
void NormalBallState::AttemptFireOfOmniBullets(double dT, GameModel* gameModel) {
    // If we're still waiting to fire the bullets then don't try to fire any
    if (this->timeSinceLastOmniBullets < OMNI_BULLET_WAIT_TIME_IN_SECS) {
        this->timeSinceLastOmniBullets += dT;
        return;
    }

    // Attempt to fire some bullet(s)...
    int numBullets = Randomizer::GetInstance()->RandomUnsignedInt() % (MAX_BULLETS_AT_A_TIME+1);
    for (int i = 0; i < numBullets; i++) {
        // Determine a random angle to fire the bullet at and fire it into the level from the ball
        float randomAngle = (Randomizer::GetInstance()->RandomUnsignedInt() % 360);
        Vector2D randomVector(0, 1);
        randomVector.Rotate(randomAngle);
        randomVector.Normalize();
        
        // Calculate the size of the bullet based on the size of the ball...
        float projectileWidth  = (this->gameBall->GetBounds().Radius() * 0.25f) + 
            (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.75f * this->gameBall->GetBounds().Radius());
        float projectileHeight = 1.75f * projectileWidth; 

		Projectile* newProjectile = Projectile::CreateProjectile(Projectile::PaddleLaserBulletProjectile, 
            this->gameBall->GetCenterPosition2D() + (this->gameBall->GetBounds().Radius() + projectileHeight * 0.5f) * randomVector);
		newProjectile->SetWidth(projectileWidth);
		newProjectile->SetHeight(projectileHeight);
        newProjectile->SetVelocity(randomVector, newProjectile->GetVelocityMagnitude());

		gameModel->AddProjectile(newProjectile);
    }

    this->timeSinceLastOmniBullets = 0.0;
}