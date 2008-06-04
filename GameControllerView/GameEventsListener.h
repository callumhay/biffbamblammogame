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
	virtual void LevelStartedEvent(const GameLevel& level);
	virtual void LevelCompletedEvent(const GameLevel& level);

	// Paddle related events
	virtual void PaddleHitWallEvent(const Point2D& hitLoc);

	// Ball related events
	virtual void BallDeathEvent(const GameBall& deadBall, int livesLeft);
	virtual void BallSpawnEvent(const GameBall& spawnedBall);
	virtual void BallShotEvent(const GameBall& shotBall);
	virtual void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block);
	virtual void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle);

	// Misc game events
	virtual void BlockDestroyedEvent(const LevelPiece& block);
	virtual void ScoreChangedEvent(int amt);
};

#endif