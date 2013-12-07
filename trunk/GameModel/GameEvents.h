/**
 * GameEvents.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
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
#include "RegenBlock.h"
#include "CollateralBlock.h"
#include "RandomItem.h"
#include "Beam.h"
#include "PointAward.h"
#include "BallBoostModel.h"
#include "SafetyNet.h"
#include "PaddleMineProjectile.h"
#include "PaddleRemoteControlRocketProjectile.h"
#include "CollateralBlockProjectile.h"
#include "GeneralEffectEventInfo.h"
#include "BossEffectEventInfo.h"

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
	 * Arguments: world - The world that is being started.
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
	 * Arguments: world - The world of the level that just started.
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
	 * Arguments: paddle - The paddle that just hit the wall.
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
	 * Arguments: livesLeft - The number of lives/balls left for the player.
	 */
    virtual void AllBallsDeadEvent(int livesLeft) { UNUSED_PARAMETER(livesLeft); }

	/**
	 * Event triggered before the last ball dies (i.e., before the camera follows the last ball
	 * to its tragic death). This event is triggered once just as the last ball dies.
	 * Arguments: lastBallToDie - The last ball left, which is now about to die.
	 */
    virtual void LastBallAboutToDieEvent(const GameBall& lastBallToDie) { UNUSED_PARAMETER(lastBallToDie); }

	/**
	 * Event triggered when the last ball explodes in a blaze of horrible, horrible glory.
	 * This event is triggered once just as the ball explodes (but is not officially dead yet).
	 * Arguments: explodedBall - The ball that is exploding / just exploded.
     *            wasSkipped   - Whether or not the dead sequence was skipped by the player.
	 */
    virtual void LastBallExplodedEvent(const GameBall& explodedBall, bool wasSkipped) {
        UNUSED_PARAMETER(explodedBall);
        UNUSED_PARAMETER(wasSkipped);
    }

	/**
	 * Event triggered when one of the game balls has died. This event is triggered
	 * once per ball death.
	 * Arguments: deadBall - The ball that has died (just before it is deleted).
	 */
    virtual void BallDiedEvent(const GameBall& deadBall) { UNUSED_PARAMETER(deadBall); }

	/**
	 * Event triggered whenever the ball re-spawns on the player paddle. Only occurs once
	 * per re-spawn event.
	 * Arguments: spawnedBall - The ball, just as it re-spawns.
	 */
    virtual void BallSpawnEvent(const GameBall& spawnedBall) { UNUSED_PARAMETER(spawnedBall); }

	/**
	 * Event triggered when the ball is shot from the player paddle. Only occurs once
	 * just as the ball is being shot.
	 * Arguments: shotBall - The ball with the velocity given to it by the shot and its
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
	 * Arguments: ball				 - The ball as it is colliding with the block.
	 *            block       - The block being affected by the ball.
	 */
    virtual void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) { UNUSED_PARAMETER(ball); UNUSED_PARAMETER(block); }

	/**
	 * Event triggered when a projectile collides with a level block. Only occurs once as the projectile
	 * collides with the block.
	 * Arguments: projectile - The projectile as it is colliding with the block.
	 *            block      - The block being affected by the projectile.
	 */
    virtual void ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block) { UNUSED_PARAMETER(projectile); UNUSED_PARAMETER(block); }

    /**
     * Event triggered when a projectile collides with the safety net. Occurs once just as the projectile collides.
     * Arguments: projectile - The projectile that collided with the safety net.
     *            safetyNet  - The safety net that was collided with.
     */
    virtual void ProjectileSafetyNetCollisionEvent(const Projectile& projectile, const SafetyNet& safetyNet) {
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(safetyNet);
    }

    /**
     * Event triggered when a projectile collides with a boss. Occurs once just as the projectile collides.
     * Arguments: projectile    - The projectile that collided with the boss.
     *            boss          - The boss that was collided with.
     *            collisionPart - The specific part of the boss that was collided with.
     */
    virtual void ProjectileBossCollisionEvent(const Projectile& projectile, const Boss& boss, const BossBodyPart& collisionPart) {
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(boss);
        UNUSED_PARAMETER(collisionPart);
    }

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
	 * Event triggered when a beam hits the paddle . Occurs once right after the hit.
	 * Arguments: paddle      - The paddle whose shield deflected the projectile.
     *            beam        - The beam that hit the paddle.
     *            beamSegment - The beam segment of the beam that hit the paddle.
	 */
    virtual void PaddleHitByBeamEvent(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment) {
        UNUSED_PARAMETER(paddle);
        UNUSED_PARAMETER(beam);
        UNUSED_PARAMETER(beamSegment);
    }

	/**
	 * Event triggered when a beam hits the paddle shield. Occurs once right after the hit.
	 * Arguments: paddle      - The paddle whose shield deflected the projectile.
     *            beam        - The beam that hit the paddle.
     *            beamSegment - The beam segment of the beam that hit the paddle.
	 */
    virtual void PaddleShieldHitByBeamEvent(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment) {
        UNUSED_PARAMETER(paddle);
        UNUSED_PARAMETER(beam);
        UNUSED_PARAMETER(beamSegment);
    }

	/**
	 * Event triggered when a part of the boss hits the paddle. Occurs once right after the hit.
	 * Arguments: paddle      - The paddle that got hit.
     *            bossPart    - The part of the boss that hit the paddle.
	 */
    virtual void PaddleHitByBossEvent(const PlayerPaddle& paddle, const BossBodyPart& bossPart) {
        UNUSED_PARAMETER(paddle);
        UNUSED_PARAMETER(bossPart);
    }

	/**
	 * Event triggered when the ball collides with the player paddle. Only occurs once as the ball
	 * collides with the paddle.
	 * Arguments: ball   - The ball as it is colliding with the player paddle.
	 *            paddle - The paddle as it is colliding with the ball.
     *            hitPaddleUnderside - true if the ball hit the underside of the paddle, false if not.
     */
    virtual void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle, bool hitPaddleUnderside) {
        UNUSED_PARAMETER(ball);
        UNUSED_PARAMETER(paddle);
        UNUSED_PARAMETER(hitPaddleUnderside);
    }

	/**
	 * Event triggered when two balls collide. Only occurs once as a ball collides with another.
	 * Arguments: ball1 - A ball in the collision.
	 *             ball2 - The other ball in the collision.
	 */
    virtual void BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2) { 
        UNUSED_PARAMETER(ball1);
        UNUSED_PARAMETER(ball2);
    }

	/**
	 * Event triggered when a ball collides with a part of a boss. Only occurs once as the ball collides.
	 * Arguments: ball - The ball in the collision.
	 *            boss - The boss in the collision.
     *            bossPart - The body part of the boss that was collided with.
	 */
    virtual void BallBossCollisionEvent(GameBall& ball, const Boss& boss, const BossBodyPart& bossPart) {
        UNUSED_PARAMETER(ball);
        UNUSED_PARAMETER(boss);
        UNUSED_PARAMETER(bossPart);
    }

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
	 * Arguments: projectile  - The projectile teleported.
	 *             enterPortal - The portal entered by the projectile.
	 */
    virtual void ProjectilePortalBlockTeleportEvent(const Projectile& projectile, const PortalBlock& enterPortal) {
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(enterPortal);
    }

    /**
	 * Event triggered when a ball enters a cannon block. Only occurs once as the ball is JUST entering.
	 * Arguments: ball        - The ball entering the cannon.
	 *             cannonBlock - The cannon block that the ball is entering.
	 */
    virtual void BallEnteredCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock) {
        UNUSED_PARAMETER(ball);
        UNUSED_PARAMETER(cannonBlock);
    }

	/**
	 * Event triggered when a ball is fired out of a cannon block. Only occurs once as the ball is JUST being
	 * fired out of the barrel of the cannon.
	 * Arguments: ball        - The ball being fired out of the cannon.
	 *             cannonBlock - The cannon block firing the ball.
	 */
    virtual void BallFiredFromCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock) {
        UNUSED_PARAMETER(ball);
        UNUSED_PARAMETER(cannonBlock);
    }

	/**
	 * Event triggered when a projectile enters a cannon block. Only occurs once as the projectile is JUST being
	 * loaded into the barrel of the cannon.
	 * Arguments: projectile  - The projectile being loaded by the cannon.
	 *             cannonBlock - The cannon block loading the rocket. 
	 */
    virtual void ProjectileEnteredCannonEvent(const Projectile& projectile, const CannonBlock& cannonBlock) {
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(cannonBlock);
    }

	/**
	 * Event triggered when a projectile is fired out of a cannon block. Only occurs once as the projectile is JUST being
	 * fired out of the barrel of the cannon.
	 * Arguments: projectile  - The rocket being fired out of the cannon.
	 *             cannonBlock - The cannon block firing the rocket. 
	 */
    virtual void ProjectileFiredFromCannonEvent(const Projectile& projectile, const CannonBlock& cannonBlock) {
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(cannonBlock);
    }

	/**
	 * Event triggered when the ball hits a tesla block to tesla block lightning arc.
	 * Occurs right before the the repel of the ball from the arc takes place.
	 * Arguments: ball         - The ball that hit the lightning arc.
	 */
    virtual void BallHitTeslaLightningArcEvent(const GameBall& ball) {
        UNUSED_PARAMETER(ball);
    }

	/**
	 * Event triggered when the pause state of the GameModel changes. 
     * Occurs right after the change in the pause state.
	 * Arguments: oldPauseState - The previous/old pause state.
	 *            newPauseState - The current/new pause state.
	 */
    virtual void GamePauseStateChangedEvent(int32_t oldPauseState, int32_t newPauseState) {
        UNUSED_PARAMETER(oldPauseState);
        UNUSED_PARAMETER(newPauseState);
    }

    /**
     * Event triggered when a ball loses the fireball ability due to the player acquiring the iceball ability.
	 * Arguments: ball - The ball that lost the fireball ability.
     */
    virtual void FireBallCancelledByIceBallEvent(const GameBall& ball) { UNUSED_PARAMETER(ball); }
    
    /**
     * Event triggered when a ball loses the iceball ability due to the player acquiring the fireball ability.
	 * Arguments: ball - The ball that lost the iceball ability.
     */
    virtual void IceBallCancelledByFireBallEvent(const GameBall& ball) { UNUSED_PARAMETER(ball); }

	// Misc Events (Destruction, Combos, etc.) ************************************************

    /// <summary> Block destroyed event. </summary>
    /// <remarks> Beowulf, 08/10/2013. </remarks>
    /// <param name="block">  The block. </param>
    /// <param name="method"> The method. </param>
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
	 * Arguments: ball - The ball that collided and destroyed the safety net.
	 */
    virtual void BallSafetyNetDestroyedEvent(const GameBall& ball) { UNUSED_PARAMETER(ball); }
	// Same as above, only the paddle did it.
    virtual void BallSafetyNetDestroyedEvent(const PlayerPaddle& paddle) { UNUSED_PARAMETER(paddle); }
	// Same as above, only a projectile did it.
    virtual void BallSafetyNetDestroyedEvent(const Projectile& projectile) { UNUSED_PARAMETER(projectile); }

    /**
     * Event triggered when the bullet time state has changed. Triggered right after the state changed.
     * Arguments: boostModel - The ball boost model that contains the new bullet time state.
     */
    virtual void BulletTimeStateChangedEvent(const BallBoostModel& boostModel) { UNUSED_PARAMETER(boostModel); }
 
    /**
     * Event triggered when the ball is boosted. The event is triggered once right after the
     * boost has been applied to the ball.
     * Arguments: boostModel - The ball boost model.
     */
    virtual void BallBoostExecutedEvent(const BallBoostModel& boostModel) { UNUSED_PARAMETER(boostModel); }


    /**
     * Events triggered when a ball boost is gained/lost, respectively. The events are triggered
     * right after the number of available boosts is incremented/decremented.
     */
    virtual void BallBoostGainedEvent() {}
    // Args: allBoostsLost - true if all boosts were lost
    virtual void BallBoostLostEvent(bool allBoostsLost) { UNUSED_PARAMETER(allBoostsLost); }
    virtual void BallBoostUsedEvent() {}
    virtual void BoostFailedDueToNoBallsAvailableEvent() {}

    /** 
     * Event triggered when the ball camera has been set or unset (this is after the full animation into or out of
     * the ball has been completed, respectively).
     * Arguments: ball  - The ball that has or had the camera in it.
     *            isSet - true if ball camera mode was just fully activated, false if not.
     */
    virtual void BallCameraSetOrUnsetEvent(const GameBall& ball, bool isSet) {
        UNUSED_PARAMETER(ball);
        UNUSED_PARAMETER(isSet);
    }

    /** 
     * Event triggered whenever the player controls the rotation of a cannon block, is triggered
     * only once the player has applied some rotation, not when the rotation is set to zero.
     * Arguments: ball   - The ball that is inside the cannon.
     *            cannon - The cannon block being rotated.
     */
    virtual void BallCameraCannonRotationEvent(const GameBall& ball, const CannonBlock& cannon) {
        UNUSED_PARAMETER(ball);
        UNUSED_PARAMETER(cannon);
    }

	/**
	 * Event triggered when a level piece / block changes from one type to another either within the same
	 * object or to a different object. Only occurs once as the piece changes - this may be triggered along side
	 * other similar events e.g., BlockDestroyedEvent, BallBlockCollisionEvent.
	 * Arguments: pieceBefore - The LevelPiece object before the change.
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
	 * Arguments: piece       - The level piece that had the status added to it.
	 *             addedStatus - The status added to the piece. 
	 */
    virtual void LevelPieceStatusAddedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus) {
        UNUSED_PARAMETER(piece);
        UNUSED_PARAMETER(addedStatus);
    }

	/**
	 * Event triggered when a level piece has a status effect removed from it. Only occurs once, just after the status is removed,
	 * for a particular status effect. A LevelPieceStatusAdded event for that same status is gaurenteed to have been made before this event.
	 * Arguments: piece         - The level piece that had the status removed from it.
	 *             removedStatus - The status removed from the piece. 
	 */
    virtual void LevelPieceStatusRemovedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus) {
        UNUSED_PARAMETER(piece);
        UNUSED_PARAMETER(removedStatus);
    }

	/**
	 * Event triggered when a level piece has all status effects removed from it. Only occurs once just after all status is removed.
	 * Arguments: piece - The level piece that had all the status removed from it.
	 */
    virtual void LevelPieceAllStatusRemovedEvent(const LevelPiece& piece) { UNUSED_PARAMETER(piece); }

    
    virtual void CollateralBlockChangedStateEvent(const CollateralBlock& collateralBlock,
                                                  const CollateralBlockProjectile& projectile,
                                                  CollateralBlock::CollateralBlockState oldState, 
                                                  CollateralBlock::CollateralBlockState newState) { 
        UNUSED_PARAMETER(collateralBlock);
        UNUSED_PARAMETER(projectile);
        UNUSED_PARAMETER(oldState);
        UNUSED_PARAMETER(newState);
    }

	/**
	 * Event triggered when an item is spawned and becomes part of the game.
	 * Only occurs once per spawn.
	 * Arguments: item - The newly spawned item.
	 */
    virtual void ItemSpawnedEvent(const GameItem& item) { UNUSED_PARAMETER(item); }

	/**
	 * Event triggered when an item drop is removed because it has either been
	 * acquired by the player paddle or has left the game boundries.
	 * Arguments: item - The removed item.
	 */
    virtual void ItemRemovedEvent(const GameItem& item) { UNUSED_PARAMETER(item); }

	/**
	 * Event triggered when an item hits the player paddle (i.e., the player gains the item).
	 * Only occurs once at the happenstance of collision.
	 * Arguments: item   - The item that has just been 'consumed' by the player.
	 *             paddle - The paddle as it collides with the item.
	 */
    virtual void ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle) {
        UNUSED_PARAMETER(item);
        UNUSED_PARAMETER(paddle);
    }

	/**
	 * Event triggered when an item is activated (begins its timer or just starts its effect).
	 * Only occurs once per item.
	 * Arguments: item - The item that was just activated.
	 */
    virtual void ItemActivatedEvent(const GameItem& item) { UNUSED_PARAMETER(item); }

	/**
	 * Event triggered when an item is deactivated (ends its timer or is just ends its effect).
	 * Only occurs once per item.
	 * Arguments: item - The item that was just deactivated.
	 */
    virtual void ItemDeactivatedEvent(const GameItem& item)	{ UNUSED_PARAMETER(item); }
	

	/** 
	 * Event triggered when a random item is activated - this is distinct from a typical item since a
	 * random item alone has no effect (what it becomes does). This event is triggered just before the random
     * item's actual item is activated.
	 * Arguments: randomItem - The random item.
     *             actualItem - The actual item that the random item turns into.
	 */
    virtual void RandomItemActivatedEvent(const RandomItem& randomItem, const GameItem& actualItem) {
        UNUSED_PARAMETER(randomItem);
        UNUSED_PARAMETER(actualItem);
    }

	/**
	 * Event triggered when an item's timer is started for the first time. Only occurs
	 * once at item activation, when that item has an associated time period of activity.
	 * Arguments: itemTimer - The item timer that just started.
	 */
    virtual void ItemTimerStartedEvent(const GameItemTimer& itemTimer) { UNUSED_PARAMETER(itemTimer); }

	/**
	 * Event triggered when an item's timer stops. Only occurs once when an item runs
	 * out of its active time and expires.
	 * Arguments: itemTimer - The item timer that just stopped/expired.
     *            didExpire - Whether the timer expired (true) or whether it was forced to stop (false)
	 */
    virtual void ItemTimerStoppedEvent(const GameItemTimer& itemTimer, bool didExpire) { 
        UNUSED_PARAMETER(itemTimer); 
        UNUSED_PARAMETER(didExpire);
    }

	/**
	 * Event triggered when an item drop block changes the item type that it will drop next. Occurs
	 * once right after the item drop block changes the type of item it will drop.
	 * Arguments: dropBlock - the drop block that changed.
	 */
    virtual void ItemDropBlockItemChangeEvent(const ItemDropBlock& dropBlock) { UNUSED_PARAMETER(dropBlock); }

    /**
     * Event triggered when a switch block is initially activated. Occurs once just after the switch
     * activates its triggers.
     * Arguments: switchBlock - the switch block that was activated.
     */
    virtual void SwitchBlockActivatedEvent(const SwitchBlock& switchBlock) { UNUSED_PARAMETER(switchBlock); }
        
	/**
	 * Event triggered when a projectile is spawned. Only occurs once per spawned projectile.
	 * Arguments: projectile - The projectile that was just spawned.
	 */
    virtual void ProjectileSpawnedEvent(const Projectile& projectile) { UNUSED_PARAMETER(projectile); }

	/**
	 * Event triggered when a projectile is removed. Only occurs once per removed projectile.
	 * Arguments: projectile - The projectile that was just removed.
	 */
    virtual void ProjectileRemovedEvent(const Projectile& projectile) { UNUSED_PARAMETER(projectile); }

	/**
	 * Event triggered when a rocket explodes. Only occurs once just as it explodes.
	 * Arguments: rocket - The rocket do'n the explod'n.
	 */
    virtual void RocketExplodedEvent(const RocketProjectile& rocket) { UNUSED_PARAMETER(rocket); } 

	/**
	 * Event triggered when a mine explodes. Only occurs once just as it explodes.
	 * Arguments: mine - The mine that is exploding.
	 */
    virtual void MineExplodedEvent(const MineProjectile& mine) { UNUSED_PARAMETER(mine); }

    /**
     * Event triggered when a mine lands on something but doesn't explode.
     * Arguments: mine - The mine that just landed on something.
     */
    virtual void MineLandedEvent(const MineProjectile& mine) { UNUSED_PARAMETER(mine); }

	/**
	 * Event triggered when a remote controlled rocket is running out of fuel. This occurs multiple times and happens more frequently
     * as the rocket gets closer to running out of fuel.
	 * Arguments: rocket - The rocket emitting the warning.
	 */
    virtual void RemoteControlRocketFuelWarningEvent(const PaddleRemoteControlRocketProjectile& rocket) {
        UNUSED_PARAMETER(rocket);
    }

	/**
	 * Event triggered when a remote controlled rocket has it's thrust applied. This occurs once every time the
     * thrust is turned on.
	 * Arguments: rocket - The rocket that is thrusting.
	 */
    virtual void RemoteControlRocketThrustAppliedEvent(const PaddleRemoteControlRocketProjectile& rocket) {
        UNUSED_PARAMETER(rocket);
    }

	/**
	 * Event triggered when a new beam is spawned. Only occurs once per spawned beam.
	 * Arguments: beam - the beam that was just spawned.
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
	 * Event triggered when a Tesla block is activated and one of its neighbor/connecting Tesla blocks is
	 * already active - thus spawning a lightning arc between the two.
	 * Arguments: newlyOnTeslaBlock      - The Tesla block that just got turned on and now has lightning arc shooting out of it
	 *            previouslyOnTeslaBlock - The previously active connecting Tesla block that now has a lightning arc between itself and 'newlyOnTeslaBlock'
	 */
    virtual void TeslaLightningBarrierSpawnedEvent(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock) {
        UNUSED_PARAMETER(newlyOnTeslaBlock);
        UNUSED_PARAMETER(previouslyOnTeslaBlock);
    }

	/**
	 * Event triggered when a Tesla block is deactivated and an arc of lightning drops between it and one of its still
	 * active neighbors.
	 * Arguments: newlyOffTeslaBlock - The Tesla block that was previously turned on but is now off.
	 *            stillOnTeslaBlock  - The neighbor Tesla block that previously had an arc between itself and 'newlyOffTeslaBlock'
	 */
    virtual void TeslaLightningBarrierRemovedEvent(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock) {
        UNUSED_PARAMETER(newlyOffTeslaBlock);
        UNUSED_PARAMETER(stillOnTeslaBlock);
    }
	
	/**
	 * Event triggered whenever the number of player lives changes. Only occurs once per life or
	 * set of lives are added or removed.
	 * Arguments: livesLeftBefore - Number of lives left before the change.
	 *             livesLeftAfter  - Number of lives left after the change.
	 */
    virtual void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter) {
        UNUSED_PARAMETER(livesLeftBefore);
        UNUSED_PARAMETER(livesLeftAfter);
    }

	/**
	 * Event triggered when the ice on a block is shattered. Only occurs once just before the ice shatters.
	 * Arguments: block - The block whose ice shattered.
	 */
    virtual void BlockIceShatteredEvent(const LevelPiece& block) { UNUSED_PARAMETER(block); }

    /**
     * Event triggered when a frozen block is canceled-out by something fiery hitting it. Occurs just after
     * the ice status is removed.
	 * Arguments: block - The block whose ice was canceled-out.
     */
    virtual void BlockIceCancelledWithFireEvent(const LevelPiece& block) { UNUSED_PARAMETER(block); }

    /**
     * Event triggered when a block on fire is canceled-out by something icy hitting it. Occurs just after
     * the fire status is removed.
	 * Arguments: block - The block whose fire was canceled-out.
     */
    virtual void BlockFireCancelledWithIceEvent(const LevelPiece& block) { UNUSED_PARAMETER(block); }

    /**
     * Event triggered when the release timer starts ticking while the ball is on the paddle. Only
     * occurs once just as the timer starts.
     */
    virtual void ReleaseTimerStartedEvent() {}

    // Point related events ****************************************************************

    /**
     * Event triggered whenever the player earns a fixed point amount, occurs right after the point
     * amount is added to the score.
     * Arguments: pointAward - information about the point notification.
     */
    virtual void PointNotificationEvent(const PointAward& pointAward) { UNUSED_PARAMETER(pointAward); }

	/**
	 * Event triggered when there is a change to the player's score. Only occurs once per change.
	 * Arguments: newScore - The new score after the change.
	 */
    virtual void ScoreChangedEvent(int newScore) { UNUSED_PARAMETER(newScore); }


    /**
     * Event triggered whenever a block is destroyed or something happens that causes the multiplier
     * counter to increase or decrease - the counter is what causes the multiplier to change.
     * Arguments: oldCounterValue - The previous/old counter value (0 to 9).
     *             newCounterValue - The new counter value (0 to 9).
     */
    virtual void ScoreMultiplierCounterChangedEvent(int oldCounterValue, int newCounterValue) {
        UNUSED_PARAMETER(oldCounterValue);
        UNUSED_PARAMETER(newCounterValue);
    }

	/**
	 * Event triggered when there is a change to the player's score multiplier. 
	 * Only occurs once per change.
	 * Arguments: oldMultiplier - The multiplier before the change.
     *             newMultiplier - The multiplier after the change.
     *             position      - The position in the game where the event occurred that changed the multiplier.
	 */
    virtual void ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier, const Point2D& position) {
        UNUSED_PARAMETER(oldMultiplier);
        UNUSED_PARAMETER(newMultiplier);
        UNUSED_PARAMETER(position);
    }

    /**
     * Event triggered when the number of stars awarded to the player has changed
     * Occurs once per change, right after the change occurs.
     * Arguments: pointAward  - The point award associated with the star amount change, NULL if the number of stars is being reset.
     *             oldNumStars - The previous number of stars.
     *             newNumStars - The new number of stars.
     */
    virtual void NumStarsChangedEvent(const PointAward* pointAward, int oldNumStars, int newNumStars) {
        UNUSED_PARAMETER(pointAward);
        UNUSED_PARAMETER(oldNumStars);
        UNUSED_PARAMETER(newNumStars);
    }

    /**
     * Event triggered once right after the difficulty is changed in the gamemodel.
     * Arguments: newDifficulty - The new difficulty level of the game.
     */
    virtual void DifficultyChangedEvent(const GameModel::Difficulty& newDifficulty) {
        UNUSED_PARAMETER(newDifficulty);
    }

    // AI Events ***********************************************************

    /**
     * Event triggered when the laser turret AI state of a particular block changes.
     * Only occurs once per change.
     * Arguments: block - The block with the changed state.
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
     * Arguments: block - The block that fired the laser.
     */
    virtual void LaserFiredByTurretEvent(const LaserTurretBlock& block) {
        UNUSED_PARAMETER(block);
    }

    /**
     * Event triggered when the rocket turret AI state of a particular block changes.
     * Only occurs once per change.
     * Arguments: block - The block with the changed state.
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
     * Arguments: block - the rocket turret block that fired.
     */
    virtual void RocketFiredByTurretEvent(const RocketTurretBlock& block) {
        UNUSED_PARAMETER(block);
    }

    /**
     * Event triggered when the mine turret AI state of a particular block changes.
     * Only occurs once per change.
     * Arguments: block - The block with the changed state.
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
     * Arguments: block - the mine turret block that fired.
     */
    virtual void MineFiredByTurretEvent(const MineTurretBlock& block) {
        UNUSED_PARAMETER(block);
    }

    /**
     * Event triggered when a regen block's life changes. Event is triggered immediately after the change.
     * Arguments: block - The regen block that changed.
     *             lifePercentBefore - The life the block had before its life percent changed.
     */
    virtual void RegenBlockLifeChangedEvent(const RegenBlock& block, float lifePercentBefore) {
        UNUSED_PARAMETER(block);
        UNUSED_PARAMETER(lifePercentBefore);
    }

    /** 
     * Signals that a regen block was preturbed (hit by something that could have done damage, or affected in some negative way.
     * Arguments: block - The regen block that was preturbed.
     */
    virtual void RegenBlockPreturbedEvent(const RegenBlock& block) {
        UNUSED_PARAMETER(block);
    }

    // Boss Events ****************************************************************

    /**
     * Event triggered once right after a boss weakpoint is hurt.
     * Arguments: hurtPart - The part that was hurt.
     */
    virtual void BossHurtEvent(const BossWeakpoint* hurtPart) {
        UNUSED_PARAMETER(hurtPart);
    }

    /**
     * Event triggered when the boss gets really angry. Happens once right when the boss is angry.
     * Arguments: boss - The boss that got angry.
     *             angryPart - The part of the boss that thinks / is associated with being angry.
     */
    virtual void BossAngryEvent(const Boss* boss, const BossBodyPart* angryPart) {
        UNUSED_PARAMETER(boss);
        UNUSED_PARAMETER(angryPart);
    }
    virtual void BossAngryEvent(const Boss* boss, const Point2D& angryPartLoc, float angryPartWidth, float angryPartHeight) {
        UNUSED_PARAMETER(boss);
        UNUSED_PARAMETER(angryPartLoc);
        UNUSED_PARAMETER(angryPartWidth);
        UNUSED_PARAMETER(angryPartHeight);
    }

    // Effect Events *********************************************************************
    
    virtual void GeneralEffectEvent(const GeneralEffectEventInfo& effectEvent) {
        UNUSED_PARAMETER(effectEvent);
    }

    virtual void BossEffectEvent(const BossEffectEventInfo& effectEvent) {
        UNUSED_PARAMETER(effectEvent);
    }
};


#endif
