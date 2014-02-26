/**
 * BossWeakpoint.cpp
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

#include "BossWeakpoint.h"
#include "Projectile.h"
#include "Beam.h"
#include "GameEventManager.h"

const double BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS = 1.5;

BossWeakpoint::BossWeakpoint(float lifePoints, float dmgOnBallHit, const BoundingLines& bounds) :
BossBodyPart(bounds), totalLifePoints(lifePoints), currLifePoints(lifePoints), dmgOnBallHit(dmgOnBallHit),
invulnerableTimer(0.0), totalInvulnerableTime(DEFAULT_INVULNERABLE_TIME_IN_SECS) {

    this->SetFlashingColourAnim();
}

BossWeakpoint::~BossWeakpoint() {
}

/**
 * Builds a BossWeakpoint body part from the given BossBodyPart and other information.
 */
BossWeakpoint* BossWeakpoint::BuildWeakpoint(BossBodyPart* part, float lifePoints, float dmgOnBallHit) {
    if (part == NULL) {
        assert(false);
        return NULL;
    }

    BossWeakpoint* result = new BossWeakpoint(lifePoints, dmgOnBallHit, part->GetLocalBounds());
    result->ToggleSimpleBoundingCalc(part->GetIsSimpleBoundingCalcOn());
    result->worldTransform  = part->GetWorldTransform();
    result->attachedProjectiles = part->GetAttachedProjectilesMap();
    
    return result;
}

void BossWeakpoint::Tick(double dT) {
    BossBodyPart::Tick(dT);
    this->invulnerableTimer = std::max<double>(0.0, this->invulnerableTimer - dT);
}

void BossWeakpoint::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
    assert(gameModel != NULL);
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(ball);

    if (this->collisionsDisabled || this->GetIsDestroyed() || this->IsCurrentlyInvulnerable()) {
        return;
    }
    this->Diminish(this->dmgOnBallHit);
}

void BossWeakpoint::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
    assert(gameModel != NULL);
    assert(projectile != NULL);
    UNUSED_PARAMETER(gameModel);

    if (this->collisionsDisabled || this->GetIsDestroyed() || this->IsCurrentlyInvulnerable()) {
        return;
    }

    switch (projectile->GetType()) {

        case Projectile::LaserTurretBulletProjectile:
        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::CollateralBlockProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::PaddleRocketBulletProjectile:
        case Projectile::FireGlobProjectile:
        case Projectile::PaddleFlameBlastProjectile: // TODO?
        case Projectile::PaddleIceBlastProjectile:   // TODO?
            this->Diminish(projectile->GetDamage());
            break;

        // Mines don't hurt bosses directly (they need to explode to do damage)
        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            break;

        // Boss projectiles are ignored (they are for hitting the player only)
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
            break;

        default:
            assert(false);
            return;
    }
}

void BossWeakpoint::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
    assert(beamSegment != NULL);
    assert(gameModel != NULL);

    UNUSED_PARAMETER(gameModel);
    if (this->collisionsDisabled || this->GetIsDestroyed()) {
        return;
    }
    this->Diminish(static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond())));
}

void BossWeakpoint::Diminish(float damageAmt) {

    // If the boss is invulnerable then we don't do damage
    if (this->IsCurrentlyInvulnerable() || damageAmt <= 0.0f) {
        return;
    }
    
    this->currLifePoints = std::max<float>(0.0f, this->currLifePoints - damageAmt);
    
    // EVENT: Boss just got hurt
    GameEventManager::Instance()->ActionBossHurt(this);

    if (this->currLifePoints <= 0.0) {
        this->isDestroyed = true;
        this->invulnerableTimer = 0.0;
    }
    else {
        this->invulnerableTimer = this->totalInvulnerableTime;
    }
}

void BossWeakpoint::ColourAnimationFinished() {
    if (!this->GetIsDestroyed()) {
        this->SetFlashingColourAnim();
    }
}

void BossWeakpoint::SetFlashingColourAnim() {
    std::vector<double> timeValues;
    timeValues.reserve(4);
    timeValues.push_back(0.0);
    timeValues.push_back(0.65);
    timeValues.push_back(0.67);
    timeValues.push_back(1.32);
    
    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(4);
    colourValues.push_back(ColourRGBA(1.0f, 0.0f, 0.0f, 1.0f));
    colourValues.push_back(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    colourValues.push_back(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    colourValues.push_back(ColourRGBA(1.0f, 0.0f, 0.0f, 1.0f));

    this->rgbaAnim.SetLerp(timeValues, colourValues);
    this->rgbaAnim.SetRepeat(true);
}