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

class FuturismBoss;
class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

class ESPAbstractEmitter;

/**
 * Abstract superclass for all of the FuturismBoss AI state machine classes.
 */
class FuturismBossAIState : public BossAIState {
public:
    FuturismBossAIState(FuturismBoss* boss);
    virtual ~FuturismBossAIState();

    enum AIState {
        // Movement, Status and Teleportation-related States ----------------
        MoveToPositionAIState,     // Boss moves to a particular location, boss may spin and/or attack while doing this
        BallAttractAIState,        // Boss sucks the ball into orbit (happens right before teleporting with the ball)
        //TeleportWithBallAIState,   // Boss teleports with the ball in orbit
        //BallDiscardAIState,        // Boss lets go of the orbiting ball (typically happens right after the teleport)
        TeleportAIState,           // Boss teleports itself only
        //FrozenAIState,             // Boss is frozen in ice

        // Attack-specific States ------------------------------------
        // Basic Weapon Attacks:
        BasicBurstLineFireAIState, // Boss fires a single/double/triple burst of its basic weapon at the paddle
        BasicBurstWaveFireAIState, // Boss fires a set of waves of its basic weapon, blanketing the playing area around the paddle
        // Teleport-Attack Combos:
        TeleportAttackComboAIState,

        // Laser Beam Attacks:
        LaserBeamTwitchAIState,    // Boss twitches briefly and then immediately fires a laser beam at the paddle
        LaserBeamArcAIState,       // Boss fires its laser beam at a wall and arcs it through the paddle movement area
        //LaserBeamStarAIState,      // Boss rotates, firing laser beams outwards from all its 'bulbs'
        // Portal Attacks:
        StationaryFireStrategyPortalAIState, // Boss opens a portal that leads to an item that can be used to defeat the boss
        //FireWeaponPortalAIState,   // Boss opens a portal used to make it harder for the player

        // Angry/Hurt/Destruction-related States ----------------------
        AngryAIState,
        ShieldPartCrackedAIState,
        ShieldPartDestroyedAIState,
        //BulbHitAIState,

        FinalDeathThroesAIState
    };

    AIState GetCurrentAIState() const { return this->currState; }


    virtual bool IsCoreShieldVulnerable() const = 0;

    // Inherited functions
    Boss* GetBoss() const;
    bool CanHurtPaddleWithBody() const { return false; }
    bool IsStateMachineFinished() const { return (this->currState == FinalDeathThroesAIState); }
    Collision::AABB2D GenerateDyingAABB() const;

protected:
    static const float DEFAULT_ACCELERATION;
    static const float DEFAULT_SPEED;

    static const float SPAWNED_PORTAL_WIDTH;
    static const float SPAWNED_PORTAL_HEIGHT;
    static const float SPAWNED_PORTAL_HALF_WIDTH;
    static const float SPAWNED_PORTAL_HALF_HEIGHT;

    static const double STRATEGY_PORTAL_TERMINATION_TIME_IN_SECS;
    static const double PORTAL_SPAWN_EFFECT_TIME_IN_SECS;

    FuturismBoss* boss;
    AIState currState;

    enum ArenaState { InLeftArena, InRightArena, InFullyOpenedArena };
    ArenaState arenaState;

    // State-specific variables
    AnimationMultiLerp<float> bossEffortShakeAnim;
    AnimationMultiLerp<float> coreRotVelAnim; // Animation value is degrees per second
    double countdownToPortalShot;             // Countdown time until a portal is shot
    bool portalWasShot;                       // Has the portal been shot yet?
    Colour nextPortalColour;                  // The colour of the next portal
    double countdownToTeleport;               // Countdown time until the boss teleports

    // Pure virtual functions
    virtual void SetState(FuturismBossAIState::AIState newState) = 0;
    virtual float GetMaxSpeed() const = 0;
    virtual void GetRandomMoveToPositions(std::vector<Point2D>& positions) const = 0;
    virtual double GetPauseTimeBeforeSingleTeleport() const = 0;
    virtual double GetTotalStrategyPortalShootTime() const = 0;
    virtual void GetStrategyPortalCandidatePieces(const GameLevel& level, Collision::AABB2D& pieceBounds, std::set<LevelPiece*>& candidates) const = 0;
    

    // State functions
    void InitMoveToPositionAIState();
    void ExecuteMoveToPositionAIState(double dT, GameModel* gameModel);
    void InitTeleportAIState();
    void ExecuteTeleportAIState(double dT, GameModel* gameModel);
    void InitStationaryFireStrategyPortalAIState();
    void ExecuteStationaryFireStrategyPortalAIState(double dT, GameModel* gameModel);

    // Inherited functions
    void CollisionOccurred(GameModel*, PlayerPaddle&, BossBodyPart*) {}; // Shouldn't happen
    void UpdateState(double dT, GameModel* gameModel);

    // Misc. Helper functions
    void GetArenaBounds(float& minX, float& maxX, float& minY, float& maxY) const;
    const std::vector<Point2D>& GetArenaMoveToPositions() const;
    void GetPortalCandidatePieces(const GameModel& gameModel, std::set<LevelPiece*>& candidates) const;
    void GetBossCandidatePieces(const GameLevel& level, float paddingX, float paddingY, std::set<LevelPiece*>& candidates) const;
    bool GetArenaPortalLocation(const GameModel& gameModel, Point2D& portalPos) const;
    bool GetStrategyPortalLocation(const GameModel& gameModel, Point2D& portalPos) const;
    bool GetTeleportationLocation(const GameModel& gameModel, bool withBall, Point2D& teleportPos) const;
    void SpawnPortals(GameModel& gameModel, const Point2D& portal1Pos, const Point2D& portal2Pos);

    

private:
    DISALLOW_COPY_AND_ASSIGN(FuturismBossAIState);
};

#endif