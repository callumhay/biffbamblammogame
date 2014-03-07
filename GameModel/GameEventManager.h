/**
 * GameEventManager.h
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

#ifndef __GAMEEVENTMANAGER_H__
#define __GAMEEVENTMANAGER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "LevelPiece.h"
#include "LaserTurretBlock.h"
#include "RocketTurretBlock.h"
#include "MineTurretBlock.h"
#include "CollateralBlock.h"

class GameEvents;
class GameBall;
class Point2D;
class PlayerPaddle;
class PortalBlock;
class CannonBlock;
class TeslaBlock;
class ItemDropBlock;
class SwitchBlock;
class RegenBlock;
class GameItem;
class RandomItem;
class GameItemTimer;
class GameWorld;
class GameLevel;
class Projectile;
class Beam;
class RocketProjectile;
class PointAward;
class BallBoostModel;
class PaddleMineProjectile;
class BossWeakpoint;
class AbstractBossBodyPart;
class Boss;
class GeneralEffectEventInfo;
class BossEffectEventInfo;
class PaddleRemoteControlRocketProjectile;
class CollateralBlockProjectile;

/**
 * This class is intended to provide hooks for any view that wants to know
 * about any types of events that are occurring within the game model.
 * This class will receive occurrences of those events and tell all registered
 * listeners about them.
 */
class GameEventManager {

public:
	static GameEventManager* Instance();
	static void DeleteInstance();
	
	// Register functions and lists of registered listeners
	void RegisterGameEventListener(GameEvents* listener);
	void UnregisterGameEventListener(GameEvents* listener);

	// Action functions (when events happen, call these)
	void ActionPaddleHitWall(const PlayerPaddle& paddle, const Point2D& hitLoc);
    void ActionPaddlePortalBlockTeleport(const PlayerPaddle& paddle, const PortalBlock& enterPortal);
	void ActionPaddleHitByProjectile(const PlayerPaddle& paddle, const Projectile& projectile);
	void ActionPaddleShieldHitByProjectile(const PlayerPaddle& paddle, const Projectile& projectile);
	void ActionProjectileDeflectedByPaddleShield(const Projectile& projectile, const PlayerPaddle& paddle);
    void ActionPaddleHitByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
    void ActionPaddleShieldHitByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
    void ActionPaddleHitByBoss(const PlayerPaddle& paddle, const BossBodyPart& bossPart);
    void ActionPaddleStatusUpdate(const PlayerPaddle& paddle, PlayerPaddle::PaddleSpecialStatus status, bool isActive);
    void ActionFrozenPaddleCanceledByFire(const PlayerPaddle& paddle);
    void ActionOnFirePaddleCanceledByIce(const PlayerPaddle& paddle);
    void ActionPaddleFlipped(const PlayerPaddle& paddle, bool isUpsideDown);

	void ActionBallDied(const GameBall& deadBall);
	void ActionLastBallAboutToDie(const GameBall& ballAboutToDie);
	void ActionLastBallExploded(const GameBall& explodedBall, bool wasSkipped);
	void ActionAllBallsDead(int livesLeft);
	void ActionBallSpawn(const GameBall& spawnBall);
	void ActionBallShot(const GameBall& shotBall);
    void ActionPaddleWeaponFired();
	void ActionProjectileBlockCollision(const Projectile& projectile, const LevelPiece& block);
    void ActionProjectileSafetyNetCollision(const Projectile& projectile, const SafetyNet& safetyNet);
    void ActionProjectileBossCollision(const Projectile& projectile, const Boss& boss, const BossBodyPart& collisionPart);
	void ActionBallBlockCollision(const GameBall& ball, const LevelPiece& block);
	void ActionBallPortalBlockTeleport(const GameBall& ball, const PortalBlock& enterPortal);
	void ActionProjectilePortalBlockTeleport(const Projectile& projectile, const PortalBlock& enterPortal);
    void ActionItemPortalBlockTeleport(const GameItem& item, const PortalBlock& enterPortal);
    void ActionBallEnteredCannon(const GameBall& ball, const CannonBlock& cannonBlock, bool canShootWithoutObstruction);
	void ActionBallFiredFromCannon(const GameBall& ball, const CannonBlock& cannonBlock);
	void ActionProjectileEnteredCannon(const Projectile& projectile, const CannonBlock& cannonBlock);
	void ActionProjectileFiredFromCannon(const Projectile& projectile, const CannonBlock& cannonBlock);
	void ActionBallHitTeslaLightningArc(const GameBall& ball);

    void ActionGamePauseStateChanged(int32_t oldPauseState, int32_t newPauseState);

    void ActionFireBallCanceledByIceBall(const GameBall& ball);
    void ActionIceBallCanceledByFireBall(const GameBall& ball);

    void ActionPaddleIceBlasterCanceledByFireBlaster(const PlayerPaddle& paddle);
    void ActionPaddleFireBlasterCanceledByIceBlaster(const PlayerPaddle& paddle);

	void ActionBallPaddleCollision(const GameBall& ball, const PlayerPaddle& paddle, bool hitPaddleUnderside);
	void ActionBallBallCollision(const GameBall& ball1, const GameBall& ball2);
    void ActionBallBossCollision(GameBall& ball, const Boss& boss, const BossBodyPart& bossPart);
	void ActionBlockDestroyed(const LevelPiece& block, const LevelPiece::DestructionMethod& method);
	void ActionBallSafetyNetCreated(bool bottomSafetyNet);
	void ActionBallSafetyNetDestroyed(const GameBall& ball, bool bottomSafetyNet);
	void ActionBallSafetyNetDestroyed(const PlayerPaddle& paddle, bool bottomSafetyNet);
	void ActionBallSafetyNetDestroyed(const Projectile& projectile, bool bottomSafetyNet);
	void ActionLevelPieceChanged(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void ActionLevelPieceStatusAdded(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus);
	void ActionLevelPieceStatusRemoved(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus);
	void ActionLevelPieceAllStatusRemoved(const LevelPiece& piece);
    void ActionCollateralBlockChangedState(const CollateralBlock& collateralBlock, const CollateralBlockProjectile& projectile,
        CollateralBlock::CollateralBlockState oldState, CollateralBlock::CollateralBlockState newState);
	void ActionItemSpawned(const GameItem& item);
	void ActionItemRemoved(const GameItem& item);
	void ActionItemPaddleCollision(const GameItem& item, const PlayerPaddle& paddle);
	void ActionItemActivated(const GameItem& item);
	void ActionItemDeactivated(const GameItem& item);
	void ActionRandomItemActivated(const RandomItem& randomItem, const GameItem& actualItem);
	void ActionItemTimerStarted(const GameItemTimer& itemTimer);
	void ActionItemTimerStopped(const GameItemTimer& itemTimer, bool didExpire);
	void ActionItemDropBlockItemChange(const ItemDropBlock& dropBlock);
    void ActionItemDropBlockLocked(const ItemDropBlock& dropBlock);
    void ActionSwitchBlockActivated(const SwitchBlock& switchBlock);
    void ActionBulletTimeStateChanged(const BallBoostModel& boostModel);
    void ActionBallBoostExecuted(const BallBoostModel& boostModel);
    void ActionBallBoostGained();
    void ActionBallBoostLost(bool allBoostsLost);
    void ActionBallBoostUsed();
    void ActionBoostFailedDueToNoBallsAvailable();

    void ActionBallCameraSetOrUnset(const GameBall& ball, bool isSet, bool canShootWithoutObstruction);
    void ActionBallCameraCannonRotation(const GameBall& ball, const CannonBlock& cannon, bool canShootWithoutObstruction);
    void ActionCantFireBallCamFromCannon();

    void ActionPaddleCameraSetOrUnset(const PlayerPaddle& paddle, bool isSet);

    void ActionReleaseTimerStarted();

	void ActionProjectileSpawned(const Projectile& projectile);
	void ActionProjectileRemoved(const Projectile& projectile);
	void ActionRocketExploded(const RocketProjectile& rocket);
    void ActionMineExploded(const MineProjectile& mine);
    void ActionMineLanded(const MineProjectile& mine);
    void ActionRemoteControlRocketFuelWarning(const PaddleRemoteControlRocketProjectile& rocket);
    void ActionRemoteControlRocketThrustApplied(const PaddleRemoteControlRocketProjectile& rocket);

	void ActionBeamSpawned(const Beam& beam);
	void ActionBeamChanged(const Beam& beam);
	void ActionBeamRemoved(const Beam& beam);

	void ActionTeslaLightningBarrierSpawned(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock);
	void ActionTeslaLightningBarrierRemoved(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock);

	void ActionGameCompleted();
	void ActionWorldStarted(const GameWorld& world);
	void ActionWorldCompleted(const GameWorld& world);
	void ActionLevelStarted(const GameWorld& world, const GameLevel& level);
	void ActionLevelAlmostComplete(const GameLevel& level);
    void ActionLevelCompleted(const GameWorld& world, const GameLevel& level, int furthestLevelIdxBefore, int furthestLevelIdxAfter);
	void ActionLevelResetting();

	void ActionLivesChanged(int livesLeftBefore, int livesLeftAfter);

	void ActionBlockIceShattered(const LevelPiece& block);
    void ActionBlockIceCancelledWithFire(const LevelPiece& block);
    void ActionBlockFireCancelledWithIce(const LevelPiece& block);

    void ActionPointNotification(const PointAward& pointAward);
	void ActionScoreChanged(int newScore);
    void ActionScoreMultiplierCounterChanged(int oldCounterValue, int newCounterValue);
	void ActionScoreMultiplierChanged(int oldMultiplier, int newMultiplier, const Point2D& position);
    void ActionNumStarsChanged(const PointAward* pointAward, int oldNumStars, int newNumStars);

    void ActionDifficultyChanged(int newDifficulty);

    void ActionLaserTurretAIStateChanged(const LaserTurretBlock& block, LaserTurretBlock::TurretAIState oldState,
        LaserTurretBlock::TurretAIState newState);
    void ActionLaserFiredByTurret(const LaserTurretBlock& block);
    void ActionRocketTurretAIStateChanged(const RocketTurretBlock& block, RocketTurretBlock::TurretAIState oldState,
        RocketTurretBlock::TurretAIState newState);
    void ActionRocketFiredByTurret(const RocketTurretBlock& block);
    void ActionMineTurretAIStateChanged(const MineTurretBlock& block, MineTurretBlock::TurretAIState oldState,
        MineTurretBlock::TurretAIState newState);
    void ActionMineFiredByTurret(const MineTurretBlock& block);
    
    void ActionRegenBlockLifeChanged(const RegenBlock& block, float lifePercentBefore);
    void ActionRegenBlockPreturbed(const RegenBlock& block);

    // Boss Events
    void ActionBossHurt(const BossWeakpoint* hurtPart);
    void ActionBossAngry(const Boss* boss, const BossBodyPart* angryPart);
    void ActionBossAngry(const Boss* boss, const Point2D& angryPartLoc, float angryPartWidth, float angryPartHeight);

    // Effect Events
    void ActionGeneralEffect(const GeneralEffectEventInfo& effectEvent);
    void ActionBossEffect(const BossEffectEventInfo& effectEvent);

private:
	GameEventManager();
	
	std::list<GameEvents*> eventListeners;

	static GameEventManager* instance;
};

#endif
