/**
 * CannonBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
#include "PaddleMineProjectile.h"

const float CannonBlock::CANNON_BARREL_LENGTH       = 1.30f;
const float CannonBlock::HALF_CANNON_BARREL_LENGTH	= CannonBlock::CANNON_BARREL_LENGTH	/ 2.0f;
const float CannonBlock::CANNON_BARREL_HEIGHT       = 0.9f;
const float CannonBlock::HALF_CANNON_BARREL_HEIGHT	= CannonBlock::CANNON_BARREL_HEIGHT / 2.0f;

// Rotation will happen for some random period of time in between these values
const double CannonBlock::MIN_ROTATION_TIME_IN_SECS	= 1.0f;
const double CannonBlock::MAX_ROTATION_TIME_IN_SECS = 2.0f;

// Rotation will occur at some random speed in between these values
const float CannonBlock::MIN_ROTATION_SPD_IN_DEGS_PER_SEC = 180.0f;
const float CannonBlock::MAX_ROTATION_SPD_IN_DEGS_PER_SEC = 500.0f;

// When the rotation angle is fixed, we can still spin the cannon a whole bunch of times...
const float CannonBlock::MIN_DEGREES_PER_FIXED_ROTATION = 0.65f;
const float CannonBlock::MAX_DEGREES_PER_FIXED_ROTATION = 1.2f;

CannonBlock::CannonBlock(unsigned int wLoc, unsigned int hLoc, int setRotation) : LevelPiece(wLoc, hLoc), 
rotationInterval(setRotation, setRotation), loadedBall(NULL), loadedProjectile(NULL), currRotationFromXInDegs(0.0f),
currRotationSpeed(0.0f), elapsedRotationTime(0.0), totalRotationTime(0.0) {
    assert(setRotation >= 0 && setRotation <= 359);
}

CannonBlock::CannonBlock(unsigned int wLoc, unsigned int hLoc, 
                         const std::pair<int, int>& rotationInterval) : LevelPiece(wLoc, hLoc), 
rotationInterval(rotationInterval), loadedBall(NULL), loadedProjectile(NULL), currRotationFromXInDegs(0.0f),
currRotationSpeed(0.0f), elapsedRotationTime(0.0), totalRotationTime(0.0) {
    
    assert(rotationInterval.first <= rotationInterval.second);
}

CannonBlock::~CannonBlock() {
    this->loadedBall = NULL;
    this->loadedProjectile = NULL;
}

// Determine whether the given projectile will pass through this block...
bool CannonBlock::ProjectilePassesThrough(const Projectile* projectile) const {
	return projectile->IsRocket();
}

LevelPiece* CannonBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(method);

	// You can't destroy the cannon block...
	return this;
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

    // Disable any boosting for the ball
    gameModel->BallBoostReleasedForBall(ball);

    // EVENT: Ball has entered the cannon block
    GameEventManager::Instance()->ActionBallEnteredCannon(ball, *this);

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

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			// Do nothing, typical laser bullets just extinguish on contact
			break;

		case Projectile::CollateralBlockProjectile:
			// Do nothing, collateral block will be destroyed..
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
			// If the cannon isn't already loaded with a projectile then
			// the rocket gets captured by the cannon block and shot somewhere else...
			if (!projectile->IsLastThingCollidedWith(this) && !this->GetIsLoaded()) {
				PaddleRocketProjectile* rocketProjectile = static_cast<PaddleRocketProjectile*>(projectile);
				this->SetupCannonFireTimeAndDirection();
				rocketProjectile->LoadIntoCannonBlock(this);
				rocketProjectile->SetLastThingCollidedWith(this);
				this->loadedProjectile = rocketProjectile;
			}
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile: {
            MineProjectile* mineProjectile = static_cast<MineProjectile*>(projectile);
            
            if (!projectile->IsLastThingCollidedWith(this)) {
                // If the cannon isn't already loaded with a projectile then
			    // the mine gets captured by the cannon block and shot somewhere else...
			    if (this->GetIsLoaded()) {
                    // Change the trajectory of the mine by making it fall...
                    mineProjectile->SetAsFalling();
                }
                else {
				    this->SetupCannonFireTimeAndDirection();
				    mineProjectile->LoadIntoCannonBlock(this);
				    this->loadedProjectile = mineProjectile;
			    }

                mineProjectile->SetLastThingCollidedWith(this);
            }
            break;
        }

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

    // If we're done rotating then reset the cannon...
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
		this->loadedProjectile = NULL;
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

    float rotationAngleToFireAt = 0;

	// Based on whether the cannon fires randomly or not, set up the time until the cannon will fire
	// and the speed with which it rotate until that time...
	if (this->GetHasRandomRotation()) {
        // Pick the random angle...
        rotationAngleToFireAt = this->rotationInterval.first + Randomizer::GetInstance()->RandomNumZeroToOne() * 
            abs(this->rotationInterval.second - this->rotationInterval.first);
        rotationAngleToFireAt = fmod(rotationAngleToFireAt, 360.0f);
        rotationAngleToFireAt = 90.0f - rotationAngleToFireAt;
	}
	else {
		// We need to actually fire in a proper direction dictated by the degree angle in 'fixedRotation',
		// which measures from the y-axis from 0 to 359 degrees
        rotationAngleToFireAt = fmod(this->GetFixedRotationDegsFromX(), 360.0f);
    }

	// Pick a random rotation time (this will dictate all other values)
	this->totalRotationTime = CannonBlock::MIN_ROTATION_TIME_IN_SECS +
		Randomizer::GetInstance()->RandomNumZeroToOne() * 
        (CannonBlock::MAX_ROTATION_TIME_IN_SECS - CannonBlock::MIN_ROTATION_TIME_IN_SECS);
	assert(this->totalRotationTime > 0.0);

	// And we still rotate at some speed that allows us to get to the proper firing direction at the end of
	// the totalRotationTime, after some randomNumberOfRotations
	
	float noSpinNumDegrees = rotationAngleToFireAt - this->currRotationFromXInDegs;
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