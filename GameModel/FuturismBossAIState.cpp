/**
 * FuturismBossAIState.cpp
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

#include "FuturismBossAIState.h"
#include "FuturismBoss.h"
#include "GameModel.h"
#include "PortalProjectile.h"
#include "PortalSpawnEffectInfo.h"
#include "GenericEmitterEffectInfo.h"
#include "SummonPortalsEffectInfo.h"
#include "PowerChargeEffectInfo.h"
#include "FullscreenFlashEffectInfo.h"
#include "BossTeleportEffectInfo.h"

#include "../ESPEngine/ESPPointToPointBeam.h"

const float FuturismBossAIState::DEFAULT_ACCELERATION = 0.1f * PlayerPaddle::DEFAULT_ACCELERATION;
const float FuturismBossAIState::DEFAULT_SPEED = 5.0f;

const float FuturismBossAIState::SPAWNED_PORTAL_WIDTH       = 0.9f*LevelPiece::PIECE_WIDTH;
const float FuturismBossAIState::SPAWNED_PORTAL_HEIGHT      = 0.9f*LevelPiece::PIECE_HEIGHT;
const float FuturismBossAIState::SPAWNED_PORTAL_HALF_WIDTH  = FuturismBossAIState::SPAWNED_PORTAL_WIDTH / 2.0f;
const float FuturismBossAIState::SPAWNED_PORTAL_HALF_HEIGHT = FuturismBossAIState::SPAWNED_PORTAL_HEIGHT / 2.0f;

const double FuturismBossAIState::STRATEGY_PORTAL_TERMINATION_TIME_IN_SECS = 10.0;
const double FuturismBossAIState::PORTAL_SPAWN_EFFECT_TIME_IN_SECS = 1.0;

FuturismBossAIState::FuturismBossAIState(FuturismBoss* boss): BossAIState(), boss(boss), 
countdownToPortalShot(-1), portalWasShot(false), arenaState(InLeftArena) {
}

FuturismBossAIState::~FuturismBossAIState() {
}

Boss* FuturismBossAIState::GetBoss() const {
    return this->boss;
}

Collision::AABB2D FuturismBossAIState::GenerateDyingAABB() const {
    return this->boss->GetCoreBody()->GenerateWorldAABB();
}

void FuturismBossAIState::InitMoveToPositionAIState() {
    // Figure out a good position and move to it...
    std::vector<Point2D> moveToPositions;
    this->GetRandomMoveToPositions(moveToPositions);
    // Choose a position at random...
    int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(moveToPositions.size());
    this->SetMoveToTargetPosition(this->boss->alivePartsRoot->GetTranslationPt2D(), moveToPositions[randomIdx]);
}

void FuturismBossAIState::ExecuteMoveToPositionAIState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);

    if (this->MoveToTargetPosition(this->GetMaxSpeed())) {
        // Finished moving to the target position...
        this->SetState(MoveToPositionAIState); // TODO
        return;
    }
}

void FuturismBossAIState::InitTeleportAIState() {
    this->countdownToTeleport = this->GetPauseTimeBeforeSingleTeleport();
}

void FuturismBossAIState::ExecuteTeleportAIState(double dT, GameModel* gameModel) {
    this->countdownToTeleport -= dT;
    if (this->countdownToTeleport <= 0) {

        // Teleport the boss (if possible)
        Point2D teleportPos;
        if (this->GetTeleportationLocation(*gameModel, false, teleportPos)) {
            
            // The boss is now teleporting...
            Point2D currPos = this->boss->alivePartsRoot->GetTranslationPt2D();
            
            // Do visual effects
            GameEventManager::Instance()->ActionBossEffect(BossTeleportEffectInfo(currPos, teleportPos));
            GameEventManager::Instance()->ActionBossEffect(FullscreenFlashEffectInfo(0.5, 0.0f));

            // Move the boss to the new location
            Vector2D translationVec = teleportPos - currPos;
            this->boss->alivePartsRoot->Translate(Vector3D(translationVec, 0));
        }

        this->SetState(TeleportAIState); // TODO
    }
}

void FuturismBossAIState::InitStationaryFireStrategyPortalAIState() {

    double totalTime = this->GetTotalStrategyPortalShootTime();
    this->countdownToPortalShot = 0.5*totalTime;
    this->portalWasShot = false;
    this->nextPortalColour = this->boss->GetAndIncrementPortalColour();

    // Setup the core rotation animation...
    {
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

    // Add a bit of shaking to the boss' body to indicate that it's firing the portal...
    this->boss->alivePartsRoot->AnimateLocalTranslation(Boss::BuildShakeAnim(0.2f*totalTime, 0.6f*totalTime, 15, 0.03, 0.005));

    // EVENT: Do energy gathering effects
    GameEventManager::Instance()->ActionBossEffect(PowerChargeEffectInfo(
        this->boss->GetCoreBody(), this->countdownToPortalShot, Colour(0.5f,0.5f,0.5f)+1.25f*this->nextPortalColour, 0.5f, 
        Vector3D(0,0,FuturismBoss::CORE_Z_DIST_FROM_ORIGIN)));
}

void FuturismBossAIState::ExecuteStationaryFireStrategyPortalAIState(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    // Check the status of shooting the portal...
    if (!this->portalWasShot) {
        this->countdownToPortalShot -= dT;
        if (this->countdownToPortalShot <= 0.0) {

            // Shoot the portal... or keep trying to shoot it
            Point2D arenaPortalPos, stratPortalPos;
            if (this->GetArenaPortalLocation(*gameModel, arenaPortalPos) &&
                this->GetStrategyPortalLocation(*gameModel, stratPortalPos)) {

                this->SpawnPortals(*gameModel, arenaPortalPos, stratPortalPos);
            }
        }
    }

    // Tick the rotation animation
    bool isFinished = this->coreRotVelAnim.Tick(dT);

    // If the core shield is destroyed we rotate the core assembly/body instead
    AbstractBossBodyPart* partToAnimate = NULL;
    float fullRotationDegAmt;
    if (this->boss->GetCoreShield()->GetIsDestroyed()) {
        partToAnimate = this->boss->bodyParts[this->boss->coreAssemblyIdx];
        fullRotationDegAmt = 90.0f;
    }
    else {
        partToAnimate = this->boss->bodyParts[this->boss->coreShieldIdx];
        fullRotationDegAmt = 45.0f;
    }

    if (isFinished) {

        if (this->boss->currCoreRotInDegs == 0.0f) {
            this->portalWasShot = true;

            // Clear up all animations on the boss that were used during this state
            partToAnimate->SetLocalZRotation(this->boss->currCoreRotInDegs);
            this->boss->alivePartsRoot->ClearLocalTranslationAnimation();
            this->boss->alivePartsRoot->SetLocalTranslation(Vector3D(0,0,0));

            // We're done with this state, go to the next one
            this->SetState(StationaryFireStrategyPortalAIState); // TODO
            return;
        }
        else {
            this->boss->currCoreRotInDegs += dT*this->coreRotVelAnim.GetInterpolantValue();
            if (this->boss->currCoreRotInDegs >= fullRotationDegAmt) {
                this->boss->currCoreRotInDegs = 0.0f;
            }
            partToAnimate->SetLocalZRotation(this->boss->currCoreRotInDegs);
        }
    }
    else {
        this->boss->currCoreRotInDegs += dT*this->coreRotVelAnim.GetInterpolantValue();
        this->boss->currCoreRotInDegs = fmod(this->boss->currCoreRotInDegs, fullRotationDegAmt);
        partToAnimate->SetLocalZRotation(this->boss->currCoreRotInDegs);
    }

}

void FuturismBossAIState::UpdateState(double dT, GameModel* gameModel) {
    switch (this->currState) {

        case MoveToPositionAIState:
            this->ExecuteMoveToPositionAIState(dT, gameModel);
            break;

        case TeleportAIState:
            this->ExecuteTeleportAIState(dT, gameModel);
            break;

        case StationaryFireStrategyPortalAIState:
            this->ExecuteStationaryFireStrategyPortalAIState(dT, gameModel);
            break;

        case FinalDeathThroesAIState:
            break;

        default:
            assert(false);
            return;
    }
}

void FuturismBossAIState::GetArenaBounds(float& minX, float& maxX, float& minY, float& maxY) const {
    // Find the blocks that are currently colliding with the general area around the ball and boss
    switch (this->arenaState) {
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

const std::vector<Point2D>& FuturismBossAIState::GetArenaMoveToPositions() const {
    switch (this->arenaState) {
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

bool FuturismBossAIState::GetArenaPortalLocation(const GameModel& gameModel, Point2D& portalPos) const {
    assert(!gameModel.GetGameBalls().empty());

    const GameLevel* level = gameModel.GetCurrentLevel();
    const GameBall* ball   = gameModel.GetGameBalls().front();

    std::set<const LevelPiece*> emptyLevelPieceSet;
    std::set<LevelPiece::LevelPieceType> emptyLevelPieceTypeSet;

    std::set<LevelPiece*>* allPieces;

    // Find the blocks that are currently colliding with the general area around the ball and boss
    switch (this->arenaState) {
        case InLeftArena: {
            allPieces = &this->boss->leftSubArenaPieces;
            break;
        }
            
        case InRightArena:  {
            allPieces = &this->boss->rightSubArenaPieces;
            break;
        }

        case InFullyOpenedArena: {
            assert(false);
            // TODO!!!! allPieces = &this->boss->fullArenaPieces;
            return false;
        }
        
        default:
            // Should never get here...
            assert(false);
            return false;
    }

    // Add all relevant level pieces that might be colliding with the ball, boss, and existing portals
    std::set<LevelPiece*> colliders;
    level->GetLevelPieceColliders(Collision::Ray2D(ball->GetCenterPosition2D(), 
        ball->GetDirection()), emptyLevelPieceSet, emptyLevelPieceTypeSet, colliders, 
        this->boss->GetSubArenaHeight(), ball->GetBounds().Radius());

    this->GetBossCandidatePieces(*level, 2*FuturismBoss::PORTAL_PROJECTILE_WIDTH, 2*FuturismBoss::PORTAL_PROJECTILE_WIDTH, colliders);
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
    this->GetArenaBounds(minX, maxX, minY, maxY);

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

bool FuturismBossAIState::GetStrategyPortalLocation(const GameModel& gameModel, Point2D& portalPos) const {
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

bool FuturismBossAIState::GetTeleportationLocation(const GameModel& gameModel, bool withBall, Point2D& teleportPos) const {
    const GameLevel* level = gameModel.GetCurrentLevel();
    assert(level != NULL);

    Point2D bossPos = this->boss->alivePartsRoot->GetTranslationPt2D();
    Collision::AABB2D bossAABB = this->boss->alivePartsRoot->GenerateWorldAABB();

    // TODO: withBall ... the bounds of the boss should be made larger to accommodate the ball

    // Grab the locations where the boss typically moves...
    const std::vector<Point2D>& allLocations = this->GetArenaMoveToPositions();
    
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
        if (activePortals != NULL) {
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
        if (activeRockets != NULL) {
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
        if (activeIceBlasts != NULL) {
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
        if (!withBall && Collision::IsCollision(translatedBossAABB, currBall->GetBounds())) {
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
    assert(numberToChoose > 1);
    
    BossAIState::GetFurthestDistFromBossPositions(bossPos, possibleLocations, numberToChoose, possibleLocations);
    assert(static_cast<int>(possibleLocations.size()) == numberToChoose);
    int randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % numberToChoose;
    teleportPos = possibleLocations[randomIdx];

    return true;
}

void FuturismBossAIState::SpawnPortals(GameModel& gameModel, const Point2D& portal1Pos, const Point2D& portal2Pos) {
    // Create twin portals...
    std::pair<PortalProjectile*, PortalProjectile*> portalPair =
        PortalProjectile::BuildSiblingPortalProjectiles(portal1Pos, portal2Pos, 
        FuturismBoss::PORTAL_PROJECTILE_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HEIGHT,
        this->nextPortalColour, STRATEGY_PORTAL_TERMINATION_TIME_IN_SECS);

    gameModel.AddProjectile(portalPair.first);
    gameModel.AddProjectile(portalPair.second);

    // EVENT(S): Effects for the boss "summoning" the portals
    // NOTE: Make sure these effects are BEFORE the effects for the 
    // portals themselves (to ensure proper draw order)
    GameEventManager::Instance()->ActionBossEffect(
        SummonPortalsEffectInfo(this->boss->GetCoreBody(), FuturismBoss::CORE_EYE_SIZE,
        PORTAL_SPAWN_EFFECT_TIME_IN_SECS, portal1Pos, portal2Pos, this->nextPortalColour, 
        Vector3D(0,0,FuturismBoss::CORE_Z_SHOOT_DIST_FROM_ORIGIN)));

    // EVENT(S): Effects for the "spawning" of the portals
    GameEventManager::Instance()->ActionGeneralEffect(PortalSpawnEffectInfo(
        PORTAL_SPAWN_EFFECT_TIME_IN_SECS, portal1Pos, this->nextPortalColour, 
        FuturismBoss::PORTAL_PROJECTILE_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HEIGHT));
    GameEventManager::Instance()->ActionGeneralEffect(PortalSpawnEffectInfo(
        PORTAL_SPAWN_EFFECT_TIME_IN_SECS, portal2Pos, this->nextPortalColour, 
        FuturismBoss::PORTAL_PROJECTILE_WIDTH, FuturismBoss::PORTAL_PROJECTILE_HEIGHT));


    this->portalWasShot = true;
}
