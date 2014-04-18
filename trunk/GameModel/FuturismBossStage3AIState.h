/**
 * FuturismBossStage3AIState.h
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

#ifndef __FUTURISMBOSSSTAGE3AISTATE_H__
#define __FUTURISMBOSSSTAGE3AISTATE_H__

#include "FuturismBossAIState.h"

class FuturismBossStage3AIState : public FuturismBossAIState {
public:
    FuturismBossStage3AIState(FuturismBoss* boss);
    ~FuturismBossStage3AIState();


private:
    bool hasDestroyedBarrier;
    SoundID transitionSoundID;

    // Inherited from FuturismBossAIState -----------------------------------------------------
    void GoToNextState(const GameModel& gameModel);
    FuturismBossAIState* BuildNextAIState() const { assert(false); return NULL; } // This is the last state!
    bool IsCoreShieldVulnerable() const { return false; }
    float GetMaxSpeed() const { return 1.25*FuturismBossAIState::DEFAULT_SPEED; }
    void GetRandomMoveToPositions(const GameModel& gameModel, std::vector<Point2D>& positions) const;
    double GetAfterAttackWaitTime(FuturismBossAIState::AIState attackState) const;
    double GetBallAttractTime() const { return 4.0; }
    double GetBallDiscardTime() const { return 0.4; }
    double GetFrozenTime() const { return FuturismBossAIState::DEFAULT_FROZEN_TIME_IN_SECS; }
    double GetSingleTeleportInAndOutTime() const { return 0.8; }
    double GetTotalStrategyPortalShootTime() const { return 3.3; }
    double GetTotalWeaponPortalShootTime() const { return 3.1; }
    double GetTwitchBeamShootTime() const { return 0.5; }
    double GetBeamArcShootTime() const { return this->GetTwitchBeamShootTime(); }
    double GetBeamArcHoldTime() const  { return 1.1; }
    double GetBeamArcingTime() const { return 3.0; }
    double GetTimeBetweenBurstLineShots() const { return 0.16 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.04; }
    double GetTimeBetweenBurstWaveShots() const { return 0.33 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.15; }
    int GetNumBurstLineShots() const { return 4 + (Randomizer::GetInstance()->RandomUnsignedInt() % 4); }
    int GetNumWaveBurstShots() const { return 3 + (Randomizer::GetInstance()->RandomUnsignedInt() % 2); }
    int GetNumShotsPerWaveBurst() const { return 5; }

    bool AllShieldsDestroyedToEndThisState() const { assert(false); return false; } // There are no shields left at any point during this state
    bool ArenaBarrierExists() const { return this->hasDestroyedBarrier; }

    void GetStrategyPortalCandidatePieces(const GameLevel& level, 
        Collision::AABB2D& pieceBounds, std::set<LevelPiece*>& candidates);
    // -----------------------------------------------------------------------------------------

    // Inherited from BossAIState
    void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void RocketExplosionOccurred(GameModel* gameModel, const RocketProjectile* rocket);
    float GetTotalLifePercent() const;
    float GetAccelerationMagnitude() const { return 1.2*FuturismBossAIState::DEFAULT_ACCELERATION; }

    DISALLOW_COPY_AND_ASSIGN(FuturismBossStage3AIState);
};

#endif // __FUTURISMBOSSSTAGE3AISTATE_H__
