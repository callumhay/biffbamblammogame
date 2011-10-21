/**
 * Projectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Projectile.h"
#include "LevelPiece.h"
#include "GameEventManager.h"
#include "PaddleLaserProjectile.h"
#include "PaddleRocketProjectile.h"
#include "BallLaserProjectile.h"
#include "GameModel.h"

// Projectile ====================================================================================================================

Projectile::Projectile(const Point2D& spawnLoc, float width, float height) : 
position(spawnLoc), lastThingCollidedWith(NULL), currWidth(width), currHeight(height) {
}

Projectile::Projectile(const Projectile& copy) : position(copy.position),
lastThingCollidedWith(copy.lastThingCollidedWith), currWidth(copy.currWidth), currHeight(copy.currHeight),
velocityDir(copy.velocityDir), velocityMag(copy.velocityMag), rightVec(copy.rightVec) {
}

Projectile::~Projectile() {
}

void Projectile::AugmentDirectionOnPaddleMagnet(double seconds, const GameModel& model, float degreesChangePerSec) {
    // If the paddle has the magnet item active and the projectile is moving towards the paddle, then we need to
    // modify the velocity to make it move towards the paddle...
    const PlayerPaddle* paddle = model.GetPlayerPaddle();
    if ((paddle->GetPaddleType() & PlayerPaddle::MagnetPaddle) != PlayerPaddle::MagnetPaddle) {
        return;
    }
        
    // Also, don't keep augmenting the direction if the projectile has passed the paddle going out of bounds...
    if (this->position[1] < paddle->GetCenterPosition()[1]) {
        return;
    }

    // Figure out the vector from the projectile to the paddle...
    Vector2D projectileToPaddleVec = paddle->GetCenterPosition() - this->position;
    projectileToPaddleVec.Normalize();

    if (Vector2D::Dot(this->velocityDir, projectileToPaddleVec) <= 0) {
        // The projectile is not within a 90 degree angle of the vector from the projectile to the paddle
        return;
    }
    
    float currRotAngle   = atan2(this->velocityDir[1], this->velocityDir[0]);
    
    // Don't let the angle get to sharp or the projectile will collide with the paddle no matter what
    if (currRotAngle > Trig::degreesToRadians(-30) || currRotAngle < Trig::degreesToRadians(-150)) {
        return;
    }

    float targetRotAngle = atan2(projectileToPaddleVec[1],  projectileToPaddleVec[0]);

    if (fabs(targetRotAngle - currRotAngle) <= 0.01) {
        // If the direction is already pointing at the paddle then exit immediately
        return;
    }

    // Figure out the shortest way to get there...
    float targetMinusCurr = targetRotAngle - currRotAngle;
    float currMinusTarget = currRotAngle   - targetRotAngle;

    float rotSgn;
    if (fabs(targetMinusCurr) > fabs(currMinusTarget)) {
        rotSgn = NumberFuncs::SignOf(currMinusTarget);
    }
    else {
        rotSgn = NumberFuncs::SignOf(targetMinusCurr);
    }

    float rotationAmt = seconds * degreesChangePerSec * rotSgn;
    this->velocityDir.Rotate(rotationAmt);
    this->velocityDir.Normalize();
}

/**
 * Static factory creator pattern method for making projectiles based on the given type.
 */
Projectile* Projectile::CreateProjectileFromType(ProjectileType type, const Point2D& spawnLoc) {
	switch (type) {
		case Projectile::PaddleLaserBulletProjectile:
			return new PaddleLaserProjectile(spawnLoc);
        case Projectile::BallLaserBulletProjectile:
            return new BallLaserProjectile(spawnLoc);
		default:
			assert(false);
			break;
	}

	return NULL;
}

Projectile* Projectile::CreateProjectileFromCopy(const Projectile* p) {
    assert(p != NULL);
	switch (p->GetType()) {
		case Projectile::PaddleLaserBulletProjectile:
			return new PaddleLaserProjectile(*static_cast<const PaddleLaserProjectile*>(p));
        case Projectile::BallLaserBulletProjectile:
            return new BallLaserProjectile(*static_cast<const BallLaserProjectile*>(p));
		default:
			assert(false);
			break;
	}

	return NULL;
}