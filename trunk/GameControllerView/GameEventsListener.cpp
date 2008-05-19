#include "GameEventsListener.h"
#include "GameDisplay.h"

#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/PlayerPaddle.h"

#include "../Utils/Includes.h"

GameEventsListener::GameEventsListener(GameDisplay* d) : display(d) {
	assert(d != NULL);
}

GameEventsListener::~GameEventsListener() {
}

void GameEventsListener::PaddleHitWallEvent(const Point2D& hitLoc) {
	debug_output("EVENT: Paddle hit wall");
}

void GameEventsListener::BallDeathEvent(const GameBall& deadBall, unsigned int livesLeft) {
	debug_output("EVENT: Ball death, lives left: " << livesLeft);
}

void GameEventsListener::BallSpawnEvent(const GameBall& spawnedBall) {
	debug_output("EVENT: Ball respawning");
}

void GameEventsListener::BallShotEvent(const GameBall& shotBall) {
	debug_output("EVENT: Ball shot");
}

void GameEventsListener::BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) {
	debug_output("EVENT: Ball-Block Collision");
}

void GameEventsListener::BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) {
	debug_output("EVENT: Ball-Paddle Collision");
}