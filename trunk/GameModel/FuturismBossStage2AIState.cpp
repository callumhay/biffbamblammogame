/**
 * FuturismStage2AIState.cpp
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

#include "FuturismBossStage2AIState.h"
#include "FuturismBossStage3AIState.h"
#include "GameModel.h"

const double FuturismBossStage2AIState::MIN_TIME_UNTIL_FIRST_PORTAL = 10.0;

FuturismBossStage2AIState::FuturismBossStage2AIState(FuturismBoss* boss) : 
FuturismBossAIState(boss), numConsecutiveRocketStratPortals(0),
hasDoneInitialBallTeleport(false) {

    // Regenerate the boss bounds
    this->boss->RegenerateBoundsForCoreWithShield();

    // We don't want the boss to shoot portals right away...
    this->timeSinceLastStratPortal = FuturismBossAIState::STRATEGY_PORTAL_TERMINATION_TIME_IN_SECS - MIN_TIME_UNTIL_FIRST_PORTAL;

    // The boss starts this state by attempting to 'grab' the ball and go to the other side of the level
    this->SetState(BallAttractAIState);
}

FuturismBossStage2AIState::~FuturismBossStage2AIState() {
}

void FuturismBossStage2AIState::GoToNextState(const GameModel& gameModel) {

    // Special cases for ensuring that the boss attempts to teleport with the ball to the other sub-arena
    if (this->currState == BallDiscardAIState) {
        this->hasDoneInitialBallTeleport = true;
    }
    else if (!this->hasDoneInitialBallTeleport) {
        // Try to teleport the ball if it's available...
        if (this->IsBallAvailableForAttractingAndTeleporting(gameModel) &&
            this->IsBallFarEnoughAwayToInitiateAttracting(gameModel)) {

            this->SetState(BallAttractAIState);
            return;
        }
    }

    // Check to see if the boss is on the same side of the arena as the ball, 
    // if not, teleport to that side immediately
    const GameBall* ball = gameModel.GetGameBalls().front();
    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();

    bool ballInLeftSubArena = FuturismBoss::IsInLeftSubArena(ball->GetCenterPosition2D());
    bool ballNotOnSameSideAsBoss = (ballInLeftSubArena || FuturismBoss::IsInRightSubArena(ball->GetCenterPosition2D())) &&
        ballInLeftSubArena != FuturismBoss::IsInLeftSubArena(bossPos);

    if (ballNotOnSameSideAsBoss) {
        // Teleport the boss to the other side of the arena 
        // (this will be decided and done automatically by the teleport state)...
        this->SetState(TeleportAIState);
        return;
    }

    // If it's been too long since the last strategy portal was shot then we should shoot one
    // or else the battle could take forever (the player NEEDS the portal to get the item required
    // to move on to the next high-level AI state!)
    if (this->timeSinceLastStratPortal > 2.3*FuturismBossAIState::STRATEGY_PORTAL_TERMINATION_TIME_IN_SECS &&
        FuturismBoss::IsInRightSubArena(bossPos)) {
        this->SetState(StationaryFireStrategyPortalAIState);
        return;
    }

/*
    // FOR TESTING STATES
    static int TEMP = 0;
    switch (TEMP % 6) {
        case 0: this->SetState(BasicBurstWaveFireAIState); break;
        case 1: this->SetState(BasicBurstLineFireAIState); break;
        case 2: this->SetState(LaserBeamArcAIState); break;
        case 3: this->SetState(StationaryFireStrategyPortalAIState); break;
        case 4: this->SetState(LaserBeamTwitchAIState); break; 
        case 5: this->SetState(TeleportAIState); break;
        default: break;
    }
    TEMP++;
    return;
*/

    // If the boss and ball are in the left sub-arena then the boss will tend to want to move play to
    // the right sub-arena (since the entire first stage is played in the left sub-arena, and the ice blast
    // is easier to acquire from the right sub-arena)...
    if (this->numConsecutiveMoves == 0 && this->currState != FuturismBossAIState::BallDiscardAIState &&
        (Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) &&
        FuturismBoss::IsInLeftSubArena(ball->GetCenterPosition2D()) && FuturismBoss::IsInLeftSubArena(bossPos) &&
        this->IsBallAvailableForAttractingAndTeleporting(gameModel) && 
        this->IsBallFarEnoughAwayToInitiateAttracting(gameModel)) {
        
        this->SetState(BallAttractAIState);
        return;
    }

    // Last special case: If the boss hasn't shot a portal in a while then there's a chance of shooting
    // an attack portal
    if (this->timeSinceLastAttackPortal > 1.25*FuturismBossAIState::STRATEGY_PORTAL_TERMINATION_TIME_IN_SECS &&
        this->timeSinceLastStratPortal > FuturismBossAIState::STRATEGY_PORTAL_TERMINATION_TIME_IN_SECS &&
        Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) {

        this->SetState(StationaryFireWeaponPortalAIState);
        return;
    }

    // Regular attack/move protocol...
    static const int WAVE_BURST_IDX    = 0;
    static const int LINE_BURST_IDX    = 1;
    static const int ARC_BEAM_IDX      = 2;
    static const int TWITCH_BEAM_IDX   = 3;
    static const int TELEPORT_IDX      = 4;
    static const int MOVE_TO_POS_IDX   = 5;

    // Depending on whether the boss can see the paddle or not we decide what state to move to next
    int randomNum = 0;
    if (this->IsPaddleVisibleToShootAt(gameModel)) {

        // Boss can see the paddle -- it's a good time to shoot at it (occasionally teleport as well)
        static const int NUM_CHOICES = 22;
        static const int CHOICES[NUM_CHOICES] = { 
            MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, 
            TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
            WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX, WAVE_BURST_IDX,
            LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, LINE_BURST_IDX, 
            ARC_BEAM_IDX, ARC_BEAM_IDX,
            TWITCH_BEAM_IDX, TWITCH_BEAM_IDX, TWITCH_BEAM_IDX 
        };

        randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
    }
    else {
        // Boss can't see the paddle, make movement/teleportation more likely than anything else
        static const int NUM_CHOICES = 12;
        static const int CHOICES[NUM_CHOICES] = { 
            MOVE_TO_POS_IDX, MOVE_TO_POS_IDX, MOVE_TO_POS_IDX,
            TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
            WAVE_BURST_IDX,
            LINE_BURST_IDX,
            ARC_BEAM_IDX, ARC_BEAM_IDX
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
            if (this->numConsecutiveAttacks > 3) {
                this->GoToRandomBasicMoveState();
                return;
            }

            if (this->numConsecutiveShots > 2) {
                static const int NUM_CHOICES = 7;
                static const int CHOICES[NUM_CHOICES] = { 
                    MOVE_TO_POS_IDX, MOVE_TO_POS_IDX,
                    TELEPORT_IDX, TELEPORT_IDX, TELEPORT_IDX,
                    TWITCH_BEAM_IDX, 
                    ARC_BEAM_IDX
                };
                randomNum = CHOICES[Randomizer::GetInstance()->RandomUnsignedInt() % NUM_CHOICES];
            }

            break;

        case LaserBeamTwitchAIState:
            if (randomNum == TWITCH_BEAM_IDX) {
                this->GoToRandomBasicMoveState();
                return;
            }
        case LaserBeamArcAIState:
            if (randomNum == ARC_BEAM_IDX) {
                this->GoToRandomBasicMoveState();
                return;
            }

            if (this->numConsecutiveAttacks > 3) {
                this->GoToRandomBasicMoveState();
                return;
            }
            
            if (this->numConsecutiveBeams > 1) {
                static const int NUM_CHOICES = 4;
                static const int CHOICES[NUM_CHOICES] = { 
                    MOVE_TO_POS_IDX, TELEPORT_IDX, WAVE_BURST_IDX, LINE_BURST_IDX 
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
        case TELEPORT_IDX:     this->SetState(TeleportAIState); break;
        case MOVE_TO_POS_IDX:  this->SetState(MoveToPositionAIState); break;

        default: this->SetState(TeleportAIState); break;
    }
}

FuturismBossAIState* FuturismBossStage2AIState::BuildNextAIState() const {
    return new FuturismBossStage3AIState(this->boss);
}

void FuturismBossStage2AIState::GetRandomMoveToPositions(const GameModel& gameModel, std::vector<Point2D>& positions) const {
    
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

double FuturismBossStage2AIState::GetAfterAttackWaitTime(FuturismBossAIState::AIState attackState) const {
    switch (attackState) {

        case FuturismBossAIState::BasicBurstLineFireAIState:
        case FuturismBossAIState::BasicBurstWaveFireAIState:
            return 0.75;

        case FuturismBossAIState::LaserBeamArcAIState:
            return 1.5;

        case FuturismBossAIState::LaserBeamTwitchAIState:
            return 1.0;

        default:
            break;
    }

    return 0.75;
}

bool FuturismBossStage2AIState::AllShieldsDestroyedToEndThisState() const {
    return this->boss->GetCoreShield()->GetIsDestroyed();
}

void FuturismBossStage2AIState::GetStrategyPortalCandidatePieces(const GameLevel& level, Collision::AABB2D& pieceBounds, 
                                                                 std::set<LevelPiece*>& candidates) {

    // Choose between the ice blast and rocket, favouring the rocket...
    bool iceBlastPortal = false;
    if (this->numConsecutiveRocketStratPortals > 1) {
        iceBlastPortal = true;
    }

    if (iceBlastPortal) {
        pieceBounds = FuturismBoss::GetIceStrategyPortalSearchAABB(
            FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT);
        FuturismBoss::GetIceStrategyPortalSearchPieces(level, candidates);

        this->numConsecutiveRocketStratPortals = 0;
    }
    else {
        pieceBounds = FuturismBoss::GetRocketStrategyPortalSearchAABB(
            level, FuturismBoss::PORTAL_PROJECTILE_HALF_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HALF_HEIGHT);
        FuturismBoss::GetRocketStrategyPortalSearchPieces(level, candidates);

        this->numConsecutiveRocketStratPortals++;
    }
}

void FuturismBossStage2AIState::CollisionOccurred(GameModel* gameModel, GameBall& ball,
                                                  BossBodyPart* collisionPart) {
    UNUSED_PARAMETER(gameModel);

    if (this->attachedBall == &ball || 
        this->currState == ShieldPartCrackedAIState || this->currState == ShieldPartDestroyedAIState || 
        this->currState == AngryAIState) {

        return;
    }

    if (collisionPart == this->boss->GetCoreShield()) {

        // We can only do damage to the boss with the ball if the shield is already weakened or if the
        // boss is frozen in ice when the ball hits it...
        if (this->boss->IsCoreShieldWeakened()) {

            // If the collision is with the core shield and it's already weakened then we destroy it
            this->DestroyShield(ball.GetDirection(), FuturismBoss::CoreShield);
        }
        else if (this->currState == FrozenAIState) {
            this->WeakenShield(ball.GetDirection(), 2*ball.GetBounds().Radius(), FuturismBoss::CoreShield);
        }
    }
}

void FuturismBossStage2AIState::CollisionOccurred(GameModel* gameModel, Projectile* projectile, 
                                                  BossBodyPart* collisionPart) {

    assert(gameModel != NULL);
    assert(projectile != NULL);
    assert(collisionPart != NULL);

    // Check the type of projectile, only ice blasts should be available for affecting the boss in this way.
    // Rockets are dealt with in the "RocketExplosionOccurred" method.
    bool isIceBlast = projectile->GetType() == Projectile::PaddleIceBlastProjectile;
    if (!isIceBlast || this->currState == FrozenAIState || this->currState == ShieldPartCrackedAIState ||
        this->currState == ShieldPartDestroyedAIState || this->currState == AngryAIState) {
        return;
    }

    if (collisionPart == this->boss->GetCoreShield() || collisionPart == this->boss->GetCoreBody()) {
        // If we're dealing with an ice blast then it's going to freeze the boss, but don't hurt it
        this->SetState(FrozenAIState);
    }
}

void FuturismBossStage2AIState::RocketExplosionOccurred(GameModel* gameModel, const RocketProjectile* rocket) {
    assert(gameModel != NULL);
    assert(rocket != NULL);

    if (this->currState == ShieldPartCrackedAIState || this->currState == ShieldPartDestroyedAIState || 
        this->currState == AngryAIState) {
        return;
    }

    // Check to see if the core part of the rocket explosion included the shield of the boss
    static const float DEFAULT_EXPLOSION_RADIUS = LevelPiece::PIECE_WIDTH;
    float rocketSizeFactor = rocket->GetHeight() / rocket->GetDefaultHeight();
    Collision::Circle2D explosionCircle(rocket->GetPosition(), rocketSizeFactor*DEFAULT_EXPLOSION_RADIUS);
    
    if (this->boss->GetCoreShield()->GetWorldBounds().CollisionCheck(explosionCircle)) {

        if (this->boss->IsCoreShieldWeakened()) {
            this->DestroyShield(rocket->GetVelocityDirection(), FuturismBoss::CoreShield);
        }
        else {
            this->WeakenShield(rocket->GetVelocityDirection(), rocket->GetHeight(), FuturismBoss::CoreShield);
        }
    }
}
