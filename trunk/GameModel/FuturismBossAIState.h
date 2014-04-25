/**
 * FuturismBossAIState.h
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

#ifndef __FUTURISMBOSSAISSTATE_H__
#define __FUTURISMBOSSAISSTATE_H__

#include "BossAIState.h"
#include "GameItem.h"
#include "FuturismBoss.h"

#include "../GameSound/SoundCommon.h"

class FuturismBoss;
class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;
class BossLaserBeam;

class ESPAbstractEmitter;

/**
 * Abstract superclass for all of the FuturismBoss AI state machine classes.
 */
class FuturismBossAIState : public BossAIState {
public:
    FuturismBossAIState(FuturismBoss* boss);
    virtual ~FuturismBossAIState();
        
    enum ArenaState { InLeftArena, InRightArena, InFullyOpenedArena };

    enum AIState {
        // Movement, Status and Teleportation-related States ----------------
        MoveToPositionAIState,     // Boss moves to a particular location, boss may spin and/or attack while doing this
        MoveToCenterAIState,       // Boss moves to the center of the current sub-arena
        TeleportAIState,           // Boss teleports itself only
        AvoidanceTeleportAIState, // Boss teleports itself only, but does so without any delay to avoid getting hit by the ball
        BallAttractAIState,        // Boss sucks the ball into orbit (happens right before teleporting with the ball)
        BallDiscardAIState,        // Boss lets go of the orbiting ball (typically happens right after the teleport)
        FrozenAIState,             // Boss is frozen in ice

        // Attack-specific States ------------------------------------
        // Basic Weapon Attacks:
        BasicBurstLineFireAIState, // Boss fires a single/double/triple burst of its basic weapon at the paddle
        BasicBurstWaveFireAIState, // Boss fires a set of waves of its basic weapon, blanketing the playing area around the paddle
        
        // Laser Beam Attacks:
        LaserBeamTwitchAIState,    // Boss twitches briefly and then immediately fires a laser beam at the paddle
        LaserBeamArcAIState,       // Boss fires its laser beam at a wall and arcs it through the paddle movement area
        LaserBeamStarAIState,      // Boss rotates, firing laser beams outwards from all its 'bulbs'
        
        // Portal Attacks:
        StationaryFireStrategyPortalAIState, // Boss opens a portal that leads to an item that can be used to defeat the boss
        StationaryFireWeaponPortalAIState,   // Boss opens a portal used to make it harder for the player

        // Special States:
        DestroyLevelBarrierAIState,

        // Angry/Hurt/Destruction-related States ----------------------
        AngryAIState,
        ShieldPartCrackedAIState,
        ShieldPartDestroyedAIState,
        BulbHitAndDestroyedAIState,

        FinalDeathThroesAIState
    };

    AIState GetCurrentAIState() const { return this->currState; }
    
    bool IsFrozen() const { return this->currState == FrozenAIState; }
    virtual bool IsCoreShieldVulnerable() const = 0;
    virtual bool AreBulbsVulnerable() const = 0;

    // Inherited functions
    Boss* GetBoss() const;
    bool CanHurtPaddleWithBody() const { return false; }
    bool IsStateMachineFinished() const { return (this->currState == FinalDeathThroesAIState); }
    Collision::AABB2D GenerateDyingAABB() const;

protected:
    static const float DEFAULT_ACCELERATION;
    static const float DEFAULT_SPEED;

    static const float FINAL_STATE_ROT_VEL;

    static const float SPAWNED_PORTAL_WIDTH;
    static const float SPAWNED_PORTAL_HEIGHT;
    static const float SPAWNED_PORTAL_HALF_WIDTH;
    static const float SPAWNED_PORTAL_HALF_HEIGHT;

    static const float EYE_BEAM_HALF_RADIUS;

    static const double BOSS_PORTAL_TERMINATION_TIME_IN_SECS;
    static const double PORTAL_SPAWN_EFFECT_TIME_IN_SECS;

    static const double TWITCH_BEAM_EXPIRE_TIME_IN_SECS;
    static const double DEFAULT_FROZEN_TIME_IN_SECS;

    static const double TIME_UNTIL_BARRIER_DESTRUCTION_SHOT_IN_SECS;
    static const double BARRIER_DESTRUCTION_ARC_TIME_IN_SECS;
    static const double AVOIDANCE_PREDICTION_LOOKAHEAD_TIME_IN_SECS;

    static const float SHIELD_LIFE_POINTS;
    static const float SHIELD_BALL_DAMAGE;

    FuturismBoss* boss;
    AIState currState;
    ArenaState arenaState;
    
    // Track the number of consecutive time certain types of states have been visited,
    // this helps us update to the next state in an intelligent and not-quite-so redundant random way
    int numConsecutiveMoves;
    int numConsecutiveBeams;
    int numConsecutiveShots;
    int numConsecutiveAttacks;

    float currCoreRotInDegs;

    // State-specific variables
    AnimationMultiLerp<Vector3D> angryMoveAnim;
    AnimationMultiLerp<Vector3D> bossHurtAnim;
    AnimationMultiLerp<float> coreRotVelAnim; // Animation value is degrees per second
    AnimationMultiLerp<Colour> beamColourAnim;

    std::vector<LevelPiece*> barrierPieces; // Pointers to the barrier level pieces, NOT OWNED BY THIS!!!

    double frozenTimeCountdown;
    double frozenShakeEffectCountdown;
    double countdownToPortalShot;             // Countdown time until a portal is shot
    bool weaponWasShot;                       // Weather the weapon (e.g., portal, laser beam, etc.) has been shot yet (used by various states)
    Colour nextPortalColour;                  // The colour of the next portal
    double totalBallAttractTime;              // Total time that the boss is attracting the ball for
    double ballAttractTimeCount;              // Count of the time that the boss is attracting the ball for
    double countdownToTeleportOut;            // Countdown time until the boss teleports out from its current position
    double countdownToTeleportIn;             // Countdown time until the boss teleports in to its new position
    double countdownToShot;                   // Countdown time until the boss fires a shot of its weapon (basic/beam/etc.) -- used by various states
    Collision::Ray2D beamRay;                 // The ray that the beam will follow (if determined before firing)
    float beamSweepSpd;
    float beamSweepAngularDist;
    BossLaserBeam* currBeam;    // BE VERY CAREFUL WITH THIS, NOT OWNED OR CONTROLLED BY THIS, SHOULD ONLY BE USED TO QUERY NOT DIRECT ACCESS!!!
    GameBall* attachedBall;     // DITTO
    Vector2D ballDirBeforeAttachment;
    Point2D ballPosBeforeAttachment;
    int numBasicShotsToFire;
    double waitTimeCountdown;

    double timeSinceLastStratPortal;    // Time since the boss last spawned/shot a strategy portal
    double timeSinceLastAttackPortal;   // Time since the boss last spawned/shot an attack portal

    SoundID iceShakeSoundID;
    SoundID chargingSoundID;
    SoundID attractorBeamLoopSoundID;
    
    // Pure virtual functions
    virtual void GoToNextState(const GameModel& gameModel) = 0;
    virtual FuturismBossAIState* BuildNextAIState() const = 0;
    virtual float GetMaxSpeed() const = 0;
    virtual void GetRandomMoveToPositions(const GameModel& gameModel, std::vector<Point2D>& positions) const = 0;
    
    virtual double GetAfterAttackWaitTime(FuturismBossAIState::AIState attackState) const = 0;
    virtual double GetSingleTeleportInAndOutTime() const = 0;
    virtual double GetBallAttractTime() const = 0;
    virtual double GetBallDiscardTime() const = 0;
    virtual double GetFrozenTime() const = 0;
    virtual double GetTotalStrategyPortalShootTime() const = 0;
    virtual double GetTotalWeaponPortalShootTime() const = 0;
    virtual double GetTwitchBeamShootTime() const = 0;
    virtual double GetBeamArcShootTime() const = 0;
    virtual double GetBeamArcHoldTime() const = 0;
    virtual double GetBeamArcingTime() const = 0;  // Total time the beam will be arcing for
    virtual double GetTimeBetweenBurstLineShots() const = 0;
    virtual double GetTimeBetweenBurstWaveShots() const = 0;
    virtual int GetNumBurstLineShots() const = 0;
    virtual int GetNumWaveBurstShots() const = 0;
    virtual int GetNumShotsPerWaveBurst() const = 0;
    
    virtual bool AllShieldsDestroyedToEndThisState() const = 0;
    virtual bool ArenaBarrierExists() const = 0;
    virtual void GetStrategyPortalCandidatePieces(const GameLevel& level, Collision::AABB2D& pieceBounds, std::set<LevelPiece*>& candidates) = 0;
    
    // State functions
    void InitMoveToPositionAIState();
    void InitMoveToCenterAIState();
    void ExecuteMoveToPositionAIState(double dT, GameModel* gameModel);
    void InitTeleportAIState();
    void InitAvoidanceTeleportAIState();
    void ExecuteTeleportAIState(double dT, GameModel* gameModel);
    void InitBallAttractAIState();
    void ExecuteBallAttractAIState(double dT, GameModel* gameModel);
    void InitBallDiscardAIState();
    void ExecuteBallDiscardAIState(double dT, GameModel* gameModel);
    void InitFrozenAIState();
    void ExecuteFrozenAIState(double dT, GameModel* gameModel);
    virtual void InitBasicBurstLineFireAIState();
    virtual void ExecuteBasicBurstLineFireAIState(double dT, GameModel* gameModel);
    virtual void InitBasicBurstWaveFireAIState();
    virtual void ExecuteBasicBurstWaveFireAIState(double dT, GameModel* gameModel);
    void InitLaserBeamTwitchAIState();
    void ExecuteLaserBeamTwitchAIState(double dT, GameModel* gameModel);
    void InitLaserBeamArcAIState();
    void ExecuteLaserBeamArcAIState(double dT, GameModel* gameModel);
    virtual void InitLaserBeamStarAIState() { assert(false); };
    virtual void ExecuteLaserBeamStarAIState(double, GameModel*) { assert(false); };
    void InitStationaryFireWeaponPortalAIState();
    void ExecuteStationaryFireWeaponPortalAIState(double dT, GameModel* gameModel);
    void InitStationaryFireStrategyPortalAIState();
    void ExecuteStationaryFireStrategyPortalAIState(double dT, GameModel* gameModel);
    void ExecuteFirePortalState(double dT, GameModel* gameModel, bool isStrategyPortal);
    void InitDestroyLevelBarrierAIState();
    void ExecuteDestroyLevelBarrierAIState(double dT, GameModel* gameModel);
    void InitShieldPartCrackedAIState();
    void ExecuteShieldPartCrackedAIState(double dT, GameModel* gameModel);
    void InitShieldPartDestroyedAIState();
    void ExecuteShieldPartDestroyedAIState(double dT, GameModel* gameModel);
    void InitBulbHitAndDestroyedAIState();
    void ExecuteBulbHitAndDestroyedAIState(double dT, GameModel* gameModel);
    void InitAngryAIState();
    void ExecuteAngryAIState(double dT, GameModel* gameModel);
    void InitFinalDeathThroesAIState();
    void ExecuteFinalDeathThroesAIState(double dT, GameModel* gameModel);

    // Inherited functions
    void CollisionOccurred(GameModel*, PlayerPaddle&, BossBodyPart*) {}; // Shouldn't happen
    void SetState(FuturismBossAIState::AIState newState);
    virtual void UpdateState(double dT, GameModel* gameModel);

    // Misc. Helper functions
    static void GetArenaBounds(ArenaState arena, float& minX, float& maxX, float& minY, float& maxY);
    const std::vector<Point2D>& GetArenaMoveToPositions(ArenaState arenaState) const;
    void GetPortalCandidatePieces(const GameModel& gameModel, std::set<LevelPiece*>& candidates) const;
    void GetBossCandidatePieces(const GameLevel& level, float paddingX, float paddingY, std::set<LevelPiece*>& candidates) const;
    bool GetArenaPortalLocation(const GameModel& gameModel, Point2D& portalPos, ArenaState chosenArena) const;
    bool GetStrategyPortalLocation(const GameModel& gameModel, Point2D& portalPos);
    bool GetTeleportationLocation(const GameModel& gameModel, Point2D& teleportPos) const;
    void GetValidMoveToPositions(const GameModel& gameModel, const std::vector<Point2D>& allPositions, std::vector<Point2D>& positions) const;
    void SpawnPortals(GameModel& gameModel, const Point2D& portal1Pos, const Point2D& portal2Pos);
    Point2D GetCenterMovePosition() const;

    void ShootStrategyPortal(GameModel& gameModel);
    void ShootWeaponPortal(GameModel& gameModel);

    void CancelActiveBeam(GameModel& gameModel);
    void CloseAllActivePortals(GameModel& gameModel);

    bool IsPaddleVisibleToShootAt(const GameModel& gameModel) const;
    bool BossHasLineOfSightToBall(const GameBall& ball, const GameLevel& level) const;

    void DoBasicWeaponSingleShot(GameModel& gameModel, const Point2D& firePos, const Vector2D& fireDir) const;
    void DoBasicWeaponWaveShot(GameModel& gameModel, const Point2D& firePos, const Vector2D& middleFireDir) const;

    void DoLaserBeamWarningEffect(double timeInSecs, bool isTwitch) const;
    void DoLaserBeamEnergyEffect(double timeInSecs) const;
    void DoIceShatterIfFrozen();

    bool DoWaitTimeCountdown(double dT);

    virtual void GoToRandomBasicMoveState();
    virtual void GoToRandomBasicAttackState();

    void WeakenShield(const Vector2D& hitDir, float hitMagnitude, FuturismBoss::ShieldLimbType shieldType);
    void DestroyShield(const Vector2D& hitDir, FuturismBoss::ShieldLimbType shieldType);
    AnimationMultiLerp<Vector3D> GenerateShieldDeathTranslationAnimation(FuturismBoss::ShieldLimbType shieldType, 
        float shieldSize, float xDir, double timeInSecs) const;
    AnimationMultiLerp<float> GenerateShieldDeathRotationAnimation(FuturismBoss::ShieldLimbType shieldType, 
        double timeInSecs) const;
    
    void SetupCoreRotationVelAnim(double totalTime);
    //void SetupFrozenShakeAnim(double totalTime);

    void AttachBall(GameBall* ball);
    void DetachAndShootBall(const Vector2D& shootDir);
    void ShakeBall(GameBall* ball);
    bool IsBallAvailableForAttractingAndTeleporting(const GameModel& gameModel) const;
    bool IsBallFarEnoughAwayToInitiateAttracting(const GameModel& gameModel) const;
    void GetCenterRotationBodyPartAndFullDegAmt(AbstractBossBodyPart*& bodyPart, float& fullDegAmt) const;

    void InterruptSounds(GameModel& gameModel);

private:
    DISALLOW_COPY_AND_ASSIGN(FuturismBossAIState);
};

inline bool FuturismBossAIState::DoWaitTimeCountdown(double dT) {
    this->waitTimeCountdown -= dT;
    return (this->waitTimeCountdown <= 0);
}

#endif