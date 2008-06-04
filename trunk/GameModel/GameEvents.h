#ifndef __GAMEEVENTS_H__
#define __GAMEEVENTS_H__

class Point2D;
class GameBall;
class PlayerPaddle;
class LevelPiece;
class GameLevel;
class GameWorld;

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
	 * Arguements: level - The level that is being started.
	 */
	virtual void LevelStartedEvent(const GameLevel& level) = 0;

	/**
	 * Event occurs when a level is finished. This event occurs once just as the level finishes.
	 * Arguments: level - The level that is just finished.
	 */
	virtual void LevelCompletedEvent(const GameLevel& level) = 0;


	// Paddle related events *************************************************************
	
	/**
	 * This event occurs whenever a player smacks the paddle into a wall - it only happens
	 * ONCE per smack (they then have to remove the paddle and hit the wall again to trigger
	 * another one of these events).
	 * Arguements: hitLoc - The central location where the paddle smacked into the wall.
	 */
	virtual void PaddleHitWallEvent(const Point2D& hitLoc) = 0;


	// Ball related events ***************************************************************
	//virtual void BallCollision(

	/**
	 * Event triggered when the ball falls below the death point in the level. This event
	 * is triggered once per death.
	 * Arguements: deadBall  - The ball, just as it dies.
	 *             livesLeft - The number of lives/balls left for the player.
	 */
	virtual void BallDeathEvent(const GameBall& deadBall, int livesLeft) = 0;

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
	 * Arguements: ball  - The ball as it is colliding with the block.
	 *             block - The block before it is destroyed or affected by the ball.
	 */
	virtual void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) = 0;

	/**
	 * Event triggered when the ball collides with the player paddle. Only occurs once as the ball
	 * collides with the paddle.
	 * Arguements: ball   - The ball as it is colliding with the player paddle.
	 *             paddle - The paddle as it is colliding with the ball.
   */
	virtual void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) = 0;

	// Misc Events (Destruction, Combos, etc.) ************************************************

	/**
	 * Event triggered when a typical block is destoryed (turned into an empty space). Only occurs
	 * once as the block is being destroyed.
	 * Arguements: block - The block being destroyed, just before it is destroyed.
	 */
	virtual void BlockDestroyedEvent(const LevelPiece& block) = 0;

	/**
	 * Event triggered when there is a change to the player's score. Only occurs once per change.
	 * Arguements: amt - The amount the score changed by.
	 */
	virtual void ScoreChangedEvent(int amt) = 0;

};


#endif
