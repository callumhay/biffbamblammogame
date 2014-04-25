/**
 * FuturismStage1AIState.h
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

#ifndef __FUTURISMBOSSSTAGE1AISTATE_H__
#define __FUTURISMBOSSSTAGE1AISTATE_H__

#include "FuturismBossAIState.h"

// In the first stage the futurism boss has shielding on it's four 'limbs' (top, bottom, left, right).
// The player must destroy this shielding before the next stage.
// The shielding is invulnerable to ball hits -- the player must wait for a portal to open up to a item
// drop block that gives them a chance to get a rocket item. The rocket item must then be used to shoot
// at the shielding of the boss. A rocket weakens each shield on impact. The ball or another rocket
// can then be used to fully destroy that part of the bosses' shielding.
class FuturismBossStage1AIState : public FuturismBossAIState {
public:
    FuturismBossStage1AIState(FuturismBoss* boss);
    ~FuturismBossStage1AIState();

private:
    static const double MIN_TIME_UNTIL_FIRST_PORTAL;

    // Inherited from FuturismBossAIState -----------------------------------------------------
    void GoToNextState(const GameModel& gameModel);
    FuturismBossAIState* BuildNextAIState() const;
    bool IsCoreShieldVulnerable() const { return false; }
    bool AreBulbsVulnerable() const { return false; }
    float GetMaxSpeed() const { return FuturismBossAIState::DEFAULT_SPEED; }
    void GetRandomMoveToPositions(const GameModel& gameModel, std::vector<Point2D>& positions) const;
    double GetAfterAttackWaitTime(FuturismBossAIState::AIState attackState) const;
    double GetBallAttractTime() const { /*assert(false);*/ return 5.0; }
    double GetBallDiscardTime() const { /*assert(false);*/ return 0.5; }
    double GetFrozenTime() const { return 0.5*FuturismBossAIState::DEFAULT_FROZEN_TIME_IN_SECS; }
    double GetSingleTeleportInAndOutTime() const { return 1.0; }
    double GetTotalStrategyPortalShootTime() const { return 4.0; }
    double GetTotalWeaponPortalShootTime() const { /*assert(false);*/ return 4.0; }
    double GetTwitchBeamShootTime() const { return 0.7; }
    double GetBeamArcShootTime() const { return this->GetTwitchBeamShootTime(); }
    double GetBeamArcHoldTime() const  { return 1.4; }
    double GetBeamArcingTime() const { return 3.5; }
    double GetTimeBetweenBurstLineShots() const { return 0.2 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.08; }
    double GetTimeBetweenBurstWaveShots() const { return 0.45 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.250; }
    int GetNumBurstLineShots() const { return 3 + (Randomizer::GetInstance()->RandomUnsignedInt() % 2); }
    int GetNumWaveBurstShots() const { return 2 + (Randomizer::GetInstance()->RandomUnsignedInt() % 2); }
    int GetNumShotsPerWaveBurst() const { return 3; }
    
    bool AllShieldsDestroyedToEndThisState() const;
    bool ArenaBarrierExists() const { return true; }

    void GetStrategyPortalCandidatePieces(const GameLevel& level, 
        Collision::AABB2D& pieceBounds, std::set<LevelPiece*>& candidates);
    // -----------------------------------------------------------------------------------------

    // Inherited from BossAIState
    void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    float GetTotalLifePercent() const;
    float GetAccelerationMagnitude() const { return FuturismBossAIState::DEFAULT_ACCELERATION; }

    // Helper Methods
    bool LevelHasRocketInIt(const GameModel& gameModel) const;
    void RocketAwareGoToRandomMoveState(const GameModel& gameModel);

    DISALLOW_COPY_AND_ASSIGN(FuturismBossStage1AIState);
};

#endif // __FUTURISMBOSSSTAGE1AISTATE_H__