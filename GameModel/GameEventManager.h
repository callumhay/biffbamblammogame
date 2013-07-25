/**
 * GameEventManager.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMEEVENTMANAGER_H__
#define __GAMEEVENTMANAGER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "LevelPiece.h"
#include "LaserTurretBlock.h"
#include "RocketTurretBlock.h"
#include "MineTurretBlock.h"

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
class BossEffectEventInfo;
class PaddleRemoteControlRocketProjectile;

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
	void ActionPaddleHitByProjectile(const PlayerPaddle& paddle, const Projectile& projectile);
	void ActionPaddleShieldHitByProjectile(const PlayerPaddle& paddle, const Projectile& projectile);
	void ActionProjectileDeflectedByPaddleShield(const Projectile& projectile, const PlayerPaddle& paddle);
    void ActionPaddleHitByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
    void ActionPaddleShieldHitByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
	void ActionBallDied(const GameBall& deadBall);
	void ActionLastBallAboutToDie(const GameBall& ballAboutToDie);
	void ActionLastBallExploded(const GameBall& explodedBall);
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
    void ActionBallEnteredCannon(const GameBall& ball, const CannonBlock& cannonBlock);
	void ActionBallFiredFromCannon(const GameBall& ball, const CannonBlock& cannonBlock);
	void ActionProjectileEnteredCannon(const Projectile& projectile, const CannonBlock& cannonBlock);
	void ActionProjectileFiredFromCannon(const Projectile& projectile, const CannonBlock& cannonBlock);
	void ActionBallHitTeslaLightningArc(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2);

    void ActionGamePauseStateChanged(int32_t oldPauseState, int32_t newPauseState);

    void ActionFireBallCancelledByIceBall(const GameBall& ball);
    void ActionIceBallCancelledByFireBall(const GameBall& ball);

	void ActionBallPaddleCollision(const GameBall& ball, const PlayerPaddle& paddle);
	void ActionBallBallCollision(const GameBall& ball1, const GameBall& ball2);
	void ActionBlockDestroyed(const LevelPiece& block, const LevelPiece::DestructionMethod& method);
	void ActionBallSafetyNetCreated();
	void ActionBallSafetyNetDestroyed(const GameBall& ball);
	void ActionBallSafetyNetDestroyed(const PlayerPaddle& paddle);
	void ActionBallSafetyNetDestroyed(const Projectile& projectile);
	void ActionLevelPieceChanged(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void ActionLevelPieceStatusAdded(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus);
	void ActionLevelPieceStatusRemoved(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus);
	void ActionLevelPieceAllStatusRemoved(const LevelPiece& piece);
	void ActionItemSpawned(const GameItem& item);
	void ActionItemRemoved(const GameItem& item);
	void ActionItemPaddleCollision(const GameItem& item, const PlayerPaddle& paddle);
	void ActionItemActivated(const GameItem& item);
	void ActionItemDeactivated(const GameItem& item);
	void ActionRandomItemActivated(const RandomItem& randomItem, const GameItem& actualItem);
	void ActionItemTimerStarted(const GameItemTimer& itemTimer);
	void ActionItemTimerStopped(const GameItemTimer& itemTimer);
	void ActionItemDropBlockItemChange(const ItemDropBlock& dropBlock);
    void ActionSwitchBlockActivated(const SwitchBlock& switchBlock);
    void ActionBulletTimeStateChanged(const BallBoostModel& boostModel);
    void ActionBallBoostExecuted(const BallBoostModel& boostModel);
    void ActionBallBoostGained();
    void ActionBallBoostLost(bool allBoostsLost);
    void ActionBallBoostUsed();

    void ActionReleaseTimerStarted();

	void ActionProjectileSpawned(const Projectile& projectile);
	void ActionProjectileRemoved(const Projectile& projectile);
	void ActionRocketExploded(const RocketProjectile& rocket);
    void ActionMineExploded(const MineProjectile& mine);
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
    void ActionLevelCompleted(const GameWorld& world, const GameLevel& level);
	
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

    // Effect Events
    void ActionEffect(const BossEffectEventInfo& effectEvent);

private:
	GameEventManager();
	
	std::list<GameEvents*> eventListeners;

	static GameEventManager* instance;
};

#endif
