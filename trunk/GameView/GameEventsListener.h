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
	virtual void GameCompletedEvent();
	virtual void WorldStartedEvent(const GameWorld& world);
	virtual void WorldCompletedEvent(const GameWorld& world);
	virtual void LevelStartedEvent(const GameWorld& world, const GameLevel& level);
	virtual void LevelCompletedEvent(const GameWorld& world, const GameLevel& level);

	// Paddle related events
	virtual void PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc);

	// Ball/Block related events
	virtual void BallDiedEvent(const GameBall& deadBall);
	virtual void AllBallsDeadEvent(int livesLeft);
	virtual void BallSpawnEvent(const GameBall& spawnedBall);
	virtual void BallShotEvent(const GameBall& shotBall);
	virtual void ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block);
	virtual void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block);
	virtual void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle);
	virtual void BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2);

	// Misc game events
	virtual void BlockDestroyedEvent(const LevelPiece& block);
	virtual void BallSafetyNetCreatedEvent();
	virtual void BallSafetyNetDestroyedEvent(const GameBall& ball);
	virtual void LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	virtual void ScoreChangedEvent(int amt);
	virtual void ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier);
	virtual void ItemSpawnedEvent(const GameItem& item);
	virtual void ItemRemovedEvent(const GameItem& item);
	virtual void ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle);
	virtual void ItemActivatedEvent(const GameItem& item);
	virtual void ItemDeactivatedEvent(const GameItem& item);
	virtual void ProjectileSpawnedEvent(const Projectile& projectile);
	virtual void ProjectileRemovedEvent(const Projectile& projectile);
	virtual void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter);
};

#endif