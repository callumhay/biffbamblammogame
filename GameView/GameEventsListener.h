/**
 * GameEventsListener.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEEVENTSLISTENER_H__
#define __GAMEEVENTSLISTENER_H__

#include "../GameModel/GameEvents.h"
#include "../GameControl/BBBGameController.h"
#include "../GameSound/GameSoundAssets.h"

class GameDisplay;

class GameEventsListener : public GameEvents {
public:
	GameEventsListener(GameDisplay* d);
	virtual ~GameEventsListener();

	// Global / Game events
	void GameCompletedEvent();
	void WorldStartedEvent(const GameWorld& world);
	void WorldCompletedEvent(const GameWorld& world);
	void LevelStartedEvent(const GameWorld& world, const GameLevel& level);
	void LevelCompletedEvent(const GameWorld& world, const GameLevel& level);

	// Paddle related events
	void PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc);
	void PaddleHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile);
	void PaddleShieldHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile);
	void ProjectileDeflectedByPaddleShieldEvent(const Projectile& projectile, const PlayerPaddle& paddle);

	// Ball/Block related events
	void BallDiedEvent(const GameBall& deadBall);
	void LastBallAboutToDieEvent(const GameBall& lastBallToDie);
	void LastBallExploded(const GameBall& explodedBall);
	void AllBallsDeadEvent(int livesLeft);
	void BallSpawnEvent(const GameBall& spawnedBall);
	void BallShotEvent(const GameBall& shotBall);
	void ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block);
	void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block);
	void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle);
	void BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2);
	void BallPortalBlockTeleportEvent(const GameBall& ball, const PortalBlock& enterPortal);
	void ProjectilePortalBlockTeleportEvent(const Projectile& projectile, const PortalBlock& enterPortal);
	void BallFiredFromCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock);
	void RocketEnteredCannonEvent(const PaddleRocketProjectile& rocket, const CannonBlock& cannonBlock);
	void RocketFiredFromCannonEvent(const PaddleRocketProjectile& rocket, const CannonBlock& cannonBlock);
	void BallHitTeslaLightningArcEvent(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2);

	// Misc game events
	void BlockDestroyedEvent(const LevelPiece& block);
	void BallSafetyNetCreatedEvent();
	void BallSafetyNetDestroyedEvent(const GameBall& ball);
	void BallSafetyNetDestroyedEvent(const PlayerPaddle& paddle);
	void BallSafetyNetDestroyedEvent(const Projectile& projectile);
	void LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void LevelPieceStatusAddedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus);
	void LevelPieceStatusRemovedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus);
	void LevelPieceAllStatusRemovedEvent(const LevelPiece& piece);
	void ItemSpawnedEvent(const GameItem& item);
	void ItemRemovedEvent(const GameItem& item);
	void ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle);
	void ItemActivatedEvent(const GameItem& item);
	void ItemDeactivatedEvent(const GameItem& item);
	void RandomItemActivatedEvent(const RandomItem& randomItem, const GameItem& actualItem);
	void ItemTimerStartedEvent(const GameItemTimer& itemTimer);
	void ItemTimerStoppedEvent(const GameItemTimer& itemTimer);
	void ItemDropBlockItemChangeEvent(const ItemDropBlock& dropBlock);
    void SwitchBlockActivatedEvent(const SwitchBlock& switchBlock);
    void BulletTimeStateChangedEvent(const BallBoostModel& boostModel);
    void BallBoostExecutedEvent(const BallBoostModel& boostModel);
    void BallBoostGainedEvent();
    void BallBoostLostEvent();

	void ProjectileSpawnedEvent(const Projectile& projectile);
	void ProjectileRemovedEvent(const Projectile& projectile);
	void RocketExplodedEvent(const PaddleRocketProjectile& rocket);
	void BeamSpawnedEvent(const Beam& beam);
	void BeamChangedEvent(const Beam& beam);
	void BeamRemovedEvent(const Beam& beam);
	void TeslaLightningBarrierSpawnedEvent(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock);
	void TeslaLightningBarrierRemovedEvent(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock);
	void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter);

	void BlockIceShatteredEvent(const LevelPiece& block);

	void ScoreChangedEvent(int newScore);
    void ScoreMultiplierCounterChangedEvent(int newCounterValue);
	void ScoreMultiplierChangedEvent(int newMultiplier, const Point2D& position);
    void PointNotificationEvent(const PointAward& pointAward);

private:
	GameDisplay* display;
	long timeSinceLastBallBlockCollisionEventInMS;
	long timeSinceLastBallPaddleCollisionEventInMS;
	long timeSinceLastBallTeslaCollisionEventInMS;

	static const long EFFECT_WAIT_TIME_BETWEEN_BALL_BLOCK_COLLISIONS_IN_MS;
	static const long EFFECT_WAIT_TIME_BETWEEN_BALL_PADDLE_COLLISIONS_IN_MS;
	static const long EFFECT_WAIT_TIME_BETWEEN_BALL_TESLA_COLLISIONS_IN_MS;

	void DestroyBallSafetyNet(const Point2D& pt);

	static void GetVolAndShakeForBallSize(const GameBall::BallSize& ballSize, float& shakeMagnitude,
																				float& shakeLength, GameSoundAssets::SoundVolumeLoudness& volume,
																				BBBGameController::VibrateAmount& controllerVibeAmt);
};

#endif