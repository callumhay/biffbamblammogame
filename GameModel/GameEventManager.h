#ifndef __GAMEEVENTMANAGER_H__
#define __GAMEEVENTMANAGER_H__

#include <list>

#include "GameEvents.h"

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
	void ActionPaddleHitWall(const Point2D& hitLoc);
	void ActionBallDeath(const GameBall& deadBall, int livesLeft);
	void ActionBallSpawn(const GameBall& spawnBall);
	void ActionBallShot(const GameBall& shotBall);
	void ActionBallBlockCollision(const GameBall& ball, const LevelPiece& blockBefore, const LevelPiece& blockAfter);
	void ActionBallPaddleCollision(const GameBall& ball, const PlayerPaddle& paddle);
	void ActionBlockDestroyed(const LevelPiece& block);
	void ActionScoreChanged(int amt);
	void ActionScoreMultiplierChanged(int oldScoreMultiplier, int newScoreMultiplier);
	void ActionItemSpawned(const GameItem& item);
	void ActionItemPaddleCollision(const GameItem& item, const PlayerPaddle& paddle);
	void ActionItemActivated(const GameItem& item);
	void ActionItemDeactivated(const GameItem& item);

	void ActionGameCompleted();
	void ActionWorldStarted(const GameWorld& world);
	void ActionWorldCompleted(const GameWorld& world);
	void ActionLevelStarted(const GameWorld& world, const GameLevel& level);
	void ActionLevelCompleted(const GameWorld& world, const GameLevel& level);


protected:
	GameEventManager();
	
	std::list<GameEvents*> eventListeners;
	std::list<GameEvents*>::iterator listenerIter;

private:
	static GameEventManager* instance;

};

#endif
