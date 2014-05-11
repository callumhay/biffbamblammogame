/**
 * FuturismBossAIState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#include "FuturismBossAIState.h"
#include "FuturismBoss.h"
#include "GameModel.h"
#include "PortalProjectile.h"
#include "BossLaserBeam.h"
#include "PortalSpawnEffectInfo.h"
#include "SummonPortalsEffectInfo.h"
#include "PowerChargeEffectInfo.h"
#include "FullscreenFlashEffectInfo.h"
#include "BossTeleportEffectInfo.h"
#include "EnumBossEffectInfo.h"
#include "ExpandingHaloEffectInfo.h"
#include "DebrisEffectInfo.h"
#include "EnumGeneralEffectInfo.h"
#include "BossLaserProjectile.h"
#include "BossWeakpoint.h"

#include "../GameSound/GameSound.h"

// Generic effect prototyping
#include "GenericEmitterEffectInfo.h"
#include "../ESPEngine/ESP.h"
#include "../GameView/PersistentTextureManager.h"
#include "../GameView/GameViewConstants.h"

const float FuturismBossAIState::DEFAULT_ACCELERATION = 0.1f * PlayerPaddle::DEFAULT_ACCELERATION;
const float FuturismBossAIState::DEFAULT_SPEED = 5.0f;

const float FuturismBossAIState::FINAL_STATE_ROT_VEL = 180.0f;

const float FuturismBossAIState::SPAWNED_PORTAL_WIDTH       = 0.9f*LevelPiece::PIECE_WIDTH;
const float FuturismBossAIState::SPAWNED_PORTAL_HEIGHT      = 0.9f*LevelPiece::PIECE_HEIGHT;
const float FuturismBossAIState::SPAWNED_PORTAL_HALF_WIDTH  = FuturismBossAIState::SPAWNED_PORTAL_WIDTH / 2.0f;
const float FuturismBossAIState::SPAWNED_PORTAL_HALF_HEIGHT = FuturismBossAIState::SPAWNED_PORTAL_HEIGHT / 2.0f;

const float FuturismBossAIState::EYE_BEAM_HALF_RADIUS = 0.75f*FuturismBoss::CORE_EYE_HALF_SIZE;

const double FuturismBossAIState::BOSS_PORTAL_TERMINATION_TIME_IN_SECS = 10.0;
const double FuturismBossAIState::PORTAL_SPAWN_EFFECT_TIME_IN_SECS = 1.0;

const double FuturismBossAIState::TWITCH_BEAM_EXPIRE_TIME_IN_SECS = 0.9;
const double FuturismBossAIState::DEFAULT_FROZEN_TIME_IN_SECS = 8.0;

const double FuturismBossAIState::TIME_UNTIL_BARRIER_DESTRUCTION_SHOT_IN_SECS = 2.5;
const double FuturismBossAIState::BARRIER_DESTRUCTION_ARC_TIME_IN_SECS = 3.4;
const double FuturismBossAIState::AVOIDANCE_PREDICTION_LOOKAHEAD_TIME_IN_SECS = 0.2;

const double FuturismBossAIState::COOL_DOWN_SFX_TIME_IN_SECS = 1.3;

const float FuturismBossAIState::SHIELD_LIFE_POINTS = FLT_MAX;
const float FuturismBossAIState::SHIELD_BALL_DAMAGE = 0.0f;

#define BASIC_WEAPON_EFFECT_DISCHARGE_TIME 0.5
#define BASIC_WEAPON_EFFECT_DISCHARGE_COLOUR Colour(0.86f, 0.076f, 0.235f)

FuturismBossAIState::FuturismBossAIState(FuturismBoss* boss): BossAIState(), boss(boss), 
countdownToPortalShot(-1), weaponWasShot(false), arenaState(InLeftArena),
timeSinceLastStratPortal(0), timeSinceLastAttackPortal(0), currBeam(NULL), 
numBasicShotsToFire(0), numConsecutiveMoves(0), numConsecutiveBeams(0), 
numConsecutiveShots(0), numConsecutiveAttacks(0), attachedBall(NULL),
iceShakeSoundID(INVALID_SOUND_ID), chargingSoundID(INVALID_SOUND_ID), 
attractorBeamLoopSoundID(INVALID_SOUND_ID), spinCoolDownSoundID(INVALID_SOUND_ID), 
currCoreRotInDegs(0.0f)  {
}

FuturismBossAIState::~FuturismBossAIState() {
}

Boss* FuturismBossAIState::GetBoss() const {
    return this->boss;
}

bool FuturismBossAIState::IsStateMachineFinished() const { 
    return (this->currState == FinalDeathThroesAIState || 
        (this->currState == BulbHitAndDestroyedAIState && this->boss->AreAllBulbsDestroyed()));
}

Collision::AABB2D FuturismBossAIState::GenerateDyingAABB() const {
    return this->boss->GetCoreBody()->GenerateWorldAABB();
}

void FuturismBossAIState::InitPauseAIState(double pauseTime) {
    this->waitTimeCountdown = pauseTime;
}
void FuturismBossAIState::ExecutePauseAIState(double dT, GameModel* gameModel) {
    this->waitTimeCountdown -= dT;
    if (this->waitTimeCountdown <= 0) {
        this->GoToNextState(*gameModel);
    }
}

void FuturismBossAIState::InitMoveToPositionAIState() {
    const GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Figure out a good position and move to it...
    std::vector<Point2D> moveToPositions;
    this->GetRandomMoveToPositions(*gameModel, moveToPositions);

    if (moveToPositions.empty()) {
        this->InitMoveAIState(this->boss->alivePartsRoot->GetTranslationPt2D());
    }
    else {
        // Choose a position at random...
        int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(moveToPositions.size());
        this->InitMoveAIState(moveToPositions[randomIdx]);
    }
}

void FuturismBossAIState::InitMoveToCenterAIState() {
    this->InitMoveAIState(this->GetCenterMovePosition());
}

void FuturismBossAIState::InitMoveAIState(const Point2D& position) {
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);
    partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);

    this->DetachAndShootBall(this->ballDirBeforeAttachment);
    this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), position);

    this->numConsecutiveMoves++;
    this->numConsecutiveAttacks = 0;
    this->numConsecutiveBeams = 0;
    this->numConsecutiveShots = 0;
}

void FuturismBossAIState::ExecuteMoveToPositionAIState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);

    if (this->MoveToTargetPosition(this->GetMaxSpeed())) {
        // Finished moving to the target position...
        this->GoToNextState(*gameModel);
        return;
    }
}

void FuturismBossAIState::InitTeleportAIState() {
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);
    partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);

    this->countdownToTeleportOut = this->GetSingleTeleportInAndOutTime();
    this->countdownToTeleportIn = this->countdownToTeleportOut;

    // If the ball is attached then the teleport in will be shorter...
    if (this->attachedBall != NULL) {

        GameModel* gameModel = this->boss->GetGameModel();
        assert(gameModel != NULL);
        const std::list<GameBall*>& balls = gameModel->GetGameBalls();
        assert(!balls.empty());
        GameBall* ball = balls.front();
        assert(ball != NULL);
        
        if (ball == this->attachedBall) {
            this->countdownToTeleportIn = this->countdownToTeleportOut / 2.0;
            this->ballPosBeforeAttachment = ball->GetCenterPosition2D();
        }
        else {
            this->attachedBall = NULL;
        }
    }

    Point3D bossPos = this->boss->alivePartsRoot->GetPosition3D();
    Collision::AABB2D bossAABB = this->boss->alivePartsRoot->GenerateWorldAABB();

    GameEventManager::Instance()->ActionBossEffect(
        BossTeleportEffectInfo(BossTeleportEffectInfo::TeleportingOut, 
        this->boss->alivePartsRoot->GetTranslationPt2D(), this->countdownToTeleportOut,
        std::max<float>(bossAABB.GetWidth(), bossAABB.GetHeight())));

    AnimationMultiLerp<ColourRGBA> fadeToBlackAnim;
    fadeToBlackAnim.SetLerp(this->countdownToTeleportOut, ColourRGBA(0,0,0,0));
    this->boss->alivePartsRoot->AnimateColourRGBA(fadeToBlackAnim);

    // Play teleportation "charging" sound...
    this->chargingSoundID = this->boss->GetGameModel()->GetSound()->PlaySound(
        GameSound::FuturismBossTeleportationChargingEvent, false, true);

    this->numConsecutiveMoves++;
    this->numConsecutiveAttacks = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveShots   = 0;
}

void FuturismBossAIState::InitAvoidanceTeleportAIState() {
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);
    partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);

    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Cancel all effects from any other previous state...
    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);
    this->CancelActiveBeam(*gameModel);
    this->boss->alivePartsRoot->ClearLocalTranslationAnimation();
    this->boss->alivePartsRoot->ResetColourRGBAAnimation();

    this->InterruptSounds(*gameModel);

    this->countdownToTeleportOut = 0.05;
    this->countdownToTeleportIn = this->countdownToTeleportOut;

    Collision::AABB2D bossAABB = this->boss->alivePartsRoot->GenerateWorldAABB();
    
    GameEventManager::Instance()->ActionClearActiveBossEffects();
    GameEventManager::Instance()->ActionBossEffect(
        BossTeleportEffectInfo(BossTeleportEffectInfo::FastTeleportOut, 
        this->boss->alivePartsRoot->GetTranslationPt2D(), 0.25,
        std::max<float>(bossAABB.GetWidth(), bossAABB.GetHeight())));

    // NOTE: No teleportation charging sound: It will be a very fast teleport, no charging required

    this->numConsecutiveMoves++;
    this->numConsecutiveAttacks = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveShots   = 0;
}

void FuturismBossAIState::ExecuteTeleportAIState(double dT, GameModel* gameModel) {
    
    if (this->countdownToTeleportOut <= 0) {

        if (this->countdownToTeleportIn <= 0) {
            // Done teleporting in!
            this->boss->alivePartsRoot->ResetColourRGBAAnimation();
            if (this->attachedBall != NULL) {
                this->SetState(BallDiscardAIState);
            }
            else {
                this->GoToNextState(*gameModel);
            }
            return;
        }
        else {
            this->countdownToTeleportIn -= dT;
        }
    }
    else {
        this->countdownToTeleportOut -= dT;
        if (this->countdownToTeleportOut <= 0) {

            // Teleport the boss (if possible)
            Collision::AABB2D bossAABB = this->boss->alivePartsRoot->GenerateWorldAABB();
            Point2D currPos = this->boss->alivePartsRoot->GetTranslationPt2D();
            float bossSize = std::max<float>(bossAABB.GetWidth(), bossAABB.GetHeight());
            
            this->boss->alivePartsRoot->ResetColourRGBAAnimation();

            Point2D teleportPos;
            if (this->GetTeleportationLocation(*gameModel, teleportPos)) {

                // The boss is now teleporting...
               
                // Stop charging and play teleportation sound...
                GameSound* sound = gameModel->GetSound();
                sound->StopSound(this->chargingSoundID, 0.1);
                sound->PlaySound(GameSound::FuturismBossTeleportationEvent, false);

                // Do visual effects
                GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.4, 0.0f));
                GameEventManager::Instance()->ActionBossEffect(
                    BossTeleportEffectInfo(BossTeleportEffectInfo::TeleportingIn, teleportPos,
                    this->countdownToTeleportIn, bossSize));

                // Move the boss to the new location
                Vector2D translationVec = teleportPos - currPos;

                // If the ball is attached, move it with the boss
                if (this->attachedBall != NULL) {
                    Vector2D bossToBallVec = this->attachedBall->GetCenterPosition2D() - currPos;
                    this->attachedBall->SetCenterPosition(teleportPos + bossToBallVec);

                    // Make sure the ball is not colliding with level pieces, if it is rotate it around
                    // the teleport position until it's not
                    float randomSign = Randomizer::GetInstance()->RandomNegativeOrPositive();
                    float rotationDegs = randomSign*30.0f;
                    while (gameModel->GetCurrentLevel()->IsCollidingWithLevelPieces(this->attachedBall->GetCenterPosition2D(), 
                           this->attachedBall->GetBounds().Radius()) && fabs(rotationDegs) < 360.0f) {

                        bossToBallVec.Rotate(rotationDegs);
                        this->attachedBall->SetCenterPosition(teleportPos + bossToBallVec);
                        rotationDegs += randomSign*30.0f;
                    }

                    this->ballPosBeforeAttachment = this->attachedBall->GetCenterPosition2D();
                }

                this->boss->alivePartsRoot->Translate(Vector3D(translationVec, 0));
                assert(this->countdownToTeleportIn > 0);

                // Make sure the bosses' arena location changes if needed...
                if (this->arenaState != InFullyOpenedArena && this->ArenaBarrierExists()) {
                    if (FuturismBoss::IsInLeftSubArena(teleportPos)) {
                        this->arenaState = InLeftArena;
                    }
                    else {
                        assert(FuturismBoss::IsInRightSubArena(teleportPos));
                        this->arenaState = InRightArena;
                    }
                }
            }
            else {
                // The boss failed to teleport...
                // Show a visual effect to indicate that the teleportation failed
                GameEventManager::Instance()->ActionBossEffect(
                    BossTeleportEffectInfo(BossTeleportEffectInfo::TeleportFailed, currPos, 0.0, bossSize));

                gameModel->GetSound()->StopSound(this->attractorBeamLoopSoundID);
                this->attractorBeamLoopSoundID = INVALID_SOUND_ID;

                this->DetachAndShootBall(this->ballDirBeforeAttachment);
                this->GoToNextState(*gameModel);
                return;
            }
        }
    }

    // If the ball is attached, continue to make it look like it's struggling...
    if (this->attachedBall != NULL) {
        this->attachedBall->SetCenterPosition(this->ballPosBeforeAttachment);
        this->ShakeBall(this->attachedBall);
    }
}

void FuturismBossAIState::InitBallAttractAIState() {
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);
    partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);

    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    this->DetachAndShootBall(this->ballDirBeforeAttachment);
    this->CloseAllActivePortals(*gameModel);

    this->totalBallAttractTime = this->GetBallAttractTime();
    this->ballAttractTimeCount = 0.0;
    this->attachedBall = NULL;

    // Setup rotation of the bosses' core...
    this->SetupCoreRotationVelAnim(this->totalBallAttractTime);

    // Play sounds
    GameSound* sound = gameModel->GetSound();
    this->attractorBeamLoopSoundID = sound->PlaySound(GameSound::FuturismBossAttractorBeamEvent, false, true);

    this->numConsecutiveMoves   = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveAttacks = 0;
    this->numConsecutiveShots   = 0;
}
void FuturismBossAIState::ExecuteBallAttractAIState(double dT, GameModel* gameModel) {

    // Attract the ball (if it's available to be attracted...)
    if (!this->IsBallAvailableForAttractingAndTeleporting(*gameModel)) {

        gameModel->GetSound()->StopSound(this->attractorBeamLoopSoundID);
        this->attractorBeamLoopSoundID = INVALID_SOUND_ID;

        // Unable to attract the ball if the ball is not in play or visible to the boss... 
        // Exit this state immediately
        this->DetachAndShootBall(this->ballDirBeforeAttachment);
        this->GoToNextState(*gameModel);
        return;
    }

    const std::list<GameBall*>& balls = gameModel->GetGameBalls();
    assert(!balls.empty());
    GameBall* ball = balls.front();
    assert(ball != NULL);

    // When this state is initially executed we need to start by 
    // "Attaching" the ball to the boss, it no longer belongs in play...
    if (this->attachedBall == NULL) {
        this->AttachBall(ball);
    }

    // This should never happen, but just in case...
    if (this->attachedBall != ball) {
        assert(false);
        gameModel->GetSound()->StopSound(this->attractorBeamLoopSoundID);
        this->attractorBeamLoopSoundID = INVALID_SOUND_ID;
        this->DetachAndShootBall(this->ballDirBeforeAttachment);
        this->GoToNextState(*gameModel);
        return;
    }

    // Move the ball towards the boss so that it will be in a good position before
    // this state ends...
    this->ballAttractTimeCount += dT;
    
    // Figure out a suitable radius around the boss that the ball should not move into...
    Collision::Circle2D bossCircleBound = this->boss->alivePartsRoot->GenerateWorldCircleBounds();
    bossCircleBound.SetRadius(bossCircleBound.Radius() + 2*GameBall::DEFAULT_BALL_RADIUS);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    Vector2D ballToBossVec = bossPos - ball->GetCenterPosition2D();
    float ballToBossDist   = ballToBossVec.Magnitude();
    if (ballToBossDist < EPSILON) {
        assert(false);
        this->DetachAndShootBall(this->ballDirBeforeAttachment);
        this->GoToNextState(*gameModel);
        return;
    }

    if (ballToBossDist > bossCircleBound.Radius()) {
        Vector2D originalBallToBossVec = bossPos - this->ballPosBeforeAttachment;
        Vector2D ballAttractVel = originalBallToBossVec / this->totalBallAttractTime;

        // Only attract the ball if the ball is outside the general radius of the bosses' bounding circle
        Point2D newBallPos = this->ballPosBeforeAttachment + this->ballAttractTimeCount*(ballAttractVel);
        ball->SetCenterPosition(newBallPos);

        ballToBossVec = bossPos - newBallPos;
        ballToBossDist = ballToBossVec.Magnitude();
    }

    if (ballToBossDist < bossCircleBound.Radius()) {
        Vector2D ballToBossDir = bossPos - this->ballPosBeforeAttachment;
        ballToBossDir.Normalize();

        // Make sure the ball is exactly where it should be
        float diff = bossCircleBound.Radius() - ballToBossDist;
        ball->SetCenterPosition(ball->GetCenterPosition2D() - diff*ballToBossDir);
    }

    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);

    bool isFinished = (ballToBossDist <= bossCircleBound.Radius()) && 
        (this->ballAttractTimeCount > 0.75*this->totalBallAttractTime);
    isFinished |= this->coreRotVelAnim.Tick(dT);

    if (isFinished) {
        // Move on to the teleportation state with the ball...
        this->SetState(TeleportAIState);
        return;
    }
    else {
        this->currCoreRotInDegs += dT*this->coreRotVelAnim.GetInterpolantValue();
        this->currCoreRotInDegs = fmod(this->currCoreRotInDegs, 360.0f);
        partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);
    }

    // Make the ball looks like it's struggling...
    this->ShakeBall(ball);
}

void FuturismBossAIState::InitBallDiscardAIState() {
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);
    partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);

    this->waitTimeCountdown = this->GetBallDiscardTime();

    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);
    const std::list<GameBall*>& balls = gameModel->GetGameBalls();
    assert(!balls.empty());
    GameBall* ball = balls.front();
    assert(ball != NULL);

    if (this->attachedBall == ball) {
        this->ballPosBeforeAttachment = ball->GetCenterPosition2D();
    }

    // Fade out the attraction loop
    GameSound* sound = gameModel->GetSound();
    sound->StopSound(this->attractorBeamLoopSoundID, this->waitTimeCountdown);
    this->attractorBeamLoopSoundID = INVALID_SOUND_ID;
    // Spin cool-down sound
    sound->PlaySound(GameSound::FuturismBossSpinCoolDownEvent, false, true);

    this->coreRotVelAnim.SetLerp(this->waitTimeCountdown, 0.0f);
}
void FuturismBossAIState::ExecuteBallDiscardAIState(double dT, GameModel* gameModel) {
   
    // Make sure the ball is still attached properly...
    const std::list<GameBall*>& balls = gameModel->GetGameBalls();
    assert(!balls.empty());
    GameBall* ball = balls.front();
    assert(ball != NULL);

    if (this->attachedBall != ball) {
        assert(false);
        // This is not good... we should never be here...
        this->GoToNextState(*gameModel);
        return;
    }
    
    // Keep rotating the core...
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);
    this->coreRotVelAnim.Tick(dT);
    this->currCoreRotInDegs += dT*this->coreRotVelAnim.GetInterpolantValue();
    this->currCoreRotInDegs = fmod(this->currCoreRotInDegs, 360.0f);
    partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);

    ball->SetCenterPosition(this->ballPosBeforeAttachment);
    if (this->DoWaitTimeCountdown(dT)) {
        
        Vector2D bossToBallDir = ball->GetCenterPosition2D() - this->boss->alivePartsRoot->GetTranslationPt2D();
        bossToBallDir.Rotate(Randomizer::GetInstance()->RandomNegativeOrPositive() * 15.0f);
        bossToBallDir.Normalize();

        this->DetachAndShootBall(bossToBallDir);
        this->GoToNextState(*gameModel);
        return;
    }
    else {
        // Make the ball continue to shake...
        this->ShakeBall(ball);
    }
}

void FuturismBossAIState::InitFrozenAIState() {
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);
    partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);

    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    this->CancelActiveBeam(*gameModel);
    GameEventManager::Instance()->ActionClearActiveBossEffects();

    // When frozen all attacks are immediately canceled and the boss cannot move
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    if (this->attachedBall != NULL) {
        const std::list<GameBall*>& balls = gameModel->GetGameBalls();
        if (!balls.empty() && balls.front() == this->attachedBall) {

            Point2D ballPos = this->attachedBall->GetCenterPosition2D();
            Vector2D shootDir = ballPos - bossPos;
            shootDir.Normalize();

            this->DetachAndShootBall(shootDir);
        }
    }

    this->waitTimeCountdown = 0.0;
    this->frozenTimeCountdown = this->GetFrozenTime();
    this->frozenShakeEffectCountdown = 0.0;
    this->iceShakeSoundID = INVALID_SOUND_ID;

    // Clear local transforms and animations that might be relevant to other states
    this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));
    this->boss->alivePartsRoot->ClearLocalTranslationAnimation();
    this->boss->alivePartsRoot->ResetColourRGBAAnimation();

    this->boss->currIceRotInDegs += (1 + Randomizer::GetInstance()->RandomUnsignedInt() % 3) * 90;

    // EVENT: Effect of ice clouds and vapour around the boss to complement the freezing effect
    EnumBossEffectInfo frozenEffect(EnumBossEffectInfo::FrozenIceClouds);
    frozenEffect.SetBodyPart(this->boss->GetCoreBody());
    frozenEffect.SetTimeInSecs(2.0);
    GameEventManager::Instance()->ActionBossEffect(frozenEffect);

    // EFFECT: Very brief full screen flash for the freeze
    GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.125, 0.0f));

    // Play sound for the boss being frozen
    GameSound* sound = gameModel->GetSound();
    sound->PlaySound(GameSound::BossFrozenEvent, false, true);

    this->InterruptSounds(*gameModel);
}

void FuturismBossAIState::ExecuteFrozenAIState(double dT, GameModel* gameModel) {
    // Start shaking the boss (boss is struggling to be free of the ice!) close to the end
    // of being frozen...

    double totalTimeFrozen = this->GetFrozenTime();
    const double shakeTime = 0.4*totalTimeFrozen;
    if (this->frozenTimeCountdown <= shakeTime) {

        if (this->frozenTimeCountdown <= 0) {
            // The boss has broken free of its icy cage!?!
            this->DoIceShatterIfFrozen();

            // When the boss breaks free there's a very brief full screen flash
            GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.15, 0.5f));

            // Reset the bosses' position and go to the next state
            this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));
            this->GoToNextState(*gameModel);
            return;
        }
        else {
            // Violence of the shaking is based on how close the boss is to being free...
            float shakeViolence = NumberFuncs::LerpOverTime<float>(shakeTime, totalTimeFrozen, 0.05f, 0.1f, this->frozenTimeCountdown);
            static int SHAKE_SIGN = Randomizer::GetInstance()->RandomNegativeOrPositive();
            this->boss->alivePartsRoot->SetLocalTranslation(
                Vector3D(SHAKE_SIGN*(0.01f + Randomizer::GetInstance()->RandomNumZeroToOne()*shakeViolence), 
                         Randomizer::GetInstance()->RandomNumNegOneToOne()*0.25*shakeViolence, 0.0f));
            SHAKE_SIGN *= -1;

            // Effect for bits of ice coming off the boss as it struggles...
            // We use the wait time countdown to count time between pulsing the effect
            this->frozenShakeEffectCountdown -= dT;
            if (this->frozenShakeEffectCountdown <= 0) {
                double timeUntilNextEffect = 0.4 + Randomizer::GetInstance()->RandomNumZeroToOne()*0.15;

                // EVENT: Shake debris effect
                EnumBossEffectInfo shakeDebrisInfo(EnumBossEffectInfo::FrozenShakeDebris);
                shakeDebrisInfo.SetBodyPart(this->boss->GetCoreBody());
                shakeDebrisInfo.SetTimeInSecs(2.5*timeUntilNextEffect);
                shakeDebrisInfo.SetSize1D(FuturismBoss::CORE_BOSS_SIZE);

                GameEventManager::Instance()->ActionBossEffect(shakeDebrisInfo);

                this->frozenShakeEffectCountdown = timeUntilNextEffect;
            }

            // Sound for the boss struggling in ice...
            if (this->iceShakeSoundID == INVALID_SOUND_ID) {
                this->iceShakeSoundID = gameModel->GetSound()->PlaySound(GameSound::BossIceShakeEvent, false, true, 0.9f);
            }
            else if (!gameModel->GetSound()->IsSoundPlaying(this->iceShakeSoundID)) {
                this->iceShakeSoundID = INVALID_SOUND_ID;
            }

            // NOTE: If this sound is made then one should also be made for the paddle...

        }
    }

    this->frozenTimeCountdown -= dT;
}

void FuturismBossAIState::InitBasicBurstLineFireAIState() {
    this->DetachAndShootBall(this->ballDirBeforeAttachment);

    this->waitTimeCountdown = this->GetAfterAttackWaitTime(BasicBurstLineFireAIState);
    this->numBasicShotsToFire = this->GetNumBurstLineShots();
    this->countdownToShot = 0.0;

    this->numConsecutiveMoves = 0;
    this->numConsecutiveBeams = 0;
    this->numConsecutiveAttacks++;
    this->numConsecutiveShots++;
}

void FuturismBossAIState::ExecuteBasicBurstLineFireAIState(double dT, GameModel* gameModel) {
    
    this->countdownToShot -= dT;
    if (this->countdownToShot <= 0.0) {
        
        if (this->numBasicShotsToFire == 0) {
            if (this->DoWaitTimeCountdown(dT)) {
                this->GoToNextState(*gameModel);
            }
            return;
        }

        // Shoot a laser at the paddle!
        Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
        const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
        Vector2D bossToPaddleVec = (paddle->GetCenterPosition() + 
            Vector2D(Randomizer::GetInstance()->RandomNumNegOneToOne() * paddle->GetHalfWidthTotal(), 
                     Randomizer::GetInstance()->RandomNumNegOneToOne() * paddle->GetHalfHeight())) - bossPos;
        bossToPaddleVec.Normalize();

        this->DoBasicWeaponSingleShot(*gameModel, bossPos, bossToPaddleVec);
        this->numBasicShotsToFire--;
        this->countdownToShot = this->GetTimeBetweenBurstLineShots();
    }
}

void FuturismBossAIState::InitBasicBurstWaveFireAIState() {
    this->DetachAndShootBall(this->ballDirBeforeAttachment);

    this->waitTimeCountdown = this->GetAfterAttackWaitTime(BasicBurstWaveFireAIState);
    this->numBasicShotsToFire = this->GetNumWaveBurstShots();
    this->countdownToShot = 0.0;

    this->numConsecutiveMoves = 0;
    this->numConsecutiveBeams = 0;
    this->numConsecutiveAttacks++;
    this->numConsecutiveShots++;
}
void FuturismBossAIState::ExecuteBasicBurstWaveFireAIState(double dT, GameModel* gameModel) {
    this->countdownToShot -= dT;
    if (this->countdownToShot <= 0.0) {

        if (this->numBasicShotsToFire == 0) {
            if (this->DoWaitTimeCountdown(dT)) {
                this->GoToNextState(*gameModel);
            }
            return;
        }

        // Shoot a wave of lasers at the paddle!
        Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
        const PlayerPaddle* paddle = gameModel->GetPlayerPaddle();
        Vector2D bossToPaddleVec = (paddle->GetCenterPosition() + 
            Vector2D(Randomizer::GetInstance()->RandomNumNegOneToOne() * paddle->GetHalfWidthTotal(), 
            Randomizer::GetInstance()->RandomNumNegOneToOne() * paddle->GetHalfHeight())) - bossPos;
        bossToPaddleVec.Normalize();

        this->DoBasicWeaponWaveShot(*gameModel, bossPos, bossToPaddleVec);

        this->numBasicShotsToFire--;
        this->countdownToShot = this->GetTimeBetweenBurstWaveShots();
    }
}

void FuturismBossAIState::InitLaserBeamTwitchAIState() {
    this->DetachAndShootBall(this->ballDirBeforeAttachment);

    this->waitTimeCountdown = this->GetAfterAttackWaitTime(LaserBeamTwitchAIState);
    this->countdownToShot = this->GetTwitchBeamShootTime();
    this->weaponWasShot = false;
    
    // The boss emits a brief flare before firing its laser beam...
    this->DoLaserBeamWarningEffect(1.5*this->countdownToShot, true);

    // Determine the beam ray
    const PlayerPaddle* paddle = this->boss->GetGameModel()->GetPlayerPaddle();
    
    Point2D bossPos2D = this->boss->alivePartsRoot->GetTranslationPt2D();
    const Point2D& paddlePos = paddle->GetCenterPosition();
    Vector2D bossToPaddleDir = paddlePos - bossPos2D;
    bossToPaddleDir.Normalize();

    this->beamRay.SetOrigin(bossPos2D);
    this->beamRay.SetUnitDirection(bossToPaddleDir);

    this->numConsecutiveMoves = 0;
    this->numConsecutiveShots = 0;
    this->numConsecutiveBeams++;
    this->numConsecutiveAttacks++;
}
void FuturismBossAIState::ExecuteLaserBeamTwitchAIState(double dT, GameModel* gameModel) {
    
    this->countdownToShot -= dT;
    if (this->countdownToShot <= 0) {

        if (this->weaponWasShot) {
            // Wait until the beam is done...

            // Attempt to find the original beam in the GameModel
            Beam* foundBeam = NULL;
            std::list<Beam*>& activeBeams = gameModel->GetActiveBeams();
            for (std::list<Beam*>::iterator iter = activeBeams.begin(); iter != activeBeams.end(); ++iter) {
                if (*iter == this->currBeam) {
                    foundBeam = *iter;
                    break;
                }
            }
            if (foundBeam == NULL) {
                if (this->DoWaitTimeCountdown(dT)) {
                    this->GoToNextState(*gameModel);
                }
                return;
            }
        }
        else {
            // Fire the beam
            this->currBeam = new BossLaserBeam(gameModel, this->beamRay, 
                EYE_BEAM_HALF_RADIUS, TWITCH_BEAM_EXPIRE_TIME_IN_SECS);
            this->currBeam->SetZOffset(FuturismBoss::CORE_Z_SHOOT_DIST_FROM_ORIGIN_WITH_SHIELD);
            gameModel->AddBeam(this->currBeam);  // WARNING: After this call currBeam no longer belongs to this!!!

            // EVENT: Flash for the laser beam
            GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.2, 0.0f));

            this->DoLaserBeamEnergyEffect(TWITCH_BEAM_EXPIRE_TIME_IN_SECS);

            this->weaponWasShot = true;
        }
    }
}

void FuturismBossAIState::InitLaserBeamArcAIState() {
    this->DetachAndShootBall(this->ballDirBeforeAttachment);

    this->waitTimeCountdown = this->GetAfterAttackWaitTime(LaserBeamArcAIState);
    this->countdownToShot = this->GetBeamArcShootTime();
    this->weaponWasShot = false;
    this->currBeam = NULL;

    // The boss emits a brief flare before firing its laser beam...
    this->DoLaserBeamWarningEffect(1.75*this->countdownToShot, false);
    
    // Determine the starting beam ray: It should be directed towards the side of the level
    // (NOT AT THE PADDLE YET!!) and it will then sweep across the level...
    
    const PlayerPaddle* paddle = this->boss->GetGameModel()->GetPlayerPaddle();

    Point2D bossPos2D = this->boss->alivePartsRoot->GetTranslationPt2D();
    const Point2D& paddlePos = paddle->GetCenterPosition();
    Vector2D bossBeamDir = paddlePos - bossPos2D;
    bossBeamDir.Normalize();

    // Rotate the vector so that it's different by about 90 degrees, 
    // and cache the sweeping distance along with the sign of it
    float rotateSign = Randomizer::GetInstance()->RandomNegativeOrPositive(); // The direction the beam will be rotating in as it arcs towards the paddle
    assert(rotateSign == 1 || rotateSign == -1);

    // Find the angle between the boss to paddle vector and the x-axis so that we know how much to limit the rotation by...
    float degreesToXAxis  = Trig::radiansToDegrees(acos(NumberFuncs::Clamp(Vector2D::Dot(Vector2D(-rotateSign,0), bossBeamDir), -1, 1)));
    degreesToXAxis = std::min(degreesToXAxis, 90.0f);
    float offsetRotateAmt = (degreesToXAxis / 2.0f + Randomizer::GetInstance()->RandomNumZeroToOne() * (degreesToXAxis / 5.0f));
    bossBeamDir.Rotate(-rotateSign * offsetRotateAmt);

    this->beamSweepAngularDist = rotateSign * (100.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*30.0f);

    this->beamRay.SetOrigin(bossPos2D);
    this->beamRay.SetUnitDirection(bossBeamDir);

    this->numConsecutiveMoves = 0;
    this->numConsecutiveShots = 0;
    this->numConsecutiveBeams++;
    this->numConsecutiveAttacks++;
}
void FuturismBossAIState::ExecuteLaserBeamArcAIState(double dT, GameModel* gameModel) {
    
    if (this->countdownToShot <= 0) {
        if (this->weaponWasShot) {

            // Attempt to find the original beam in the GameModel
            Beam* foundBeam = NULL;
            std::list<Beam*>& activeBeams = gameModel->GetActiveBeams();
            for (std::list<Beam*>::iterator iter = activeBeams.begin(); iter != activeBeams.end(); ++iter) {
                if (*iter == this->currBeam) {
                    foundBeam = *iter;
                    break;
                }
            }

            if (foundBeam == NULL) {
                // The beam is gone; either it expired or was deleted due to an internal GameModel change --
                // regardless, we need to get out of this state
                if (this->DoWaitTimeCountdown(dT)) {
                    this->currBeam = NULL;
                    this->GoToNextState(*gameModel);
                }
                return;
            }
            else {
                // Rotate the beam...
                double dR = this->beamSweepSpd * dT;
                this->beamRay.SetUnitDirection(Vector2D::Rotate(dR, this->beamRay.GetUnitDirection()));

                // Update the beam in the GameModel by modifying the origin ray of the beam...
                foundBeam->UpdateOriginBeamSegment(gameModel, this->beamRay);
            }
        }
        else {
            // Based on the time we need to arc for, determine the speed...
            double sweepTime = this->GetBeamArcingTime();
            assert(sweepTime != 0);
            this->beamSweepSpd = this->beamSweepAngularDist / sweepTime;

            // Bit of a hack -- reuse the countdownToShot to hold the beam in place for a brief period of
            // time before sweeping it
            const double HOLD_BEAM_TIME = this->GetBeamArcHoldTime();
            this->countdownToShot = HOLD_BEAM_TIME;
            double totalBeamTime = HOLD_BEAM_TIME + sweepTime;

            // Fire the beam
            this->currBeam = new BossLaserBeam(gameModel, this->beamRay, 
                EYE_BEAM_HALF_RADIUS, totalBeamTime);
            this->currBeam->SetZOffset(this->boss->GetCurrentBeamZShootDistFromOrigin());
            gameModel->AddBeam(this->currBeam); // WARNING: The currBeam no longer belongs to this after this call!!!!

            // EVENT: Flash for the laser beam
            GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.2, 0.0f));

            // Show the boss charging while shooting the beam...
            this->DoLaserBeamEnergyEffect(totalBeamTime);

            this->weaponWasShot = true;
        }
    }
    else {
        this->countdownToShot -= dT;
    }
}

void FuturismBossAIState::InitStationaryFireWeaponPortalAIState() {
    this->DetachAndShootBall(this->ballDirBeforeAttachment);

    double totalTime = this->GetTotalWeaponPortalShootTime();
    this->countdownToPortalShot = 0.5*totalTime;
    this->weaponWasShot = false;
    this->nextPortalColour = this->boss->GetAndIncrementPortalColour();

    // Setup the core rotation animation...
    this->SetupCoreRotationVelAnim(totalTime);

    // Add a bit of shaking to the boss' body to indicate that it's firing the portal...
    this->boss->alivePartsRoot->AnimateLocalTranslation(
        Boss::BuildShakeAnim(0.2f*totalTime, 0.6f*totalTime, 15, 0.025, 0.004));

    // EVENT: Do energy gathering effects
    GameEventManager::Instance()->ActionBossEffect(PowerChargeEffectInfo(
        this->boss->GetCoreBody(), this->countdownToPortalShot, 
        Colour(0.5f,0.5f,0.5f)+1.25f*this->nextPortalColour, 0.5f, 
        Vector3D(0,0,FuturismBoss::CORE_Z_DIST_FROM_ORIGIN)));

    GameSound* sound = this->boss->GetGameModel()->GetSound();
    if (this->chargingSoundID != INVALID_SOUND_ID) {
        sound->StopSound(this->chargingSoundID);
        this->chargingSoundID = INVALID_SOUND_ID;
    }
    this->chargingSoundID = sound->PlaySound(GameSound::FuturismBossPortalSummonChargeEvent, false, true);

    this->numConsecutiveMoves   = 0;
    this->numConsecutiveShots   = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveAttacks = 0;
}
void FuturismBossAIState::ExecuteStationaryFireWeaponPortalAIState(double dT, GameModel* gameModel) {
    this->ExecuteFirePortalState(dT, gameModel, false);
}

void FuturismBossAIState::InitStationaryFireStrategyPortalAIState() {
    this->DetachAndShootBall(this->ballDirBeforeAttachment);

    double totalTime = this->GetTotalStrategyPortalShootTime();
    this->countdownToPortalShot = 0.5*totalTime;
    this->weaponWasShot = false;
    this->nextPortalColour = this->boss->GetAndIncrementPortalColour();

    // Setup the core rotation animation...
    this->SetupCoreRotationVelAnim(totalTime);

    // Add a bit of shaking to the boss' body to indicate that it's firing the portal...
    this->boss->alivePartsRoot->AnimateLocalTranslation(
        Boss::BuildShakeAnim(0.2f*totalTime, 0.6f*totalTime, 15, 0.03, 0.005));

    // EVENT: Do energy gathering effects
    GameEventManager::Instance()->ActionBossEffect(PowerChargeEffectInfo(
        this->boss->GetCoreBody(), this->countdownToPortalShot, 
        Colour(0.5f,0.5f,0.5f)+1.25f*this->nextPortalColour, 0.5f, 
        Vector3D(0,0,FuturismBoss::CORE_Z_DIST_FROM_ORIGIN)));

    GameSound* sound = this->boss->GetGameModel()->GetSound();
    if (this->chargingSoundID != INVALID_SOUND_ID) {
        sound->StopSound(this->chargingSoundID);
        this->chargingSoundID = INVALID_SOUND_ID;
    }
    this->chargingSoundID = sound->PlaySound(GameSound::FuturismBossPortalSummonChargeEvent, false, true);
    this->spinCoolDownSoundID = INVALID_SOUND_ID;

    this->numConsecutiveMoves   = 0;
    this->numConsecutiveShots   = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveAttacks = 0;
}

void FuturismBossAIState::ExecuteStationaryFireStrategyPortalAIState(double dT, GameModel* gameModel) {
    this->ExecuteFirePortalState(dT, gameModel, true);
}

void FuturismBossAIState::ExecuteFirePortalState(double dT, GameModel* gameModel, bool isStrategyPortal) {
    UNUSED_PARAMETER(gameModel);

    // Check the status of shooting the portal...
    if (!this->weaponWasShot) {
        this->countdownToPortalShot -= dT;
        if (this->countdownToPortalShot <= 0.0) {
            if (isStrategyPortal) {
                this->ShootStrategyPortal(*gameModel);
            }
            else {
                this->ShootWeaponPortal(*gameModel);
            }
        }
    }

    // Tick the rotation animation
    bool isFinished = this->coreRotVelAnim.Tick(dT);
    if (this->spinCoolDownSoundID == INVALID_SOUND_ID && 
        (this->coreRotVelAnim.GetFinalTimeValue() - this->coreRotVelAnim.GetCurrentTimeValue()) <= COOL_DOWN_SFX_TIME_IN_SECS) {
        
        this->spinCoolDownSoundID = gameModel->GetSound()->PlaySound(GameSound::FuturismBossSpinCoolDownEvent, false, true);
    }

    // If the core shield is destroyed we rotate the core assembly/body instead
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);

    if (isFinished) {
        this->weaponWasShot = true;

        // Clear up all animations on the boss that were used during this state
        partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);
        this->boss->alivePartsRoot->ClearLocalTranslationAnimation();
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        // We're done with this state, go to the next one
        this->GoToNextState(*gameModel);
        return;
    }
    else {
        this->currCoreRotInDegs += dT*this->coreRotVelAnim.GetInterpolantValue();
        this->currCoreRotInDegs = fmod(this->currCoreRotInDegs, 360.0f);
        partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);
    }
}

void FuturismBossAIState::InitDestroyLevelBarrierAIState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Cancel anything of relevance for the movement and attack states...
    this->DetachAndShootBall(this->ballDirBeforeAttachment);    
    this->CancelActiveBeam(*gameModel);
    this->CloseAllActivePortals(*gameModel);

    this->currVel    = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    this->countdownToShot = TIME_UNTIL_BARRIER_DESTRUCTION_SHOT_IN_SECS;

    // We need to calculate the full beam sweep animation...
    FuturismBoss::GetBarrierPiecesTopToBottom(*gameModel->GetCurrentLevel(), this->barrierPieces);
    assert(this->barrierPieces.size() >= 2);
    const LevelPiece* topPiece    = this->barrierPieces.front();
    const LevelPiece* bottomPiece = this->barrierPieces.back();
    assert(topPiece->GetType() == LevelPiece::Solid && bottomPiece->GetType() == LevelPiece::Solid);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    Vector2D startingBeamDir = (topPiece->GetCenter() + Vector2D(0, 0.1f*LevelPiece::HALF_PIECE_HEIGHT)) - bossPos;
    startingBeamDir.Normalize();
    Vector2D endingBeamDir   = (bottomPiece->GetCenter() - Vector2D(0, 0.1f*LevelPiece::HALF_PIECE_HEIGHT)) - bossPos;
    endingBeamDir.Normalize();

    this->beamRay.SetOrigin(bossPos);
    this->beamRay.SetUnitDirection(startingBeamDir);

    float sweepAngleInDegs = Trig::radiansToDegrees(acos(Vector2D::Dot(startingBeamDir, endingBeamDir)));
    if (this->arenaState == InLeftArena) {
        sweepAngleInDegs *= -1;
    }

    this->beamSweepSpd = sweepAngleInDegs / BARRIER_DESTRUCTION_ARC_TIME_IN_SECS;
    {
        std::vector<double> timeVals;
        timeVals.reserve(4);
        timeVals.push_back(0);
        timeVals.push_back(this->countdownToShot);
        timeVals.push_back(timeVals.back() + BARRIER_DESTRUCTION_ARC_TIME_IN_SECS);
        timeVals.push_back(timeVals.back() + 1.0);

        std::vector<float> rotVelVals; // degrees per second values
        rotVelVals.reserve(timeVals.size());
        rotVelVals.push_back(0.0f);
        rotVelVals.push_back(720.0f);
        rotVelVals.push_back(720.0f);
        rotVelVals.push_back(FuturismBossAIState::FINAL_STATE_ROT_VEL);

        this->coreRotVelAnim.SetLerp(timeVals, rotVelVals);
        this->coreRotVelAnim.SetRepeat(false);
        this->coreRotVelAnim.SetInterpolantValue(0.0f);
    }
    {
        static const double FLASH_TIME = 0.1;
        std::vector<double> timeVals;
        timeVals.reserve(6);
        timeVals.push_back(0.0);
        timeVals.push_back(timeVals.back() + FLASH_TIME);
        timeVals.push_back(timeVals.back() + FLASH_TIME);
        timeVals.push_back(timeVals.back() + FLASH_TIME);
        timeVals.push_back(timeVals.back() + FLASH_TIME);
        timeVals.push_back(timeVals.back() + FLASH_TIME);

        std::vector<Colour> colourVals;
        colourVals.reserve(6);
        colourVals.push_back(Colour(1,0,0));
        colourVals.push_back(Colour(1,1,0));
        colourVals.push_back(Colour(0,1,0));
        colourVals.push_back(Colour(0,1,1));
        colourVals.push_back(Colour(1,0,1));
        colourVals.push_back(Colour(1,0,0));

        this->beamColourAnim.SetLerp(timeVals, colourVals);
        this->beamColourAnim.SetRepeat(true);
        this->beamColourAnim.SetInterpolantValue(colourVals.front());
    }

    // Add a bit of shaking to the boss' body to indicate that it's firing the portal...
    this->boss->alivePartsRoot->AnimateLocalTranslation(
        Boss::BuildShakeAnim(0.25f*this->countdownToShot, 0.9f*this->coreRotVelAnim.GetFinalTimeValue(), 18, 0.04, 0.007));

    // TODO: MAKE THESE LOOK NICER!!
    // EVENT: Do energy gathering effects
    GameEventManager::Instance()->ActionBossEffect(PowerChargeEffectInfo(
        this->boss->GetCoreBody(), this->countdownToShot, Colour(1,1,1), 0.5f, 
        Vector3D(0, 0, this->boss->GetCurrArmZOffset())));

    // Charging sound...
    this->chargingSoundID = gameModel->GetSound()->PlaySound(GameSound::FuturismBossChargingEvent, false, true);

    // Reset AI trackers
    this->numConsecutiveMoves   = 0;
    this->numConsecutiveShots   = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveAttacks = 0;
}
void FuturismBossAIState::ExecuteDestroyLevelBarrierAIState(double dT, GameModel* gameModel) {
    // This state starts by the boss accelerating its rotation speed until it's going really fast and
    // then it blasts its laser across the middle blocks of the level, destroying all of them

    this->countdownToShot -= dT;
    if (this->countdownToShot <= 0.0) {
        this->countdownToShot = 0.0;
        Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

        if (this->weaponWasShot) {

            if (this->currBeam != NULL) {

                bool beamFound = false;
                const std::list<Beam*>& beams = gameModel->GetActiveBeams();
                for (std::list<Beam*>::const_iterator iter = beams.begin(); iter != beams.end(); ++iter) {
                    Beam* beam = *iter;
                    if (beam == this->currBeam) {

                        // Update the beam colour
                        this->beamColourAnim.Tick(dT);
                        this->currBeam->SetBeamColour(this->beamColourAnim.GetInterpolantValue());
                        
                        // Update the angle of the beam
                        double dR = this->beamSweepSpd * dT;
                        this->beamRay.SetUnitDirection(Vector2D::Rotate(dR, this->beamRay.GetUnitDirection()));
                        beam->UpdateOriginBeamSegment(gameModel, this->beamRay);

                        // Destroy any blocks that have been passed over by the beam in the barrier
                        // Find the y-coordinate where the beam intersects the line...
                        const LevelPiece* topPiece    = this->barrierPieces.front();
                        const LevelPiece* bottomPiece = this->barrierPieces.back();

                        float rayT = -1;
                        Collision::IsCollision(this->beamRay, Collision::LineSeg2D(
                            Point2D(topPiece->GetCenter() + Vector2D(0,LevelPiece::HALF_PIECE_HEIGHT)), 
                            Point2D(bottomPiece->GetCenter() - Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT))), rayT);
                        assert(rayT != -1);
                        
                        Point2D intersectionPt = this->beamRay.GetPointAlongRayFromOrigin(rayT);
                        assert(fabs(intersectionPt[0] - topPiece->GetCenter()[0]) < EPSILON);
                        intersectionPt[1] += 0.9f*LevelPiece::HALF_PIECE_HEIGHT;

                        // Go through all the barrier pieces, make sure we've destroyed all the ones that the beam has passed over
                        // so far, also update the barrier pieces as they change
                        float currY = topPiece->GetCenter()[1];
                        int pieceIdx = 0;
                        while (currY > intersectionPt[1] && pieceIdx < static_cast<int>(this->barrierPieces.size())) {
                            LevelPiece* currPiece = this->barrierPieces[pieceIdx];
                            if (currPiece->GetType() == LevelPiece::Solid) {
                                
                                // Play a sound for the disintegration / block destruction
                                gameModel->GetSound()->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, 
                                    currPiece->GetPosition3D(), true, true, true);
                                
                                // Add some extra effects for the piece's destruction...
                                EnumGeneralEffectInfo disintegrateEffect(EnumGeneralEffectInfo::FuturismBarrierBlockDisintegrationEffect);
                                disintegrateEffect.SetPosition(currPiece->GetCenter());
                                disintegrateEffect.SetDirection(this->beamRay.GetUnitDirection());
                                disintegrateEffect.SetSize2D(LevelPiece::PIECE_WIDTH, LevelPiece::PIECE_HEIGHT);
                                disintegrateEffect.SetTimeInSecs(2.0);
                                disintegrateEffect.SetColour(Colour(0.5f, 0.5f, 0.5f));
                                GameEventManager::Instance()->ActionGeneralEffect(disintegrateEffect);

                                // Destroy the piece...
                                this->barrierPieces[pieceIdx] = currPiece->Destroy(gameModel, LevelPiece::DisintegrationDestruction);
                            }

                            currY -= LevelPiece::PIECE_HEIGHT;
                            pieceIdx++;
                        }

                        beamFound = true;
                        break;
                    }
                }

                if (!beamFound) {
                    // We're done firing the beam, the barrier is now fully destroyed
                    this->currBeam = NULL;
                }
            }
        }
        else {
            // Shoot the laser beam at the barrier!!!
            this->currBeam = new BossLaserBeam(gameModel, this->beamRay, 
                EYE_BEAM_HALF_RADIUS, BARRIER_DESTRUCTION_ARC_TIME_IN_SECS + 0.5);
            this->currBeam->SetZOffset(this->boss->GetCurrentBeamZShootDistFromOrigin());

            // Start animating the colour of the beam...
            this->currBeam->SetBeamColour(this->beamColourAnim.GetInterpolantValue());

            gameModel->AddBeam(this->currBeam);  // WARNING: After this call currBeam no longer belongs to this!!!

            // EVENT: Flash for the laser beam
            GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.3, 1.0f));

            this->DoLaserBeamEnergyEffect(BARRIER_DESTRUCTION_ARC_TIME_IN_SECS + 0.45);
            
            // Stop the charging sound
            gameModel->GetSound()->StopSound(this->chargingSoundID);
            this->chargingSoundID = INVALID_SOUND_ID;
            
            this->weaponWasShot = true;
        }
    }

    // Update positioning of the arms...
    assert(this->countdownToShot >= 0.0 && this->countdownToShot <= TIME_UNTIL_BARRIER_DESTRUCTION_SHOT_IN_SECS);
    this->boss->currArmMoveForwardOffset = NumberFuncs::LerpOverTime(TIME_UNTIL_BARRIER_DESTRUCTION_SHOT_IN_SECS, 0.0, 0.0f, 
        FuturismBoss::CORE_ARMS_MOVE_FORWARD_AMT, this->countdownToShot);

    // Update the rotation of the center body of the boss...
    // Tick the rotation animation
    bool isFinished = this->coreRotVelAnim.Tick(dT);

    // If the core shield is destroyed we rotate the core assembly/body instead
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    this->GetCenterRotationBodyPartAndFullDegAmt(partToAnimate, fullRotationDegAmt);

    if (isFinished && this->currBeam == NULL) {

        // Add the now empty barrier pieces to the pieces in the fully-opened arena
        this->boss->fullArenaPieces.insert(this->barrierPieces.begin(), this->barrierPieces.end());

        this->boss->alivePartsRoot->ClearLocalTranslationAnimation();

        this->GoToNextState(*gameModel);
        return;
    }
    else {
        this->currCoreRotInDegs += dT*this->coreRotVelAnim.GetInterpolantValue();
        this->currCoreRotInDegs = fmod(this->currCoreRotInDegs, 360.0f);
        partToAnimate->SetLocalZRotation(this->currCoreRotInDegs);
    }
}

void FuturismBossAIState::InitShieldPartCrackedAIState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Cancel anything of relevance for the movement and attack states...
    this->DetachAndShootBall(this->ballDirBeforeAttachment);    
    this->CancelActiveBeam(*gameModel);

    // Stop sounds from other states
    this->InterruptSounds(*gameModel);

    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    this->boss->alivePartsRoot->ResetColourRGBAAnimation();

    // Cause the boss to animate briefly...
    this->bossHurtAnim.ResetToStart();

    // Reset AI trackers
    this->numConsecutiveMoves   = 0;
    this->numConsecutiveShots   = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveAttacks = 0;
}
void FuturismBossAIState::ExecuteShieldPartCrackedAIState(double dT, GameModel* gameModel) {

    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->bossHurtAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->bossHurtAnim.GetInterpolantValue());
    if (isFinished) {
        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        this->GoToNextState(*gameModel);
        return;
    }
}

void FuturismBossAIState::InitShieldPartDestroyedAIState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Cancel anything of relevance for the movement and attack states...
    this->DetachAndShootBall(this->ballDirBeforeAttachment);
    this->CancelActiveBeam(*gameModel);

    // Stop sounds from other states
    this->InterruptSounds(*gameModel);

    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    // Boss should flash due to being hit...
    this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim(
        BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS));

    this->bossHurtAnim.ResetToStart();

    // Reset AI trackers
    this->numConsecutiveMoves   = 0;
    this->numConsecutiveShots   = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveAttacks = 0;
}
void FuturismBossAIState::ExecuteShieldPartDestroyedAIState(double dT, GameModel* gameModel) {
    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->bossHurtAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->bossHurtAnim.GetInterpolantValue());
    if (isFinished) {

        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        // Check to see if all the shields are destroyed...
        if (this->AllShieldsDestroyedToEndThisState()) {
            // The boss is angry!! This will lead to the next high-level AI state
            this->SetState(AngryAIState);
        }
        else {
            this->GoToNextState(*gameModel);
        }
        return;
    }
}

void FuturismBossAIState::InitBulbHitAndDestroyedAIState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Cancel anything of relevance for the movement and attack states...
    this->DetachAndShootBall(this->ballDirBeforeAttachment);    
    this->CancelActiveBeam(*gameModel);

    // Stop sounds from other states
    this->InterruptSounds(*gameModel);

    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    // Boss should flash due to being hit...
    this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossHurtAndInvulnerableColourAnim(
        BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS));

    // Cause the boss to animate briefly...
    this->bossHurtAnim.ResetToStart();

    // Reset AI trackers
    this->numConsecutiveMoves   = 0;
    this->numConsecutiveShots   = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveAttacks = 0;
}
void FuturismBossAIState::ExecuteBulbHitAndDestroyedAIState(double dT, GameModel* gameModel) {
    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->bossHurtAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->bossHurtAnim.GetInterpolantValue());
    if (isFinished) {
        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        // Check to see if all the bulbs have been destroyed, if so then the boss is dead.
        if (this->boss->GetCoreTopBulb()->GetIsDestroyed() && this->boss->GetCoreBottomBulb()->GetIsDestroyed() &&
            this->boss->GetCoreLeftBulb()->GetIsDestroyed() && this->boss->GetCoreRightBulb()->GetIsDestroyed()) {

            this->SetState(FinalDeathThroesAIState);
        }
        else {
            this->GoToNextState(*gameModel);
        }
        return;
    }
}

void FuturismBossAIState::InitAngryAIState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Cancel anything of relevance for the movement and attack states...
    this->DetachAndShootBall(this->ballDirBeforeAttachment);
    this->CancelActiveBeam(*gameModel);

    // Stop sounds from other states
    this->InterruptSounds(*gameModel);

    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    this->boss->alivePartsRoot->AnimateColourRGBA(Boss::BuildBossAngryFlashAnim());
    this->angryMoveAnim = Boss::BuildBossAngryShakeAnim(0.8f);

    // EVENT: Boss is angry! Rawr.
    GameEventManager::Instance()->ActionBossAngry(this->boss, 
        this->boss->GetCoreBody()->GetTranslationPt2D(), 
        FuturismBoss::CORE_BOSS_SIZE, FuturismBoss::CORE_BOSS_SIZE);

    // Reset AI trackers
    this->numConsecutiveMoves   = 0;
    this->numConsecutiveShots   = 0;
    this->numConsecutiveBeams   = 0;
    this->numConsecutiveAttacks = 0;
}
void FuturismBossAIState::ExecuteAngryAIState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    this->currVel = Vector2D(0,0);
    this->desiredVel = Vector2D(0,0);

    bool isFinished = this->angryMoveAnim.Tick(dT);
    this->boss->alivePartsRoot->SetLocalTranslation(this->angryMoveAnim.GetInterpolantValue());
    if (isFinished) {

        // Stop any colour animation that was set for this state
        this->boss->alivePartsRoot->ResetColourRGBAAnimation();
        // Clean up all the translations on the body of the boss for this state
        this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

        // Go to the next top-level AI state...
        FuturismBossAIState* nextAIState = this->BuildNextAIState();
        assert(nextAIState != NULL);
        this->boss->SetNextAIState(nextAIState);
        return;
    }
}

void FuturismBossAIState::InitFinalDeathThroesAIState() {
    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Cancel anything of relevance for the movement and attack states...
    this->DetachAndShootBall(this->ballDirBeforeAttachment);
    this->CancelActiveBeam(*gameModel);
    GameEventManager::Instance()->ActionClearActiveBossEffects();
    
    // Stop sounds from other states
    this->InterruptSounds(*gameModel);

    this->desiredVel = Vector2D(0,0);
    this->currVel    = Vector2D(0,0);

    // Final fade-out for the whole boss
    this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));
    AnimationMultiLerp<ColourRGBA> deathColourAnim = Boss::BuildBossFinalDeathFlashAnim();
    
    this->boss->alivePartsRoot->AnimateColourRGBA(deathColourAnim);
    this->boss->bodyParts[this->boss->topBulbIdx]->AnimateColourRGBA(deathColourAnim);
    this->boss->bodyParts[this->boss->bottomBulbIdx]->AnimateColourRGBA(deathColourAnim);
    this->boss->bodyParts[this->boss->leftBulbIdx]->AnimateColourRGBA(deathColourAnim);
    this->boss->bodyParts[this->boss->rightBulbIdx]->AnimateColourRGBA(deathColourAnim);
}
void FuturismBossAIState::ExecuteFinalDeathThroesAIState(double, GameModel*) {
    // The boss is dead dead dead.
}

void FuturismBossAIState::SetState(FuturismBossAIState::AIState newState) {

    switch (newState) {

        case MoveToPositionAIState:
            this->InitMoveToPositionAIState();
            break;
        case MoveToCenterAIState:
            this->InitMoveToCenterAIState();
            break;
        case TeleportAIState:
            this->InitTeleportAIState();
            break;
        case AvoidanceTeleportAIState:
            this->InitAvoidanceTeleportAIState();
            break;
        case BallAttractAIState:
            this->InitBallAttractAIState();
            break;
        case BallDiscardAIState:
            this->InitBallDiscardAIState();
            break;
        case FrozenAIState:
            this->InitFrozenAIState();
            break;

        case BasicBurstLineFireAIState:
            this->InitBasicBurstLineFireAIState();
            break;
        case BasicBurstWaveFireAIState:
            this->InitBasicBurstWaveFireAIState();
            break;

        case LaserBeamTwitchAIState:
            this->InitLaserBeamTwitchAIState();
            break;
        case LaserBeamArcAIState:
            this->InitLaserBeamArcAIState();
            break;
        case LaserBeamStarAIState:
            this->InitLaserBeamStarAIState();
            break;
        case StationaryFireStrategyPortalAIState:
            this->InitStationaryFireStrategyPortalAIState();
            break;
        case StationaryFireWeaponPortalAIState:
            this->InitStationaryFireWeaponPortalAIState();
            break;

        case IntroStartingAIState:
            this->InitPauseAIState(1.0);
            break;
        case IntroTeleportAIState:
            this->InitTeleportAIState();
            break;
        case DestroyLevelBarrierAIState:
            this->InitDestroyLevelBarrierAIState();
            break;

        case ShieldPartCrackedAIState:
            this->InitShieldPartCrackedAIState();
            break;
        case ShieldPartDestroyedAIState:
            this->InitShieldPartDestroyedAIState();
            break;
        case BulbHitAndDestroyedAIState:
            this->InitBulbHitAndDestroyedAIState();
            break;
        case AngryAIState:
            this->InitAngryAIState();
            break;

        case FinalDeathThroesAIState:
            this->InitFinalDeathThroesAIState();
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void FuturismBossAIState::UpdateState(double dT, GameModel* gameModel) {
    switch (this->currState) {

        case MoveToPositionAIState:
            this->ExecuteMoveToPositionAIState(dT, gameModel);
            break;
        case MoveToCenterAIState:
            this->ExecuteMoveToPositionAIState(dT, gameModel);
            break;
        case TeleportAIState:
            this->ExecuteTeleportAIState(dT, gameModel);
            break;
        case AvoidanceTeleportAIState:
            this->ExecuteTeleportAIState(dT, gameModel);
            break;
        case BallAttractAIState:
            this->ExecuteBallAttractAIState(dT, gameModel);
            break;
        case BallDiscardAIState:
            this->ExecuteBallDiscardAIState(dT, gameModel);
            break;
        case FrozenAIState:
            this->ExecuteFrozenAIState(dT, gameModel);
            break;

        case BasicBurstLineFireAIState:
            this->ExecuteBasicBurstLineFireAIState(dT, gameModel);
            break;
        case BasicBurstWaveFireAIState:
            this->ExecuteBasicBurstWaveFireAIState(dT, gameModel);
            break;
        case LaserBeamTwitchAIState:
            this->ExecuteLaserBeamTwitchAIState(dT, gameModel);
            break;
        case LaserBeamArcAIState:
            this->ExecuteLaserBeamArcAIState(dT, gameModel);
            break;
        case LaserBeamStarAIState:
            this->ExecuteLaserBeamStarAIState(dT, gameModel);
            break;
        case StationaryFireStrategyPortalAIState:
            this->ExecuteStationaryFireStrategyPortalAIState(dT, gameModel);
            break;
        case StationaryFireWeaponPortalAIState:
            this->ExecuteStationaryFireWeaponPortalAIState(dT, gameModel);
            break;

        case IntroStartingAIState:
            this->ExecutePauseAIState(dT, gameModel);
            break;
        case IntroTeleportAIState:
            this->ExecuteTeleportAIState(dT, gameModel);
            break;
        case DestroyLevelBarrierAIState:
            this->ExecuteDestroyLevelBarrierAIState(dT, gameModel);
            break;

        case ShieldPartCrackedAIState:
            this->ExecuteShieldPartCrackedAIState(dT, gameModel);
            break;
        case ShieldPartDestroyedAIState:
            this->ExecuteShieldPartDestroyedAIState(dT, gameModel);
            break;
        case BulbHitAndDestroyedAIState:
            this->ExecuteBulbHitAndDestroyedAIState(dT, gameModel);
            break;
        case AngryAIState:
            this->ExecuteAngryAIState(dT, gameModel);
            break;

        case FinalDeathThroesAIState:
            this->ExecuteFinalDeathThroesAIState(dT, gameModel);
            break;

        default:
            assert(false);
            return;
    }

    this->timeSinceLastStratPortal  += dT;
    this->timeSinceLastAttackPortal += dT;
}

void FuturismBossAIState::GetArenaBounds(ArenaState arena, float& minX, float& maxX, float& minY, float& maxY) {
    // Find the blocks that are currently colliding with the general area around the ball and boss
    switch (arena) {
        case InLeftArena: {
            minX = FuturismBoss::GetLeftSubArenaMinXConfines() + SPAWNED_PORTAL_HALF_WIDTH;
            maxX = FuturismBoss::GetLeftSubArenaMaxXConfines() - SPAWNED_PORTAL_HALF_WIDTH;
            minY = FuturismBoss::GetLeftSubArenaMinYConfines() + SPAWNED_PORTAL_HALF_HEIGHT;
            maxY = FuturismBoss::GetLeftSubArenaMaxYConfines() - SPAWNED_PORTAL_HALF_HEIGHT;
            break;
        }
        case InRightArena:  {
            minX = FuturismBoss::GetRightSubArenaMinXConfines() + SPAWNED_PORTAL_HALF_WIDTH;
            maxX = FuturismBoss::GetRightSubArenaMaxXConfines() - SPAWNED_PORTAL_HALF_WIDTH;
            minY = FuturismBoss::GetRightSubArenaMinYConfines() + SPAWNED_PORTAL_HALF_HEIGHT;
            maxY = FuturismBoss::GetRightSubArenaMaxYConfines() - SPAWNED_PORTAL_HALF_HEIGHT;
            break;
        }
        case InFullyOpenedArena: {
            minX = FuturismBoss::GetLeftSubArenaMinXConfines()  + SPAWNED_PORTAL_HALF_WIDTH;
            maxX = FuturismBoss::GetRightSubArenaMaxXConfines() - SPAWNED_PORTAL_HALF_WIDTH;
            minY = FuturismBoss::GetRightSubArenaMinYConfines() + SPAWNED_PORTAL_HALF_HEIGHT;
            maxY = FuturismBoss::GetLeftSubArenaMaxYConfines()  - SPAWNED_PORTAL_HALF_HEIGHT;
            break;
        }
        default:
            assert(false);
            break;
    }
}

const std::vector<Point2D>& FuturismBossAIState::GetArenaMoveToPositions(ArenaState arenaState) const {
    switch (arenaState) {
        case InLeftArena:
            return this->boss->leftSideMovePositions;
        case InRightArena:
            return this->boss->rightSideMovePositions;
        case InFullyOpenedArena:
            return this->boss->fullLevelMovePositions;
        default:
            assert(false);
            break;
    }
    
    return this->boss->leftSideMovePositions;
}

void FuturismBossAIState::GetPortalCandidatePieces(const GameModel& gameModel, 
                                                   std::set<LevelPiece*>& candidates) const {
    
    const GameLevel* level = gameModel.GetCurrentLevel();
    assert(level != NULL);

    const GameModel::ProjectileList* activePortals = 
        gameModel.GetActiveProjectilesWithType(Projectile::PortalBlobProjectile);

    if (activePortals != NULL) {
        for (GameModel::ProjectileListConstIter iter = activePortals->begin(); iter != activePortals->end(); ++iter) {
            const Projectile* portal = *iter;
            level->GetLevelPieceCollisionCandidatesNotMoving(portal->GetPosition(), 
                1.5f*portal->GetWidth(), 1.5f*portal->GetHeight(), candidates);
        }
    }
}

void FuturismBossAIState::GetBossCandidatePieces(const GameLevel& level, float paddingX, float paddingY, 
                                                 std::set<LevelPiece*>& candidates) const {

    Vector2D paddingVec(paddingX, paddingY);

    // Add a bit of padding room around the boss' AABB
    Collision::AABB2D bossAABB = boss->alivePartsRoot->GenerateWorldAABB();
    bossAABB.SetMax(bossAABB.GetMax() + paddingVec);
    bossAABB.SetMin(bossAABB.GetMin() - paddingVec);

    level.GetLevelPieceCollisionCandidates(bossAABB, candidates);
}

bool FuturismBossAIState::GetArenaPortalLocation(const GameModel& gameModel, Point2D& portalPos, 
                                                 ArenaState chosenArena) const {

    assert(!gameModel.GetGameBalls().empty());

    const GameLevel* level = gameModel.GetCurrentLevel();
    const GameBall* ball   = gameModel.GetGameBalls().front();

    std::set<const void*> emptyThingSet;
    std::set<LevelPiece::LevelPieceType> emptyLevelPieceTypeSet;

    std::set<LevelPiece*>* allPieces;

    // Find the blocks that are currently colliding with the general area around the ball and boss
    switch (chosenArena) {
        case InLeftArena: {
            allPieces = &this->boss->leftSubArenaPieces;
            break;
        }
        case InRightArena:  {
            allPieces = &this->boss->rightSubArenaPieces;
            break;
        }
        case InFullyOpenedArena: {
            allPieces = &this->boss->fullArenaPieces;
            break;
        }
        
        default:
            // Should never get here...
            assert(false);
            return false;
    }

    // Add all relevant level pieces that might be colliding with the ball, boss, and existing portals
    std::set<LevelPiece*> colliders;
    level->GetLevelPieceColliders(Collision::Ray2D(ball->GetCenterPosition2D(), 
        ball->GetDirection()), emptyThingSet, emptyLevelPieceTypeSet, colliders, 
        0.5f*ball->GetSpeed(), ball->GetBounds().Radius());

    this->GetBossCandidatePieces(*level, 2*FuturismBoss::PORTAL_PROJECTILE_WIDTH, 
        2*FuturismBoss::PORTAL_PROJECTILE_HEIGHT, colliders);
    this->GetPortalCandidatePieces(gameModel, colliders);

    std::vector<LevelPiece*> result(allPieces->size());
    std::vector<LevelPiece*>::iterator diffIter = 
        std::set_difference(allPieces->begin(), allPieces->end(), colliders.begin(), colliders.end(), result.begin());
    result.resize(diffIter - result.begin());

    // Handle a special case where it's just not possible to spawn a portal...
    if (result.empty()) {
        return false;
    }

    // "result" will now contain all of the level pieces that aren't colliding with the ball or the boss...
    int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(result.size());
    const LevelPiece* randomLevelPiece = result[randomIdx];
    assert(randomLevelPiece->GetType() == LevelPiece::Empty);

    portalPos = randomLevelPiece->GetAABB().GetRandomPointInside(); 
    
    // Make sure the point doesn't intersect with the level bounds...
    float minX, minY, maxX, maxY;
    FuturismBossAIState::GetArenaBounds(chosenArena, minX, maxX, minY, maxY);

    if (portalPos[0] - FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH < minX) {
        portalPos[0] += minX - (portalPos[0] - FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH);
    }
    if (portalPos[0] + FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH > maxX) {
        portalPos[0] -= (portalPos[0] + FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH) - maxX;
    }
    if (portalPos[1] - FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT < minY) {
        portalPos[1] += minY - (portalPos[1] - FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT);
    }
    if (portalPos[1] + FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT > maxY) {
        portalPos[1] -= (portalPos[1] + FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT) - maxY;
    }

    return true;
}

bool FuturismBossAIState::GetStrategyPortalLocation(const GameModel& gameModel, Point2D& portalPos) {
    const GameLevel* level = gameModel.GetCurrentLevel();
    assert(level != NULL);

    // We're going to need to check to see if any of the existing portal projectiles are taking up area
    // in the strategy spawn zone and avoid them...
    std::set<LevelPiece*> candidatePieces;
    Collision::AABB2D portalBounds;
    this->GetStrategyPortalCandidatePieces(*level, portalBounds, candidatePieces);
    assert(!candidatePieces.empty());

    const LevelPiece* randomLevelPiece = NULL;
    std::set<LevelPiece*> removePieces;
    this->GetPortalCandidatePieces(gameModel, removePieces);

    if (!removePieces.empty()) {
        
        std::vector<LevelPiece*> result(candidatePieces.size());
        std::vector<LevelPiece*>::iterator diffIter = 
            std::set_difference(candidatePieces.begin(), candidatePieces.end(), removePieces.begin(), removePieces.end(), result.begin());
        result.resize(diffIter - result.begin());

        // Handle a special case where it's just not possible to spawn a portal...
        if (result.empty()) {
            return false;
        }

        // "result" will now contain all of the level pieces that aren't colliding with the ball or the boss...
        int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(result.size());
        randomLevelPiece = result[randomIdx];
        
    }
    else {
        int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(candidatePieces.size());
        std::set<LevelPiece*>::const_iterator randomIter = candidatePieces.begin();
        std::advance(randomIter, randomIdx);
        randomLevelPiece = *randomIter;
    }

    assert(randomLevelPiece->GetType() == LevelPiece::Empty);
    portalPos = randomLevelPiece->GetAABB().GetRandomPointInside(); 

    // Make sure the point doesn't intersect with the level bounds...
    if (portalPos[0] - FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH < portalBounds.GetMin()[0]) {
        portalPos[0] += portalBounds.GetMin()[0] - (portalPos[0] - FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH);
    }
    if (portalPos[0] + FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH > portalBounds.GetMax()[0]) {
        portalPos[0] -= (portalPos[0] + FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH) - portalBounds.GetMax()[0];
    }
    if (portalPos[1] - FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT < portalBounds.GetMin()[1]) {
        portalPos[1] += portalBounds.GetMin()[1] - (portalPos[1] - FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT);
    }
    if (portalPos[1] + FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT > portalBounds.GetMax()[1]) {
        portalPos[1] -= (portalPos[1] + FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT) - portalBounds.GetMax()[1];
    }

    return true;
}

bool FuturismBossAIState::GetTeleportationLocation(const GameModel& gameModel, Point2D& teleportPos) const {
    

    std::vector<Point2D> allLocations;
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    if (this->currState == IntroTeleportAIState) {
        // Teleport to positions in the top half of the arena...
        float midY = FuturismBoss::GetLeftSubArenaCenterPos()[1];
        std::vector<Point2D> tempLocations = this->GetArenaMoveToPositions(InLeftArena);
        allLocations.reserve(tempLocations.size());
        for (int i = 0; i < static_cast<int>(tempLocations.size()); i++) {
            if (tempLocations[i][1] >= midY) {
                allLocations.push_back(tempLocations[i]);
            }
        }
    }
    else {

        // If the ball isn't attached and the boss is on the opposite side of the arena as the ball AND
        // the mid-arena barrier still exists then the boss MUST teleport to the same side of the arena as the ball...
        const GameBall* ball = gameModel.GetGameBalls().front();    
        if (this->attachedBall != ball && this->ArenaBarrierExists() && this->arenaState != InFullyOpenedArena) {

            // When checking whether the ball is on the same side of the arena as the boss, first make sure the
            // ball is ACTUALLY IN any of the arenas!!
            bool ballNotOnSameSideAsBoss = (FuturismBoss::IsInLeftSubArena(ball->GetCenterPosition2D()) || 
                FuturismBoss::IsInRightSubArena(ball->GetCenterPosition2D())) &&
                FuturismBoss::IsInLeftSubArena(ball->GetCenterPosition2D()) != FuturismBoss::IsInLeftSubArena(bossPos);

            if (ballNotOnSameSideAsBoss) {
                ArenaState otherArenaState = (this->arenaState == InLeftArena) ? InRightArena : InLeftArena;
                allLocations = this->GetArenaMoveToPositions(otherArenaState);
            }
            else {
                allLocations = this->GetArenaMoveToPositions(this->arenaState);
            }
        }
        else {
            // If the ball is attached and there's still a barrier, then the boss will be
            // teleporting to the other sub-arena...
            if (this->attachedBall == ball && this->ArenaBarrierExists() && this->arenaState != InFullyOpenedArena) {

                ArenaState otherArenaState = (this->arenaState == InLeftArena) ? InRightArena : InLeftArena;
                allLocations = this->GetArenaMoveToPositions(otherArenaState);

                // Make sure the boss teleports near the top of the respective arena...
                if (otherArenaState == InLeftArena) {
                    float midY = this->boss->GetLeftSubArenaCenterPos()[1];
                    for (std::vector<Point2D>::iterator iter = allLocations.begin(); iter != allLocations.end();) {
                        if ((*iter)[1] < midY) {
                            iter = allLocations.erase(iter);
                        }
                        else {
                            ++iter;
                        }
                    }
                }
                else {
                    float midY = this->boss->GetRightSubArenaCenterPos()[1];
                    for (std::vector<Point2D>::iterator iter = allLocations.begin(); iter != allLocations.end();) {
                        if ((*iter)[1] > midY) {
                            iter = allLocations.erase(iter);
                        }
                        else {
                            ++iter;
                        }
                    }
                }
            }
            else {
                // Grab the locations where the boss typically moves...
                allLocations = this->GetArenaMoveToPositions(this->arenaState);
            }
        }
    }

    const GameLevel* level = gameModel.GetCurrentLevel();
    assert(level != NULL);

    Collision::AABB2D bossAABB = this->boss->alivePartsRoot->GenerateWorldAABB();
    std::vector<Point2D> possibleLocations;
    possibleLocations.reserve(allLocations.size());
    for (std::vector<Point2D>::const_iterator ptIter = allLocations.begin(); ptIter != allLocations.end(); ++ptIter) {
        
        const Point2D& currLocation = *ptIter;
        if (currLocation == bossPos) {
            // Don't teleport the boss to the same position
            continue;
        }

        // If the boss were to be positioned at the current location, find out if
        // it would collide with anything important
        Vector2D bossToLoc = currLocation - bossPos;
        Collision::AABB2D translatedBossAABB(bossAABB.GetMin() + bossToLoc, bossAABB.GetMax() + bossToLoc);
        bool isCollision = false;

        // Existing portals
        const GameModel::ProjectileList* activePortals = gameModel.GetActiveProjectilesWithType(Projectile::PortalBlobProjectile);
        if (activePortals != NULL && !activePortals->empty()) {
            for (GameModel::ProjectileListConstIter portalIter = activePortals->begin(); portalIter != activePortals->end(); ++portalIter) {
                const Projectile* currProjectile = *portalIter;
                if (Collision::IsCollision(translatedBossAABB, currProjectile->BuildAABB())) {
                    isCollision = true;
                    break;
                }
            }
            if (isCollision) {
                continue;
            }
        }

        // Existing rockets
        const GameModel::ProjectileList* activeRockets = gameModel.GetActiveProjectilesWithType(Projectile::PaddleRocketBulletProjectile);
        if (activeRockets != NULL && !activeRockets->empty()) {
            for (GameModel::ProjectileListConstIter rocketIter = activeRockets->begin(); rocketIter != activeRockets->end(); ++rocketIter) {
                const Projectile* currProjectile = *rocketIter;
                if (Collision::IsCollision(translatedBossAABB, currProjectile->BuildAABB())) {
                    isCollision = true;
                    break;
                }
            }
            if (isCollision) {
                continue;
            }
        }

        // Existing Ice blasts
        const GameModel::ProjectileList* activeIceBlasts = gameModel.GetActiveProjectilesWithType(Projectile::PaddleIceBlastProjectile);
        if (activeIceBlasts != NULL && !activeIceBlasts->empty()) {
            for (GameModel::ProjectileListConstIter iceBlastIter = activeIceBlasts->begin(); iceBlastIter != activeIceBlasts->end(); ++iceBlastIter) {
                const Projectile* currProjectile = *iceBlastIter;
                if (Collision::IsCollision(translatedBossAABB, currProjectile->BuildAABB())) {
                    isCollision = true;
                    break;
                }
            }
            if (isCollision) {
                continue;
            }
        }

        // Existing ball
        const std::list<GameBall*>& balls = gameModel.GetGameBalls();
        const GameBall* currBall = balls.front();
        if ((this->attachedBall == NULL) && Collision::IsCollision(translatedBossAABB, currBall->GetBounds())) {
            continue;
        }

        // If we made it through all the tests then the current location is a good candidate for teleportation
        possibleLocations.push_back(currLocation);
    }

    if (possibleLocations.empty()) {
        return false;
    }
    if (possibleLocations.size() == 1) {
        teleportPos = possibleLocations.front();
        return true;
    }

    // We now have a list of possible locations, find the furthest few away and choose one at random...
    int numberToChoose = static_cast<int>(ceilf(static_cast<float>(possibleLocations.size()) / 2.0f));
    assert(numberToChoose >= 1);

    BossAIState::GetFurthestDistFromBossPositions(bossPos, possibleLocations, numberToChoose, possibleLocations);
    assert(static_cast<int>(possibleLocations.size()) == numberToChoose);
    int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % numberToChoose;
    teleportPos = possibleLocations[randomIdx];

    return true;
}

void FuturismBossAIState::GetValidMoveToPositions(const GameModel& gameModel, const std::vector<Point2D>& allPositions, 
                                                  std::vector<Point2D>& positions) const {
    positions.reserve(allPositions.size());

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    Collision::AABB2D bossAABB = this->boss->alivePartsRoot->GenerateWorldAABB();
    
    const GameModel::ProjectileList* activeProjectiles = NULL;

    // Make sure the boss doesn't move through a portal, rocket or ice blast on its way to any of the positions
    for (std::vector<Point2D>::const_iterator iter = allPositions.begin(); iter != allPositions.end(); ++iter) {

        const Point2D& currPos = *iter;
        if (currPos == bossPos) {
            // Don't include the current location of the boss (we don't want to stay still)
            continue;
        }

        Vector2D bossToPosVec = currPos - bossPos;
        Collision::AABB2D translatedBossAABB(bossAABB.GetMin() + bossToPosVec, bossAABB.GetMax() + bossToPosVec);
        bool isCollision = false;

#define CHECK_PROJECTILES(projectileType) \
        activeProjectiles = gameModel.GetActiveProjectilesWithType(projectileType); \
        if (activeProjectiles != NULL && !activeProjectiles->empty()) { \
            for (GameModel::ProjectileListConstIter iter2 = activeProjectiles->begin(); iter2 != activeProjectiles->end(); ++iter2) { \
                const Projectile* currProjectile = *iter2; \
                Collision::AABB2D projectileAABB = currProjectile->BuildAABB(); \
                if (Collision::IsCollision(translatedBossAABB, currProjectile->BuildAABB()) || \
                    Collision::IsCollision(bossAABB, projectileAABB, bossToPosVec, currProjectile->GetVelocity())) { \
                    isCollision = true; break; \
                } \
            } \
            if (isCollision) { continue; } \
        }

        CHECK_PROJECTILES(Projectile::PortalBlobProjectile);
        CHECK_PROJECTILES(Projectile::PaddleRocketBulletProjectile);
        CHECK_PROJECTILES(Projectile::PaddleIceBlastProjectile);

#undef CHECK_PROJECTILES

        positions.push_back(currPos);
    }
}

void FuturismBossAIState::SpawnPortals(GameModel& gameModel, const Point2D& portal1Pos, const Point2D& portal2Pos) {
    // Create twin portals...
    std::pair<PortalProjectile*, PortalProjectile*> portalPair =
        PortalProjectile::BuildSiblingPortalProjectiles(portal1Pos, portal2Pos, 
        FuturismBoss::PORTAL_PROJECTILE_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HEIGHT,
        this->nextPortalColour, BOSS_PORTAL_TERMINATION_TIME_IN_SECS);

    gameModel.AddProjectile(portalPair.first);
    gameModel.AddProjectile(portalPair.second);

    // EVENT(S): Effects for the boss "summoning" the portals
    // NOTE: Make sure these effects are BEFORE the effects for the 
    // portals themselves (to ensure proper draw order)
    GameEventManager::Instance()->ActionBossEffect(
        SummonPortalsEffectInfo(this->boss->GetCoreBody(), FuturismBoss::CORE_EYE_SIZE,
        PORTAL_SPAWN_EFFECT_TIME_IN_SECS, portal1Pos, portal2Pos, this->nextPortalColour, 
        Vector3D(0,0,this->boss->GetCurrentZShootDistFromOrigin())));

    // EVENT(S): Effects for the "spawning" of the portals
    GameEventManager::Instance()->ActionGeneralEffect(PortalSpawnEffectInfo(
        PORTAL_SPAWN_EFFECT_TIME_IN_SECS, portal1Pos, this->nextPortalColour, 
        FuturismBoss::PORTAL_PROJECTILE_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HEIGHT));
    GameEventManager::Instance()->ActionGeneralEffect(PortalSpawnEffectInfo(
        PORTAL_SPAWN_EFFECT_TIME_IN_SECS, portal2Pos, this->nextPortalColour, 
        FuturismBoss::PORTAL_PROJECTILE_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HEIGHT));

    // Sound for the portal shooting effect
    gameModel.GetSound()->PlaySound(GameSound::FuturismBossPortalFireEvent, false, true);

    this->weaponWasShot = true;
}

Point2D FuturismBossAIState::GetCenterMovePosition() const {
    switch (this->arenaState) {
        case InLeftArena:
            return FuturismBoss::GetLeftSubArenaCenterPos();
        case InRightArena:
            return FuturismBoss::GetRightSubArenaCenterPos();
        case InFullyOpenedArena:
            return FuturismBoss::GetFullArenaCenterPos();
        default:
            assert(false);
            break;
    }

    // Should never get here
    return this->boss->alivePartsRoot->GetTranslationPt2D();
}

void FuturismBossAIState::ShootStrategyPortal(GameModel& gameModel) {

    Point2D arenaPortalPos, stratPortalPos;
    if (this->GetArenaPortalLocation(gameModel, arenaPortalPos, this->arenaState) &&
        this->GetStrategyPortalLocation(gameModel, stratPortalPos)) {

        this->SpawnPortals(gameModel, arenaPortalPos, stratPortalPos);
        this->timeSinceLastStratPortal = 0.0;
    }
}

void FuturismBossAIState::ShootWeaponPortal(GameModel& gameModel) {

    // Weapon portals are usually shot to opposite sides of the arena to make game play
    // more difficult for the player...
    ArenaState firstArena, secondArena;
    if (this->ArenaBarrierExists() && this->arenaState != InFullyOpenedArena) {
        firstArena  = this->arenaState;
        secondArena = firstArena == InLeftArena ? InRightArena : InLeftArena;
    }
    else {
        // The barrier between the left and right arenas is gone, doesn't really matter
        // where we shoot the portals, try to make the initial portal closer to the boss though
        if (FuturismBoss::IsInLeftSubArena(this->boss->alivePartsRoot->GetTranslationPt2D())) {
            firstArena  = InLeftArena;
            secondArena = InRightArena;
        }
        else {
            firstArena  = InRightArena;
            secondArena = InLeftArena;
        }
    }

    Point2D firstPortalPos, secondPortalPos;
    if (this->GetArenaPortalLocation(gameModel, firstPortalPos, firstArena) &&
        this->GetArenaPortalLocation(gameModel, secondPortalPos, secondArena)) {

        this->SpawnPortals(gameModel, firstPortalPos, secondPortalPos);
        this->timeSinceLastAttackPortal = 0.0;
    }
}

void FuturismBossAIState::CancelActiveBeam(GameModel& gameModel) {
    if (this->currBeam == NULL) {
        return;
    }

    // Remove any boss beams from the game model
    gameModel.ClearSpecificBeams(Beam::BossBeam);
    this->currBeam = NULL;
}

void FuturismBossAIState::CloseAllActivePortals(GameModel& gameModel) {
    gameModel.ClearSpecificProjectiles(Projectile::PortalBlobProjectile);
}

bool FuturismBossAIState::IsPaddleVisibleToShootAt(const GameModel& gameModel) const {
    std::set<const void*> temp;
    
    const GameLevel* level = gameModel.GetCurrentLevel();
    const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    float rayT, bossToPaddleRayLength;
    Vector2D bossToPaddleVec;

    bossToPaddleVec = paddle->GetCenterPosition() - bossPos;
    bossToPaddleRayLength = bossToPaddleVec.Magnitude();
    if (bossToPaddleRayLength > EPSILON) {
        rayT = std::numeric_limits<float>::max();
        level->GetLevelPieceFirstCollider(Collision::Ray2D(bossPos, bossToPaddleVec / bossToPaddleRayLength), temp, rayT);
    
        if (rayT > bossToPaddleRayLength) {
            return true;
        }
    }

    bossToPaddleVec  = (paddle->GetCenterPosition() + Vector2D(paddle->GetHalfWidthTotal(),0)) - bossPos;
    bossToPaddleRayLength = bossToPaddleVec.Magnitude();
    if (bossToPaddleRayLength > EPSILON) {
        rayT = std::numeric_limits<float>::max();
        level->GetLevelPieceFirstCollider(Collision::Ray2D(bossPos, bossToPaddleVec / bossToPaddleRayLength), temp, rayT);

        if (rayT > bossToPaddleRayLength) {
            return true;
        }
    }

    bossToPaddleVec = (paddle->GetCenterPosition() - Vector2D(paddle->GetHalfWidthTotal(),0)) - bossPos;
    bossToPaddleRayLength = bossToPaddleVec.Magnitude();
    if (bossToPaddleRayLength > EPSILON) {
        rayT = std::numeric_limits<float>::max();
        level->GetLevelPieceFirstCollider(Collision::Ray2D(bossPos, bossToPaddleVec / bossToPaddleRayLength), temp, rayT);

        if (rayT > bossToPaddleRayLength) {
            return true;
        }
    }

    return false;
}

bool FuturismBossAIState::BossHasLineOfSightToBall(const GameBall& ball, const GameLevel& level) const {
    // Shoot a ray from the boss center to the ball, if anything is in between then there's no line of sight...

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    float rayT, bossToBallRayLength;
    Vector2D bossToBallVec;
    std::set<const void*> temp;

    bossToBallVec = ball.GetCenterPosition2D() - bossPos;
    bossToBallRayLength = bossToBallVec.Magnitude();
    if (bossToBallRayLength > EPSILON) {
        rayT = std::numeric_limits<float>::max();
        level.GetLevelPieceFirstCollider(Collision::Ray2D(bossPos, bossToBallVec / bossToBallRayLength), temp, rayT);

        if (rayT > bossToBallRayLength) {
            return true;
        }
    }
    else {
        return true;
    }

    return false;
}

void FuturismBossAIState::DoBasicWeaponSingleShot(GameModel& gameModel, const Point2D& firePos, 
                                                  const Vector2D& fireDir) const {  

    gameModel.AddProjectile(new BossLaserProjectile(firePos, fireDir));

    float sizeMultiplier = 0.3f;
    if (this->boss->GetCoreBody()->GetLocalBounds().IsEmpty()) {
        sizeMultiplier *= FuturismBoss::CORE_BOSS_SIZE / LevelPiece::PIECE_HEIGHT;
    }

    GameEventManager::Instance()->ActionBossEffect(
        ExpandingHaloEffectInfo(this->boss->GetCoreBody(), BASIC_WEAPON_EFFECT_DISCHARGE_TIME, 
        BASIC_WEAPON_EFFECT_DISCHARGE_COLOUR, sizeMultiplier, 
        Vector3D(0,0,this->boss->GetCurrentZShootDistFromOrigin())));
}

void FuturismBossAIState::DoBasicWeaponWaveShot(GameModel& gameModel, const Point2D& firePos, 
                                                const Vector2D& middleFireDir) const {

    int numShotsInWave = this->GetNumShotsPerWaveBurst();
    assert(numShotsInWave > 0);
    if (numShotsInWave % 2 == 1) {
        gameModel.AddProjectile(new BossLaserProjectile(firePos, middleFireDir));
        numShotsInWave--;
    }

    static const float ROTATE_DEGS_BETWEEN_SHOTS = 10.0f;

    Vector2D nextShotDir1 = middleFireDir;
    Vector2D nextShotDir2 = middleFireDir;
    for (int i = 0; i < numShotsInWave; i += 2) {
        nextShotDir1.Rotate(ROTATE_DEGS_BETWEEN_SHOTS);
        nextShotDir2.Rotate(-ROTATE_DEGS_BETWEEN_SHOTS);
        
        gameModel.AddProjectile(new BossLaserProjectile(firePos, nextShotDir1));
        gameModel.AddProjectile(new BossLaserProjectile(firePos, nextShotDir2));
    }

    float sizeMultiplier = 0.3f;
    if (this->boss->GetCoreBody()->GetLocalBounds().IsEmpty()) {
        sizeMultiplier *= FuturismBoss::CORE_BOSS_SIZE / LevelPiece::PIECE_HEIGHT;
    }

    GameEventManager::Instance()->ActionBossEffect(
        ExpandingHaloEffectInfo(this->boss->GetCoreBody(), BASIC_WEAPON_EFFECT_DISCHARGE_TIME, 
        BASIC_WEAPON_EFFECT_DISCHARGE_COLOUR, sizeMultiplier, 
        Vector3D(0,0,this->boss->GetCurrentZShootDistFromOrigin())));
}

void FuturismBossAIState::DoLaserBeamWarningEffect(double timeInSecs, bool isTwitch) const {

    // Effect for the warning...
    EnumBossEffectInfo warningEffectInfo(EnumBossEffectInfo::FuturismBossWarningFlare);
    warningEffectInfo.SetBodyPart(this->boss->GetCoreBody());
    warningEffectInfo.SetOffset(Vector3D(0,0,this->boss->GetCurrentZShootDistFromOrigin()));
    warningEffectInfo.SetSize1D(FuturismBoss::CORE_EYE_SIZE);
    warningEffectInfo.SetTimeInSecs(timeInSecs);
    GameEventManager::Instance()->ActionBossEffect(warningEffectInfo);

    // Sound for the warning...
    GameSound::SoundType soundType = isTwitch ? GameSound::FuturismBossTwitchBeamWarningEvent : GameSound::FuturismBossArcBeamWarningEvent;
    this->boss->GetGameModel()->GetSound()->PlaySound(soundType, false, true, 0.8f);
}
void FuturismBossAIState::DoLaserBeamEnergyEffect(double timeInSecs) const {
    EnumBossEffectInfo energyEffectInfo(EnumBossEffectInfo::FuturismBossBeamEnergy);
    energyEffectInfo.SetBodyPart(this->boss->GetCoreBody());
    energyEffectInfo.SetOffset(Vector3D(0,0,this->boss->GetCurrentZShootDistFromOrigin()));
    energyEffectInfo.SetSize1D(FuturismBoss::CORE_EYE_SIZE);
    energyEffectInfo.SetTimeInSecs(timeInSecs);

    GameEventManager::Instance()->ActionBossEffect(energyEffectInfo);
}

void FuturismBossAIState::DoIceShatterIfFrozen() {
    if (this->currState != FrozenAIState) {
        return;
    }

    // EVENT: Ice break effect
    EnumBossEffectInfo iceBreakEffect(EnumBossEffectInfo::IceBreak);
    iceBreakEffect.SetBodyPart(this->boss->GetCoreBody());
    iceBreakEffect.SetTimeInSecs(2.1);
    iceBreakEffect.SetSize1D(FuturismBoss::CORE_BOSS_SIZE);
    GameEventManager::Instance()->ActionBossEffect(iceBreakEffect);

    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    // Play a sound for the ice shattering
    gameModel->GetSound()->PlaySound(GameSound::BossIceShatterEvent, false, true);
}

void FuturismBossAIState::Refreeze(GameModel& gameModel) {
    if (this->currState != FrozenAIState) {
        return;
    }

    this->waitTimeCountdown = 0.0;
    this->frozenTimeCountdown = this->GetFrozenTime();
    this->frozenShakeEffectCountdown = 0.0;

    GameSound* sound = gameModel.GetSound();
    sound->StopSound(this->iceShakeSoundID);
    this->iceShakeSoundID = INVALID_SOUND_ID;

    // Play sound for the boss being frozen
    sound->PlaySound(GameSound::BossFrozenEvent, false, true);

    // EVENT: Effect of ice clouds and vapour around the boss to complement the freezing effect
    EnumBossEffectInfo frozenEffect(EnumBossEffectInfo::FrozenIceClouds);
    frozenEffect.SetBodyPart(this->boss->GetCoreBody());
    frozenEffect.SetTimeInSecs(1.5);
    GameEventManager::Instance()->ActionBossEffect(frozenEffect);

    // EVENT: Full-screen flash for the re-freeze
    GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.11, 0.0f));
}

void FuturismBossAIState::GoToRandomBasicMoveState() {
    
    int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 5;
    switch (randomNum) {
        case 0: case 1: case 2: this->SetState(MoveToPositionAIState); return;
        case 3: case 4: default: this->SetState(TeleportAIState); return;
    }
}

void FuturismBossAIState::GoToRandomBasicAttackState() {

    int randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % 9;

    switch (randomNum) {
        case 0: case 1: case 2: 
            this->SetState(BasicBurstLineFireAIState); 
            return;

        case 3: case 4: 
            this->SetState(BasicBurstWaveFireAIState);
            return;

        case 5:
            this->SetState(LaserBeamArcAIState);
            return;
        case 6:
            this->SetState(LaserBeamTwitchAIState);
            return;

        case 7: case 8:
        default: {
            float currLifePercent = this->GetTotalLifePercent();
            if (currLifePercent < 1.0) {
                if (currLifePercent < 0.75) {
                    if (currLifePercent < 0.5) {
                        this->SetState(LaserBeamArcAIState);
                    }
                    else {
                        this->SetState(LaserBeamTwitchAIState);
                    }
                }
                else {
                    this->SetState(BasicBurstWaveFireAIState);
                }
            }
            else {
                this->SetState(BasicBurstLineFireAIState); 
            }            
            return;
        }
    }  
}

void FuturismBossAIState::WeakenShieldDebrisEffect(const BossWeakpoint* crackedShield) {
    static const int NUM_DEBRIS_COLOURS = 5;
    static const Colour DEBRIS_COLOURS[NUM_DEBRIS_COLOURS] = {
        Colour(0.57f, 0.6f, 0.63f), Colour(0.467f, 0.463f, 0.455f), Colour(0.5f, 0.52f, 0.545f),
        Colour(0.58f, 0.58f, 0.58f), Colour(0.36f, 0.384f, 0.396f)
    };
    
    Collision::AABB2D shieldAABB = crackedShield->GenerateWorldAABB();

    const float aabbHalfWidth  = shieldAABB.GetWidth() / 2.0f;
    const float aabbHalfHeight = shieldAABB.GetHeight() / 2.0f;

    const Point2D topLeft     = shieldAABB.GetCenter() + Vector2D(-aabbHalfWidth, aabbHalfHeight);
    const Point2D bottomLeft  = shieldAABB.GetCenter() + Vector2D(-aabbHalfWidth, -aabbHalfHeight);
    const Point2D topRight    = shieldAABB.GetCenter() + Vector2D(aabbHalfWidth, aabbHalfHeight);
    const Point2D bottomRight = shieldAABB.GetCenter() + Vector2D(aabbHalfWidth, -aabbHalfHeight);

    // Do the various effects for the shield getting cracked...
    Point2D centerPos = crackedShield->GetTranslationPt2D();
    DebrisEffectInfo info0(crackedShield, topLeft, 
        DEBRIS_COLOURS[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_DEBRIS_COLOURS], 2.5, 3.5, 4, 1.33f, 
        2.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*1.0);
    info0.OverrideDirection(topLeft - centerPos);
    GameEventManager::Instance()->ActionBossEffect(info0);

    DebrisEffectInfo info1(crackedShield, topRight, 
        DEBRIS_COLOURS[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_DEBRIS_COLOURS], 2.5, 3.5, 3, 1.33f, 
        2.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*1.0);
    info1.OverrideDirection(topRight - centerPos);
    GameEventManager::Instance()->ActionBossEffect(info1);

    DebrisEffectInfo info2(crackedShield, bottomLeft, 
        DEBRIS_COLOURS[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_DEBRIS_COLOURS], 2.5, 3.5, 4, 1.33f, 
        2.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*1.0);
    info2.OverrideDirection(bottomLeft - centerPos);
    GameEventManager::Instance()->ActionBossEffect(info2);

    DebrisEffectInfo info3(crackedShield, bottomRight, 
        DEBRIS_COLOURS[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_DEBRIS_COLOURS], 2.5, 3.5, 3, 1.33f, 
        2.0f + Randomizer::GetInstance()->RandomNumZeroToOne()*1.0);
    info3.OverrideDirection(bottomRight - centerPos);
    GameEventManager::Instance()->ActionBossEffect(info3);
}

void FuturismBossAIState::WeakenShield(const Vector2D& hitDir, float hitMagnitude, 
                                       FuturismBoss::ShieldLimbType shieldType, bool doStateChange) {

    if (this->boss->GetShieldBodyPart(shieldType)->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
        assert(false);
        return;
    }

    size_t shieldIdx = this->boss->GetShieldIndex(shieldType);
    this->boss->ConvertAliveBodyPartToWeakpoint(shieldIdx, SHIELD_LIFE_POINTS, SHIELD_BALL_DAMAGE);

    assert(dynamic_cast<const BossWeakpoint*>(this->boss->bodyParts[shieldIdx]) != NULL);
    const BossWeakpoint* crackedShield = static_cast<const BossWeakpoint*>(this->boss->bodyParts[shieldIdx]);
    
    this->WeakenShieldDebrisEffect(crackedShield);

    if (doStateChange) {
        GameEventManager::Instance()->ActionBossHurt(crackedShield);
        this->bossHurtAnim = Boss::BuildBossHurtMoveAnim(hitDir, hitMagnitude, 1.0);
        this->DoIceShatterIfFrozen();
        this->SetState(ShieldPartCrackedAIState);
    }
}

void FuturismBossAIState::DestroyShield(const Vector2D& hitDir, FuturismBoss::ShieldLimbType shieldType, 
                                        bool doStateChange) {

    BossBodyPart* shieldToDestroy = this->boss->GetShieldBodyPart(shieldType);
    if (shieldToDestroy == NULL || shieldToDestroy->GetType() != AbstractBossBodyPart::WeakpointBodyPart) {
        assert(false);
        return;
    }

    static const double TOTAL_BLOWUP_TIME = 3.0;

    Collision::AABB2D shieldAABB = shieldToDestroy->GenerateWorldAABB();
    float shieldSize = std::max<float>(shieldAABB.GetWidth(), shieldAABB.GetHeight());

    // Destroy the shield...
    shieldToDestroy->SetLocalTransform(Vector3D(0,0,0), 0.0f);
    this->boss->ConvertAliveBodyPartToDeadBodyPart(shieldToDestroy);
    this->boss->UpdateBoundsForDestroyedShieldLimb(shieldType);

    shieldToDestroy->AnimateColourRGBA(Boss::BuildBossHurtFlashAndFadeAnim(TOTAL_BLOWUP_TIME));
    shieldToDestroy->AnimateLocalTranslation(this->GenerateShieldDeathTranslationAnimation(shieldType, shieldSize, 
        (hitDir[0] == 0 ? Randomizer::GetInstance()->RandomNegativeOrPositive() : NumberFuncs::SignOf(hitDir[0])), TOTAL_BLOWUP_TIME));
    shieldToDestroy->AnimateLocalZRotation(this->GenerateShieldDeathRotationAnimation(shieldType, TOTAL_BLOWUP_TIME));
   
    if (doStateChange) {
        // ... and make the boss look noticeably hurt
        this->bossHurtAnim = Boss::BuildBossHurtMoveAnim(hitDir, FuturismBoss::CORE_BOSS_HALF_SIZE/2.0f, 
            BossWeakpoint::DEFAULT_INVULNERABLE_TIME_IN_SECS);
        GameEventManager::Instance()->ActionBossHurt(static_cast<BossWeakpoint*>(shieldToDestroy));

        this->DoIceShatterIfFrozen();
        this->SetState(ShieldPartDestroyedAIState);
    }
}

AnimationMultiLerp<Vector3D> 
FuturismBossAIState::GenerateShieldDeathTranslationAnimation(FuturismBoss::ShieldLimbType shieldType,
                                                             float shieldSize, float xDir, 
                                                             double timeInSecs) const {
    
    const float addedHeight = 5*shieldSize;
    const float bossHalfHeight = this->boss->alivePartsRoot->GenerateWorldAABB().GetHeight() / 2.0f;
    const float maxHeight = 1.2f*FuturismBoss::GetRightSubArenaMaxYBossPos(bossHalfHeight);

    float xDist = 1.75f*xDir*shieldSize;
    float yDist = -(maxHeight + addedHeight);
    switch (shieldType) {

        case FuturismBoss::BottomShield:
            yDist *= -1; // Because the mesh is rotated locally by 180 degrees!!
            break;
        case FuturismBoss::LeftShield:
            yDist *= -1; // Because the mesh is rotated locally by 180 degrees!!
            break;

        case FuturismBoss::RightShield:
        case FuturismBoss::CoreShield:
        case FuturismBoss::TopShield:
        default:
            break;
    }

    return Boss::BuildLimbFallOffTranslationAnim(timeInSecs, xDist, yDist);
}

AnimationMultiLerp<float> 
FuturismBossAIState::GenerateShieldDeathRotationAnimation(FuturismBoss::ShieldLimbType shieldType, 
                                                          double timeInSecs) const {

    float randomAngle = 50.0f + Randomizer::GetInstance()->RandomUnsignedInt() % 20;
    switch (shieldType) {

        case FuturismBoss::CoreShield:
        case FuturismBoss::TopShield:
        case FuturismBoss::BottomShield:
            randomAngle *= Randomizer::GetInstance()->RandomNegativeOrPositive();
            break;

        case FuturismBoss::RightShield:
            randomAngle *= -1;
            break;

        case FuturismBoss::LeftShield:
        default:
            break;
    }

    return Boss::BuildLimbFallOffZRotationAnim(timeInSecs, randomAngle);
}

void FuturismBossAIState::SetupCoreRotationVelAnim(double totalTime) {
    std::vector<double> timeVals;
    timeVals.reserve(5);
    timeVals.push_back(0.0);
    timeVals.push_back(0.10*totalTime);
    timeVals.push_back(0.25*totalTime);
    timeVals.push_back(0.75*totalTime);
    timeVals.push_back(totalTime);

    std::vector<float> rotVelVals;
    rotVelVals.reserve(timeVals.size());
    rotVelVals.push_back(0.0f);
    rotVelVals.push_back(300.0f);
    rotVelVals.push_back(600.0f);
    rotVelVals.push_back(600.0f);
    rotVelVals.push_back(300.0f);

    this->coreRotVelAnim.SetLerp(timeVals, rotVelVals);
    this->coreRotVelAnim.SetRepeat(false);
    this->coreRotVelAnim.SetInterpolantValue(0.0f);
}

//void FuturismBossAIState::SetupFrozenShakeAnim(double totalTime) {
//    this->frozenShakeAnim.SetLerp(timeVals, shakeVals);
//    this->frozenShakeAnim.SetRepeat(false);
//    this->frozenShakeAnim.SetInterpolantValue(0.0f);
//}

void FuturismBossAIState::AttachBall(GameBall* ball) {
    assert(ball != NULL);

    this->ballDirBeforeAttachment = ball->GetDirection();
    this->ballPosBeforeAttachment = ball->GetCenterPosition2D();

    // Basic attaching procedure -- make sure the ball is put into a complacent state so it can be controlled
    ball->SetLastPieceCollidedWith(NULL);
    ball->SetVelocity(ball->GetSpeed(), Vector2D(0, 0));
    ball->SetBallBallCollisionsDisabled();
    ball->SetBallPaddleCollisionsDisabled();
    this->attachedBall = ball;

    // EVENT: Start shooting the "attractor" beam/ray at the ball...
    assert(this->totalBallAttractTime > 0);
    EnumBossEffectInfo attractorBeamInfo(EnumBossEffectInfo::FuturismBossAttractorBeam);
    attractorBeamInfo.SetSize1D(FuturismBoss::CORE_EYE_SIZE);
    attractorBeamInfo.SetTimeInSecs(this->totalBallAttractTime + this->GetSingleTeleportInAndOutTime() + this->GetBallDiscardTime());
    attractorBeamInfo.SetBodyPart(this->boss->GetCoreBody());
    attractorBeamInfo.SetDirection(this->ballPosBeforeAttachment - this->boss->alivePartsRoot->GetTranslationPt2D());

    GameEventManager::Instance()->ActionBossEffect(attractorBeamInfo);
}

void FuturismBossAIState::DetachAndShootBall(const Vector2D& shootDir) {
    if (this->attachedBall == NULL) {
        return;
    }

    GameModel* gameModel = this->boss->GetGameModel();
    assert(gameModel != NULL);

    const std::list<GameBall*>& balls = gameModel->GetGameBalls();
    if (balls.empty() || this->attachedBall != balls.front()) {
        return;
    }

    this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), shootDir);
    this->attachedBall->SetBallBallCollisionsEnabled();
    this->attachedBall->SetBallPaddleCollisionsEnabled();
    this->attachedBall = NULL;
}

void FuturismBossAIState::ShakeBall(GameBall* ball) {
    if (ball != this->attachedBall) {
        return;
    }

    static int SIGN_SWITCH = 1;
    ball->SetCenterPosition(ball->GetCenterPosition2D() + 
        Vector2D(SIGN_SWITCH*0.33f*Randomizer::GetInstance()->RandomNumNegOneToOne()*GameBall::DEFAULT_BALL_RADIUS,
        SIGN_SWITCH*0.33f*Randomizer::GetInstance()->RandomNumNegOneToOne()*GameBall::DEFAULT_BALL_RADIUS));
    SIGN_SWITCH *= -1;
}

bool FuturismBossAIState::IsBallAvailableForAttractingAndTeleporting(const GameModel& gameModel) const {
    const std::list<GameBall*>& balls = gameModel.GetGameBalls();
    assert(!balls.empty());

    const GameLevel* level = gameModel.GetCurrentLevel();
    GameBall* ball = balls.front();
    assert(level != NULL && ball != NULL);
    
    // Attract the ball (if it's available to be attracted...)
    return gameModel.GetCurrentStateType() == GameState::BallInPlayStateType && this->BossHasLineOfSightToBall(*ball, *level);
}

bool FuturismBossAIState::IsBallFarEnoughAwayToInitiateAttracting(const GameModel& gameModel) const {
    const std::list<GameBall*>& balls = gameModel.GetGameBalls();
    assert(!balls.empty());
    GameBall* ball = balls.front();
    assert(ball != NULL);

    // Make sure the ball is far enough away as well...
    Vector2D bossToBallVec = (ball->GetCenterPosition2D() - this->boss->alivePartsRoot->GetTranslationPt2D());
    double distToBallSqr = Vector2D::Dot(bossToBallVec, bossToBallVec);

    Collision::Circle2D bossCircleBounds = this->boss->alivePartsRoot->GenerateWorldCircleBounds();

    return distToBallSqr >= pow(2.0f*bossCircleBounds.Radius(), 2);
}

void FuturismBossAIState::GetCenterRotationBodyPartAndFullDegAmt(AbstractBossBodyPart*& bodyPart, float& fullDegAmt) const {
    // If the core shield is destroyed we rotate the core assembly/body instead
    if (this->boss->GetCoreShield()->GetIsDestroyed()) {
        bodyPart = this->boss->bodyParts[this->boss->coreAssemblyIdx];
        fullDegAmt = 90.0f;
    }
    else {
        bodyPart = this->boss->bodyParts[this->boss->coreShieldIdx];
        fullDegAmt = 45.0f;
    }
}

void FuturismBossAIState::InterruptSounds(GameModel& gameModel) {
    GameSound* sound = gameModel.GetSound();
    sound->StopSound(this->attractorBeamLoopSoundID);
    sound->StopSound(this->chargingSoundID);
    sound->StopSound(this->spinCoolDownSoundID);

    this->attractorBeamLoopSoundID = INVALID_SOUND_ID;
    this->chargingSoundID = INVALID_SOUND_ID;
    this->spinCoolDownSoundID = INVALID_SOUND_ID;
}