
#include "PaddleRocketProjectile.h"
#include "PlayerPaddle.h"

const float PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT = 1.50f;
const float PaddleRocketProjectile::PADDLEROCKET_WIDTH_DEFAULT  = 0.8f;

const Vector2D PaddleRocketProjectile::PADDLEROCKET_VELOCITYDIR	= Vector2D(0, 1);
const Vector2D PaddleRocketProjectile::PADDLEROCKET_RIGHTDIR		= Vector2D(1, 0);

const float PaddleRocketProjectile::ACCELERATION_MAG					 = 2.0f;
const float PaddleRocketProjectile::ROTATION_ACCELERATION_MAG = 60.0f;

const float PaddleRocketProjectile::MAX_VELOCITY_MAG					= 20.0f;
const float PaddleRocketProjectile::MAX_ROTATION_VELOCITY_MAG	= 400.0f;

PaddleRocketProjectile::PaddleRocketProjectile(const Point2D& spawnLoc, float width, float height) :
Projectile(Projectile::PaddleRocketBulletProjectile, spawnLoc, width, height) {

	// TODO: Add acceleration and start with 0 initial velocity...
  // do this for rotation as well...
	this->velocityDir   = PADDLEROCKET_VELOCITYDIR;
	this->rightVec      = PADDLEROCKET_RIGHTDIR;
	this->velocityMag   = 0.0f;
	this->currYRotation = 0.0f;
	this->currYRotationSpd = 0.0f;
}

PaddleRocketProjectile::~PaddleRocketProjectile() {
}

/**
 * Executed every game tick, updates the velocities, 
 * position and rotation of the rocket projectile.
 */
void PaddleRocketProjectile::Tick(double seconds) {
	// Update the rocket's velocity and position
	float dA = seconds * PaddleRocketProjectile::ACCELERATION_MAG;
	this->velocityMag = std::min<float>(PaddleRocketProjectile::MAX_VELOCITY_MAG ,this->velocityMag + dA);
	float dV = seconds * this->velocityMag;
	this->SetPosition(this->GetPosition() + dV * this->velocityDir);

	// Update the rocket's rotation
	dA = seconds * PaddleRocketProjectile::ROTATION_ACCELERATION_MAG;
	this->currYRotationSpd = std::min<float>(PaddleRocketProjectile::MAX_ROTATION_VELOCITY_MAG, this->currYRotationSpd + dA);
	dV = static_cast<float>(seconds * this->currYRotationSpd);
	this->currYRotation += dV;
}

BoundingLines PaddleRocketProjectile::BuildBoundingLines() const {
	const Vector2D UP_DIR			= this->GetVelocityDirection();
	const Vector2D RIGHT_DIR	= this->GetRightVectorDirection();

	Point2D topRight = this->GetPosition() + this->GetHalfHeight()*UP_DIR + this->GetHalfWidth()*RIGHT_DIR;
	Point2D bottomRight = topRight - this->GetHeight()*UP_DIR;
	Point2D topLeft = topRight - this->GetWidth()*RIGHT_DIR;
	Point2D bottomLeft = topLeft - this->GetHeight()*UP_DIR;

	std::vector<Collision::LineSeg2D> sideBounds;
	sideBounds.reserve(2);
	sideBounds.push_back(Collision::LineSeg2D(topLeft, bottomLeft));
	sideBounds.push_back(Collision::LineSeg2D(topRight, bottomRight));
	std::vector<Vector2D> normBounds;
	normBounds.resize(2);

	return BoundingLines(sideBounds, normBounds);
}