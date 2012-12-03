/**
 * GameEvents.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEEVENTS_H__
#define __GAMEEVENTS_H__

#include "LevelPiece.h"
#include "GameWorld.h"
#include "GameLevel.h"
#include "PortalBlock.h"
#include "CannonBlock.h"
#include "TeslaBlock.h"
#include "SwitchBlock.h"
#include "ItemDropBlock.h"
#include "LaserTurretBlock.h"
#include "RandomItem.h"
#include "Beam.h"
#include "PointAward.h"
#include "BallBoostModel.h"
#include "SafetyNet.h"
#include "PaddleMineProjectile.h"

class Point2D;
class GameBall;
class PlayerPaddle;
class GameItemTimer;
class Projectile;
class RocketProjectile;


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
    virtual void GameCompletedEvent() {}

	/**
	 * Event occurs when a new world is started (i.e., the player enters the new world
	 * in the game). This event occurs once just as the world starts.
	 * Arguements: world - The world that is being started.
	 */
    virtual void WorldStartedEvent(const GameWorld& world) { UNUSED_PARAMETER(world); }
	
	/**
	 * Event occurs when a world is finished. This event occurs once just as the world finishes.
	 * Arguments: world - The world that is just finished.
	 */
    virtual void WorldCompletedEvent(const GameWorld& world) { UNUSED_PARAMETER(world); }

	/**
	 * Event occurs when a new level is started (i.e., the player enters the new level
	 * in the game). This event occurs once just as the level starts.
	 * Arguements: world - The world of the level that just started.
	 *             level - The level that is being started.
	 */
    virtual void LevelStartedEvent(const GameWorld& world, const GameLevel& level) { UNUSED_PARAMETER(world); UNUSED_PARAMETER(level); }

    /**
	 * Event occurs when a level is ALMOST finished (i.e., a few more blocks are left). This event occurs once just as a
     * certain threshold of blocks left is reached.
	 * Arguments: level - The level that is almost complete.
	 */
    virtual void LevelAlmostCompleteEvent(const GameLevel& level) { UNUSED_PARAMETER(level); }

	/**
	 * Event occurs when a level is finished. This event occurs once just as the level finishes.
	 * Arguments: world - The world of the level that just finished.
	 *            level - The level that is just finished.
	 */
    virtual void LevelCompletedEvent(const GameWorld& world, const GameLevel& level) { UNUSED_PARAMETER(world); UNUSED_PARAMETER(level); }


	// Paddle related events *************************************************************
	
	/**
	 * This event occurs whenever a player smacks the paddle into a wall - it only happens
	 * ONCE per smack (they then have to remove the paddle and hit the wall again to trigger
	 * another one of these events).
	 * Arguements: paddle - The paddle that just hit the wall.
	 *             hitLoc - The central location where the paddle smacked into the wall.
	 */
    virtual void PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc) { UNUSED_PARAMETER(paddle); UNUSED_PARAMETER(hitLoc); }

	/**
	 * This event occurs when the player paddle gets hit by a projectile - it only happens once
	 * per projectile and happens just before the effects of the hit take place.
	 * Arguments: paddle     - The paddle that just got hit by the projectile.
	 *            projectile - The projectile that just hit the paddle.
	 */
    virtual void PaddleHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile) { UNUSED_PARAMETER(paddle); UNUSED_PARAMETER(projectile); }

	/**
	 * This event occurs when the player paddle's shield gets hit by a projectile - it only happens once
	 * per projectile and happens just before the effects of the hit take place.
	 * Arguments: paddle     - The paddle whose shield just got hit by the projectile.
	 *            projectile - The projectile that just hit the paddle.
	 */
    virtual void PaddleShieldHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile) { UNUSED_PARAMETER(paddle); UNUSED_PARAMETER(projectile); }

	// Ball related events ***************************************************************

	/**
	 * Event triggered when all balls have fallen below the death point in the level. This event
	 * is triggered once per complete ball death.
	 * Arguements: livesLeft - The number of lives/balls left for the player.
	 */
    virtual void AllBallsDeadEvent(int livesLeft) { UNUSED_PARAMETER(livesLeft); }

	/**
	 * Event triggered before the last ball dies (i.e., before the camera follows the last ball
	 * to its tragic death). This event is triggered once just as the last ball dies.
	 * Arguements: lastBallToDie - The last ball left, which is now about to die.
	 */
    virtual void LastBallAboutToDieEvent(const GameBall& lastBallToDie) { UNUSED_PARAMETER(lastBallToDie); }

	/**
	 * Event triggered when the last ball explodes in a blaze of horrible, horrible glory.
	 * This event is triggered once just as the ball explodes (but is not officially dead yet).
	 * Arguements: explodedBall - The ball that is exploding / just exploded.
	 */
    virtual void LastBallExploded(const GameBall& explodedBall) { UNUSED_PARAMETER(explodedBall); }

	/**
	 * Event triggered when one of the game balls has died. This event is triggered
	 * once per ball death.
	 * Arguements: deadBall - The ball that has died (just before it is deleted).
	 */
    virtual void BallDiedEvent(const GameBall& deadBall) { UNUSED_PARAMETER(deadBall); }

	/**
	 * Event triggered whenever the ball respawns on the player paddle. Only occurs once
	 * per respawn event.
	 * Arguements: spawnedBall - The ball, just as it respawns.
	 */
    virtual void BallSpawnEvent(const GameBall& spawnedBall) { UNUSED_PARAMETER(spawnedBall); }

	/**
	 * Event triggered when the ball is shot from the player paddle. Only occurs once
	 * just as the ball is being shot.
	 * Arguements: shotBall - The ball with the velocity given to it by the shot and its
	 * initial position as it is being shot.
	 */
    virtual void BallShotEvent(const GameBall& shotBall) { UNUSED_PARAMETER(shotBall); }

    /** 
     * Event triggered whenever a weapon is fired from the paddle during the game.
     */
    virtual void PaddleWeaponFiredEvent() {}

	/**
	 * Event triggered when the ball collides with a level block. Only occurs once as the ball
	 * collides with the block.
	 * Arguements: ball				 - The ball as it is colliding with the block.
	 *             block       - The block being affected by the ball.
	 */
    virtual void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) { UNUSED_PARAMETER(ball); UNUSED_PARAMETER(block); }

	/**
	 * Event triggered when a projectile collides with a level block. Only occurs once as the projectile
	 * collides with the block.
	 * Arguements: projectile - The projectile as it is colliding with the block.
	 *             block      - The block being affected by the projectile.
	 */
    virtual void ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block) { UNUSED_PARAMETER(projectile); UNUSED_PARAMETER(block); }

    /**
     * Event triggered when a projectile collides with the safety net. Occurs once just as the projectile collides.
     * Arguements: projectile - The projectile that collided with the safety net.
     *             safetyNet  - The safety net that was collided with.
     */
    virtual void ProjectileSafetyNetCollisionEvent(const Projectile& projectile, const SafetyNet& safetyNet) { UNUSED_PARAMETER(projectile); UNUSED_PARAMETER(safetyNet); }

	/**
	 * Event triggered when a projectile hits the paddle shield and is deflected by it. Occurs once right after
	 * the deflection.
	 * Arguments: projectile - The projectile that was just deflected.
	 *            paddle     - The paddle whose shield deflected the projectile.
	 */
    virtual void ProjectileDeflectedByPaddleShieldEvent(const Projectile& projectile, const PlayerPaddle& paddle) { 
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(paddle);
    }
		
	/**
	 * Event triggered when the ball collides with the player paddle. Only occurs once as the ball
	 * collides with the paddle.
	 * Arguements: ball   - The ball as it is colliding with the player paddle.
	 *             paddle - The paddle as it is colliding with the ball.
   */
    virtual void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) { UNUSED_PARAMETER(ball); UNUSED_PARAMETER(paddle); }

	/**
	 * Event triggered when two balls collide. Only occurs once as a ball collides with another.
	 * Arguements: ball1 - A ball in the collision.
	 *             ball2 - The other ball in the collision.
	 */
    virtual void BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2) { UNUSED_PARAMETER(ball1); UNUSED_PARAMETER(ball2); }


	/** 
	 * Event triggered when a ball is teleported by a portal block. Only occurs once as the ball enters
	 * the portal block, one frame before it is shown coming out of the sibling portal.
	 * Arguments: ball				- The ball teleported.
	 *            enterPortal	- The portal entered by the ball.
	 */
    virtual void BallPortalBlockTeleportEvent(const GameBall& ball, const PortalBlock& enterPortal) { UNUSED_PARAMETER(ball); UNUSED_PARAMETER(enterPortal); }

	/**
	 * Event triggered when a projectile is teleported by a portal block. Only occurs once per projectile,
	 * occurs one frame before the projectile is shown coming out of the sibling portal.
	 * Arguements: projectile  - The projectile teleported.
	 *             enterPortal - The portal entered by the projectile.
	 */
    virtual void ProjectilePortalBlockTeleportEvent(const Projectile& projectile, const PortalBlock& enterPortal) {
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(enterPortal);
    }

	/**
	 * Event triggered when a ball is fired out of a cannon block. Only occurs once as the ball is JUST being
	 * fired out of the barrel of the cannon.
	 * Arguements: ball        - The ball being fired out of the cannon.
	 *             cannonBlock - The cannon block firing the ball.
	 */
    virtual void BallFiredFromCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock) {
        UNUSED_PARAMETER(ball);
        UNUSED_PARAMETER(cannonBlock);
    }

	/**
	 * Event triggered when a projectile enters a cannon block. Only occurs once as the projectile is JUST being
	 * loaded into the barrel of the cannon.
	 * Arguements: projectile  - The projectile being loaded by the cannon.
	 *             cannonBlock - The cannon block loading the rocket. 
	 */
    virtual void ProjectileEnteredCannonEvent(const Projectile& projectile, const CannonBlock& cannonBlock) {
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(cannonBlock);
    }

	/**
	 * Event triggered when a projectile is fired out of a cannon block. Only occurs once as the projectile is JUST being
	 * fired out of the barrel of the cannon.
	 * Arguements: projectile  - The rocket being fired out of the cannon.
	 *             cannonBlock - The cannon block firing the rocket. 
	 */
    virtual void ProjectileFiredFromCannonEvent(const Projectile& projectile, const CannonBlock& cannonBlock) {
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(cannonBlock);
    }

	/**
	 * Event triggered when the ball hits a tesla block to tesla block lightning arc.
	 * Occurs right before the the repel of the ball from the arc takes place.
	 * Arguements: ball         - The ball that hit the lightning arc.
	 *             teslaBlock1  - One of the tesla blocks responsible for the lightning arc.
	 *             teslaBlock2  - The other tesla block responsible for the lightning arc.    
	 */
    virtual void BallHitTeslaLightningArcEvent(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2) {
        UNUSED_PARAMETER(ball);
        UNUSED_PARAMETER(teslaBlock1);
        UNUSED_PARAMETER(teslaBlock2);
    }

	// Misc Events (Destruction, Combos, etc.) ************************************************

	/**
	 * Event triggered when a typical block is destoryed (turned into an empty space). Only occurs
	 * once as the block is being destroyed.
	 * Arguements: block  - The block being destroyed, just before it is destroyed.
	 *             method - Method of block destruction.
     */
    virtual void BlockDestroyedEvent(const LevelPiece& block, const LevelPiece::DestructionMethod& method) {
        UNUSED_PARAMETER(block);
        UNUSED_PARAMETER(method);
    }

	/**
	 * Event triggered when a ball safety net is created. Only occurs once as the safety net
	 * becomes active.
	 */
    virtual void BallSafetyNetCreatedEvent() {}

	/**
	 * Event triggered when a ball collides with the safety net and the safety net is
	 * destroyed by it.
	 * Arguements: ball - The ball that collided and destroyed the safety net.
	 */
    virtual void BallSafetyNetDestroyedEvent(const GameBall& ball) { UNUSED_PARAMETER(ball); }
	// Same as above, only the paddle did it.
    virtual void BallSafetyNetDestroyedEvent(const PlayerPaddle& paddle) { UNUSED_PARAMETER(paddle); }
	// Same as above, only a projectile did it.
    virtual void BallSafetyNetDestroyedEvent(const Projectile& projectile) { UNUSED_PARAMETER(projectile); }

    /**
     * Event triggered when the bullet time state has changed. Triggered right after the state changed.
     * Arguements: boostModel - The ball boost model that contains the new bullet time state.
     */
    virtual void BulletTimeStateChangedEvent(const BallBoostModel& boostModel) { UNUSED_PARAMETER(boostModel); }
 
    /**
     * Event triggered when the ball is boosted. The event is triggered once right after the
     * boost has been applied to the ball.
     * Arguements: boostModel - The ball boost model.
     */
    virtual void BallBoostExecutedEvent(const BallBoostModel& boostModel) { UNUSED_PARAMETER(boostModel); }


    /**
     * Events triggered when a ball boost is gained/lost, respectively. The events are triggered
     * right after the number of available boosts is incremented/decremented.
     */
    virtual void BallBoostGainedEvent() {}
    // Args: allBoostsLost - true if all boosts were lost
    virtual void BallBoostLostEvent(bool allBoostsLost) { UNUSED_PARAMETER(allBoostsLost); }

	/**
	 * Event triggered when a level piece / block changes from one type to another either within the same
	 * object or to a different object. Only occurs once as the piece changes - this may be triggered along side
	 * other similar events e.g., BlockDestroyedEvent, BallBlockCollisionEvent.
	 * Arguements: pieceBefore - The LevelPiece object before the change.
	 *						 pieceAfter  - The LevelPiece object after the change.
	 */
    virtual void LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
        UNUSED_PARAMETER(pieceBefore);
        UNUSED_PARAMETER(pieceAfter);
    }

	/**
	 * Event triggered when a level piece has a status effect added to it. Only occurs once, just after the status is applied, 
	 * for a particular status effect. All status effects are eventually removed either via the 
	 * LevelPieceStatusRemoved or LevelPieceAllStatusRemoved events.
	 * Arguements: piece       - The level piece that had the status added to it.
	 *             addedStatus - The status added to the piece. 
	 */
    virtual void LevelPieceStatusAddedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus) {
        UNUSED_PARAMETER(piece);
        UNUSED_PARAMETER(addedStatus);
    }

	/**
	 * Event triggered when a level piece has a status effect removed from it. Only occurs once, just after the status is removed,
	 * for a particular status effect. A LevelPieceStatusAdded event for that same status is gaurenteed to have been made before this event.
	 * Arguements: piece         - The level piece that had the status removed from it.
	 *             removedStatus - The status removed from the piece. 
	 */
    virtual void LevelPieceStatusRemovedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus) {
        UNUSED_PARAMETER(piece);
        UNUSED_PARAMETER(removedStatus);
    }

	/**
	 * Event triggered when a level piece has all status effects removed from it. Only occurs once just after all status is removed.
	 * Arguements: piece - The level piece that had all the status removed from it.
	 */
    virtual void LevelPieceAllStatusRemovedEvent(const LevelPiece& piece) { UNUSED_PARAMETER(piece); }

	/**
	 * Event triggered when an item is spawned and becomes part of the game.
	 * Only occurs once per spawn.
	 * Arguements: item - The newly spawned item.
	 */
    virtual void ItemSpawnedEvent(const GameItem& item) { UNUSED_PARAMETER(item); }

	/**
	 * Event triggered when an item drop is removed because it has either been
	 * acquired by the player paddle or has left the game boundries.
	 * Arguements: item - The removed item.
	 */
    virtual void ItemRemovedEvent(const GameItem& item) { UNUSED_PARAMETER(item); }

	/**
	 * Event triggered when an item hits the player paddle (i.e., the player gains the item).
	 * Only occurs once at the happenstance of collision.
	 * Arguements: item   - The item that has just been 'consumed' by the player.
	 *             paddle - The paddle as it collides with the item.
	 */
    virtual void ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle) {
        UNUSED_PARAMETER(item);
        UNUSED_PARAMETER(paddle);
    }

	/**
	 * Event triggered when an item is activated (begins its timer or just starts its effect).
	 * Only occurs once per item.
	 * Arguements: item - The item that was just activated.
	 */
    virtual void ItemActivatedEvent(const GameItem& item) { UNUSED_PARAMETER(item); }

	/**
	 * Event triggered when an item is deactivated (ends its timer or is just ends its effect).
	 * Only occurs once per item.
	 * Arguements: item - The item that was just deactivated.
	 */
    virtual void ItemDeactivatedEvent(const GameItem& item)	{ UNUSED_PARAMETER(item); }
	

	/** 
	 * Event triggered when a random item is activated - this is distinct from a typical item since a
	 * random item alone has no effect (what it becomes does). This event is triggered just before the random
     * item's actual item is activated.
	 * Arguements: randomItem - The random item.
     *             actualItem - The actual item that the random item turns into.
	 */
    virtual void RandomItemActivatedEvent(const RandomItem& randomItem, const GameItem& actualItem) {
        UNUSED_PARAMETER(randomItem);
        UNUSED_PARAMETER(actualItem);
    }

	/**
	 * Event triggered when an item's timer is started for the first time. Only occurs
	 * once at item activation, when that item has an associated time period of activity.
	 * Arguements: itemTimer - The item timer that just started.
	 */
    virtual void ItemTimerStartedEvent(const GameItemTimer& itemTimer) { UNUSED_PARAMETER(itemTimer); }

	/**
	 * Event triggered when an item's timer stops. Only occurs once when an item runs
	 * out of its active time and expires.
	 * Arguements: itemTimer - The item timer that just stopped/expired.
	 */
    virtual void ItemTimerStoppedEvent(const GameItemTimer& itemTimer) { UNUSED_PARAMETER(itemTimer); }

	/**
	 * Event triggered when an item drop block changes the item type that it will drop next. Occurs
	 * once right after the item drop block changes the type of item it will drop.
	 * Arguements: dropBlock - the drop block that changed.
	 */
    virtual void ItemDropBlockItemChangeEvent(const ItemDropBlock& dropBlock) { UNUSED_PARAMETER(dropBlock); }

    /**
     * Event triggered when a switch block is initially activated. Occurs once just after the switch
     * activates its triggers.
     * Arguements: switchBlock - the switch block that was activated.
     */
    virtual void SwitchBlockActivatedEvent(const SwitchBlock& switchBlock) { UNUSED_PARAMETER(switchBlock); }
        
	/**
	 * Event triggered when a projectile is spawned. Only occurs once per spawned projectile.
	 * Arguements: projectile - The projectile that was just spawned.
	 */
    virtual void ProjectileSpawnedEvent(const Projectile& projectile) { UNUSED_PARAMETER(projectile); }

	/**
	 * Event triggered when a projectile is removed. Only occurs once per removed projectile.
	 * Arguements: projectile - The projectile that was just removed.
	 */
    virtual void ProjectileRemovedEvent(const Projectile& projectile) { UNUSED_PARAMETER(projectile); }

	/**
	 * Event triggered when a rocket explodes. Only occurs once just as it explodes.
	 * Arguements: rocket - The rocket do'n the explod'n.
	 */
    virtual void RocketExplodedEvent(const PaddleRocketProjectile& rocket) { UNUSED_PARAMETER(rocket); } 

	/**
	 * Event triggered when a mine explodes. Only occurs once just as it explodes.
	 * Arguements: mine - The mine that is exploding.
	 */
    virtual void MineExplodedEvent(const MineProjectile& mine) { UNUSED_PARAMETER(mine); }

	/**
	 * Event triggered when a new beam is spawned. Only occurs once per spawned beam.
	 * Arguements: beam - the beam that was just spawned.
	 */
    virtual void BeamSpawnedEvent(const Beam& beam) { UNUSED_PARAMETER(beam); }

	/**
	 * Event triggered when a beam changes (i.e., new segments are created destroyed or change position).
	 * Only occurs once per change.
	 * Arguments: beam - The beam that just changed (in its new state after the change).
	 */
	virtual void BeamChangedEvent(const Beam& beam) { UNUSED_PARAMETER(beam); }

	/**
	 * Event triggered when a beam is removed from the game. Only occurs once right before removal of the beam.
	 * Arguments: beam - The beam about to be removed / being removed.
	 */
	virtual void BeamRemovedEvent(const Beam& beam) { UNUSED_PARAMETER(beam); }


	/**
	 * Event triggered when a tesla block is activated and one of its neighbour/connecting tesla blocks is
	 * already active - thus spawning a lightning arc between the two.
	 * Arguements: newlyOnTeslaBlock      - The tesla block that just got turned on and now has lightning arc shooting out of it
	 *             previouslyOnTeslaBlock - The previously active connecting tesla block that now has a lightning arc between itself and 'newlyOnTeslaBlock'
	 */
    virtual void TeslaLightningBarrierSpawnedEvent(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock) {
        UNUSED_PARAMETER(newlyOnTeslaBlock);
        UNUSED_PARAMETER(previouslyOnTeslaBlock);
    }

	/**
	 * Event triggered when a tesla block is deactivated and an arc of lightning drops between it and one of its still
	 * active neighbours.
	 * Arguements: newlyOffTeslaBlock - The tesla block that was previously turned on but is now off.
	 *             stillOnTeslaBlock  - The neighbour tesla block that previously had an arc between itself and 'newlyOffTeslaBlock'
	 */
    virtual void TeslaLightningBarrierRemovedEvent(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock) {
        UNUSED_PARAMETER(newlyOffTeslaBlock);
        UNUSED_PARAMETER(stillOnTeslaBlock);
    }
	
	/**
	 * Event triggered whenever the number of player lives changes. Only occurs once per life or
	 * set of lives are added or removed.
	 * Arguements: livesLeftBefore - Number of lives left before the change.
	 *             livesLeftAfter  - Number of lives left after the change.
	 */
    virtual void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter) {
        UNUSED_PARAMETER(livesLeftBefore);
        UNUSED_PARAMETER(livesLeftAfter);
    }

	/**
	 * Event triggered when the ice on a block is shattered. Only occurs once just before the ice shatters.
	 * Arguements: block - The block whose ice shattered.
	 */
    virtual void BlockIceShatteredEvent(const LevelPiece& block) { UNUSED_PARAMETER(block); }


    /**
     * Event triggered when the release timer starts ticking while the ball is on the paddle. Only
     * occurs once just as the timer starts.
     */
    virtual void ReleaseTimerStartedEvent() {}

    // Point related events ****************************************************************

    /**
     * Event tiggered whenever the player earns a fixed point amount, occurs right after the point
     * amount is added to the score.
     * Arguements: pointAward - information about the point notification.
     */
    virtual void PointNotificationEvent(const PointAward& pointAward) { UNUSED_PARAMETER(pointAward); }

	/**
	 * Event triggered when there is a change to the player's score. Only occurs once per change.
	 * Arguements: newScore - The new score after the change.
	 */
    virtual void ScoreChangedEvent(int newScore) { UNUSED_PARAMETER(newScore); }


    /**
     * Event triggered whenever a block is destroyed or something happens that causes the multiplier
     * counter to increase or decrease - the counter is what causes the multiplier to change.
     * Arguements: newCounterValue - The new counter value (0 to 9).
     */
    virtual void ScoreMultiplierCounterChangedEvent(int newCounterValue) { UNUSED_PARAMETER(newCounterValue); }

	/**
	 * Event triggered when there is a change to the player's score multiplier. 
	 * Only occurs once per change.
	 * Arguements: newMultiplier - The multiplier after the change.
     *             position      - The position in the game where the event occurred that changed the multiplier.
	 */
    virtual void ScoreMultiplierChangedEvent(int newMultiplier, const Point2D& position) {
        UNUSED_PARAMETER(newMultiplier);
        UNUSED_PARAMETER(position);
    }

    /**
     * Event triggered when the number of stars awarded to the player has changed
     * Occurs once per change, right after the change occurs.
     * Arguements: oldNumStars - The previous number of stars.
     *             newNumStars - The new number of stars.
     */
    virtual void NumStarsChangedEvent(int oldNumStars, int newNumStars) {
        UNUSED_PARAMETER(oldNumStars);
        UNUSED_PARAMETER(newNumStars);
    }

    // AI Events ***********************************************************

    /**
     * Event triggered when the laser turret AI state of a particular block changes.
     * Only occurs once per change.
     * Arguements: block - The block with the changed state.
     *             oldState - The state that the block used to have.
     *             newState - The state that the block changed to.
     */
    virtual void LaserTurretAIStateChangedEvent(const LaserTurretBlock& block, LaserTurretBlock::TurretAIState oldState,
                                                LaserTurretBlock::TurretAIState newState) {
        UNUSED_PARAMETER(block);
        UNUSED_PARAMETER(oldState);
        UNUSED_PARAMETER(newState);
    }

    /**
     * Event triggered when a laser is fired by a laser turret.
     * Arguements: block - The block that fired the laser.
     */
    virtual void LaserFiredByTurretEvent(const LaserTurretBlock& block) {
        UNUSED_PARAMETER(block);
    }

    /**
     * Event triggered when the rocket turret AI state of a particular block changes.
     * Only occurs once per change.
     * Arguements: block - The block with the changed state.
     *             oldState - The state that the block used to have.
     *             newState - The state that the block changed to.
     */
    virtual void RocketTurretAIStateChangedEvent(const RocketTurretBlock& block,
                                                 RocketTurretBlock::TurretAIState oldState,
                                                 RocketTurretBlock::TurretAIState newState) {
        UNUSED_PARAMETER(block);
        UNUSED_PARAMETER(oldState);
        UNUSED_PARAMETER(newState);
    }
    
    /**
     * Event triggered when the rocket turret fires a rocket.
     * Arguements: block - the rocket turret block that fired.
     */
    virtual void RocketFiredByTurretEvent(const RocketTurretBlock& block) {
        UNUSED_PARAMETER(block);
    }

    /**
     * Event triggered when the mine turret AI state of a particular block changes.
     * Only occurs once per change.
     * Arguements: block - The block with the changed state.
     *             oldState - The state that the block used to have.
     *             newState - The state that the block changed to.
     */
    virtual void MineTurretAIStateChangedEvent(const MineTurretBlock& block,
                                               MineTurretBlock::TurretAIState oldState,
                                               MineTurretBlock::TurretAIState newState) {
        UNUSED_PARAMETER(block);
        UNUSED_PARAMETER(oldState);
        UNUSED_PARAMETER(newState);
    }

    /**
     * Event triggered when the mine turret fires a rocket.
     * Arguements: block - the mine turret block that fired.
     */
    virtual void MineFiredByTurretEvent(const MineTurretBlock& block) {
        UNUSED_PARAMETER(block);
    }
};


#endif
