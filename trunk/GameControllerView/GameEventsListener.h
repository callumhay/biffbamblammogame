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

	// Paddle related events
	virtual void PaddleHitWallEvent(const Point2D& hitLoc);

	// Ball related events
	virtual void BallDeathEvent(const GameBall& deadBall, unsigned int livesLeft);
	virtual void BallSpawnEvent(const GameBall& spawnedBall);
	virtual void BallShotEvent(const GameBall& shotBall);
	virtual void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block);
	virtual void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle);
};

#endif