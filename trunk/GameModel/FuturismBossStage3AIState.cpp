/**
 * FuturismBossStage3AIState.cpp
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

#include "FuturismBossStage3AIState.h"

FuturismBossStage3AIState::FuturismBossStage3AIState(FuturismBoss* boss) : 
FuturismBossAIState(boss), hasDestroyedBarrier(false), transitionSoundID(INVALID_SOUND_ID) {

    /*
    GameSound* sound = this->boss->GetGameModel()->GetSound();

    // Stop the current boss background music
    sound->StopAllSoundsWithType(GameSound::BossBackgroundLoop, 1.0);
    // ... and play the transition sound in a loop
    sound->PlaySound(GameSound::BossBackgroundLoopTransitionSingleHitEvent, false, false);
    this->transitionSoundID = sound->PlaySound(GameSound::BossBackgroundLoopTransition, true, false);
    */


    this->boss->RegenerateBoundsForFinalCore();

    // The first thing the boss will do is destroy the barrier that separates the
    // two sub-arenas...
    //this->SetState(DestroyLevelBarrierAIState);
}

FuturismBossStage3AIState::~FuturismBossStage3AIState() {
}

void FuturismBossStage3AIState::GoToNextState(const GameModel& gameModel) {
    // TODO
}

void FuturismBossStage3AIState::GetRandomMoveToPositions(const GameModel& gameModel, 
                                                     std::vector<Point2D>& positions) const {

    // TODO
}

double FuturismBossStage3AIState::GetAfterAttackWaitTime(FuturismBossAIState::AIState attackState) const {
    switch (attackState) {

        case FuturismBossAIState::BasicBurstLineFireAIState:
        case FuturismBossAIState::BasicBurstWaveFireAIState:
            return 0.6;

        case FuturismBossAIState::LaserBeamArcAIState:
            return 1.25;

        case FuturismBossAIState::LaserBeamTwitchAIState:
            return 0.9;

        default:
            break;
    }

    return 0.6;
}

void FuturismBossStage3AIState::GetStrategyPortalCandidatePieces(const GameLevel& level, Collision::AABB2D& pieceBounds, 
                                                             std::set<LevelPiece*>& candidates) {
    // TODO

}

void FuturismBossStage3AIState::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {

}

void FuturismBossStage3AIState::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {

}

void FuturismBossStage3AIState::RocketExplosionOccurred(GameModel* gameModel, const RocketProjectile* rocket) {

}

float FuturismBossStage3AIState::GetTotalLifePercent() const {
    // The bulbs at the end of the bosses' four core "arms" must all be destroyed...
    float totalLife = 1.0f;
    if (this->boss->GetCoreTopBulb()->GetIsDestroyed()) {
        totalLife -= 0.25f;
    }
    if (this->boss->GetCoreBottomBulb()->GetIsDestroyed()) {
        totalLife -= 0.25f;
    }
    if (this->boss->GetCoreLeftBulb()->GetIsDestroyed()) {
        totalLife -= 0.25f;
    }
    if (this->boss->GetCoreRightBulb()->GetIsDestroyed()) {
        totalLife -= 0.25f;
    }

    return totalLife;
}