#ifndef __GAMEEVENTMANAGER_H__
#define __GAMEEVENTMANAGER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "LevelPiece.h"

class GameEvents;
class GameBall;
class Point2D;
class PlayerPaddle;
class PortalBlock;
class CannonBlock;
class TeslaBlock;
class ItemDropBlock;
class GameItem;
class RandomItem;
class GameItemTimer;
class GameWorld;
class GameLevel;
class Projectile;
class Beam;
class PaddleRocketProjectile;

/**
 * This class is intended to provide hooks for any view that wants to know
 * about any types of events that are occuring within the game model.
 * This class will receive occurances of those events and tell all registered
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
	void ActionBallDied(const GameBall& deadBall);
	void ActionLastBallAboutToDie(const GameBall& ballAboutToDie);
	void ActionLastBallExploded(const GameBall& explodedBall);
	void ActionAllBallsDead(int livesLeft);
	void ActionBallSpawn(const GameBall& spawnBall);
	void ActionBallShot(const GameBall& shotBall);
	void ActionProjectileBlockCollision(const Projectile& projectile, const LevelPiece& block);
	void ActionBallBlockCollision(const GameBall& ball, const LevelPiece& block);
	void ActionBallPortalBlockTeleport(const GameBall& ball, const PortalBlock& enterPortal);
	void ActionProjectilePortalBlockTeleport(const Projectile& projectile, const PortalBlock& enterPortal);
	void ActionBallFiredFromCannon(const GameBall& ball, const CannonBlock& cannonBlock);
	void ActionRocketEnteredCannon(const PaddleRocketProjectile& rocket, const CannonBlock& cannonBlock);
	void ActionRocketFiredFromCannon(const PaddleRocketProjectile& rocket, const CannonBlock& cannonBlock);
	void ActionBallHitTeslaLightningArc(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2);

	void ActionBallPaddleCollision(const GameBall& ball, const PlayerPaddle& paddle);
	void ActionBallBallCollision(const GameBall& ball1, const GameBall& ball2);
	void ActionBlockDestroyed(const LevelPiece& block);
	void ActionBallSafetyNetCreated();
	void ActionBallSafetyNetDestroyed(const GameBall& ball);
	void ActionBallSafetyNetDestroyed(const PlayerPaddle& paddle);
	void ActionBallSafetyNetDestroyed(const Projectile& projectile);
	void ActionLevelPieceChanged(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void ActionLevelPieceStatusAdded(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus);
	void ActionLevelPieceStatusRemoved(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus);
	void ActionLevelPieceAllStatusRemoved(const LevelPiece& piece);
	void ActionScoreChanged(int amt);
	void ActionScoreMultiplierChanged(int oldScoreMultiplier, int newScoreMultiplier);
	void ActionItemSpawned(const GameItem& item);
	void ActionItemRemoved(const GameItem& item);
	void ActionItemPaddleCollision(const GameItem& item, const PlayerPaddle& paddle);
	void ActionItemActivated(const GameItem& item);
	void ActionItemDeactivated(const GameItem& item);
	void ActionRandomItemActivated(const RandomItem& randomItem, const GameItem& actualItem);
	void ActionItemTimerStarted(const GameItemTimer& itemTimer);
	void ActionItemTimerStopped(const GameItemTimer& itemTimer);
	void ActionItemDropBlockItemChange(const ItemDropBlock& dropBlock);

	void ActionProjectileSpawned(const Projectile& projectile);
	void ActionProjectileRemoved(const Projectile& projectile);
	void ActionRocketExploded(const PaddleRocketProjectile& rocket);

	void ActionBeamSpawned(const Beam& beam);
	void ActionBeamChanged(const Beam& beam);
	void ActionBeamRemoved(const Beam& beam);

	void ActionTeslaLightningBarrierSpawned(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock);
	void ActionTeslaLightningBarrierRemoved(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock);

	void ActionGameCompleted();
	void ActionWorldStarted(const GameWorld& world);
	void ActionWorldCompleted(const GameWorld& world);
	void ActionLevelStarted(const GameWorld& world, const GameLevel& level);
	void ActionLevelCompleted(const GameWorld& world, const GameLevel& level);
	
	void ActionLivesChanged(int livesLeftBefore, int livesLeftAfter);

protected:
	GameEventManager();
	
	std::list<GameEvents*> eventListeners;
	std::list<GameEvents*>::iterator listenerIter;

private:
	static GameEventManager* instance;

};

#endif
