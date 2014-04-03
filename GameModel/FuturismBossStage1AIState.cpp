/**
 * FuturismBossStage1AIState.cpp
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

#include "FuturismBossStage1AIState.h"
#include "FuturismBoss.h"
#include "PaddleRocketProjectile.h"
#include "GameModel.h"

const float FuturismBossStage1AIState::BOSS_SPEED = FuturismBossAIState::DEFAULT_SPEED;

FuturismBossStage1AIState::FuturismBossStage1AIState(FuturismBoss* boss) : FuturismBossAIState(boss),
bottomShieldWeakened(false), topShieldWeakened(false), leftShieldWeakened(false), rightShieldWeakened(false) {

    // The boss spends this entire state in the initial, left sub arena of the level
    this->arenaState = FuturismBossAIState::InLeftArena;

    // NOTE: Shields aren't weak-points, but they can be destroyed by rockets

    // Boss starts off by moving to some location
    //this->SetState(FuturismBossAIState::StationaryFireStrategyPortalAIState);
    this->SetState(FuturismBossAIState::TeleportAIState);
}

FuturismBossStage1AIState::~FuturismBossStage1AIState() {
}

void FuturismBossStage1AIState::GetRandomMoveToPositions(std::vector<Point2D>& positions) const {
    // In this state the boss will only be moving around in the left sub arena
    BossAIState::GetFurthestDistFromBossPositions(this->boss->alivePartsRoot->GetTranslationPt2D(), 
        this->GetArenaMoveToPositions(), 5, positions);
}

void FuturismBossStage1AIState::GetStrategyPortalCandidatePieces(const GameLevel& level, Collision::AABB2D& pieceBounds,
                                                                 std::set<LevelPiece*>& candidates) const {

    pieceBounds = FuturismBoss::GetRocketStrategyPortalSearchAABB(
        level, FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT);
    FuturismBoss::GetRocketStrategyPortalSearchPieces(level, candidates);
}

void FuturismBossStage1AIState::SetState(FuturismBossAIState::AIState newState) {

    // Only handle states that are relevant to this class' state
    // All others are considered erroneous
    switch (newState) {

        case MoveToPositionAIState:
            this->InitMoveToPositionAIState();
            break;

        case TeleportAIState:
            this->InitTeleportAIState();
            break;

        case BasicBurstLineFireAIState:
            break;
        case BasicBurstWaveFireAIState:
            break;
        case TeleportAttackComboAIState:
            break;
        case LaserBeamTwitchAIState:
            break;
        case LaserBeamArcAIState:
            break;
        case StationaryFireStrategyPortalAIState:
            this->InitStationaryFireStrategyPortalAIState();
            break;
        
        case ShieldPartCrackedAIState:
            break;
        case ShieldPartDestroyedAIState:
            break;
        case AngryAIState:
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void FuturismBossStage1AIState::CollisionOccurred(GameModel* gameModel, GameBall& ball, 
                                                  BossBodyPart* collisionPart) {

    
    if (collisionPart == this->boss->GetTopShield() && this->topShieldWeakened) {

    }
    else if (collisionPart == this->boss->GetBottomShield() && this->bottomShieldWeakened) {

    }
    else if (collisionPart == this->boss->GetLeftShield() && this->leftShieldWeakened) {

    }
    else if (collisionPart == this->boss->GetRightShield() && this->rightShieldWeakened) {

    }
}

void FuturismBossStage1AIState::CollisionOccurred(GameModel* gameModel, Projectile* projectile, 
                                                  BossBodyPart* collisionPart) {

    // Only rocket projectiles can hurt the boss in this state
    if (!projectile->IsRocket()) {
        return;
    }

    if (collisionPart == this->boss->GetTopShield()) {
        if (this->topShieldWeakened) {

        }
        else {

        }
    }
    else if (collisionPart == this->boss->GetBottomShield()) {
        if (this->bottomShieldWeakened) {

        }
        else {

        }
    }
    else if (collisionPart == this->boss->GetLeftShield()) {
        if (this->leftShieldWeakened) {

        }
        else {

        }
    }
    else if (collisionPart == this->boss->GetRightShield()) {
        if (this->rightShieldWeakened) {

        }
        else {

        }
    }
}

float FuturismBossStage1AIState::GetTotalLifePercent() const {
    float lifePercent = 0.0f;
    if (!this->boss->GetTopShield()->GetIsDestroyed()) {
        if (this->topShieldWeakened) {
            lifePercent += 0.125f;
        }
        else {
            lifePercent += 0.25f;
        }
    }
    if (!this->boss->GetBottomShield()->GetIsDestroyed()) {
        if (this->bottomShieldWeakened) {
            lifePercent += 0.125f;
        }
        else {
            lifePercent += 0.25f;
        }
    }
    if (!this->boss->GetLeftShield()->GetIsDestroyed()) {
        if (this->leftShieldWeakened) {
            lifePercent += 0.125f;
        }
        else {
            lifePercent += 0.25f;
        }
    }
    if (!this->boss->GetRightShield()->GetIsDestroyed()) {
        if (this->rightShieldWeakened) {
            lifePercent += 0.125f;
        }
        else {
            lifePercent += 0.25f;
        }
    }

    return lifePercent;
}