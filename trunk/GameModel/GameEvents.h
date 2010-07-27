#ifndef __GAMEEVENTS_H__
#define __GAMEEVENTS_H__

class Point2D;
class GameBall;
class PlayerPaddle;
class LevelPiece;
class PortalBlock;
class CannonBlock;
class TeslaBlock;
class GameLevel;
class GameWorld;
class GameItem;
class GameItemTimer;
class Projectile;
class Beam;

class GameEvents {
public:
	// Global game events ****************************************************************

	//virtual void GameStarted(...);
	//virtual void GamePaused(...);
	//virtual void GameUnpaused(...);
	
	/**
	 * Event occurs when the entire game has been completed by the player. Event only occurs
	 * once, just as the game completes.
	 */
	virtual void GameCompletedEvent() = 0;

	/**
	 * Event occurs when a new world is started (i.e., the player enters the new world
	 * in the game). This event occurs once just as the world starts.
	 * Arguements: world - The world that is being started.
	 */
	virtual void WorldStartedEvent(const GameWorld& world) = 0;
	
	/**
	 * Event occurs when a world is finished. This event occurs once just as the world finishes.
	 * Arguments: world - The world that is just finished.
	 */
	virtual void WorldCompletedEvent(const GameWorld& world) = 0;

	/**
	 * Event occurs when a new level is started (i.e., the player enters the new level
	 * in the game). This event occurs once just as the level starts.
	 * Arguements: world - The world of the level that just started.
	 *             level - The level that is being started.
	 */
	virtual void LevelStartedEvent(const GameWorld& world, const GameLevel& level) = 0;

	/**
	 * Event occurs when a level is finished. This event occurs once just as the level finishes.
	 * Arguments: world - The world of the level that just finished.
	 *            level - The level that is just finished.
	 */
	virtual void LevelCompletedEvent(const GameWorld& world, const GameLevel& level) = 0;


	// Paddle related events *************************************************************
	
	/**
	 * This event occurs whenever a player smacks the paddle into a wall - it only happens
	 * ONCE per smack (they then have to remove the paddle and hit the wall again to trigger
	 * another one of these events).
	 * Arguements: paddle - The paddle that just hit the wall.
	 *             hitLoc - The central location where the paddle smacked into the wall.
	 */
	virtual void PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc) = 0;

	/**
	 * This event occurs when the player paddle gets hit by a projectile - it only happens once
	 * per projectile and happens just before the effects of the hit take place.
	 * Arguments: paddle     - The paddle that just got hit by the projectile.
	 *            projectile - The projectile that just hit the paddle.
	 */
	virtual void PaddleHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile) = 0;

	// Ball related events ***************************************************************

	/**
	 * Event triggered when all balls have fallen below the death point in the level. This event
	 * is triggered once per complete ball death.
	 * Arguements: livesLeft - The number of lives/balls left for the player.
	 */
	virtual void AllBallsDeadEvent(int livesLeft) = 0;

	/**
	 * Event triggered before the last ball dies (i.e., before the camera follows the last ball
	 * to its tragic death). This event is triggered once just as the last ball dies.
	 * Arguements: lastBallToDie - The last ball left, which is now about to die.
	 */
	virtual void LastBallAboutToDieEvent(const GameBall& lastBallToDie) = 0;

	/**
	 * Event triggered when the last ball explodes in a blaze of horrible, horrible glory.
	 * This event is triggered once just as the ball explodes (but is not officially dead yet).
	 * Arguements: explodedBall - The ball that is exploding / just exploded.
	 */
	virtual void LastBallExploded(const GameBall& explodedBall) = 0;

	/**
	 * Event triggered when one of the game balls has died. This event is triggered
	 * once per ball death.
	 * Arguements: deadBall - The ball that has died (just before it is deleted).
	 */
	virtual void BallDiedEvent(const GameBall& deadBall) = 0;

	/**
	 * Event triggered whenever the ball respawns on the player paddle. Only occurs once
	 * per respawn event.
	 * Arguements: spawnedBall - The ball, just as it respawns.
	 */
	virtual void BallSpawnEvent(const GameBall& spawnedBall) = 0;

	/**
	 * Event triggered when the ball is shot from the player paddle. Only occurs once
	 * just as the ball is being shot.
	 * Arguements: shotBall - The ball with the velocity given to it by the shot and its
	 * initial position as it is being shot.
	 */
	virtual void BallShotEvent(const GameBall& shotBall) = 0;

	/**
	 * Event triggered when the ball collides with a level block. Only occurs once as the ball
	 * collides with the block.
	 * Arguements: ball				 - The ball as it is colliding with the block.
	 *             block       - The block being affected by the ball.
	 */
	virtual void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) = 0;

	/**
	 * Event triggered when a projectile collides with a level block. Only occurs once as the projectile
	 * collides with the block.
	 * Arguements: projectile - The projectile as it is colliding with the block.
	 *             block      - The block being affected by the projectile.
	 */
	virtual void ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block) = 0;

	/**
	 * Event triggered when the ball collides with the player paddle. Only occurs once as the ball
	 * collides with the paddle.
	 * Arguements: ball   - The ball as it is colliding with the player paddle.
	 *             paddle - The paddle as it is colliding with the ball.
   */
	virtual void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) = 0;

	/**
	 * Event triggered when two balls collide. Only occurs once as a ball collides with another.
	 * Arguements: ball1 - A ball in the collision.
	 *             ball2 - The other ball in the collision.
	 */
	virtual void BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2) = 0;


	/** 
	 * Event triggered when a ball is teleported by a portal block. Only occurs once as the ball enters
	 * the portal block, one frame before it is shown coming out of the sibling portal.
	 * Arguments: ball				- The ball teleported.
	 *            enterPortal	- The portal entered by the ball.
	 */
	virtual void BallPortalBlockTeleportEvent(const GameBall& ball, const PortalBlock& enterPortal) = 0;

	/**
	 * Event triggered when a projectile is teleported by a portal block. Only occurs once per projectile,
	 * occurs one frame before the projectile is shown coming out of the sibling portal.
	 * Arguements: projectile  - The projectile teleported.
	 *             enterPortal - The portal entered by the projectile.
	 */
	virtual void ProjectilePortalBlockTeleportEvent(const Projectile& projectile, const PortalBlock& enterPortal) = 0;

	/**
	 * Event triggered when a ball is fired out of a cannon block. Only occurs once as the ball is JUST being
	 * fired out of the barrel of the cannon.
	 * Arguements: ball        - The ball being fired out of the cannon.
	 *             cannonBlock - The cannon block firing the ball.
	 */
	virtual void BallFiredFromCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock) = 0;

	/**
	 * Event triggered when the ball hits a tesla block to tesla block lightning arc.
	 * Occurs right before the the repel of the ball from the arc takes place.
	 * Arguements: ball         - The ball that hit the lightning arc.
	 *             teslaBlock1  - One of the tesla blocks responsible for the lightning arc.
	 *             teslaBlock2  - The other tesla block responsible for the lightning arc.    
	 */
	virtual void BallHitTeslaLightningArcEvent(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2) = 0;

	// Misc Events (Destruction, Combos, etc.) ************************************************

	/**
	 * Event triggered when a typical block is destoryed (turned into an empty space). Only occurs
	 * once as the block is being destroyed.
	 * Arguements: block - The block being destroyed, just before it is destroyed.
	 */
	virtual void BlockDestroyedEvent(const LevelPiece& block) = 0;

	/**
	 * Event triggered when a ball safety net is created. Only occurs once as the safety net
	 * becomes active.
	 */
	virtual void BallSafetyNetCreatedEvent() = 0;

	/**
	 * Event triggered when a ball collides with the safety net and the safety net is
	 * destroyed by it.
	 * Arguements: ball - The ball that collided and destroyed the safety net.
	 */
	virtual void BallSafetyNetDestroyedEvent(const GameBall& ball) = 0;
	// Same as above, only the paddle did it.
	virtual void BallSafetyNetDestroyedEvent(const PlayerPaddle& paddle) = 0;

	/**
	 * Event triggered when a level piece / block changes from one type to another either within the same
	 * object or to a different object. Only occurs once as the piece changes - this may be triggered along side
	 * other similar events e.g., BlockDestroyedEvent, BallBlockCollisionEvent.
	 * Arguements: pieceBefore - The LevelPiece object before the change.
	 *						 pieceAfter  = The LevelPiece object after the change.
	 */
	virtual void LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) = 0;

	/**
	 * Event triggered when there is a change to the player's score. Only occurs once per change.
	 * Arguements: amt - The amount the score changed by.
	 */
	virtual void ScoreChangedEvent(int amt) = 0;

	/**
	 * Event triggered when there is a change to the player's score multiplier. 
	 * Only occurs once per change.
	 * Arguements: oldMultiplier - The multiplier before the change.
	 *             newMultiplier - The multiplier after the change.
	 */
	virtual void ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier) = 0;

	/**
	 * Event triggered when an item is spawned and becomes part of the game.
	 * Only occurs once per spawn.
	 * Arguements: item - The newly spawned item.
	 */
	virtual void ItemSpawnedEvent(const GameItem& item) = 0;

	/**
	 * Event triggered when an item drop is removed because it has either been
	 * acquired by the player paddle or has left the game boundries.
	 * Arguements: item - The removed item.
	 */
	virtual void ItemRemovedEvent(const GameItem& item) = 0;

	/**
	 * Event triggered when an item hits the player paddle (i.e., the player gains the item).
	 * Only occurs once at the happenstance of collision.
	 * Arguements: item   - The item that has just been 'consumed' by the player.
	 *             paddle - The paddle as it collides with the item.
	 */
	virtual void ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle) = 0;

	/**
	 * Event triggered when an item is activated (begins its timer or just starts its effect).
	 * Only occurs once per item.
	 * Arguements: item - The item that was just activated.
	 */
	virtual void ItemActivatedEvent(const GameItem& item)		= 0;

	/**
	 * Event triggered when an item is deactivated (ends its timer or is just ends its effect).
	 * Only occurs once per item.
	 * Arguements: item - The item that was just deactivated.
	 */
	virtual void ItemDeactivatedEvent(const GameItem& item)	= 0;
	
	/**
	 * Event triggered when an item's timer is started for the first time. Only occurs
	 * once at item activation, when that item has an associated time period of activity.
	 * Arguements: itemTimer - The item timer that just started.
	 */
	virtual void ItemTimerStartedEvent(const GameItemTimer& itemTimer) = 0;

	/**
	 * Event triggered when an item's timer stops. Only occurs once when an item runs
	 * out of its active time and expires.
	 * Arguements: itemTimer - The item timer that just stopped/expired.
	 */
	virtual void ItemTimerStoppedEvent(const GameItemTimer& itemTimer) = 0;

	/**
	 * Event triggered when a projectile is spawned. Only occurs once per spawned projectile.
	 * Arguements: projectile - The projectile that was just spawned.
	 */
	virtual void ProjectileSpawnedEvent(const Projectile& projectile) = 0;

	/**
	 * Event triggered when a projectile is removed. Only occurs once per removed projectile.
	 * Arguements: projectile - The projectile that was just removed.
	 */
	virtual void ProjectileRemovedEvent(const Projectile& projectile) = 0;

	/**
	 * Event triggered when a new beam is spawned. Only occurs once per spawned beam.
	 * Arguements: beam - the beam that was just spawned.
	 */
	virtual void BeamSpawnedEvent(const Beam& beam) = 0;

	/**
	 * Event triggered when a beam changes (i.e., new segments are created destroyed or change position).
	 * Only occurs once per change.
	 * Arguments: beam - The beam that just changed (in its new state after the change).
	 */
	virtual void BeamChangedEvent(const Beam& beam) = 0;

	/**
	 * Event triggered when a beam is removed from the game. Only occurs once right before removal of the beam.
	 * Arguments: beam - The beam about to be removed / being removed.
	 */
	virtual void BeamRemovedEvent(const Beam& beam) = 0;


	/**
	 * Event triggered when a tesla block is activated and one of its neighbour/connecting tesla blocks is
	 * already active - thus spawning a lightning arc between the two.
	 * Arguements: newlyOnTeslaBlock      - The tesla block that just got turned on and now has lightning arc shooting out of it
	 *             previouslyOnTeslaBlock - The previously active connecting tesla block that now has a lightning arc between itself and 'newlyOnTeslaBlock'
	 */
	virtual void TeslaLightningBarrierSpawnedEvent(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock) = 0;

	/**
	 * Event triggered when a tesla block is deactivated and an arc of lightning drops between it and one of its still
	 * active neighbours.
	 * Arguements: newlyOffTeslaBlock - The tesla block that was previously turned on but is now off.
	 *             stillOnTeslaBlock  - The neighbour tesla block that previously had an arc between itself and 'newlyOffTeslaBlock'
	 */
	virtual void TeslaLightningBarrierRemovedEvent(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock) = 0;
	
	/**
	 * Event triggered whenever the number of player lives changes. Only occurs once per life or
	 * set of lives are added or removed.
	 * Arguements: livesLeftBefore - Number of lives left before the change.
	 *             livesLeftAfter  - Number of lives left after the change.
	 */
	virtual void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter) = 0;

};


#endif
