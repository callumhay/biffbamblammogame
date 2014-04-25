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
#include "FuturismBossStage2AIState.h"
#include "PaddleRocketProjectile.h"
#include "GameModel.h"
#include "BossWeakpoint.h"

const double FuturismBossStage1AIState::MIN_TIME_UNTIL_FIRST_PORTAL = 10.0;

FuturismBossStage1AIState::FuturismBossStage1AIState(FuturismBoss* boss) : FuturismBossAIState(boss) {
    
    // We don't want the boss to shoot portals right away, let the player get
    // acquainted with the boss first -- we do this by setting a negative time on the 
    // time since last portal...
    // To calculate this we need to offset by the termination time of the portal since
    // the boss won't shoot if a portal still exists
    this->timeSinceLastStratPortal = FuturismBossAIState::BOSS_PORTAL_TERMINATION_TIME_IN_SECS - MIN_TIME_UNTIL_FIRST_PORTAL;

    // The boss spends this entire state in the initial, left sub arena of the level
    this->arenaState = FuturismBossAIState::InLeftArena;

    // NOTE: Shields aren't weak-points, but they can be destroyed by rockets

    // Boss starts off by teleporting into the left arena...
    this->SetState(FuturismBossAIState::TeleportAIState);
}

FuturismBossStage1AIState::~FuturismBossStage1AIState() {
}

void FuturismBossStage1AIState::GetRandomMoveToPositions(const GameModel& gameModel,
                                                         std::vector<Point2D>& positions) const {
    static const int NUM_RANDOM_POSITIONS = 5;
    
    const std::vector<Point2D>& allPositions = this->GetArenaMoveToPositions(this->arenaState);
    this->GetValidMoveToPositions(gameModel, allPositions, positions);
    
    // Exit if the number of positions is already the maximum or less
    if (static_cast<int>(positions.size()) <= NUM_RANDOM_POSITIONS) {
        return;
    }

    // In this state the boss will only be moving around in the left sub arena
    BossAIState::GetFurthestDistFromBossPositions(this->boss->alivePartsRoot->GetTranslationPt2D(), 
        positions, NUM_RANDOM_POSITIONS, positions);
}

double FuturismBossStage1AIState::GetAfterAttackWaitTime(FuturismBossAIState::AIState attackState) const {
    switch (attackState) {

        case FuturismBossAIState::BasicBurstLineFireAIState:
        case FuturismBossAIState::BasicBurstWaveFireAIState:
            return 1.2;

        case FuturismBossAIState::LaserBeamArcAIState:
            return 1.8;

        case FuturismBossAIState::LaserBeamTwitchAIState:
            return 1.5;

        default:
            break;
    }

    return 1.2;
}

bool FuturismBossStage1AIState::AllShieldsDestroyedToEndThisState() const { 
    return this->boss->AllLimbShieldsDestroyed();
} 

void FuturismBossStage1AIState::GetStrategyPortalCandidatePieces(const GameLevel& level, Collision::AABB2D& pieceBounds,
                                                                 std::set<LevelPiece*>& candidates) {

    pieceBounds = FuturismBoss::GetRocketStrategyPortalSearchAABB(
        level, FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT);
    FuturismBoss::GetRocketStrategyPortalSearchPieces(level, candidates);
}

void FuturismBossStage1AIState::GoToNextState(const GameModel& gameModel) {

    // FOR TESTING STATES
    /*
    {
        static int TEMP = 0;
        if (TEMP % 2 == 0) {
            this->arenaState = InRightArena;
            this->SetState(MoveToCenterAIState);
        }
        else {
            
            this->SetState(DestroyLevelBarrierAIState);
        }
        TEMP++;
        return;
    }
        
    {
        // Check to see if the boss is on the same side of the arena as the ball, 
        // if not, teleport to that side immediately
        const GameBall* ball = gameModel.GetGameBalls().front();
        Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
        if (FuturismBoss::IsInLeftSubArena(ball->GetCenterPosition2D()) != FuturismBoss::IsInLeftSubArena(bossPos)) {
            // Teleport the boss to the other side of the arena 
            // (this will be decided and done automatically by the teleport state)...
            this->SetState(TeleportAIState);
            return;
        }

        static int TEMP = 4;
        switch (TEMP % 7) {
            case 0: this->SetState(BasicBurstWaveFireAIState); break;
            case 1: this->SetState(BasicBurstLineFireAIState); break;
            case 2: this->SetState(LaserBeamArcAIState); break;
            case 3: this->SetState(StationaryFireStrategyPortalAIState); break;
            case 4: this->SetState(LaserBeamTwitchAIState); break; 
            case 5: this->SetState(TeleportAIState); break;
            case 6:
                if (this->IsBallAvailableForAttractingAndTeleporting(gameModel)) {
                    this->SetState(BallAttractAIState);
                }
                break;
            default: break;
        }
        TEMP++;
        return;
    }
    */

    // If it's been too long since the last strategy portal was shot then we should shoot one
    // or else the battle could take forever (the player NEEDS the portal to get the item required
    // to move on to the next high-level AI state!)
    if (this->timeSinceLastStratPortal > 2.4*FuturismBossAIState::BOSS_PORTAL_TERMINATION_TIME_IN_SECS) {
        this->SetState(StationaryFireStrategyPortalAIState);
        return;
    }
    
    // If a rocket is in-transit then we always go to an attack state...

    // Special check: Make sure the boss isn't 'camping' at the bottom of the level
    static int BOTTOM_COUNT = 0;
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    float bottomY = FuturismBoss::GetLeftSubArenaMinYBossPos(FuturismBoss::FULLY_SHIELDED_BOSS_HALF_HEIGHT);
    if (bossPos[1] <= bottomY) {
        BOTTOM_COUNT++;
        if (BOTTOM_COUNT > 3) {
            this->SetState(Randomizer::GetInstance()->RandomTrueOrFalse() ? TeleportAIState : MoveToPositionAIState);
            BOTTOM_COUNT = 0;
            return;
        }
    }
    else {
        BOTTOM_COUNT = 0;
    }

    static const int WAVE_BURST_IDX  = 0;
    static const int LINE_BURST_IDX  = 1;
    static const int ARC_BEAM_IDX    = 2;
    static const int TWITCH_BEAM_IDX = 3;
    static const int TELEPORT_IDX    = 4;
    static const int MOVE_TO_POS_IDX = 5;

    // Depending on whether the boss can see the paddle or not we decide what state to move to next
    int randomNum = 0;
    if (this->IsPaddleVisibleToShootAt(gameModel)) {
        
        // Boss can see the paddle -- it's a good time to shoot at it (occasionally teleport as well)
        static const int NUM_CHOICES = 19;
        static const int CHOICES[NUM_CHOICES] = { 
            MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, 
            TELEPORT_IDX, TELEPORT_IDX, 
            WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX,
            LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, 
            ARC_BEAM_IDX, 
            TWITCH_BEAM_IDX, TWITCH_BEAM_IDX, TWITCH_BEAM_IDX 
        };

        randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
    }
    else {
        
        // Boss can't see the paddle, make movement/teleportation more likely than anything else
        static const int NUM_CHOICES = 16;
        static const int CHOICES[NUM_CHOICES] = { 
            MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, MOVE_TO_POS_IDX,
            TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
            WAVE_BURST_IDX, WAVE_BURST_IDX,
            LINE_BURST_IDX, LINE_BURST_IDX,
            ARC_BEAM_IDX 
        };

        randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
    }

    // Do a redundancy check (make sure we don't revisit states that we were just at in a way that
    // looks obvious and robotic)
    switch (this->currState) {
        case MoveToPositionAIState:
        case TeleportAIState:
            if (this->numConsecutiveMoves > 2) {
                // Set the state to an attack-type state
                this->GoToRandomBasicAttackState();
                return;
            }
            break;

        case BasicBurstLineFireAIState:
        case BasicBurstWaveFireAIState:
            if (this->numConsecutiveAttacks > 4) {
                this->RocketAwareGoToRandomMoveState(gameModel);
                return;
            }
            else if (this->numConsecutiveShots > 3) {
                static const int NUM_CHOICES = 4;
                static const int CHOICES[NUM_CHOICES] = { 
                    MOVE_TO_POS_IDX, TELEPORT_IDX, ARC_BEAM_IDX, TWITCH_BEAM_IDX 
                };
                randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
            }
            break;

        case LaserBeamTwitchAIState:
            if (randomNum == TWITCH_BEAM_IDX) {
                this->RocketAwareGoToRandomMoveState(gameModel);
                return;
            }
        case LaserBeamArcAIState:
            if (randomNum == ARC_BEAM_IDX) {
                this->RocketAwareGoToRandomMoveState(gameModel);
                return;
            }

            if (this->numConsecutiveAttacks > 4) {
                this->RocketAwareGoToRandomMoveState(gameModel);
                return;
            }
            else if (this->numConsecutiveBeams > 1) {
                static const int NUM_CHOICES = 4;
                static const int CHOICES[NUM_CHOICES] = { 
                    MOVE_TO_POS_IDX, TELEPORT_IDX, WAVE_BURST_IDX, LINE_BURST_IDX 
                };
                randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
            }
            break;

        case StationaryFireStrategyPortalAIState:
        default:
            break;
    }

    // Choose a next state at random (with some bias based on previous state(s))
    switch (randomNum) {
        case WAVE_BURST_IDX:   this->SetState(BasicBurstWaveFireAIState); break;
        case LINE_BURST_IDX:   this->SetState(BasicBurstLineFireAIState); break;
        case ARC_BEAM_IDX:     this->SetState(LaserBeamArcAIState); break;
        case TWITCH_BEAM_IDX:  this->SetState(LaserBeamTwitchAIState); break; 
        
        case TELEPORT_IDX:
            if (this->LevelHasRocketInIt(gameModel)) { 
                this->GoToRandomBasicAttackState(); 
            }
            else {
                this->SetState(TeleportAIState);
            }
            break;

        case MOVE_TO_POS_IDX:  
        default: 
            if (this->LevelHasRocketInIt(gameModel)) { 
                this->GoToRandomBasicAttackState(); 
            }
            else {
                this->SetState(MoveToPositionAIState); 
            }
            break;
    }
}

FuturismBossAIState* FuturismBossStage1AIState::BuildNextAIState() const {
    return new FuturismBossStage2AIState(this->boss, this->arenaState, this->currCoreRotInDegs);
}

float FuturismBossStage1AIState::GetTotalLifePercent() const {
    float lifePercent = 0.0f;
    if (!this->boss->GetTopShield()->GetIsDestroyed()) {
        if (this->boss->IsTopShieldWeakened()) {
            lifePercent += 0.125f;
        }
        else {
            lifePercent += 0.25f;
        }
    }
    if (!this->boss->GetBottomShield()->GetIsDestroyed()) {
        if (this->boss->IsBottomShieldWeakened()) {
            lifePercent += 0.125f;
        }
        else {
            lifePercent += 0.25f;
        }
    }
    if (!this->boss->GetLeftShield()->GetIsDestroyed()) {
        if (this->boss->IsLeftShieldWeakened()) {
            lifePercent += 0.125f;
        }
        else {
            lifePercent += 0.25f;
        }
    }
    if (!this->boss->GetRightShield()->GetIsDestroyed()) {
        if (this->boss->IsRightShieldWeakened()) {
            lifePercent += 0.125f;
        }
        else {
            lifePercent += 0.25f;
        }
    }

    return lifePercent;
}

void FuturismBossStage1AIState::CollisionOccurred(GameModel* gameModel, GameBall& ball, 
                                                  BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);
    
    assert(gameModel != NULL);
    assert(collisionPart != NULL);

    if (this->currState == ShieldPartCrackedAIState ||
        this->currState == ShieldPartDestroyedAIState || this->currState == AngryAIState) {
        return;
    }

    if (collisionPart == this->boss->GetTopShield() && this->boss->IsTopShieldWeakened()) {
        this->DestroyShield(ball.GetDirection(), FuturismBoss::TopShield);
    }
    else if (collisionPart == this->boss->GetBottomShield() && this->boss->IsBottomShieldWeakened()) {
        this->DestroyShield(ball.GetDirection(), FuturismBoss::BottomShield);
    }
    else if (collisionPart == this->boss->GetLeftShield() && this->boss->IsLeftShieldWeakened()) {
        this->DestroyShield(ball.GetDirection(), FuturismBoss::LeftShield);
    }
    else if (collisionPart == this->boss->GetRightShield() && this->boss->IsRightShieldWeakened()) {
        this->DestroyShield(ball.GetDirection(), FuturismBoss::RightShield);
    }
}

void FuturismBossStage1AIState::CollisionOccurred(GameModel* gameModel, Projectile* projectile, 
                                                  BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);

    assert(gameModel != NULL);
    assert(projectile != NULL);
    assert(collisionPart != NULL);

    // Only rocket projectiles can hurt the boss in this state, also make sure the boss is in a good state to be hurt
    if ((!projectile->IsRocket() && projectile->GetType() != Projectile::PaddleIceBlastProjectile) || 
        this->currState == ShieldPartCrackedAIState || this->currState == ShieldPartDestroyedAIState || 
        this->currState == AngryAIState) {

        return;
    }

    if (projectile->IsRocket()) {
        if (collisionPart == this->boss->GetTopShield()) {
            if (this->boss->IsTopShieldWeakened()) {
                this->DestroyShield(projectile->GetVelocityDirection(), FuturismBoss::TopShield);
            }
            else {
                this->WeakenShield(projectile->GetVelocityDirection(), projectile->GetHeight(), FuturismBoss::TopShield);
            }
        }
        else if (collisionPart == this->boss->GetBottomShield()) {
            if (this->boss->IsBottomShieldWeakened()) {
                this->DestroyShield(projectile->GetVelocityDirection(), FuturismBoss::BottomShield);
            }
            else {
                this->WeakenShield(projectile->GetVelocityDirection(), projectile->GetHeight(), FuturismBoss::BottomShield);
            }
        }
        else if (collisionPart == this->boss->GetLeftShield()) {
            if (this->boss->IsLeftShieldWeakened()) {
                this->DestroyShield(projectile->GetVelocityDirection(), FuturismBoss::LeftShield);
            }
            else {
                this->WeakenShield(projectile->GetVelocityDirection(), projectile->GetHeight(), FuturismBoss::LeftShield);
            }
        }
        else if (collisionPart == this->boss->GetRightShield()) {
            if (this->boss->IsRightShieldWeakened()) {
                this->DestroyShield(projectile->GetVelocityDirection(), FuturismBoss::RightShield);
            }
            else {
                this->WeakenShield(projectile->GetVelocityDirection(), projectile->GetHeight(), FuturismBoss::RightShield);
            }
        }
    }
    else if (collisionPart == this->boss->GetCoreBody() || collisionPart == this->boss->GetCoreShield() &&
             projectile->GetType() == Projectile::PaddleIceBlastProjectile) {

        // Freeze the boss if an ice blast hits the core... this has no real effect in this state damage-wise, but
        // since it's still possible to get the ice blaster item, do we should do it
        this->SetState(FrozenAIState);
    }
}

bool FuturismBossStage1AIState::LevelHasRocketInIt(const GameModel& gameModel) const {
    const GameModel::ProjectileList* activeRockets = 
        gameModel.GetActiveProjectilesWithType(Projectile::PaddleRocketBulletProjectile);
    return (activeRockets != NULL && !activeRockets->empty());
}

void FuturismBossStage1AIState::RocketAwareGoToRandomMoveState(const GameModel& gameModel) {
    if (this->LevelHasRocketInIt(gameModel)) {
        // One in three chance of moving...
        if (Randomizer::GetInstance()->RandomUnsignedInt() % 3 != 0) {
            this->GoToRandomBasicAttackState();
            return;
        }
    }
    
    this->GoToRandomBasicMoveState();
}