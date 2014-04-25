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
    FuturismBossStage3AIState(FuturismBoss* boss, FuturismBossAIState::ArenaState arena);
    ~FuturismBossStage3AIState();

private:
    static const int TIMES_PER_ONE_NON_AVOIDANCE = 3;

    static const double FULL_STAR_BEAM_ROTATION_TIME_IN_SECS;
    static const double MIN_TIME_BETWEEN_ATTRACT_ATTACKS_IN_SECS;
    static const double MAX_TIME_BETWEEN_ATTRACT_ATTACKS_IN_SECS;

    static const double HALTING_AVOIDANCE_TELEPORT_WAIT_TIME_IN_SECS;

    static const float DEFAULT_MOVE_ROTATION_SPD;

    float currRotationSpd;
    float currRotationAccel;

    bool hasDestroyedBarrier;
    SoundID transitionSoundID;
    int idxOfNonAvoidance, currNonAvoidanceIdx;

    double timeSinceLastAttractAttack;
    double avoidanceHaltingTime;

    // BE VERY CAREFUL WITH THIS, NOT OWNED OR CONTROLLED BY THIS, SHOULD ONLY BE USED TO QUERY NOT DIRECT ACCESS!!!
    // Maps boss laser beams to their original beam rays
    std::map<BossLaserBeam*, Collision::Ray2D> currMultiBeam; 
   
    // Inherited from FuturismBossAIState -----------------------------------------------------
    void GoToNextState(const GameModel& gameModel);
    void GoToRandomBasicMoveState();
    void GoToRandomBasicAttackState();

    FuturismBossAIState* BuildNextAIState() const { assert(false); return NULL; } // This is the last state!
    bool IsCoreShieldVulnerable() const { return false; }
    bool AreBulbsVulnerable() const { return true; }
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
    double GetTimeBetweenBurstLineShots() const { return 0.2 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.08; }
    double GetTimeBetweenBurstWaveShots() const { return 0.7 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.40; }
    int GetNumBurstLineShots() const { return 0; } // Doesn't matter, depends on movement time in this state
    int GetNumWaveBurstShots() const { return 0; } // Doesn't matter, depends on movement time in this state
    int GetNumShotsPerWaveBurst() const { return 5; }

    bool AllShieldsDestroyedToEndThisState() const { assert(false); return false; } // There are no shields left at any point during this state
    bool ArenaBarrierExists() const { return !this->hasDestroyedBarrier; }

    void GetStrategyPortalCandidatePieces(const GameLevel& level, 
        Collision::AABB2D& pieceBounds, std::set<LevelPiece*>& candidates);

    void UpdateState(double dT, GameModel* gameModel);
    void InitBasicBurstLineFireAIState();
    void ExecuteBasicBurstLineFireAIState(double dT, GameModel* gameModel);
    void InitBasicBurstWaveFireAIState();
    void ExecuteBasicBurstWaveFireAIState(double dT, GameModel* gameModel);
    void InitLaserBeamStarAIState();
    void ExecuteLaserBeamStarAIState(double dT, GameModel* gameModel);

    // -----------------------------------------------------------------------------------------

    // Inherited from BossAIState
    void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void RocketExplosionOccurred(GameModel* gameModel, const RocketProjectile* rocket);
    float GetTotalLifePercent() const;
    float GetAccelerationMagnitude() const { return 1.2*FuturismBossAIState::DEFAULT_ACCELERATION; }

    // Helper Methods
    bool BallAboutToHitBulbWeakpoints(const GameModel& gameModel) const;
    //bool BallAboutToHitBulbWeakpoint(const BossBodyPart& bulb, const GameBall& ball, const Vector2D& relativeBallDir, 
    //    const Point2D& ballSideAPt, const Point2D& ballSideBPt, float ballMoveMag) const;
    void BuildAndAddBossBulbBeam(GameModel& gameModel, const BossBodyPart& bulb, const Collision::Ray2D& initBeamRay,
        const Collision::Ray2D& noRotBeamRay, double beamLifeTime);
    void DestroyBulb(size_t bulbIdx, const Vector2D& hitDir);

    void DoBasicRotation(double dT);

    DISALLOW_COPY_AND_ASSIGN(FuturismBossStage3AIState);
};

#endif // __FUTURISMBOSSSTAGE3AISTATE_H__
