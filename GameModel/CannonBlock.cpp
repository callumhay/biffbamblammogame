/**
 * CannonBlock.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    return projectile->IsRocket() || projectile->GetType() == Projectile::PaddleFlameBlastProjectile || 
        projectile->GetType() == Projectile::PaddleIceBlastProjectile;
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

void CannonBlock::DrawWireframe() const {
    LevelPiece::DrawWireframe();

    // Draw an arrow showing the direction the cannon is pointing in if this cannon has the ball camera ball inside it...
    if (GameBall::GetBallCameraBall() == this->loadedBall) {
        CannonBlock::DrawCannonDirIntoWireframe(this->GetCenter(), this->GetCurrentCannonDirection());
    }
}

void CannonBlock::DrawCannonDirIntoWireframe(const Point2D& center, const Vector2D& cannonDir) {

    Vector2D dir = 1.5f * HALF_CANNON_BARREL_LENGTH * cannonDir;

    Point2D arrowBase = center + dir;
    Point2D arrowEnd  = arrowBase + dir;

    Point2D pointerLoc = arrowBase + 0.6f * dir;
    Vector2D perpVec = arrowEnd - arrowBase;
    perpVec.Rotate(90);
    perpVec *= 0.25f;

    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0, 1, 0);

    glBegin(GL_LINES);

    // Arrow shaft
    glVertex2f(arrowBase[0], arrowBase[1]);
    glVertex2f(arrowEnd[0], arrowEnd[1]);

    // Arrow angled pointer lines
    glVertex2f(arrowEnd[0], arrowEnd[1]);
    glVertex2f(pointerLoc[0] + perpVec[0], pointerLoc[1] + perpVec[1]);

    glVertex2f(arrowEnd[0], arrowEnd[1]);
    glVertex2f(pointerLoc[0] - perpVec[0], pointerLoc[1] - perpVec[1]);

    glEnd();

    glPopAttrib();
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
 * Call this when a collision has actually occurred with a projectile and this block.
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
        case Projectile::PaddleIceBlastProjectile:
            // If the cannon isn't already loaded with a projectile then
            // the blast gets captured by the cannon block and shot somewhere else...
            if (!projectile->IsLastThingCollidedWith(this) && !this->GetIsLoaded()) {
                PaddleBlasterProjectile* blastProjectile = static_cast<PaddleBlasterProjectile*>(projectile);
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

        // Rebuild the bounds of this cannon to conform to the new rotation
        this->bounds = this->BuildBounds();
        this->bounds.RotateLinesAndNormals(this->currRotationFromXInDegs, this->center);
    }
}

/**
 * Simulates the results of the RotateAndFire function without having any impact on this block.
 */
bool CannonBlock::TestRotateAndFire(double dT, bool overrideFireRotation, Vector2D& firingDir) const {
    
    if (this->elapsedRotationTime >= this->totalRotationTime) {
        float firingRotationDegsFromX = this->currRotationFromXInDegs;
        if (!this->GetHasRandomRotation() && !overrideFireRotation) {
            firingRotationDegsFromX = this->fixedRotationXInDegs;
        }

        firingDir = CannonBlock::GetDirectionFromRotationInDegs(firingRotationDegsFromX);
        return true;
    }

    float firingRotationDegsFromX = this->currRotationFromXInDegs + static_cast<float>(this->currRotationSpeed * dT);
    if (firingRotationDegsFromX >= 360.0f) {
        firingRotationDegsFromX -= 360.0f;
    }
    else if (firingRotationDegsFromX <= -360.0f) {
        firingRotationDegsFromX += 360.0f;
    }
    assert(firingRotationDegsFromX < 360);
    assert(firingRotationDegsFromX > -360);

    firingDir = CannonBlock::GetDirectionFromRotationInDegs(firingRotationDegsFromX);
    return false;
}

/**
 * Tell the cannon block to continually rotate (ONLY IF IT HAS A BALL INSIDE) and eventually
 * fire based on some random time.
 * Returns: A pair:
 *          - first:  A LevelPiece of whatever piece is now residing in this pieces place after this function call
 *          - second: A boolean of true if the ball has fired, false if still rotating.
 */
std::pair<LevelPiece*, bool> CannonBlock::RotateAndFire(double dT, GameModel* gameModel, bool overrideFireRotation) {
	assert(this->totalRotationTime > 0.0);

    // If we're done rotating then reset the cannon...
	if (this->elapsedRotationTime >= this->totalRotationTime) {
		// In the case of fixed direction firing, we need to make sure the degree angle
		// is set to exactly the firing angle when we fire...
		if (!this->GetHasRandomRotation() && !overrideFireRotation) {
            this->currRotationFromXInDegs = this->fixedRotationXInDegs;
		}

        this->bounds = this->BuildBounds();
        this->bounds.RotateLinesAndNormals(this->currRotationFromXInDegs, this->center);

        // Shoot the ball!
		this->elapsedRotationTime = this->totalRotationTime;
		this->loadedBall = NULL;
		this->loadedProjectile = NULL;
        return std::make_pair(this->CannonWasFired(gameModel), true);
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

    return std::make_pair(this, false);
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