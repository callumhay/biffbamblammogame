/**
 * CannonBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CannonBlock.h"
#include "GameBall.h"
#include "Projectile.h"
#include "GameEventManager.h"
#include "GameLevel.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "GameTransformMgr.h"
#include "PaddleRocketProjectile.h"

const float CannonBlock::CANNON_BARREL_LENGTH				= 1.20f;
const float CannonBlock::HALF_CANNON_BARREL_LENGTH	= CannonBlock::CANNON_BARREL_LENGTH	/ 2.0f;
const float CannonBlock::CANNON_BARREL_HEIGHT				= 0.8f;
const float CannonBlock::HALF_CANNON_BARREL_HEIGHT	= CannonBlock::CANNON_BARREL_HEIGHT / 2.0f;
const int CannonBlock::RANDOM_SET_ROTATION					= -1;

// Rotation will happen for some random period of time in between these values
const double CannonBlock::MIN_ROTATION_TIME_IN_SECS	= 1.0f;
const double CannonBlock::MAX_ROTATION_TIME_IN_SECS = 2.8f;

// Rotation will occur at some random speed in between these values
const float CannonBlock::MIN_ROTATION_SPD_IN_DEGS_PER_SEC	= 150.0f;
const float CannonBlock::MAX_ROTATION_SPD_IN_DEGS_PER_SEC = 450.0f;

// When the rotation angle is fixed, we can still spin the cannon a whole bunch of times...
const float CannonBlock::MIN_DEGREES_PER_FIXED_ROTATION = 0.65f;
const float CannonBlock::MAX_DEGREES_PER_FIXED_ROTATION = 1.2f;

CannonBlock::CannonBlock(unsigned int wLoc, unsigned int hLoc, int setRotation) : LevelPiece(wLoc, hLoc), 
fixedRotation(setRotation), loadedBall(NULL), loadedRocket(NULL), currRotationFromXInDegs(0.0f), currRotationSpeed(0.0f), elapsedRotationTime(0.0), 
totalRotationTime(0.0) {
	assert(setRotation == -1 || (setRotation >= 0 && setRotation <= 359));
}

CannonBlock::~CannonBlock() {
}

// Determine whether the given projectile will pass through this block...
bool CannonBlock::ProjectilePassesThrough(Projectile* projectile) const {
	if (projectile->GetType() == Projectile::PaddleRocketBulletProjectile) {
		return true;
	}
	return false;
}

LevelPiece* CannonBlock::Destroy(GameModel* gameModel) {
	UNUSED_PARAMETER(gameModel);

	// You can't destroy the cannon block...
	return this;
}

// We need to override this in order to make sure it actually checks for a collision
bool CannonBlock::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, 
																 Collision::LineSeg2D& collisionLine, 
																 double& timeSinceCollision) const {

    return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision);
}

bool CannonBlock::CollisionCheck(const Collision::AABB2D& aabb) const {
	// See if there's a collision between this and the given AABB
	return Collision::IsCollision(this->bounds.GenerateAABBFromLines(), aabb);	
}

bool CannonBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

bool CannonBlock::CollisionCheck(const BoundingLines& boundingLines) const {
	return this->bounds.CollisionCheck(boundingLines);
}

/**
 * Updating the bounds on the cannon block is overridden - we only want the ball to go 
 * into the cannon block if it actually hits the cannon.
 */
void CannonBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                               const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                               const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                               const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	UNUSED_PARAMETER(leftNeighbor);
	UNUSED_PARAMETER(bottomNeighbor);
	UNUSED_PARAMETER(rightNeighbor);
	UNUSED_PARAMETER(topNeighbor);
	UNUSED_PARAMETER(topRightNeighbor);
	UNUSED_PARAMETER(topLeftNeighbor);
	UNUSED_PARAMETER(bottomRightNeighbor);
	UNUSED_PARAMETER(bottomLeftNeighbor);

	// We only update the bounds on the cannon block once...
	if (this->bounds.GetNumLines() > 0) {
		return;
	}

	// Clear all the currently existing boundry lines first
    this->SetBounds(this->BuildBounds(), leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, 
        bottomLeftNeighbor);
}

/**
 * Build default position (facing in x axis direction) bounding lines for this cannon block.
 */
BoundingLines CannonBlock::BuildBounds() const {
	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D> boundingNorms;

	// Left boundry of the piece
	Collision::LineSeg2D l1(this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT), 
							this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	Vector2D n1(-1, 0);
	boundingLines.push_back(l1);
	boundingNorms.push_back(n1);

	// Bottom boundry of the piece
	Collision::LineSeg2D l2(this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT),
							 this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	Vector2D n2(0, -1);
	boundingLines.push_back(l2);
	boundingNorms.push_back(n2);

	// Right boundry of the piece
	Collision::LineSeg2D l3(this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT),
							 this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	Vector2D n3(1, 0);
	boundingLines.push_back(l3);
	boundingNorms.push_back(n3);

	// Top boundry of the piece
	Collision::LineSeg2D l4(this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT),
							 this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	Vector2D n4(0, 1);
	boundingLines.push_back(l4);
	boundingNorms.push_back(n4);

	return BoundingLines(boundingLines, boundingNorms);
}

// The cannon block will capture the colliding ball and spin around a bit before shooting it in some random direction
LevelPiece* CannonBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	UNUSED_PARAMETER(gameModel);

	// If the cannon already has something inside it then just leave and have the ball bounce off of it
	// Also if the ball already just collided with this piece, ignore the collision
	if (this->GetIsLoaded() || ball.IsLastPieceCollidedWith(this) || ball.IsLoadedInCannonBlock()) {
		return this;
	}

	this->SetupCannonFireTimeAndDirection();

	// Place the ball in the cannon and make sure we don't continue colliding...
	ball.LoadIntoCannonBlock(this);
	ball.SetLastPieceCollidedWith(this);
	this->loadedBall = &ball;

	return this;
}

/**
 * Call this when a collision has actually occured with a projectile and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* CannonBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	UNUSED_PARAMETER(gameModel);

	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
			// Do nothing, typical laser bullets just extinguish on contact
			break;

		case Projectile::CollateralBlockProjectile:
			// Do nothing, collateral block will be destroyed..
			break;

		case Projectile::PaddleRocketBulletProjectile:
			// If the cannon isn't already loaded with a projectile then
			// the rocket gets captured by the cannon block and shot somewhere else...
			if (!projectile->IsLastLevelPieceCollidedWith(this) && !this->GetIsLoaded()) {
				PaddleRocketProjectile* rocketProjectile = dynamic_cast<PaddleRocketProjectile*>(projectile);
				assert(rocketProjectile != NULL);
				this->SetupCannonFireTimeAndDirection();
				rocketProjectile->LoadIntoCannonBlock(this);
				rocketProjectile->SetLastLevelPieceCollidedWith(this);
				this->loadedRocket = rocketProjectile;
			}
			break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes...
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

/**
 * Tell the cannon block to continually rotate (ONLY IF IT HAS A BALL INSIDE) and eventually
 * fire based on some random time.
 * Returns: true if the ball has fired, false if still rotating.
 */
bool CannonBlock::RotateAndEventuallyFire(double dT) {
	assert(this->totalRotationTime > 0.0);

	if (this->elapsedRotationTime >= this->totalRotationTime) {
		// In the case of fixed direction firing, we need to make sure the degree angle
		// is set to exactly the firing angle when we fire...
		if (!this->GetHasRandomRotation()) {
			this->currRotationFromXInDegs = this->GetFixedRotationDegsFromX();
			this->bounds = this->BuildBounds();
			this->bounds.RotateLinesAndNormals(this->currRotationFromXInDegs, this->center);
		}
		this->elapsedRotationTime = this->totalRotationTime;
		this->loadedBall = NULL;
		this->loadedRocket = NULL;
		return true;
	}

	// Increment the rotation...
	float rotationIncrement = static_cast<float>(this->currRotationSpeed * dT);
	this->currRotationFromXInDegs += rotationIncrement;
	if (this->currRotationFromXInDegs >= 360.0f) {
		this->currRotationFromXInDegs -= 360.0f;
	}
	else if (this->currRotationFromXInDegs <= -360.0f) {
		this->currRotationFromXInDegs += 360.0f;
	}
	assert(this->currRotationFromXInDegs < 360);
	assert(this->currRotationFromXInDegs > -360);

	// Also rotate the the bounding lines appropriately...
	this->bounds.RotateLinesAndNormals(rotationIncrement, this->center);
	
	this->elapsedRotationTime += dT;

	return false;
}

void CannonBlock::SetupCannonFireTimeAndDirection() {
	// Reset the elapsed rotation time
	this->elapsedRotationTime = 0.0;

	// Based on whether the cannon fires randomly or not, set up the time until the cannon will fire
	// and the speed with which it rotate until that time...
	if (this->GetHasRandomRotation()) {
		// Pick a random rotation speed
		this->currRotationSpeed = CannonBlock::MIN_ROTATION_SPD_IN_DEGS_PER_SEC	+ 
			Randomizer::GetInstance()->RandomNumZeroToOne() * (CannonBlock::MAX_ROTATION_SPD_IN_DEGS_PER_SEC - CannonBlock::MIN_ROTATION_SPD_IN_DEGS_PER_SEC);
		// Rotation direction is random as well...
		this->currRotationSpeed *= Randomizer::GetInstance()->RandomNegativeOrPositive();

		// Pick a random rotation time
		this->totalRotationTime = CannonBlock::MIN_ROTATION_TIME_IN_SECS +
			Randomizer::GetInstance()->RandomNumZeroToOne() * (CannonBlock::MAX_ROTATION_TIME_IN_SECS - CannonBlock::MIN_ROTATION_TIME_IN_SECS);
		assert(this->totalRotationTime > 0.0);
	}
	else {
		// We need to actually fire in a proper direction dictated by the degree angle in 'fixedRotation',
		// which measures from the y-axis from 0 to 359 degrees

		// Pick a random rotation time (this will dictate all other values)
		this->totalRotationTime = CannonBlock::MIN_ROTATION_TIME_IN_SECS +
			Randomizer::GetInstance()->RandomNumZeroToOne() * (CannonBlock::MAX_ROTATION_TIME_IN_SECS - CannonBlock::MIN_ROTATION_TIME_IN_SECS);
		assert(this->totalRotationTime > 0.0);

		// And we still rotate at some speed that allows us to get to the proper firing direction at the end of
		// the totalRotationTime, after some randomNumberOfRotations
		
		float noSpinNumDegrees = this->GetFixedRotationDegsFromX() - this->currRotationFromXInDegs;
		if (noSpinNumDegrees >= 360.0f) {
			noSpinNumDegrees -= 360.0f;
		}
		else if (noSpinNumDegrees <= -360.0f) {
			noSpinNumDegrees += 360.0f;
		}
		assert(noSpinNumDegrees < 360);
		assert(noSpinNumDegrees > -360);

		if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
			if (noSpinNumDegrees < 0) {
				noSpinNumDegrees = 360 + noSpinNumDegrees;
			}
			else {
				noSpinNumDegrees = noSpinNumDegrees - 360;
			}
		}
		
		float approxTimePerRotation = MIN_DEGREES_PER_FIXED_ROTATION + Randomizer::GetInstance()->RandomNumZeroToOne() * 
			                           (MAX_DEGREES_PER_FIXED_ROTATION - MIN_DEGREES_PER_FIXED_ROTATION);
		int randomNumberOfRotations = std::max<int>(1, static_cast<int>(ceil(this->totalRotationTime / approxTimePerRotation)));
		
		float totalDegAngleDist = noSpinNumDegrees + (Randomizer::GetInstance()->RandomNegativeOrPositive() * 360.0f * randomNumberOfRotations);
		if (fabs(totalDegAngleDist) < 0.1f) {
			totalDegAngleDist = Randomizer::GetInstance()->RandomNegativeOrPositive() * 360.0f; 
		}

		assert(fabs(totalDegAngleDist) > 0.0f);
		this->currRotationSpeed = totalDegAngleDist / this->totalRotationTime;
	}
}