/**
 * GameEventManager.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GameEventManager.h"
#include "GameEvents.h"
#include "GameModel.h"

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
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->PaddleHitWallEvent(paddle, hitLoc);
	}
}

// Action for when the player paddle is hit by a projectile
void GameEventManager::ActionPaddleHitByProjectile(const PlayerPaddle& paddle, const Projectile& projectile) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->PaddleHitByProjectileEvent(paddle, projectile);
	}	
}

// Action for when the player paddle shield is hit by a projectile
void GameEventManager::ActionPaddleShieldHitByProjectile(const PlayerPaddle& paddle, const Projectile& projectile) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->PaddleShieldHitByProjectileEvent(paddle, projectile);
	}	
}

// Action for when the player paddle shield deflects a projectile
void GameEventManager::ActionProjectileDeflectedByPaddleShield(const Projectile& projectile, const PlayerPaddle& paddle) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ProjectileDeflectedByPaddleShieldEvent(projectile, paddle);
	}	
}

void GameEventManager::ActionPaddleHitByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment) {
    std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
        (*listenerIter)->PaddleHitByBeamEvent(paddle, beam, beamSegment);
    }	
}

void GameEventManager::ActionPaddleShieldHitByBeam(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment) {
    std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
        (*listenerIter)->PaddleShieldHitByBeamEvent(paddle, beam, beamSegment);
    }	
}

void GameEventManager::ActionPaddleHitByBoss(const PlayerPaddle& paddle, const BossBodyPart& bossPart) {
    std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
        (*listenerIter)->PaddleHitByBossEvent(paddle, bossPart);
    }	
}

// Action for when a ball has died (gone out of bounds)
void GameEventManager::ActionBallDied(const GameBall& deadBall) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallDiedEvent(deadBall);
	}	
}

// Action for when the last ball left is about to die - (before its spiralling death animations and explosion)
void GameEventManager::ActionLastBallAboutToDie(const GameBall& ballAboutToDie) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LastBallAboutToDieEvent(ballAboutToDie);
	}	
}

// Action for when the last ball is exploding
void GameEventManager::ActionLastBallExploded(const GameBall& explodedBall) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LastBallExploded(explodedBall);
	}	
}

// Action for when the all balls have died (gone out of bounds)
void GameEventManager::ActionAllBallsDead(int livesLeft) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->AllBallsDeadEvent(livesLeft);
	}	
}

// Action for when the ball respawns on the player paddle
void GameEventManager::ActionBallSpawn(const GameBall& spawnBall) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallSpawnEvent(spawnBall);
	}	
}

// Action for when the ball is being shot off the player paddle
void GameEventManager::ActionBallShot(const GameBall& shotBall) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallShotEvent(shotBall);
	}		
}

// Action for when a weapon is fired from the paddle
void GameEventManager::ActionPaddleWeaponFired() {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->PaddleWeaponFiredEvent();
	}		
}

// Action for when a projectile collides with a block in the level
void GameEventManager::ActionProjectileBlockCollision(const Projectile& projectile, const LevelPiece& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ProjectileBlockCollisionEvent(projectile, block);
	}	
}

void GameEventManager::ActionProjectileSafetyNetCollision(const Projectile& projectile, const SafetyNet& safetyNet) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ProjectileSafetyNetCollisionEvent(projectile, safetyNet);
	}
}

void GameEventManager::ActionProjectileBossCollision(const Projectile& projectile, const Boss& boss,
                                                     const BossBodyPart& collisionPart) {
     std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
     for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
         (*listenerIter)->ProjectileBossCollisionEvent(projectile, boss, collisionPart);
     }
}

// Action for when the ball collides with a block in the level
void GameEventManager::ActionBallBlockCollision(const GameBall& ball, const LevelPiece& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallBlockCollisionEvent(ball, block);
	}	
}

// Action for when the ball enters a portal block
void GameEventManager::ActionBallPortalBlockTeleport(const GameBall& ball, const PortalBlock& enterPortal) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallPortalBlockTeleportEvent(ball, enterPortal);
	}	
}

void GameEventManager::ActionProjectilePortalBlockTeleport(const Projectile& projectile, const PortalBlock& enterPortal) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ProjectilePortalBlockTeleportEvent(projectile, enterPortal);
	}	
}

// Action when a ball enters a cannon block
void GameEventManager::ActionBallEnteredCannon(const GameBall& ball, const CannonBlock& cannonBlock) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallEnteredCannonEvent(ball, cannonBlock);
	}	
}

// Action for when the ball is fired from a cannon block
void GameEventManager::ActionBallFiredFromCannon(const GameBall& ball, const CannonBlock& cannonBlock) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallFiredFromCannonEvent(ball, cannonBlock);
	}		
}

// Action for when a rocket enters/is loaded into a cannon block
void GameEventManager::ActionProjectileEnteredCannon(const Projectile& projectile, const CannonBlock& cannonBlock) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ProjectileEnteredCannonEvent(projectile, cannonBlock);
	}	
}

// Action for when the rocket is fired from a cannon block
void GameEventManager::ActionProjectileFiredFromCannon(const Projectile& projectile, const CannonBlock& cannonBlock) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ProjectileFiredFromCannonEvent(projectile, cannonBlock);
	}		
}

// Action for when the ball hits a lightning arc between two tesla blocks
void GameEventManager::ActionBallHitTeslaLightningArc(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallHitTeslaLightningArcEvent(ball, teslaBlock1, teslaBlock2);
	}		
}

void GameEventManager::ActionGamePauseStateChanged(int32_t oldPauseState, int32_t newPauseState) {
    std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
        (*listenerIter)->GamePauseStateChangedEvent(oldPauseState, newPauseState);
    }	
}

// Action for when a fireball is cancelled by the player acquiring an iceball item
void GameEventManager::ActionFireBallCancelledByIceBall(const GameBall& ball) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->FireBallCancelledByIceBallEvent(ball);
	}	
}

// Action for when an iceball is cancelled by the player acquiring a fireball item
void GameEventManager::ActionIceBallCancelledByFireBall(const GameBall& ball) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->IceBallCancelledByFireBallEvent(ball);
	}
}

// Action for when the ball collides with the player paddle
void GameEventManager::ActionBallPaddleCollision(const GameBall& ball, const PlayerPaddle& paddle) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallPaddleCollisionEvent(ball, paddle);
	}	
}

// Action for when a ball collides with another ball
void GameEventManager::ActionBallBallCollision(const GameBall& ball1, const GameBall& ball2) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallBallCollisionEvent(ball1, ball2);
	}	
}

// Action for when a block is destroyed
void GameEventManager::ActionBlockDestroyed(const LevelPiece& block, const LevelPiece::DestructionMethod& method) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BlockDestroyedEvent(block, method);
	}	
}

void GameEventManager::ActionBallSafetyNetCreated() {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallSafetyNetCreatedEvent();
	}		
}

// Action for when the ball safety net is destroyed
void GameEventManager::ActionBallSafetyNetDestroyed(const GameBall& ball) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallSafetyNetDestroyedEvent(ball);
	}	
}
void GameEventManager::ActionBallSafetyNetDestroyed(const PlayerPaddle& paddle) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallSafetyNetDestroyedEvent(paddle);
	}	
}
void GameEventManager::ActionBallSafetyNetDestroyed(const Projectile& projectile) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallSafetyNetDestroyedEvent(projectile);
	}	
}

// Action for when a level piece changes
void GameEventManager::ActionLevelPieceChanged(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LevelPieceChangedEvent(pieceBefore, pieceAfter);
	}	
}

// Action for when a level piece has a status added to it
void GameEventManager::ActionLevelPieceStatusAdded(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LevelPieceStatusAddedEvent(piece, addedStatus);
	}	
}

// Action for when a level piece has a status removed from it
void GameEventManager::ActionLevelPieceStatusRemoved(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LevelPieceStatusRemovedEvent(piece, removedStatus);
	}	
}

// Action for when a level piece has all its status removed from it
void GameEventManager::ActionLevelPieceAllStatusRemoved(const LevelPiece& piece) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LevelPieceAllStatusRemovedEvent(piece);
	}	
}


// Action for when a new item is spawned in the game
void GameEventManager::ActionItemSpawned(const GameItem& item) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ItemSpawnedEvent(item);
	}	
}

// Action for when an item is removed from dropping
void GameEventManager::ActionItemRemoved(const GameItem& item) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ItemRemovedEvent(item);
	}	
}

// Action for when an item and the player paddle collide
void GameEventManager::ActionItemPaddleCollision(const GameItem& item, const PlayerPaddle& paddle) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ItemPaddleCollsionEvent(item, paddle);
	}	
}

// Action for whan an item is activated (i.e, collected by the player paddle)
void GameEventManager::ActionItemActivated(const GameItem& item) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ItemActivatedEvent(item);
	}		
}

// Action for when an item deactivates (runs out of time or effect is spent)
void GameEventManager::ActionItemDeactivated(const GameItem& item) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ItemDeactivatedEvent(item);
	}		
}

// Action for when a random item is activated
void GameEventManager::ActionRandomItemActivated(const RandomItem& randomItem, const GameItem& actualItem) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->RandomItemActivatedEvent(randomItem, actualItem);
	}	
}

// Action for when an item timer starts
void GameEventManager::ActionItemTimerStarted(const GameItemTimer& itemTimer) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ItemTimerStartedEvent(itemTimer);
	}		
}

// Action for when an item timer stops/expires
void GameEventManager::ActionItemTimerStopped(const GameItemTimer& itemTimer) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ItemTimerStoppedEvent(itemTimer);
	}		
}

// Action for when the item drop block's drop item type changes
void GameEventManager::ActionItemDropBlockItemChange(const ItemDropBlock& dropBlock) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ItemDropBlockItemChangeEvent(dropBlock);
	}	
}

// Action for when a switch block is initially turned on
void GameEventManager::ActionSwitchBlockActivated(const SwitchBlock& switchBlock) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->SwitchBlockActivatedEvent(switchBlock);
	}	
}

// Action for when the bullet time state changes
void GameEventManager::ActionBulletTimeStateChanged(const BallBoostModel& boostModel) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BulletTimeStateChangedEvent(boostModel);
	}	
}

// Action for when the ball is boosted
void GameEventManager::ActionBallBoostExecuted(const BallBoostModel& boostModel) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallBoostExecutedEvent(boostModel);
	}	
}

// Action for when a boost is gained
void GameEventManager::ActionBallBoostGained() {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallBoostGainedEvent();
	}
}

// Action for when a boost is lost
void GameEventManager::ActionBallBoostLost(bool allBoostsLost) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallBoostLostEvent(allBoostsLost);
	}
}

void GameEventManager::ActionBallBoostUsed() {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BallBoostUsedEvent();
	}
}

// Action for when the release timer starts up
void GameEventManager::ActionReleaseTimerStarted() {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ReleaseTimerStartedEvent();
	}
}

// Action for when a projectile is fired/spawned
void GameEventManager::ActionProjectileSpawned(const Projectile& projectile) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ProjectileSpawnedEvent(projectile);
	}	
}

// Action for when a projectile is removed from the game
void GameEventManager::ActionProjectileRemoved(const Projectile& projectile) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ProjectileRemovedEvent(projectile);
	}	
}

// Action for when a rocket explodes
void GameEventManager::ActionRocketExploded(const RocketProjectile& rocket) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->RocketExplodedEvent(rocket);
	}	
}

// Action for when an armed mine explodes
void GameEventManager::ActionMineExploded(const MineProjectile& mine) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->MineExplodedEvent(mine);
	}	
}

void GameEventManager::ActionRemoteControlRocketFuelWarning(const PaddleRemoteControlRocketProjectile& rocket) {
    std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
        (*listenerIter)->RemoteControlRocketFuelWarningEvent(rocket);
    }
}

void GameEventManager::ActionRemoteControlRocketThrustApplied(const PaddleRemoteControlRocketProjectile& rocket) {
    std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
        (*listenerIter)->RemoteControlRocketThrustAppliedEvent(rocket);
    }
}

// Action for when a beam is fired/spawned
void GameEventManager::ActionBeamSpawned(const Beam& beam) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BeamSpawnedEvent(beam);
	}	
}

// Action for when a beam changes shape/position
void GameEventManager::ActionBeamChanged(const Beam& beam) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BeamChangedEvent(beam);
	}	
}

// Action for when a beam is removed from the game
void GameEventManager::ActionBeamRemoved(const Beam& beam) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BeamRemovedEvent(beam);
	}	
}

// Action for when a tesla lightning barrier is newly spawned between two tesla blocks
void GameEventManager::ActionTeslaLightningBarrierSpawned(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->TeslaLightningBarrierSpawnedEvent(newlyOnTeslaBlock, previouslyOnTeslaBlock);
	}	
}

// Action for when an existing tesla lightning barrier is removed from between two tesla blocks
void GameEventManager::ActionTeslaLightningBarrierRemoved(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->TeslaLightningBarrierRemovedEvent(newlyOffTeslaBlock, stillOnTeslaBlock);
	}	
}

// Action for when the game is completed
void GameEventManager::ActionGameCompleted() {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->GameCompletedEvent();
	}		
}

// Action for when a world starts in-game
void GameEventManager::ActionWorldStarted(const GameWorld& world) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->WorldStartedEvent(world);
	}		
}

// Action for when a world completes in-game
void GameEventManager::ActionWorldCompleted(const GameWorld& world) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->WorldCompletedEvent(world);
	}	
}

// Action for when a level starts in-game
void GameEventManager::ActionLevelStarted(const GameWorld& world, const GameLevel& level) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LevelStartedEvent(world, level);
    }
}

void GameEventManager::ActionLevelAlmostComplete(const GameLevel& level) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LevelAlmostCompleteEvent(level);
	}	
}

// Action for when a level completes in-game
void GameEventManager::ActionLevelCompleted(const GameWorld& world, const GameLevel& level) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LevelCompletedEvent(world, level);
	}	
}

// Action for when the number of player lives changes
void GameEventManager::ActionLivesChanged(int livesLeftBefore, int livesLeftAfter) {
	assert(livesLeftBefore != livesLeftAfter);
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LivesChangedEvent(livesLeftBefore, livesLeftAfter);
	}	
}

// Action for when ice shatters on a block
void GameEventManager::ActionBlockIceShattered(const LevelPiece& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BlockIceShatteredEvent(block);
	}	
}

// Action for when a block that is frozen is cancelled-out by something firey hitting it
void GameEventManager::ActionBlockIceCancelledWithFire(const LevelPiece& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BlockIceCancelledWithFireEvent(block);
	}
}

// Action for when a block that is on fire is cancelled-out by something icy hitting it
void GameEventManager::ActionBlockFireCancelledWithIce(const LevelPiece& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BlockFireCancelledWithIceEvent(block);
	}
}

// Action for when points are awarded and the GUI needs to be notified
void GameEventManager::ActionPointNotification(const PointAward& pointAward) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->PointNotificationEvent(pointAward);
	}
}

// Action for when the player's score changes
void GameEventManager::ActionScoreChanged(int newScore) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ScoreChangedEvent(newScore);
	}	
}

// Action for when the multiplier counter changes
void GameEventManager::ActionScoreMultiplierCounterChanged(int oldCounterValue, int newCounterValue) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ScoreMultiplierCounterChangedEvent(oldCounterValue, newCounterValue);
	}	
}

// Action for when the score multiplier changes
void GameEventManager::ActionScoreMultiplierChanged(int oldMultiplier, int newMultiplier, const Point2D& position) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->ScoreMultiplierChangedEvent(oldMultiplier, newMultiplier, position);
	}	
}

// Action for when the number of awarded stars changes
void GameEventManager::ActionNumStarsChanged(const PointAward* pointAward, int oldNumStars, int newNumStars) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->NumStarsChangedEvent(pointAward, oldNumStars, newNumStars);
	}
}

void GameEventManager::ActionDifficultyChanged(int newDifficulty) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
        (*listenerIter)->DifficultyChangedEvent(static_cast<GameModel::Difficulty>(newDifficulty));
	}
}

void GameEventManager::ActionLaserTurretAIStateChanged(const LaserTurretBlock& block,
                                                       LaserTurretBlock::TurretAIState oldState,
                                                       LaserTurretBlock::TurretAIState newState) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LaserTurretAIStateChangedEvent(block, oldState, newState);
	}
}

void GameEventManager::ActionLaserFiredByTurret(const LaserTurretBlock& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->LaserFiredByTurretEvent(block);
	}
}

void GameEventManager::ActionRocketTurretAIStateChanged(const RocketTurretBlock& block,
                                                        RocketTurretBlock::TurretAIState oldState,
                                                        RocketTurretBlock::TurretAIState newState) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->RocketTurretAIStateChangedEvent(block, oldState, newState);
	}
}

void GameEventManager::ActionRocketFiredByTurret(const RocketTurretBlock& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->RocketFiredByTurretEvent(block);
	}
}

void GameEventManager::ActionMineTurretAIStateChanged(const MineTurretBlock& block,
                                                      MineTurretBlock::TurretAIState oldState,
                                                      MineTurretBlock::TurretAIState newState) {

    std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->MineTurretAIStateChangedEvent(block, oldState, newState);
	}
}

void GameEventManager::ActionMineFiredByTurret(const MineTurretBlock& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->MineFiredByTurretEvent(block);
	}
}

void GameEventManager::ActionRegenBlockLifeChanged(const RegenBlock& block, float lifePercentBefore) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->RegenBlockLifeChangedEvent(block, lifePercentBefore);
	}
}

void GameEventManager::ActionRegenBlockPreturbed(const RegenBlock& block) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->RegenBlockPreturbedEvent(block);
	}
}

void GameEventManager::ActionBossHurt(const BossWeakpoint* hurtPart) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BossHurtEvent(hurtPart);
	}
}

void GameEventManager::ActionBossAngry(const Boss* boss, const BossBodyPart* angryPart) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BossAngryEvent(boss, angryPart);
	}
}

void GameEventManager::ActionGeneralEffect(const GeneralEffectEventInfo& effectEvent) {
    std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
        (*listenerIter)->GeneralEffectEvent(effectEvent);
    }
}

void GameEventManager::ActionBossEffect(const BossEffectEventInfo& effectEvent) {
	std::list<GameEvents*>::iterator listenerIter = this->eventListeners.begin();
	for (; listenerIter != this->eventListeners.end(); ++listenerIter) {
		(*listenerIter)->BossEffectEvent(effectEvent);
	}
}