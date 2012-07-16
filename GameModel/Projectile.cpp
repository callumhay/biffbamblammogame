/**
 * Projectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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
#include "TurretRocketProjectile.h"
#include "BallLaserProjectile.h"
#include "LaserTurretProjectile.h"
#include "PaddleMineProjectile.h"
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
    paddle->AugmentDirectionOnPaddleMagnet(seconds, degreesChangePerSec, this->position, this->velocityDir);
}

Projectile* Projectile::CreateProjectileFromCopy(const Projectile* p) {
    assert(p != NULL);
	
    switch (p->GetType()) {
		case Projectile::PaddleLaserBulletProjectile:
			return new PaddleLaserProjectile(*static_cast<const PaddleLaserProjectile*>(p));
        case Projectile::BallLaserBulletProjectile:
            return new BallLaserProjectile(*static_cast<const BallLaserProjectile*>(p));
        case Projectile::LaserTurretBulletProjectile:
            return new LaserTurretProjectile(*static_cast<const LaserTurretProjectile*>(p));
        case Projectile::PaddleRocketBulletProjectile:
            return new PaddleRocketProjectile(*static_cast<const PaddleRocketProjectile*>(p));
        case Projectile::RocketTurretBulletProjectile:
            return new TurretRocketProjectile(*static_cast<const TurretRocketProjectile*>(p));
        case Projectile::PaddleMineBulletProjectile:
            return new PaddleMineProjectile(*static_cast<const PaddleMineProjectile*>(p));
        default:
			assert(false);
			break;
	}

	return NULL;
}