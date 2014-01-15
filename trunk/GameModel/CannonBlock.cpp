/**
 * CannonBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2013
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
#include "PaddleFlameBlasterProjectile.h"

const float CannonBlock::CANNON_BARREL_LENGTH       = 1.30f;
const float CannonBlock::HALF_CANNON_BARREL_LENGTH	= CannonBlock::CANNON_BARREL_LENGTH	/ 2.0f;
const float CannonBlock::CANNON_BARREL_HEIGHT       = 0.9f;
const float CannonBlock::HALF_CANNON_BARREL_HEIGHT	= CannonBlock::CANNON_BARREL_HEIGHT / 2.0f;

// Rotation will happen for some random period of time in between these values
const double CannonBlock::MIN_ROTATION_TIME_IN_SECS	        = 1.0;
const double CannonBlock::MAX_ROTATION_TIME_IN_SECS         = 2.0;
const double CannonBlock::BALL_CAMERA_ROTATION_TIME_IN_SECS = 8.0;

// Rotation will occur at some random speed in between these values
const float CannonBlock::MIN_ROTATION_SPD_IN_DEGS_PER_SEC = 360.0f;
const float CannonBlock::MAX_ROTATION_SPD_IN_DEGS_PER_SEC = 500.0f;
const float CannonBlock::BALL_CAMERA_ROTATION_SPD_IN_DEGS_PER_SEC = MIN_ROTATION_SPD_IN_DEGS_PER_SEC / 2.5f;

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
    if (!this->GetHasRandomRotation()) {
        this->fixedRotationXInDegs = this->GetFixedRotationDegsFromX();
    }
}

CannonBlock::~CannonBlock() {
    this->loadedBall = NULL;
    this->loadedProjectile = NULL;
}

// Determine whether the given projectile will pass through this block...
bool CannonBlock::ProjectilePassesThrough(const Projectile* projectile) const {
    return projectile->IsRocket() || projectile->GetType() == Projectile::PaddleFlameBlastProjectile;
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

	// Clear all the currently existing boundary lines first
    this->SetBounds(this->BuildBounds(), leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, 
        bottomLeftNeighbor);
}

/**
 * Build default position (facing in x axis direction) bounding lines for this cannon block.
 */
BoundingLines CannonBlock::BuildBounds() const {

	// Set the bounding lines for a rectangular block
    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D  boundingNorms[MAX_NUM_LINES];

	// Left boundary of the piece
    boundingLines[0] = Collision::LineSeg2D(this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT), 
        this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	boundingNorms[0] = Vector2D(-1, 0);

	// Bottom boundary of the piece
    boundingLines[1] = Collision::LineSeg2D(this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT),
        this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	boundingNorms[1] = Vector2D(0, -1);

	// Right boundary of the piece
    boundingLines[2] = Collision::LineSeg2D(this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT),
        this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	boundingNorms[2] = Vector2D(1, 0);

	// Top boundary of the piece
    boundingLines[3] = Collision::LineSeg2D(this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT),
        this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	boundingNorms[3] = Vector2D(0, 1);

	return BoundingLines(MAX_NUM_LINES, boundingLines, boundingNorms);
}

// The cannon block will capture the colliding ball and spin around a bit before shooting it in some random direction
LevelPiece* CannonBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	UNUSED_PARAMETER(gameModel);

	// If the cannon already has something inside it then just leave and have the ball bounce off of it
	// Also if the ball already just collided with this piece, ignore the collision
	if (this->GetIsLoaded() || ball.IsLastPieceCollidedWith(this) || ball.IsLoadedInCannonBlock()) {
		return this;
	}

    // Special case: the ball is in ball camera mode
    if (ball.HasBallCameraActive()) {
        this->InitBallCameraInCannonValues(true, ball);
    }
    else {
	    this->SetupRandomCannonFireTimeAndDirection();
    }

	// Place the ball in the cannon and make sure we don't continue colliding...
	ball.LoadIntoCannonBlock(this);
	ball.SetLastPieceCollidedWith(this);
	this->loadedBall = &ball;

    // Disable any boosting for the ball
    gameModel->BallBoostReleasedForBall(ball);

    // EVENT: Ball has entered the cannon block
    GameEventManager::Instance()->ActionBallEnteredCannon(ball, *this,
        ball.CanShootBallCamOutOfCannon(*this, *gameModel->GetCurrentLevel()));

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
        case Projectile::BossLightningBoltBulletProjectile:
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
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
			// If the cannon isn't already loaded with a projectile then
			// the rocket gets captured by the cannon block and shot somewhere else...
			if (!projectile->IsLastThingCollidedWith(this) && !this->GetIsLoaded()) {
				RocketProjectile* rocketProjectile = static_cast<RocketProjectile*>(projectile);
				this->SetupRandomCannonFireTimeAndDirection();
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
				    this->SetupRandomCannonFireTimeAndDirection();
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

        case Projectile::PaddleFlameBlastProjectile:
            // If the cannon isn't already loaded with a projectile then
            // the blast gets captured by the cannon block and shot somewhere else...
            if (!projectile->IsLastThingCollidedWith(this) && !this->GetIsLoaded()) {
                PaddleFlameBlasterProjectile* blastProjectile = static_cast<PaddleFlameBlasterProjectile*>(projectile);
                this->SetupRandomCannonFireTimeAndDirection();
                blastProjectile->LoadIntoCannonBlock(this);
                blastProjectile->SetLastThingCollidedWith(this);
                this->loadedProjectile = blastProjectile;
            }
            break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

void CannonBlock::InitBallCameraInCannonValues(bool changeRotation, const GameBall& ball) {
    // The player gains control of the cannon and will be able to fire it in any direction,
    // to avoid disorienting the player, we re-orient the cannon to face downwards
    this->totalRotationTime   = CannonBlock::BALL_CAMERA_ROTATION_TIME_IN_SECS;
    this->elapsedRotationTime = 0.0;
    this->currRotationSpeed   = 0.0f;

    if (changeRotation) {
        // Figure out the direction to point in based on the direction the ball was coming from...
        const Vector2D& ballDir = ball.GetDirection();
        if (ballDir.IsZero()) {
            this->currRotationFromXInDegs = -90.0f;
        }
        else {
            this->currRotationFromXInDegs = Trig::radiansToDegrees(atan2f(-ballDir[1], -ballDir[0]));
        }
    }
}

/**
 * Tell the cannon block to continually rotate (ONLY IF IT HAS A BALL INSIDE) and eventually
 * fire based on some random time.
 * Returns: true if the ball has fired, false if still rotating.
 */
bool CannonBlock::RotateAndEventuallyFire(double dT, bool overrideFireRotation) {
	assert(this->totalRotationTime > 0.0);

    // If we're done rotating then reset the cannon...
	if (this->elapsedRotationTime >= this->totalRotationTime) {
		// In the case of fixed direction firing, we need to make sure the degree angle
		// is set to exactly the firing angle when we fire...
		if (!this->GetHasRandomRotation() && !overrideFireRotation) {
            this->currRotationFromXInDegs = this->fixedRotationXInDegs;
			this->bounds = this->BuildBounds();
			this->bounds.RotateLinesAndNormals(this->currRotationFromXInDegs, this->center);
		}

        // Shoot the ball!
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

void CannonBlock::SetupRandomCannonFireTimeAndDirection() {
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
		// which measures from the y-axis in [0,360) degrees
        float fixedRotatationFromX = this->GetFixedRotationDegsFromX();
        rotationAngleToFireAt = fmod(fixedRotatationFromX, 360.0f);
        this->fixedRotationXInDegs = fixedRotatationFromX;
    }

	// Pick a random rotation time (this will dictate all other values)
	this->totalRotationTime = CannonBlock::MIN_ROTATION_TIME_IN_SECS +
		Randomizer::GetInstance()->RandomNumZeroToOne() * 
        (CannonBlock::MAX_ROTATION_TIME_IN_SECS - CannonBlock::MIN_ROTATION_TIME_IN_SECS);
	assert(this->totalRotationTime > CannonBlock::MIN_ROTATION_TIME_IN_SECS);

	// And we still rotate at some speed that allows us to get to the proper firing direction at the end of
	// the totalRotationTime, after some randomNumberOfRotations
	
	float diffNumDegrees = rotationAngleToFireAt - this->currRotationFromXInDegs;
	if (diffNumDegrees >= 360.0f) {
		diffNumDegrees -= 360.0f;
	}
	else if (diffNumDegrees <= -360.0f) {
		diffNumDegrees += 360.0f;
	}
	assert(diffNumDegrees < 360);
	assert(diffNumDegrees > -360);
	
	float approxTimePerRotation = MIN_DEGREES_PER_FIXED_ROTATION + Randomizer::GetInstance()->RandomNumZeroToOne() * 
		                          (MAX_DEGREES_PER_FIXED_ROTATION - MIN_DEGREES_PER_FIXED_ROTATION);
	int randomNumberOfRotations = std::max<int>(1, static_cast<int>(ceil(this->totalRotationTime / approxTimePerRotation)));
	
	float totalDegAngleDist = diffNumDegrees + (Randomizer::GetInstance()->RandomNegativeOrPositive() * 360.0f * randomNumberOfRotations);
	if (fabs(totalDegAngleDist) < 90.0f) {
        totalDegAngleDist = totalDegAngleDist + NumberFuncs::SignOf(totalDegAngleDist) * 360.0f; 
	}

	assert(fabs(totalDegAngleDist) > 0.0f);
	this->currRotationSpeed = totalDegAngleDist / this->totalRotationTime;
}