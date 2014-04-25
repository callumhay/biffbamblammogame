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
#include "GameModel.h"
#include "BossWeakpoint.h"
#include "BossLaserBeam.h"
#include "FullscreenFlashEffectInfo.h"

#include "../GameSound/GameSound.h"

const double FuturismBossStage3AIState::FULL_STAR_BEAM_ROTATION_TIME_IN_SECS = 12.0;

const double FuturismBossStage3AIState::MIN_TIME_BETWEEN_ATTRACT_ATTACKS_IN_SECS = 40.0;
const double FuturismBossStage3AIState::MAX_TIME_BETWEEN_ATTRACT_ATTACKS_IN_SECS = 80.0;

const double FuturismBossStage3AIState::HALTING_AVOIDANCE_TELEPORT_WAIT_TIME_IN_SECS = 3.5;

const float FuturismBossStage3AIState::DEFAULT_MOVE_ROTATION_SPD = 270.0f;

FuturismBossStage3AIState::FuturismBossStage3AIState(FuturismBoss* boss, 
                                                     FuturismBossAIState::ArenaState arena) : 
FuturismBossAIState(boss), hasDestroyedBarrier(false), transitionSoundID(INVALID_SOUND_ID),
currRotationSpd(0.0f), currRotationAccel(0.0f), timeSinceLastAttractAttack(0.0), avoidanceHaltingTime(FLT_MAX) {

    this->arenaState = arena;
    this->currCoreRotInDegs = 0;
    this->idxOfNonAvoidance   = Randomizer::GetInstance()->RandomUnsignedInt() % TIMES_PER_ONE_NON_AVOIDANCE;
    this->currNonAvoidanceIdx = 0;

    GameSound* sound = this->boss->GetGameModel()->GetSound();

    // Stop the current boss background music
    sound->StopAllSoundsWithType(GameSound::BossBackgroundLoop, 1.0);
    // ... and play the transition sound in a loop
    sound->PlaySound(GameSound::BossBackgroundLoopTransitionSingleHitEvent, false, false);
    this->transitionSoundID = sound->PlaySound(GameSound::BossBackgroundLoopTransition, true, false);
 
    // The first thing the boss will do is destroy the barrier that separates the two sub-arenas...
    // It must move the center of its current sub arena first though...
    assert(this->arenaState != InFullyOpenedArena);
    this->SetState(MoveToCenterAIState);
}

FuturismBossStage3AIState::~FuturismBossStage3AIState() {
}

void FuturismBossStage3AIState::GoToNextState(const GameModel& gameModel) {

    if (this->currState == DestroyLevelBarrierAIState && !this->hasDestroyedBarrier) {
        // If we're here then the barrier is destroyed...

        // Queue the ridiculously fast angry music!
        GameSound* sound = gameModel.GetSound();
        sound->StopSound(this->transitionSoundID, 3.0);
        sound->PlaySound(GameSound::BossAngryBackgroundLoop, true);

        // These two things MUST be set or else many routines for the boss will be screwed up!!
        this->arenaState = InFullyOpenedArena;
        this->hasDestroyedBarrier = true;
    }
    else if (!this->hasDestroyedBarrier) {
        // Destroy the barrier!!
        // NOTE: The faster background music will automatically be played by the DestroyLevelBarrierAIState
        this->SetState(DestroyLevelBarrierAIState);
        return;
    }

    assert(this->arenaState == InFullyOpenedArena);
    assert(this->hasDestroyedBarrier);

    // If we're here the barrier has been destroyed and we're in the non-scripted AI portion of this state
    // Choose the next move/attack state based on previous info...
    
    // If it's been too long since the last strategy portal was shot then we should shoot one
    // or else the battle could take forever (the player NEEDS the portal to get the item required
    // to move on to the next high-level AI state!)
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    if (this->timeSinceLastStratPortal > 2.5*FuturismBossAIState::BOSS_PORTAL_TERMINATION_TIME_IN_SECS &&
        FuturismBoss::IsInRightSubArena(bossPos)) {

        this->SetState(StationaryFireStrategyPortalAIState);
        return;
    }

    // Last special case: If the boss hasn't shot a portal in a while then there's a chance of shooting an attack portal
    if (this->timeSinceLastAttackPortal > FuturismBossAIState::BOSS_PORTAL_TERMINATION_TIME_IN_SECS &&
        this->timeSinceLastStratPortal > FuturismBossAIState::BOSS_PORTAL_TERMINATION_TIME_IN_SECS &&
        Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) {

        // TODO: Make this MovingFireWeaponPortalAIState?
        this->SetState(StationaryFireWeaponPortalAIState);
        return;
    }

    if (this->currState != FuturismBossAIState::BallDiscardAIState &&
        (this->timeSinceLastAttractAttack > MAX_TIME_BETWEEN_ATTRACT_ATTACKS_IN_SECS || 
        this->timeSinceLastAttractAttack > MIN_TIME_BETWEEN_ATTRACT_ATTACKS_IN_SECS && Randomizer::GetInstance()->RandomUnsignedInt() % 5 == 0) &&
        this->IsBallAvailableForAttractingAndTeleporting(gameModel) && this->IsBallFarEnoughAwayToInitiateAttracting(gameModel)) {

        this->timeSinceLastAttractAttack = 0.0;
        this->SetState(BallAttractAIState);
        return;
    }

    // Regular attack/move protocol...
    static const int WAVE_BURST_IDX    = 0;
    static const int LINE_BURST_IDX    = 1;
    static const int ARC_BEAM_IDX      = 2;
    static const int TWITCH_BEAM_IDX   = 3;
    static const int STAR_BEAM_IDX     = 4;
    static const int TELEPORT_IDX      = 5;
    
    // Depending on whether the boss can see the paddle or not we decide what state to move to next
    int randomNum = 0;
    if (this->IsPaddleVisibleToShootAt(gameModel)) {

        // Boss can see the paddle
        static const int NUM_CHOICES = 20;
        static const int CHOICES[NUM_CHOICES] = {
            TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
            WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, 
            LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX,
            ARC_BEAM_IDX, ARC_BEAM_IDX,
            TWITCH_BEAM_IDX, TWITCH_BEAM_IDX, TWITCH_BEAM_IDX,
            STAR_BEAM_IDX, STAR_BEAM_IDX,
        };

        randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
    }
    else {
        // Boss can't see the paddle
        static const int NUM_CHOICES = 15;
        static const int CHOICES[NUM_CHOICES] = { 
            TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
            WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX,
            LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX,
            ARC_BEAM_IDX, ARC_BEAM_IDX,
            STAR_BEAM_IDX, STAR_BEAM_IDX
        };

        randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
    }

    // Do a redundancy check (make sure we don't revisit states that we were just at in a way that
    // looks obvious and robotic)
    switch (this->currState) {
        case TeleportAIState:
            if (this->numConsecutiveMoves > 2) {
                // Set the state to an attack-type state
                this->GoToRandomBasicAttackState();
                return;
            }
            break;

        case BasicBurstLineFireAIState:
        case BasicBurstWaveFireAIState:
            break;

        case LaserBeamTwitchAIState:
            if (this->numConsecutiveBeams > 2) {
                this->GoToRandomBasicMoveState();
                return;
            }

            if (randomNum == TWITCH_BEAM_IDX) {
                static const int NUM_CHOICES = 15;
                static const int CHOICES[NUM_CHOICES] = {
                    TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
                    WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX,
                    LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX,
                    ARC_BEAM_IDX, ARC_BEAM_IDX,
                    STAR_BEAM_IDX, STAR_BEAM_IDX,
                };

                randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
            }
            break;

        case LaserBeamArcAIState:
            if (this->numConsecutiveBeams > 2) {
                this->GoToRandomBasicMoveState();
                return;
            }

            if (randomNum == ARC_BEAM_IDX) {
                static const int NUM_CHOICES = 12;
                static const int CHOICES[NUM_CHOICES] = { 
                    TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
                    WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX,
                    LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX,
                    STAR_BEAM_IDX,
                };
                randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
            }
            break;


        case LaserBeamStarAIState:
            if (this->numConsecutiveBeams > 2) {
                this->GoToRandomBasicMoveState();
                return;
            }

            if (randomNum == STAR_BEAM_IDX) {
                static const int NUM_CHOICES = 13;
                static const int CHOICES[NUM_CHOICES] = { 
                    TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
                    WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX,
                    LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX,
                    ARC_BEAM_IDX, ARC_BEAM_IDX,
                };
                randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
            }
            break;

        default:
            break;
    }
    
    // Choose a next state at random (with some bias based on previous state(s))
    switch (randomNum) {
        case WAVE_BURST_IDX:   this->SetState(BasicBurstWaveFireAIState); break;
        case LINE_BURST_IDX:   this->SetState(BasicBurstLineFireAIState); break;
        case ARC_BEAM_IDX:     this->SetState(LaserBeamArcAIState); break;
        case TWITCH_BEAM_IDX:  this->SetState(LaserBeamTwitchAIState); break; 
        case STAR_BEAM_IDX:    this->SetState(LaserBeamStarAIState); break;
        case TELEPORT_IDX:     this->SetState(TeleportAIState); break;
        default: this->SetState(TeleportAIState); break;
    }
}

void FuturismBossStage3AIState::GoToRandomBasicMoveState() {
    int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 8;
    switch (randomNum) {
        case 0: case 1: this->SetState(TeleportAIState); return;
        case 2: case 3: case 4: this->SetState(BasicBurstWaveFireAIState); return;
        case 5: case 6: case 7: default: this->SetState(BasicBurstLineFireAIState); return;
    }
}
void FuturismBossStage3AIState::GoToRandomBasicAttackState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 10;
    switch (randomNum) {
        
        case 0: case 1: case 2: this->SetState(BasicBurstWaveFireAIState); return;
        case 3: case 4: case 5: this->SetState(BasicBurstLineFireAIState); return;
        
        case 6: case 7: case 8: case 9: {

            if (Randomizer::GetInstance()->RandomUnsignedInt() % 4 == 0) {
                this->SetState(LaserBeamStarAIState);
            }
            else {
                if (Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0 || !this->IsPaddleVisibleToShootAt(*gameModel)) {
                    this->SetState(LaserBeamArcAIState);
                }
                else {
                    this->SetState(LaserBeamTwitchAIState);
                }
            }
            return;
        }
        default: this->SetState(BasicBurstWaveFireAIState); return;
    }
}

void FuturismBossStage3AIState::GetRandomMoveToPositions(const GameModel& gameModel, 
                                                         std::vector<Point2D>& positions) const {

    int numRandomPositions = 6 + (Randomizer::GetInstance()->RandomUnsignedInt() % 6);

    const std::vector<Point2D>& allPositions = this->GetArenaMoveToPositions(this->arenaState);
    this->GetValidMoveToPositions(gameModel, allPositions, positions);

    // Exit if the number of positions is already the maximum or less
    if (static_cast<int>(positions.size()) <= numRandomPositions) {
        return;
    }

    // In this state the boss will only be moving around in the left sub arena
    BossAIState::GetFurthestDistFromBossPositions(this->boss->alivePartsRoot->GetTranslationPt2D(), 
        positions, numRandomPositions, positions);
}

double FuturismBossStage3AIState::GetAfterAttackWaitTime(FuturismBossAIState::AIState attackState) const {
    switch (attackState) {

        case FuturismBossAIState::BasicBurstLineFireAIState:
        case FuturismBossAIState::BasicBurstWaveFireAIState:
            return 0.6;

        case FuturismBossAIState::LaserBeamStarAIState:
            return 1.5;

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
    // Only send strategy portals to the ice blaster
    pieceBounds = FuturismBoss::GetIceStrategyPortalSearchAABB(
        FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT);
    FuturismBoss::GetIceStrategyPortalSearchPieces(level, candidates);
}

void FuturismBossStage3AIState::UpdateState(double dT, GameModel* gameModel) {
    
    this->timeSinceLastAttractAttack += dT;
    this->avoidanceHaltingTime += dT;
    FuturismBossAIState::UpdateState(dT, gameModel);

    // We do some special updating in this state -- we don't let the ball get anywhere near the
    // boss unless the boss is frozen or the boss is in the ball attractor and teleport states...
    if (this->avoidanceHaltingTime > HALTING_AVOIDANCE_TELEPORT_WAIT_TIME_IN_SECS &&
        this->hasDestroyedBarrier && this->currState != BallAttractAIState && 
        this->currState != BallDiscardAIState && this->currState != FrozenAIState && 
        this->currState != DestroyLevelBarrierAIState && this->currState != FinalDeathThroesAIState &&
        this->currState != AvoidanceTeleportAIState && this->currState != BulbHitAndDestroyedAIState &&
        this->currState != AngryAIState) {

        // Check to see if the ball is close and on a trajectory to hit a weak spot on the boss...
        if (this->BallAboutToHitBulbWeakpoints(*gameModel)) {

            if (this->currNonAvoidanceIdx != this->idxOfNonAvoidance) {
                // Go to an instantaneous teleportation state to immediately avoid the ball
                this->SetState(AvoidanceTeleportAIState);
            }
            else {
                this->avoidanceHaltingTime = 0;
            }

            this->currNonAvoidanceIdx++;
            if (this->currNonAvoidanceIdx >= TIMES_PER_ONE_NON_AVOIDANCE) {
                this->currNonAvoidanceIdx = 0;
                this->idxOfNonAvoidance = Randomizer::GetInstance()->RandomUnsignedInt() % TIMES_PER_ONE_NON_AVOIDANCE;
            }
        }
    }
}

void FuturismBossStage3AIState::InitBasicBurstLineFireAIState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    int prevNumMoves = this->numConsecutiveMoves;
    FuturismBossAIState::InitBasicBurstLineFireAIState();

    // The boss will now move and rotate during this state...
    // Figure out a good position and move to it...
    std::vector<Point2D> moveToPositions;
    this->GetRandomMoveToPositions(*gameModel, moveToPositions);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    if (moveToPositions.empty()) {
        this->SetMoveToTargetPosition(bossPos, bossPos, EPSILON);
        return;
    }

    // Choose a position at random...
    int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(moveToPositions.size());
    this->SetMoveToTargetPosition(bossPos, moveToPositions[randomIdx]);

    // Rotation...
    this->currRotationSpd = Randomizer::GetInstance()->RandomNegativeOrPositive() * DEFAULT_MOVE_ROTATION_SPD;

    this->numConsecutiveMoves = prevNumMoves+1;
}
void FuturismBossStage3AIState::ExecuteBasicBurstLineFireAIState(double dT, GameModel* gameModel) {
    
    this->DoBasicRotation(dT);
    bool doneMoving = this->MoveToTargetPosition(this->GetMaxSpeed());
    
    this->countdownToShot -= dT;
    if (this->countdownToShot <= 0.0) {

        if (doneMoving) {
            if (this->DoWaitTimeCountdown(dT)) {
                this->currRotationSpd = 0;
                this->GoToNextState(*gameModel);
            }
        }
        else {
            // Shoot a laser at the paddle!
            Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
            const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
            Vector2D bossToPaddleVec = (paddle->GetCenterPosition() + 
                Vector2D(Randomizer::GetInstance()->RandomNumNegOneToOne() * paddle->GetHalfWidthTotal(), 
                Randomizer::GetInstance()->RandomNumNegOneToOne() * paddle->GetHalfHeight())) - bossPos;
            bossToPaddleVec.Normalize();

            this->DoBasicWeaponSingleShot(*gameModel, bossPos, bossToPaddleVec);
            this->countdownToShot = this->GetTimeBetweenBurstLineShots();
        }
    }
}

void FuturismBossStage3AIState::InitBasicBurstWaveFireAIState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    int prevNumMoves = this->numConsecutiveMoves;
    FuturismBossAIState::InitBasicBurstWaveFireAIState();

    // The boss will now move and rotate during this state...
    // Figure out a good position and move to it...
    std::vector<Point2D> moveToPositions;
    this->GetRandomMoveToPositions(*gameModel, moveToPositions);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    if (moveToPositions.empty()) {
        this->SetMoveToTargetPosition(bossPos, bossPos, EPSILON);
        return;
    }

    // Choose a position at random...
    int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(moveToPositions.size());
    this->SetMoveToTargetPosition(bossPos, moveToPositions[randomIdx]);

    // Rotation...
    this->currRotationSpd = Randomizer::GetInstance()->RandomNegativeOrPositive() * 180.0;

    this->numConsecutiveMoves = prevNumMoves+1;
}
void FuturismBossStage3AIState::ExecuteBasicBurstWaveFireAIState(double dT, GameModel* gameModel) {

    this->DoBasicRotation(dT);
    bool doneMoving = this->MoveToTargetPosition(this->GetMaxSpeed());

    this->countdownToShot -= dT;
    if (this->countdownToShot <= 0.0) {

        if (doneMoving) {
            if (this->DoWaitTimeCountdown(dT)) {
                this->currRotationSpd = 0;
                this->GoToNextState(*gameModel);
            }
        }
        else {
            // Shoot a wave of lasers at the paddle!
            Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
            const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
            Vector2D bossToPaddleVec = (paddle->GetCenterPosition() + 
                Vector2D(Randomizer::GetInstance()->RandomNumNegOneToOne() * paddle->GetHalfWidthTotal(), 
                Randomizer::GetInstance()->RandomNumNegOneToOne() * paddle->GetHalfHeight())) - bossPos;
            bossToPaddleVec.Normalize();

            this->DoBasicWeaponWaveShot(*gameModel, bossPos, bossToPaddleVec);

            this->countdownToShot = this->GetTimeBetweenBurstWaveShots();
        }
    }
}

void FuturismBossStage3AIState::InitLaserBeamStarAIState() {
    this->DetachAndShootBall(this->ballDirBeforeAttachment);

    this->waitTimeCountdown = this->GetAfterAttackWaitTime(LaserBeamStarAIState);
    this->countdownToShot = this->GetBeamArcShootTime();
    this->weaponWasShot = false;
    this->currMultiBeam.clear();

    this->beamSweepAngularDist = 100.0f + Randomizer::GetInstance()->RandomNumZeroToOne() * 90.0f;
    this->beamSweepSpd = Randomizer::GetInstance()->RandomNegativeOrPositive() * (360.0f / FULL_STAR_BEAM_ROTATION_TIME_IN_SECS);
    
    assert(this->countdownToShot > EPSILON);
    this->currRotationAccel = -this->currRotationSpd / this->countdownToShot;

    // The boss emits a brief flare before firing its laser beam...
    this->DoLaserBeamWarningEffect(1.75*this->countdownToShot, false);

    this->numConsecutiveMoves = 0;
    this->numConsecutiveShots = 0;
    this->numConsecutiveBeams++;
    this->numConsecutiveAttacks++;
}
void FuturismBossStage3AIState::ExecuteLaserBeamStarAIState(double dT, GameModel* gameModel) {
    
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);

    if (this->countdownToShot <= 0) {

        if (this->weaponWasShot) {

            if (this->currMultiBeam.empty()) {

                // All done shooting the beams, slow the rotation back to zero...
                this->currRotationSpd += this->currRotationAccel*dT;
                if ((this->currRotationAccel > 0 && this->currRotationSpd > 0) ||
                    (this->currRotationAccel < 0 && this->currRotationSpd < 0)) {

                    this->currRotationSpd   = 0;
                    this->currRotationAccel = 0;
                }

                this->currCoreRotInDegs += this->currRotationSpd*dT;
                this->currCoreRotInDegs = fmod(this->currCoreRotInDegs, 360.0f);
                partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);

                if (this->currRotationSpd == 0) {
                    // We're done with all the beams, do the wait time and then go to the next state
                    if (this->DoWaitTimeCountdown(dT)) {
                        this->GoToNextState(*gameModel);
                        return;
                    }
                }
            }
            else {
                // Speed up the bosses' rotation to the sweep speed...
                this->currRotationSpd += this->currRotationAccel*dT;
                if ((this->currRotationAccel > 0 && (this->beamSweepSpd < this->currRotationSpd)) ||
                    (this->currRotationAccel < 0 && (this->beamSweepSpd > this->currRotationSpd))) {

                    this->currRotationSpd = this->beamSweepSpd;
                    this->currRotationAccel = 0;
                }

                this->currCoreRotInDegs += this->currRotationSpd*dT;
                this->currCoreRotInDegs = fmod(this->currCoreRotInDegs, 360.0f);
                partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);
                
                const std::list<Beam*>& activeBeams = gameModel->GetActiveBeams();
                for (std::map<BossLaserBeam*, Collision::Ray2D>::iterator iter = this->currMultiBeam.begin(); iter != this->currMultiBeam.end();) {
                    BossLaserBeam* beam = iter->first;

                    // Make sure the beam still exists...
                    if (std::find(activeBeams.begin(), activeBeams.end(), beam) != activeBeams.end()) {
                        // Update the beam...
                        Collision::Ray2D currRay = iter->second;
                        currRay.SetOrigin(partToAnimate->GetWorldTransform() * currRay.GetOrigin());
                        currRay.SetUnitDirection(Vector2D::Normalize(partToAnimate->GetWorldTransform() * currRay.GetUnitDirection()));

                        // Update the beam in the GameModel by modifying the origin ray of the beam...
                        beam->UpdateOriginBeamSegment(gameModel, currRay);
                    }
                    else {
                        iter = this->currMultiBeam.erase(iter);
                        continue;
                    }

                    ++iter;
                }

                if (this->currMultiBeam.empty()) {
                    this->currRotationAccel = -this->currRotationSpd;
                }
            }
        }
        else {
            // Shoot all of the laser beams out of each of the boss' active bulbs...
            double totalBeamTime = fabs(this->beamSweepAngularDist / this->beamSweepSpd);

            // Bit of a hack -- reuse the countdownToShot to hold the beam in place for a brief period of
            // time before sweeping it
            const double HOLD_BEAM_TIME = this->GetBeamArcHoldTime();
            this->countdownToShot = HOLD_BEAM_TIME;
            totalBeamTime += HOLD_BEAM_TIME;

            // Create each of the beams...
            Point2D beamPos = this->boss->GetCoreTopBulbWorldPos();
            Vector2D beamLocalVec = FuturismBoss::GetCoreTopBulbLocalVec();
            this->BuildAndAddBossBulbBeam(*gameModel, *this->boss->GetCoreTopBulb(), 
                Collision::Ray2D(beamPos, Vector2D::Normalize(this->boss->GetCoreTopBulbWorldVec())),
                Collision::Ray2D(Point2D(beamLocalVec[0], beamLocalVec[1]), FuturismBoss::GetCoreTopBulbLocalDir()),
                totalBeamTime);

            beamPos = this->boss->GetCoreBottomBulbWorldPos();
            beamLocalVec = FuturismBoss::GetCoreBottomBulbLocalVec();
            this->BuildAndAddBossBulbBeam(*gameModel, *this->boss->GetCoreBottomBulb(), 
                Collision::Ray2D(beamPos, Vector2D::Normalize(this->boss->GetCoreBottomBulbWorldVec())),
                Collision::Ray2D(Point2D(beamLocalVec[0], beamLocalVec[1]), FuturismBoss::GetCoreBottomBulbLocalDir()),
                totalBeamTime);

            beamPos = this->boss->GetCoreLeftBulbWorldPos();
            beamLocalVec = FuturismBoss::GetCoreLeftBulbLocalVec();
            this->BuildAndAddBossBulbBeam(*gameModel, *this->boss->GetCoreLeftBulb(), 
                Collision::Ray2D(beamPos, Vector2D::Normalize(this->boss->GetCoreLeftBulbWorldVec())), 
                Collision::Ray2D(Point2D(beamLocalVec[0], beamLocalVec[1]), FuturismBoss::GetCoreLeftBulbLocalDir()),
                totalBeamTime);

            beamPos = this->boss->GetCoreRightBulbWorldPos();
            beamLocalVec = FuturismBoss::GetCoreRightBulbLocalVec();
            this->BuildAndAddBossBulbBeam(*gameModel, *this->boss->GetCoreRightBulb(), 
                Collision::Ray2D(beamPos, Vector2D::Normalize(this->boss->GetCoreRightBulbWorldVec())), 
                Collision::Ray2D(Point2D(beamLocalVec[0], beamLocalVec[1]), FuturismBoss::GetCoreRightBulbLocalDir()),
                totalBeamTime);

            // Show the boss charging while shooting the beam...
            this->DoLaserBeamEnergyEffect(totalBeamTime);

            // EVENT: Flash for all the laser beams
            GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.3, 0.0f));

            this->weaponWasShot = true;

            // Ensure that the rotation of the boss is now zero, upon shooting the beams we'll be ramping up
            // to the full sweep speed over the hold beam time...
            this->currRotationSpd = 0;
            this->currRotationAccel = this->beamSweepSpd / HOLD_BEAM_TIME;
        }
    }
    else {
        this->countdownToShot -= dT;

        if (!this->weaponWasShot) {
            this->currRotationSpd += this->currRotationAccel*dT;
            // Slow down the rotation speed of the boss to zero...
            if ((this->currRotationAccel > 0 && this->currRotationSpd > 0) || (this->currRotationAccel < 0 && this->currRotationSpd < 0)) {
                this->currRotationSpd = 0;
            }
            this->currCoreRotInDegs += this->currRotationSpd*dT;
            this->currCoreRotInDegs = fmod(this->currCoreRotInDegs, 360.0f);
            partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);
        }
    }
}

void FuturismBossStage3AIState::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);

    if (!this->hasDestroyedBarrier || this->attachedBall == &ball || 
        this->currState == BulbHitAndDestroyedAIState || this->currState == AngryAIState || 
        this->currState == FinalDeathThroesAIState || this->currState == DestroyLevelBarrierAIState) {

        return;
    }

    // Ball instantly destroys any bulb it comes into contact with
    if (collisionPart == this->boss->GetCoreTopBulb() && !this->boss->GetCoreTopBulb()->GetIsDestroyed()) {
        this->DestroyBulb(this->boss->topBulbIdx, ball.GetDirection());
        return;
    }
    else if (collisionPart == this->boss->GetCoreBottomBulb() && !this->boss->GetCoreBottomBulb()->GetIsDestroyed()) {
        this->DestroyBulb(this->boss->bottomBulbIdx, ball.GetDirection());
        return;
    }
    else if (collisionPart == this->boss->GetCoreLeftBulb() && !this->boss->GetCoreLeftBulb()->GetIsDestroyed()) {
        this->DestroyBulb(this->boss->leftBulbIdx, ball.GetDirection());
        return;
    }
    else if (collisionPart == this->boss->GetCoreRightBulb() && !this->boss->GetCoreRightBulb()->GetIsDestroyed()) {
        this->DestroyBulb(this->boss->rightBulbIdx, ball.GetDirection());
        return;
    }

    // Special case:
    // If the boss is frozen then we check to see how close the ball is to any particular, non-destroyed bulb;
    // if the ball is close enough then we consider that bulb destroyed.
    if (this->currState == FrozenAIState) {
        
        static const float RADIUS_COEFF = 1.1f;

        // Is the ball close enough to any of the non-destroyed bulbs?
        int bulbIdx = -1;
        if (!this->boss->GetCoreTopBulb()->GetIsDestroyed()) {
            Collision::Circle2D bulbCircle = this->boss->GetCoreTopBulb()->GenerateWorldCircleBounds();
            bulbCircle.SetRadius(RADIUS_COEFF*bulbCircle.Radius());
            if (Collision::IsCollision(ball.GetBounds(), bulbCircle)) {
                bulbIdx = this->boss->topBulbIdx;
            }
        }
        if (bulbIdx == -1 && !this->boss->GetCoreBottomBulb()->GetIsDestroyed()) {
            Collision::Circle2D bulbCircle = this->boss->GetCoreBottomBulb()->GenerateWorldCircleBounds();
            bulbCircle.SetRadius(RADIUS_COEFF*bulbCircle.Radius());
            if (Collision::IsCollision(ball.GetBounds(), bulbCircle)) {
                bulbIdx = this->boss->bottomBulbIdx;
            }
        }
        if (bulbIdx == -1 && !this->boss->GetCoreLeftBulb()->GetIsDestroyed()) {
            Collision::Circle2D bulbCircle = this->boss->GetCoreLeftBulb()->GenerateWorldCircleBounds();
            bulbCircle.SetRadius(RADIUS_COEFF*bulbCircle.Radius());
            if (Collision::IsCollision(ball.GetBounds(), bulbCircle)) {
                bulbIdx = this->boss->leftBulbIdx;
            }
        }
        if (bulbIdx == -1 && !this->boss->GetCoreRightBulb()->GetIsDestroyed()) {
            Collision::Circle2D bulbCircle = this->boss->GetCoreRightBulb()->GenerateWorldCircleBounds();
            bulbCircle.SetRadius(RADIUS_COEFF*bulbCircle.Radius());
            if (Collision::IsCollision(ball.GetBounds(), bulbCircle)) {
                bulbIdx = this->boss->rightBulbIdx;
            }
        }

        if (bulbIdx != -1) {
            this->DestroyBulb(bulbIdx, ball.GetDirection());
        }
        else {
            // No matter what we need to go to another state (the ice has been shattered)...
            this->avoidanceHaltingTime = 0.0;
            this->DoIceShatterIfFrozen();
            this->GoToNextState(*gameModel);
        }
    }
}

void FuturismBossStage3AIState::CollisionOccurred(GameModel* gameModel, Projectile* projectile,
                                                  BossBodyPart* collisionPart) {
    assert(gameModel != NULL);
    assert(projectile != NULL);
    assert(collisionPart != NULL);

    // Check the type of projectile, only ice blasts should be available for affecting the boss in this way.
    // Rockets are dealt with in the "RocketExplosionOccurred" method.
    bool isIceBlast = projectile->GetType() == Projectile::PaddleIceBlastProjectile;
    if (!isIceBlast || !this->hasDestroyedBarrier || this->currState == FrozenAIState || 
        this->currState == BulbHitAndDestroyedAIState || this->currState == AngryAIState || 
        this->currState == FinalDeathThroesAIState || this->currState == DestroyLevelBarrierAIState) {

        return;
    }

    if (collisionPart == this->boss->GetCoreBody() || collisionPart == this->boss->GetCoreTopBulb() || 
        collisionPart == this->boss->GetCoreBottomBulb() || collisionPart == this->boss->GetCoreLeftBulb() ||
        collisionPart == this->boss->GetCoreRightBulb()) {

        // If we're dealing with an ice blast then it's going to freeze the boss, but don't hurt it
        this->SetState(FrozenAIState);
    }
}

void FuturismBossStage3AIState::RocketExplosionOccurred(GameModel* gameModel, const RocketProjectile* rocket) {
    UNUSED_PARAMETER(gameModel);

    if (!this->hasDestroyedBarrier || this->currState == BulbHitAndDestroyedAIState || 
        this->currState == AngryAIState || this->currState == FinalDeathThroesAIState || 
        this->currState == DestroyLevelBarrierAIState) {
        return;
    }

    static const float DEFAULT_EXPLOSION_RADIUS = LevelPiece::PIECE_WIDTH;
    float rocketSizeFactor = rocket->GetHeight() / rocket->GetDefaultHeight();
    Collision::Circle2D explosionCircle(rocket->GetPosition(), rocketSizeFactor*DEFAULT_EXPLOSION_RADIUS);
    
    // Check to see if the explosion hit any of the bulbs, if it did find out which one was closest and destroy it...
    int closestBulbIdx = -1;
    float smallestDistFromExplosion = std::numeric_limits<float>::max();
    float tempDistFromExplosion;
    
    if (!this->boss->GetCoreTopBulb()->GetIsDestroyed() && 
        this->boss->GetCoreTopBulb()->GetWorldBounds().CollisionCheck(explosionCircle, tempDistFromExplosion)) {

        if (tempDistFromExplosion < smallestDistFromExplosion) {
            smallestDistFromExplosion = tempDistFromExplosion;
            closestBulbIdx = static_cast<int>(this->boss->topBulbIdx);
        }
    }
    if (!this->boss->GetCoreBottomBulb()->GetIsDestroyed() && 
        this->boss->GetCoreBottomBulb()->GetWorldBounds().CollisionCheck(explosionCircle, tempDistFromExplosion)) {

        if (tempDistFromExplosion < smallestDistFromExplosion) {
            smallestDistFromExplosion = tempDistFromExplosion;
            closestBulbIdx = static_cast<int>(this->boss->bottomBulbIdx);
        }
    }

    if (!this->boss->GetCoreLeftBulb()->GetIsDestroyed() && 
        this->boss->GetCoreLeftBulb()->GetWorldBounds().CollisionCheck(explosionCircle, tempDistFromExplosion)) {

        if (tempDistFromExplosion < smallestDistFromExplosion) {
            smallestDistFromExplosion = tempDistFromExplosion;
            closestBulbIdx = static_cast<int>(this->boss->leftBulbIdx);
        }
    }

    if (!this->boss->GetCoreRightBulb()->GetIsDestroyed() && 
        this->boss->GetCoreRightBulb()->GetWorldBounds().CollisionCheck(explosionCircle, tempDistFromExplosion)) {

        if (tempDistFromExplosion < smallestDistFromExplosion) {
            smallestDistFromExplosion = tempDistFromExplosion;
            closestBulbIdx = static_cast<int>(this->boss->rightBulbIdx);
        }
    }

    if (closestBulbIdx != -1) {
        this->DestroyBulb(closestBulbIdx, rocket->GetVelocityDirection());
    }
    else if (this->currState == FrozenAIState) {
        // If the rocket hit any part of the boss, unfreeze the boss...
        if (this->boss->GetCoreBody()->GetWorldBounds().CollisionCheck(explosionCircle)) {
            this->DoIceShatterIfFrozen();
            this->GoToNextState(*gameModel);
        }
    }
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

bool FuturismBossStage3AIState::BallAboutToHitBulbWeakpoints(const GameModel& gameModel) const {
    const std::list<GameBall*>& balls = gameModel.GetGameBalls();
    if (balls.empty()) {
        assert(false);
        return false;
    }

    const GameBall& ball = *balls.front();

    Vector2D relativeBallVel = ball.GetVelocity() - this->currVel;
    float relativeBallSpd    = relativeBallVel.Magnitude();
    if (relativeBallSpd <= EPSILON) {
        return false;
    }

    float ballMoveMag = AVOIDANCE_PREDICTION_LOOKAHEAD_TIME_IN_SECS*relativeBallSpd;
    Vector2D relativeBallDir = relativeBallVel / relativeBallSpd;
    Vector2D perpVec = ball.GetBounds().Radius()*Vector2D::Rotate(90, relativeBallDir);

    Point2D ballSideAPt = ball.GetCenterPosition2D() + perpVec;
    Point2D ballSideBPt = ball.GetCenterPosition2D() - perpVec;

    // Check to see if the ball is in the general vicinity of the boss or will be 
    // when projected by the prediction lookahead time
    Collision::AABB2D bossBounds = this->boss->alivePartsRoot->GenerateWorldAABB();

    return Collision::IsCollision(Collision::LineSeg2D(ball.GetCenterPosition2D(), 
        ball.GetCenterPosition2D() + ballMoveMag*relativeBallDir), bossBounds) ||
        Collision::IsCollision(Collision::LineSeg2D(ballSideAPt, 
        ballSideAPt + ballMoveMag*relativeBallDir), bossBounds) ||
        Collision::IsCollision(Collision::LineSeg2D(ballSideBPt, 
        ballSideBPt + ballMoveMag*relativeBallDir), bossBounds);
}

void FuturismBossStage3AIState::BuildAndAddBossBulbBeam(GameModel& gameModel, const BossBodyPart& bulb, 
                                                        const Collision::Ray2D& initBeamRay,
                                                        const Collision::Ray2D& noRotBeamRay, 
                                                        double beamLifeTime) {
    // Destroyed bulbs don't shoot beams
    if (bulb.GetIsDestroyed()) {
        return;
    }

    BossLaserBeam* beam = new BossLaserBeam(&gameModel, initBeamRay, FuturismBoss::CORE_BULB_HALF_SIZE, beamLifeTime);
    beam->SetZOffset(this->boss->GetCurrArmZOffset());
    this->currMultiBeam.insert(std::make_pair(beam, noRotBeamRay));

    gameModel.AddBeam(beam); // WARNING: The beam no longer belongs to this after this call!!!!
}

void FuturismBossStage3AIState::DestroyBulb(size_t bulbIdx, const Vector2D& hitDir) {
    AbstractBossBodyPart* bulbObj = this->boss->bodyParts[bulbIdx];
    if (bulbObj == NULL || bulbObj->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
        assert(false);
        return;
    }

    this->avoidanceHaltingTime = 0.0;
    
    this->boss->ConvertAliveBodyPartToWeakpoint(bulbIdx, 1, 0);

    assert(dynamic_cast<BossWeakpoint*>(this->boss->bodyParts[bulbIdx]) != NULL);
    BossWeakpoint* bulb = static_cast<BossWeakpoint*>(this->boss->bodyParts[bulbIdx]);
    bulb->SetDestroyed(true);

    this->bossHurtAnim = Boss::BuildBossHurtMoveAnim(hitDir, FuturismBoss::CORE_BOSS_SIZE/4.0f, 
        BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS);

    GameEventManager::Instance()->ActionBossHurt(bulb);

    this->DoIceShatterIfFrozen();
    this->SetState(BulbHitAndDestroyedAIState);
}

void FuturismBossStage3AIState::DoBasicRotation(double dT) {
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);
    this->currCoreRotInDegs += this->currRotationSpd*dT;
    this->currCoreRotInDegs = fmod(this->currCoreRotInDegs, 360.0f);
    partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);
}