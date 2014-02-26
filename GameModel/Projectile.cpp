/**
 * Projectile.cpp
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

bool Projectile::AugmentDirectionOnPaddleMagnet(double seconds, const GameModel& model, float degreesChangePerSec) {
    // If the paddle has the magnet item active and the projectile is moving towards the paddle, then we need to
    // modify the velocity to make it move towards the paddle...
    const PlayerPaddle* paddle = model.GetPlayerPaddle();
    if (paddle->HasBeenPausedAndRemovedFromGame(model.GetPauseState())) {
        return false;
    }
    return paddle->AugmentDirectionOnPaddleMagnet(seconds, degreesChangePerSec, this->position, this->velocityDir);
}

Projectile* Projectile::CreateProjectileFromCopy(const Projectile* p, bool createdByReflectionOrRefraction) {
    assert(p != NULL);
	
    switch (p->GetType()) {

        case Projectile::PaddleLaserBulletProjectile: {
			PaddleLaserProjectile* result = new PaddleLaserProjectile(*static_cast<const PaddleLaserProjectile*>(p));
            result->SetWasCreatedByReflectionOrRefraction(createdByReflectionOrRefraction);
            return result;
        }

        case Projectile::BallLaserBulletProjectile: {
            BallLaserProjectile* result = new BallLaserProjectile(*static_cast<const BallLaserProjectile*>(p));
            result->SetWasCreatedByReflectionOrRefraction(createdByReflectionOrRefraction);
            return result;
        }

        case Projectile::LaserTurretBulletProjectile: {
            LaserTurretProjectile* result = new LaserTurretProjectile(*static_cast<const LaserTurretProjectile*>(p));
            result->SetWasCreatedByReflectionOrRefraction(createdByReflectionOrRefraction);
            return result;
        }

        case Projectile::BossLaserBulletProjectile: {
            BossLaserProjectile* result = new BossLaserProjectile(*static_cast<const BossLaserProjectile*>(p));
            result->SetWasCreatedByReflectionOrRefraction(createdByReflectionOrRefraction);
            return result;
        }

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