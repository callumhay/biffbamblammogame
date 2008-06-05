#include "GameEventManager.h"
#include "../Utils/Includes.h"

GameEventManager* GameEventManager::instance = NULL;

GameEventManager::GameEventManager() {
}

/**
 * Used to obtain the singleton instance of this class.
 * Return: Singleton of this class.
 */
GameEventManager* GameEventManager::Instance() {
	if (instance == NULL) {
		instance = new GameEventManager();
	}
	return instance;
}
/**
 * Used to clear the game event manager from memory.
 */
void GameEventManager::DeleteInstance() {
	if (instance != NULL) {
		delete instance;
	}
}

// Functions for registering and unregistering listeners for game events
void GameEventManager::RegisterGameEventListener(GameEvents* listener) {
	assert(listener != NULL);
	this->eventListeners.push_back(listener);
}
void GameEventManager::UnregisterGameEventListener(GameEvents* listener) {
	assert(listener != NULL);
	this->eventListeners.remove(listener);
}

// Action for when the player paddle hits a wall
void GameEventManager::ActionPaddleHitWall(const Point2D& hitLoc) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->PaddleHitWallEvent(hitLoc);
	}
}
// Action for when the player ball dies (falls off screen)
void GameEventManager::ActionBallDeath(const GameBall& deadBall, int livesLeft) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->BallDeathEvent(deadBall, livesLeft);
	}	
}

// Action for when the ball respawns on the player paddle
void GameEventManager::ActionBallSpawn(const GameBall& spawnBall) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->BallSpawnEvent(spawnBall);
	}	
}

// Action for when the ball is being shot off the player paddle
void GameEventManager::ActionBallShot(const GameBall& shotBall) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->BallShotEvent(shotBall);
	}		
}

// Action for when the ball collides with a block in the level
void GameEventManager::ActionBallBlockCollision(const GameBall& ball, const LevelPiece& block) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->BallBlockCollisionEvent(ball, block);
	}	
}

// Action for when the ball collides with the player paddle
void GameEventManager::ActionBallPaddleCollision(const GameBall& ball, const PlayerPaddle& paddle) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->BallPaddleCollisionEvent(ball, paddle);
	}	
}

// Action for when a block is destroyed
void GameEventManager::ActionBlockDestroyed(const LevelPiece& block) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->BlockDestroyedEvent(block);
	}	
}

void GameEventManager::ActionScoreChanged(int amt) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->ScoreChangedEvent(amt);
	}	
}

// Action for when the game is completed
void GameEventManager::ActionGameCompleted() {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->GameCompletedEvent();
	}		
}

// Action for when a world starts in-game
void GameEventManager::ActionWorldStarted(const GameWorld& world) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->WorldStartedEvent(world);
	}		
}

// Action for when a world completes in-game
void GameEventManager::ActionWorldCompleted(const GameWorld& world) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->WorldCompletedEvent(world);
	}	
}

// Action for when a level starts in-game
void GameEventManager::ActionLevelStarted(const GameLevel& level) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->LevelStartedEvent(level);
	}	
}

// Action for when a level completes in-game
void GameEventManager::ActionLevelCompleted(const GameLevel& level) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); this->listenerIter++) {
		(*this->listenerIter)->LevelCompletedEvent(level);
	}	
}
