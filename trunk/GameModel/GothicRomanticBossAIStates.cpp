/**
 * GothicRomanticBossAIStates.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "GothicRomanticBossAIStates.h"
#include "GothicRomanticBoss.h"
#include "BossWeakpoint.h"
#include "GameModel.h"
#include "PlayerPaddle.h"
#include "Projectile.h"

using namespace gothicromanticbossai;

// Begin GothicRomanticBossAI *****************************************************
GothicRomanticBossAI::GothicRomanticBossAI(GothicRomanticBoss* boss) : BossAIState(), boss(boss) {
    assert(boss != NULL);
}

GothicRomanticBossAI::~GothicRomanticBossAI() {
}

Boss* GothicRomanticBossAI::GetBoss() const {
    return this->boss;
}

Collision::AABB2D GothicRomanticBossAI::GenerateDyingAABB() const {
    return this->boss->GetBody()->GenerateWorldAABB();
}

float GothicRomanticBossAI::GetAccelerationMagnitude() const {
    return GothicRomanticBoss::DEFAULT_ACCELERATION;
}

// End GothicRomanticBossAI *****************************************************

// Begin FireBallAI **************************************************************

// This number doesn't really matter too much, just incase we want to make it more robust later
const float FireBallAI::TOP_POINT_LIFE_POINTS = 300.0f;

FireBallAI::FireBallAI(GothicRomanticBoss* boss) : GothicRomanticBossAI(boss), topPointWeakpt(NULL) {

    boss->ConvertAliveBodyPartToWeakpoint(boss->topPointIdx, FireBallAI::TOP_POINT_LIFE_POINTS, 0);
    assert(dynamic_cast<BossWeakpoint*>(boss->bodyParts[boss->topPointIdx]) != NULL);
    this->topPointWeakpt = static_cast<BossWeakpoint*>(boss->bodyParts[boss->topPointIdx]);

    this->SetState(GothicRomanticBossAI::BasicMoveAndShootState);
}

FireBallAI::~FireBallAI() {
}

void FireBallAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile,
                                   BossBodyPart* collisionPart) {

    // We only care if the projectile hits the weakpoint on the boss
    if (collisionPart != this->topPointWeakpt) {
        return;
    }

    // Only fire globs should be able to hit the boss' weakpoint in this state
    if (projectile->GetType() != Projectile::FireGlobProjectile) {
        assert(false);
        return;
    }

    static const float FIRE_GLOB_DMG_AMT = FireBallAI::TOP_POINT_LIFE_POINTS / static_cast<float>(FireBallAI::NUM_OF_TOP_POINT_HITS);
    this->topPointWeakpt->Diminish(FIRE_GLOB_DMG_AMT, gameModel);
    this->SetState(GothicRomanticBossAI::HurtTopAIState);
}

void FireBallAI::SetState(GothicRomanticBossAI::AIState newState) {
    switch (newState) {
        case BasicMoveAndShootState:
            break;
        case SpinLaserAttackState:
            break;
        case OrbProjectileAttackAIState:
            break;
        case SummonItemsAIState:
            break;
        case HurtTopAIState:
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void FireBallAI::UpdateState(double dT, GameModel* gameModel) {
    switch (this->currState) {

        case BasicMoveAndShootState:
            break;
        case SpinLaserAttackState:
            break;
        case OrbProjectileAttackAIState:
            break;
        case SummonItemsAIState:
            break;
        case HurtTopAIState:
            break;

        default:
            assert(false);
            return;
    }
}