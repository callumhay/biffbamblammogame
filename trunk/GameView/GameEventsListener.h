/**
 * GameEventsListener.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEEVENTSLISTENER_H__
#define __GAMEEVENTSLISTENER_H__

#include "..\GameModel\GameEvents.h"

class GameDisplay;

class GameEventsListener : public GameEvents {
private:
	GameDisplay* display;

public:
	GameEventsListener(GameDisplay* d);
	virtual ~GameEventsListener();

	// Global / Game events
	void GameCompletedEvent();
	void WorldStartedEvent(const GameWorld& world);
	void WorldCompletedEvent(const GameWorld& world);
	void LevelStartedEvent(const GameWorld& world, const GameLevel& level);
	void LevelCompletedEvent(const GameWorld& world, const GameLevel& level);

	// Paddle related events
	void PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc);
	void PaddleHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile);

	// Ball/Block related events
	void BallDiedEvent(const GameBall& deadBall);
	void LastBallAboutToDieEvent(const GameBall& lastBallToDie);
	void LastBallExploded(const GameBall& explodedBall);
	void AllBallsDeadEvent(int livesLeft);
	void BallSpawnEvent(const GameBall& spawnedBall);
	void BallShotEvent(const GameBall& shotBall);
	void ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block);
	void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block);
	void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle);
	void BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2);
	void BallPortalBlockTeleportEvent(const GameBall& ball, const PortalBlock& enterPortal);
	void ProjectilePortalBlockTeleportEvent(const Projectile& projectile, const PortalBlock& enterPortal);
	void BallFiredFromCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock);

	// Misc game events
	void BlockDestroyedEvent(const LevelPiece& block);
	void BallSafetyNetCreatedEvent();
	void BallSafetyNetDestroyedEvent(const GameBall& ball);
	void LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void ScoreChangedEvent(int amt);
	void ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier);
	void ItemSpawnedEvent(const GameItem& item);
	void ItemRemovedEvent(const GameItem& item);
	void ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle);
	void ItemActivatedEvent(const GameItem& item);
	void ItemDeactivatedEvent(const GameItem& item);
	void ItemTimerStartedEvent(const GameItemTimer& itemTimer);
	void ItemTimerStoppedEvent(const GameItemTimer& itemTimer);
	void ProjectileSpawnedEvent(const Projectile& projectile);
	void ProjectileRemovedEvent(const Projectile& projectile);
	void BeamSpawnedEvent(const Beam& beam);
	void BeamChangedEvent(const Beam& beam);
	void BeamRemovedEvent(const Beam& beam);
	void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter);
};

#endif