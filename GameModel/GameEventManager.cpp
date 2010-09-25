/**
 * GameEventManager.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameEventManager.h"
#include "GameEvents.h"

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
		instance = NULL;
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
void GameEventManager::ActionPaddleHitWall(const PlayerPaddle& paddle, const Point2D& hitLoc) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->PaddleHitWallEvent(paddle, hitLoc);
	}
}

// Action for when the player paddle is hit by a projectile
void GameEventManager::ActionPaddleHitByProjectile(const PlayerPaddle& paddle, const Projectile& projectile) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->PaddleHitByProjectileEvent(paddle, projectile);
	}	
}

// Action for when the player paddle shield is hit by a projectile
void GameEventManager::ActionPaddleShieldHitByProjectile(const PlayerPaddle& paddle, const Projectile& projectile) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->PaddleShieldHitByProjectileEvent(paddle, projectile);
	}	
}

// Action for when the player paddle shield deflects a projectile
void GameEventManager::ActionProjectileDeflectedByPaddleShield(const Projectile& projectile, const PlayerPaddle& paddle) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ProjectileDeflectedByPaddleShieldEvent(projectile, paddle);
	}	
}

// Action for when a ball has died (gone out of bounds)
void GameEventManager::ActionBallDied(const GameBall& deadBall) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallDiedEvent(deadBall);
	}	
}

// Action for when the last ball left is about to die - (before its spiralling death animations and explosion)
void GameEventManager::ActionLastBallAboutToDie(const GameBall& ballAboutToDie) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->LastBallAboutToDieEvent(ballAboutToDie);
	}	
}

// Action for when the last ball is exploding
void GameEventManager::ActionLastBallExploded(const GameBall& explodedBall) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->LastBallExploded(explodedBall);
	}	
}

// Action for when the all balls have died (gone out of bounds)
void GameEventManager::ActionAllBallsDead(int livesLeft) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->AllBallsDeadEvent(livesLeft);
	}	
}

// Action for when the ball respawns on the player paddle
void GameEventManager::ActionBallSpawn(const GameBall& spawnBall) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallSpawnEvent(spawnBall);
	}	
}

// Action for when the ball is being shot off the player paddle
void GameEventManager::ActionBallShot(const GameBall& shotBall) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallShotEvent(shotBall);
	}		
}

// Action for when a projectile collides with a block in the level
void GameEventManager::ActionProjectileBlockCollision(const Projectile& projectile, const LevelPiece& block) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ProjectileBlockCollisionEvent(projectile, block);
	}	
}

// Action for when the ball collides with a block in the level
void GameEventManager::ActionBallBlockCollision(const GameBall& ball, const LevelPiece& block) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallBlockCollisionEvent(ball, block);
	}	
}

// Action for when the ball enters a portal block
void GameEventManager::ActionBallPortalBlockTeleport(const GameBall& ball, const PortalBlock& enterPortal) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallPortalBlockTeleportEvent(ball, enterPortal);
	}	
}

void GameEventManager::ActionProjectilePortalBlockTeleport(const Projectile& projectile, const PortalBlock& enterPortal) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ProjectilePortalBlockTeleportEvent(projectile, enterPortal);
	}	
}

// Action for when the ball is fired from a cannon block
void GameEventManager::ActionBallFiredFromCannon(const GameBall& ball, const CannonBlock& cannonBlock) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallFiredFromCannonEvent(ball, cannonBlock);
	}		
}

// Action for when the ball hits a lightning arc between two tesla blocks
void GameEventManager::ActionBallHitTeslaLightningArc(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallHitTeslaLightningArcEvent(ball, teslaBlock1, teslaBlock2);
	}		
}

// Action for when the ball collides with the player paddle
void GameEventManager::ActionBallPaddleCollision(const GameBall& ball, const PlayerPaddle& paddle) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallPaddleCollisionEvent(ball, paddle);
	}	
}

// Action for when a ball collides with another ball
void GameEventManager::ActionBallBallCollision(const GameBall& ball1, const GameBall& ball2) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallBallCollisionEvent(ball1, ball2);
	}	
}

// Action for when a block is destroyed
void GameEventManager::ActionBlockDestroyed(const LevelPiece& block) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BlockDestroyedEvent(block);
	}	
}

void GameEventManager::ActionBallSafetyNetCreated() {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallSafetyNetCreatedEvent();
	}		
}

// Action for when the ball safety net is destroyed
void GameEventManager::ActionBallSafetyNetDestroyed(const GameBall& ball) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallSafetyNetDestroyedEvent(ball);
	}	
}

void GameEventManager::ActionBallSafetyNetDestroyed(const PlayerPaddle& paddle) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BallSafetyNetDestroyedEvent(paddle);
	}	
}

// Action for when a level piece changes
void GameEventManager::ActionLevelPieceChanged(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->LevelPieceChangedEvent(pieceBefore, pieceAfter);
	}	
}

// Action for when the player's score changes
void GameEventManager::ActionScoreChanged(int amt) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ScoreChangedEvent(amt);
	}	
}

// Action for when the score multiplier changes
void GameEventManager::ActionScoreMultiplierChanged(int oldScoreMultiplier, int newScoreMultiplier) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ScoreMultiplierChangedEvent(oldScoreMultiplier, newScoreMultiplier);
	}	
}

// Action for when a new item is spawned in the game
void GameEventManager::ActionItemSpawned(const GameItem& item) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ItemSpawnedEvent(item);
	}	
}

void GameEventManager::ActionItemRemoved(const GameItem& item) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ItemRemovedEvent(item);
	}	
}

// Action for when an item and the player paddle collide
void GameEventManager::ActionItemPaddleCollision(const GameItem& item, const PlayerPaddle& paddle) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ItemPaddleCollsionEvent(item, paddle);
	}	
}

// Action for whan an item is activated (i.e, collected by the player paddle)
void GameEventManager::ActionItemActivated(const GameItem& item) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ItemActivatedEvent(item);
	}		
}

// Action for when an item deactivates (runs out of time or effect is spent)
void GameEventManager::ActionItemDeactivated(const GameItem& item) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ItemDeactivatedEvent(item);
	}		
}

// Action for when an item timer starts
void GameEventManager::ActionItemTimerStarted(const GameItemTimer& itemTimer) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ItemTimerStartedEvent(itemTimer);
	}		
}

// Action for when an item timer stops/expires
void GameEventManager::ActionItemTimerStopped(const GameItemTimer& itemTimer) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ItemTimerStoppedEvent(itemTimer);
	}		
}

// Action for when the item drop block's drop item type changes
void GameEventManager::ActionItemDropBlockItemChange(const ItemDropBlock& dropBlock) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ItemDropBlockItemChangeEvent(dropBlock);
	}	
}

// Action for when a projectile is fired/spawned
void GameEventManager::ActionProjectileSpawned(const Projectile& projectile) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ProjectileSpawnedEvent(projectile);
	}	
}

// Action for when a projectile is removed from the game
void GameEventManager::ActionProjectileRemoved(const Projectile& projectile) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->ProjectileRemovedEvent(projectile);
	}	
}

// Action for when a beam is fired/spawned
void GameEventManager::ActionBeamSpawned(const Beam& beam) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BeamSpawnedEvent(beam);
	}	
}

// Action for when a beam changes shape/position
void GameEventManager::ActionBeamChanged(const Beam& beam) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BeamChangedEvent(beam);
	}	
}

// Action for when a beam is removed from the game
void GameEventManager::ActionBeamRemoved(const Beam& beam) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->BeamRemovedEvent(beam);
	}	
}

// Action for when a tesla lightning barrier is newly spawned between two tesla blocks
void GameEventManager::ActionTeslaLightningBarrierSpawned(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->TeslaLightningBarrierSpawnedEvent(newlyOnTeslaBlock, previouslyOnTeslaBlock);
	}	
}

// Action for when an existing tesla lightning barrier is removed from between two tesla blocks
void GameEventManager::ActionTeslaLightningBarrierRemoved(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->TeslaLightningBarrierRemovedEvent(newlyOffTeslaBlock, stillOnTeslaBlock);
	}	
}

// Action for when the game is completed
void GameEventManager::ActionGameCompleted() {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->GameCompletedEvent();
	}		
}

// Action for when a world starts in-game
void GameEventManager::ActionWorldStarted(const GameWorld& world) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->WorldStartedEvent(world);
	}		
}

// Action for when a world completes in-game
void GameEventManager::ActionWorldCompleted(const GameWorld& world) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->WorldCompletedEvent(world);
	}	
}

// Action for when a level starts in-game
void GameEventManager::ActionLevelStarted(const GameWorld& world, const GameLevel& level) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->LevelStartedEvent(world, level);
	}	
}

// Action for when a level completes in-game
void GameEventManager::ActionLevelCompleted(const GameWorld& world, const GameLevel& level) {
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->LevelCompletedEvent(world, level);
	}	
}

// Action for when the number of player lives changes
void GameEventManager::ActionLivesChanged(int livesLeftBefore, int livesLeftAfter) {
	assert(livesLeftBefore != livesLeftAfter);
	this->listenerIter = this->eventListeners.begin();
	for (; this->listenerIter != this->eventListeners.end(); ++this->listenerIter) {
		(*this->listenerIter)->LivesChangedEvent(livesLeftBefore, livesLeftAfter);
	}	
}