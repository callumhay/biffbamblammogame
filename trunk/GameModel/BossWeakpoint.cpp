/**
 * BossWeakpoint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BossWeakpoint.h"
#include "Projectile.h"
#include "Beam.h"
#include "GameEventManager.h"

const double BossWeakpoint::INVULNERABLE_TIME_IN_SECS = 1.5;

BossWeakpoint::BossWeakpoint(float lifePoints, float dmgOnBallHit, const BoundingLines& bounds) :
BossBodyPart(bounds), totalLifePoints(lifePoints), currLifePoints(lifePoints), dmgOnBallHit(dmgOnBallHit),
invulnerableTimer(0.0) {

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
    result->worldTransform  = part->GetWorldTransform();

    return result;
}

void BossWeakpoint::Tick(double dT) {
    BossBodyPart::Tick(dT);
    this->invulnerableTimer = std::max<double>(0.0, this->invulnerableTimer - dT);
}

void BossWeakpoint::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
    assert(gameModel != NULL);
    UNUSED_PARAMETER(ball);

    if (this->GetIsDestroyed() || this->IsCurrentlyInvulnerable()) {
        return;
    }

    this->Diminish(this->dmgOnBallHit, gameModel);
}

void BossWeakpoint::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
    assert(gameModel != NULL);
    assert(projectile != NULL);

    if (this->GetIsDestroyed() || this->IsCurrentlyInvulnerable()) {
        return;
    }

    switch (projectile->GetType()) {

        case Projectile::LaserTurretBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::CollateralBlockProjectile:

        case Projectile::RocketTurretBulletProjectile:

        case Projectile::PaddleRocketBulletProjectile:

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:

        case Projectile::FireGlobProjectile:

            this->Diminish(projectile->GetDamage(), gameModel);
            break;

        case Projectile::BossLaserBulletProjectile:
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

    this->Diminish(static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond())), gameModel);
}

void BossWeakpoint::Diminish(float damageAmt, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);
    
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
        this->invulnerableTimer = BossWeakpoint::INVULNERABLE_TIME_IN_SECS;
    }
}

void BossWeakpoint::ColourAnimationFinished() {
    if (!this->GetIsDestroyed()) {
        this->SetFlashingColourAnim();
    }
}

void BossWeakpoint::SetFlashingColourAnim() {
    std::vector<double> timeValues;
    timeValues.reserve(3);
    timeValues.push_back(0.0);
    timeValues.push_back(1.0);
    timeValues.push_back(2.0);
    
    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(3);
    colourValues.push_back(ColourRGBA(1.0f, 0.9f, 0.9f, 1.0f));
    colourValues.push_back(ColourRGBA(1.0f, 0.25f, 0.25f, 1.0f));
    colourValues.push_back(ColourRGBA(1.0f, 0.9f, 0.9f, 1.0f));

    this->rgbaAnim.SetLerp(timeValues, colourValues);
    this->rgbaAnim.SetRepeat(true);
}