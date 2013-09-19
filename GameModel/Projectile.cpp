/**
 * Projectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "Projectile.h"
#include "LevelPiece.h"
#include "GameEventManager.h"
#include "PaddleLaserProjectile.h"
#include "PaddleRocketProjectile.h"
#include "PaddleRemoteControlRocketProjectile.h"
#include "RocketTurretProjectile.h"
#include "BallLaserProjectile.h"
#include "LaserTurretProjectile.h"
#include "PaddleMineProjectile.h"
#include "MineTurretProjectile.h"
#include "BossLaserProjectile.h"
#include "BossRocketProjectile.h"
#include "BossOrbProjectile.h"
#include "BossLightningBoltProjectile.h"
#include "GameModel.h"

// Projectile ====================================================================================================================

Projectile::Projectile(const Point2D& spawnLoc, float width, float height) : 
position(spawnLoc), lastThingCollidedWith(NULL), currWidth(width), currHeight(height), isInvisible(false) {
}

Projectile::Projectile(const Projectile& copy) : position(copy.position),
lastThingCollidedWith(copy.lastThingCollidedWith), currWidth(copy.currWidth), currHeight(copy.currHeight),
velocityDir(copy.velocityDir), velocityMag(copy.velocityMag), rightVec(copy.rightVec), isInvisible(copy.isInvisible) {
}

Projectile::~Projectile() {
}

void Projectile::AugmentDirectionOnPaddleMagnet(double seconds, const GameModel& model, float degreesChangePerSec) {
    // If the paddle has the magnet item active and the projectile is moving towards the paddle, then we need to
    // modify the velocity to make it move towards the paddle...
    const PlayerPaddle* paddle = model.GetPlayerPaddle();
    if (paddle->HasBeenPausedAndRemovedFromGame(model.GetPauseState())) {
        return;
    }
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
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
            return new PaddleRemoteControlRocketProjectile(*static_cast<const PaddleRemoteControlRocketProjectile*>(p));
        case Projectile::RocketTurretBulletProjectile:
            return new RocketTurretProjectile(*static_cast<const RocketTurretProjectile*>(p));
        case Projectile::PaddleMineBulletProjectile:
            return new PaddleMineProjectile(*static_cast<const PaddleMineProjectile*>(p));
        case Projectile::MineTurretBulletProjectile:
            return new MineTurretProjectile(*static_cast<const MineTurretProjectile*>(p));
        case Projectile::BossLaserBulletProjectile:
            return new BossLaserProjectile(*static_cast<const BossLaserProjectile*>(p));
        case Projectile::BossRocketBulletProjectile:
            return new BossRocketProjectile(*static_cast<const BossRocketProjectile*>(p));
        case Projectile::BossOrbBulletProjectile:
            return new BossOrbProjectile(*static_cast<const BossOrbProjectile*>(p));
        case Projectile::BossLightningBoltBulletProjectile:
            return new BossLightningBoltProjectile(*static_cast<const BossLightningBoltProjectile*>(p));
        default:
			assert(false);
			break;
	}

	return NULL;
}

#ifdef _DEBUG
void Projectile::DebugDraw() const {
    BoundingLines bounds = this->BuildBoundingLines();
    bounds.DebugDraw();
}
#endif