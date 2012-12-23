/**
 * ClassicalBossAIStates.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "ClassicalBossAIStates.h"
#include "ClassicalBoss.h"

using namespace classicalbossai;

// BEGIN ArmsBodyHeadAI ************************************************************

ArmsBodyHeadAI::ArmsBodyHeadAI(ClassicalBoss* boss) : ClassicalBossAI(boss) {
    // Grab the parts of the boss that matter to this AI state...
    //this->boss->


}

ArmsBodyHeadAI::~ArmsBodyHeadAI() {
}

void ArmsBodyHeadAI::Tick(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);
    this->UpdateState(dT);
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
}

void ArmsBodyHeadAI::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart) {
}

void ArmsBodyHeadAI::UpdateState(double dT) {
    switch (this->currState) {

        case ChanceAIState:
        case FollowPaddleAIState:
        case AttackLeftArmAIState:
        case AttackRightArmAIState:
        case AttackBothArmsAIState:
        case FarLeftPrepLaserAIState:
        case FarRightPrepLaserAIState:
        case MoveLeftLaserAIState:
        case MoveRightLaserAIState:
        case HurtAIState:
        case LostArmsAngryAIState:

        default:
            assert(false);
            break;
    }
}

// END ArmsBodyHeadAI **************************************************************