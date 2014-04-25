/**
 * FuturismStage2AIState.h
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

#ifndef __FUTURISMBOSSSTAGE2AISTATE_H__
#define __FUTURISMBOSSSTAGE2AISTATE_H__

#include "FuturismBossAIState.h"

// In the second stage the futurism boss has shielding on just its core.
// The core shield is vulnerable to rockets and being shattered. The boss is now
// capable of being frozen by ice blasts (when frozen and then hit by the ball it will
// cause its shielding to be destroyed and the end of this state).
// During this state the boss will become more aggressive with its attacks and movement,
// it will start to use portals as weapons (sending the ball back and forth across the
// playing field). Strategy portals will be opened to the ice blast.
class FuturismBossStage2AIState : public FuturismBossAIState {
public:
    FuturismBossStage2AIState(FuturismBoss* boss, FuturismBossAIState::ArenaState arena, float currCoreRotInDegs);
    ~FuturismBossStage2AIState();

private:
    static const double MIN_TIME_UNTIL_FIRST_PORTAL;

    bool hasDoneInitialBallTeleport;

    // Inherited from FuturismBossAIState -----------------------------------------------------
    void GoToNextState(const GameModel& gameModel);
    FuturismBossAIState* BuildNextAIState() const;
    bool IsCoreShieldVulnerable() const { return true; }
    bool AreBulbsVulnerable() const { return false; }
    float GetMaxSpeed() const { return 1.15*FuturismBossAIState::DEFAULT_SPEED; }
    void GetRandomMoveToPositions(const GameModel& gameModel, std::vector<Point2D>& positions) const;
    double GetAfterAttackWaitTime(FuturismBossAIState::AIState attackState) const;
    double GetBallAttractTime() const { return 5.0; }
    double GetBallDiscardTime() const { return 0.5; }
    double GetFrozenTime() const { return FuturismBossAIState::DEFAULT_FROZEN_TIME_IN_SECS; }
    double GetSingleTeleportInAndOutTime() const { return 0.9; }
    double GetTotalStrategyPortalShootTime() const { return 3.5; }
    double GetTotalWeaponPortalShootTime() const { return 3.25; }
    double GetTwitchBeamShootTime() const { return 0.55; }
    double GetBeamArcShootTime() const { return this->GetTwitchBeamShootTime(); }
    double GetBeamArcHoldTime() const  { return 1.2; }
    double GetBeamArcingTime() const { return 3.1; }
    double GetTimeBetweenBurstLineShots() const { return 0.18 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.05; }
    double GetTimeBetweenBurstWaveShots() const { return 0.40 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.20; }
    int GetNumBurstLineShots() const { return 4 + (Randomizer::GetInstance()->RandomUnsignedInt() % 3); }
    int GetNumWaveBurstShots() const { return 3 + (Randomizer::GetInstance()->RandomUnsignedInt() % 2); }
    int GetNumShotsPerWaveBurst() const { return 3; }

    bool AllShieldsDestroyedToEndThisState() const;
    bool ArenaBarrierExists() const { return true; }

    void GetStrategyPortalCandidatePieces(const GameLevel& level, 
        Collision::AABB2D& pieceBounds, std::set<LevelPiece*>& candidates);
    // -----------------------------------------------------------------------------------------

    // Inherited from BossAIState
    void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void RocketExplosionOccurred(GameModel* gameModel, const RocketProjectile* rocket);
    float GetTotalLifePercent() const { return this->boss->IsCoreShieldWeakened() ? 0.5f : 1.0f; }
    float GetAccelerationMagnitude() const { return 1.1*FuturismBossAIState::DEFAULT_ACCELERATION; }
};

#endif // __FUTURISMBOSSSTAGE2AISTATE_H__