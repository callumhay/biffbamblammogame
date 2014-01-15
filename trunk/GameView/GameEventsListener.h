/**
 * GameEventsListener.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMEEVENTSLISTENER_H__
#define __GAMEEVENTSLISTENER_H__

#include "../GameModel/GameEvents.h"
#include "../GameControl/BBBGameController.h"
#include "../GameSound/GameSound.h"

class GameDisplay;
class BossWeakpoint;

class GameEventsListener : public GameEvents {
public:
	GameEventsListener(GameDisplay* d);
	~GameEventsListener();

	// Global / Game events
	void GameCompletedEvent();
	void WorldStartedEvent(const GameWorld& world);
	void WorldCompletedEvent(const GameWorld& world);
	void LevelStartedEvent(const GameWorld& world, const GameLevel& level);
    void LevelAlmostCompleteEvent(const GameLevel& level);
	void LevelCompletedEvent(const GameWorld& world, const GameLevel& level,
        int furthestLevelIdxBefore, int furthestLevelIdxAfter);

	// Paddle related events
	void PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc);
	void PaddleHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile);
	void PaddleShieldHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile);
	void ProjectileDeflectedByPaddleShieldEvent(const Projectile& projectile, const PlayerPaddle& paddle);
    void PaddleHitByBeamEvent(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
    void PaddleShieldHitByBeamEvent(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
    void PaddleHitByBossEvent(const PlayerPaddle& paddle, const BossBodyPart& bossPart);

	// Ball/Block related events
	void BallDiedEvent(const GameBall& deadBall);
	void LastBallAboutToDieEvent(const GameBall& lastBallToDie);
	void LastBallExplodedEvent(const GameBall& explodedBall, bool wasSkipped);
	void AllBallsDeadEvent(int livesLeft);
	void BallSpawnEvent(const GameBall& spawnedBall);
	void BallShotEvent(const GameBall& shotBall);
	void ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block);
    void ProjectileSafetyNetCollisionEvent(const Projectile& projectile, const SafetyNet& safetyNet);
    void ProjectileBossCollisionEvent(const Projectile& projectile, const Boss& boss, const BossBodyPart& collisionPart);
	void BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block);
	void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle, bool hitPaddleUnderside);
    void BallBossCollisionEvent(GameBall& ball, const Boss& boss, const BossBodyPart& bossPart);
	void BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2);
	void BallPortalBlockTeleportEvent(const GameBall& ball, const PortalBlock& enterPortal);
	void ProjectilePortalBlockTeleportEvent(const Projectile& projectile, const PortalBlock& enterPortal);
	void BallEnteredCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock, bool canShootWithoutObstruction);
    void BallFiredFromCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock);
	void ProjectileEnteredCannonEvent(const Projectile& projectile, const CannonBlock& cannonBlock);
	void ProjectileFiredFromCannonEvent(const Projectile& projectile, const CannonBlock& cannonBlock);
	void GamePauseStateChangedEvent(int32_t oldPauseState, int32_t newPauseState);
    void BallHitTeslaLightningArcEvent(const GameBall& ball);
    void FireBallCancelledByIceBallEvent(const GameBall& ball);
    void IceBallCancelledByFireBallEvent(const GameBall& ball);

	// Misc game events
	void BlockDestroyedEvent(const LevelPiece& block, const LevelPiece::DestructionMethod& method);
	void BallSafetyNetCreatedEvent();
	void BallSafetyNetDestroyedEvent(const GameBall& ball);
	void BallSafetyNetDestroyedEvent(const PlayerPaddle& paddle);
	void BallSafetyNetDestroyedEvent(const Projectile& projectile);
	void LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void LevelPieceStatusAddedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus);
	void LevelPieceStatusRemovedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus);
	void LevelPieceAllStatusRemovedEvent(const LevelPiece& piece);
    void CollateralBlockChangedStateEvent(const CollateralBlock& collateralBlock, const CollateralBlockProjectile& projectile,
        CollateralBlock::CollateralBlockState oldState, CollateralBlock::CollateralBlockState newState);
	void ItemSpawnedEvent(const GameItem& item);
	void ItemRemovedEvent(const GameItem& item);
	void ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle);
	void ItemActivatedEvent(const GameItem& item);
	void ItemDeactivatedEvent(const GameItem& item);
	void RandomItemActivatedEvent(const RandomItem& randomItem, const GameItem& actualItem);
	void ItemTimerStartedEvent(const GameItemTimer& itemTimer);
	void ItemTimerStoppedEvent(const GameItemTimer& itemTimer, bool didExpire);
	void ItemDropBlockItemChangeEvent(const ItemDropBlock& dropBlock);
    void SwitchBlockActivatedEvent(const SwitchBlock& switchBlock);
    void BulletTimeStateChangedEvent(const BallBoostModel& boostModel);
    void BallBoostExecutedEvent(const BallBoostModel& boostModel);
    void BallBoostGainedEvent();
    void BallBoostLostEvent(bool allBoostsLost);
    void BoostFailedDueToNoBallsAvailableEvent();

    void BallCameraSetOrUnsetEvent(const GameBall& ball, bool isSet, bool canShootWithoutObstruction);
    void BallCameraCannonRotationEvent(const GameBall& ball, const CannonBlock& cannon, bool canShootWithoutObstruction);
    void CantFireBallCamFromCannonEvent();

	void ProjectileSpawnedEvent(const Projectile& projectile);
	void ProjectileRemovedEvent(const Projectile& projectile);
	void RocketExplodedEvent(const RocketProjectile& rocket);
    void MineExplodedEvent(const MineProjectile& mine);
    void MineLandedEvent(const MineProjectile& mine);
    void RemoteControlRocketFuelWarningEvent(const PaddleRemoteControlRocketProjectile& rocket);
    void RemoteControlRocketThrustAppliedEvent(const PaddleRemoteControlRocketProjectile& rocket);

	void BeamSpawnedEvent(const Beam& beam);
	void BeamChangedEvent(const Beam& beam);
	void BeamRemovedEvent(const Beam& beam);
	void TeslaLightningBarrierSpawnedEvent(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock);
	void TeslaLightningBarrierRemovedEvent(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock);
	void LivesChangedEvent(int livesLeftBefore, int livesLeftAfter);

	void BlockIceShatteredEvent(const LevelPiece& block);
    void BlockIceCancelledWithFireEvent(const LevelPiece& block);
    void BlockFireCancelledWithIceEvent(const LevelPiece& block);

    void ReleaseTimerStartedEvent();

	void ScoreChangedEvent(int newScore);
    void ScoreMultiplierCounterChangedEvent(int oldCounterValue, int newCounterValue);
	void ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier, const Point2D& position);
    void PointNotificationEvent(const PointAward&) {}
    void NumStarsChangedEvent(const PointAward* pointAward, int oldNumStars, int newNumStars);

    void DifficultyChangedEvent(const GameModel::Difficulty& newDifficulty);

    void LaserTurretAIStateChangedEvent(const LaserTurretBlock& block,
        LaserTurretBlock::TurretAIState oldState, LaserTurretBlock::TurretAIState newState);
    void LaserFiredByTurretEvent(const LaserTurretBlock& block);
    void RocketTurretAIStateChangedEvent(const RocketTurretBlock& block,
        RocketTurretBlock::TurretAIState oldState, RocketTurretBlock::TurretAIState newState);
    void RocketFiredByTurretEvent(const RocketTurretBlock& block);
    void MineTurretAIStateChangedEvent(const MineTurretBlock& block,
        MineTurretBlock::TurretAIState oldState, MineTurretBlock::TurretAIState newState);
    void MineFiredByTurretEvent(const MineTurretBlock& block);

    void RegenBlockLifeChangedEvent(const RegenBlock& block, float lifePercentBefore);
    void RegenBlockPreturbedEvent(const RegenBlock& block);

    void BossHurtEvent(const BossWeakpoint* hurtPart);
    void BossAngryEvent(const Boss* boss, const BossBodyPart* angryPart);
    void BossAngryEvent(const Boss* boss, const Point2D& angryPartLoc, float angryPartWidth, float angryPartHeight);

    void BossEffectEvent(const BossEffectEventInfo& effectEvent);
    void GeneralEffectEvent(const GeneralEffectEventInfo& effectEvent);

private:
	GameDisplay* display;
	long timeOfLastBallBlockCollisionEventInMS;
	long timeOfLastBallPaddleCollisionEventInMS;
	long timeOfLastBallTeslaCollisionEventInMS;
    long timeOfLastBallBossCollisionEventInMS;
    long timeOfLastControlledCannonRotationInMS;
    long timeOfLastBoostMalfunctionInMS;
    long timeOfLastCannonObstrMalfuncInMS;
    long timeOfLastCounterEventInMS;
    long timeOfLastMultiplierEventInMS;
    long timeOfLastItemSpawnInMS;

    typedef std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, SoundID> TeslaLightningSoundIDMap;
    typedef TeslaLightningSoundIDMap::iterator TeslaLightningSoundIDMapIter;
    TeslaLightningSoundIDMap teslaLightningSoundIDs;

    typedef std::map<const LevelPiece*, long> BlockTimestampMap;
    typedef BlockTimestampMap::iterator BlockTimestampMapIter;
    typedef std::map<const Projectile*, BlockTimestampMap> ProjectileEffectTimestampMap;
    typedef ProjectileEffectTimestampMap::iterator ProjectileEffectTimestampMapIter;
    ProjectileEffectTimestampMap projectileEffectTSMap;
    
    static const long EFFECT_WAIT_TIME_BETWEEN_PROJECTILE_BLOCK_COLLISIONS_IN_MS;
    static const long EFFECT_WAIT_TIME_BETWEEN_BALL_BOSS_COLLISIONS_IN_MS;
    static const long SOUND_WAIT_TIME_BETWEEN_BALL_BOSS_COLLISIONS_IN_MS;
	static const long EFFECT_WAIT_TIME_BETWEEN_BALL_BLOCK_COLLISIONS_IN_MS;
	static const long EFFECT_WAIT_TIME_BETWEEN_BALL_PADDLE_COLLISIONS_IN_MS;
	static const long EFFECT_WAIT_TIME_BETWEEN_BALL_TESLA_COLLISIONS_IN_MS;
    static const long SOUND_WAIT_TIME_BETWEEN_CONTROLLED_CANNON_ROTATIONS_IN_MS;
    static const long SOUND_WAIT_TIME_BETWEEN_BOOST_MALFUNCTIONS_IN_MS;
    static const long SOUND_WAIT_TIME_BETWEEN_CANNON_OBSTR_MALFUNCS_IN_MS;
    static const long SOUND_DAMPEN_TIME_BETWEEN_BLOCK_COUNTERS_IN_MS;
    static const long SOUND_DAMPEN_TIME_BETWEEN_MULTIPLIERS_IN_MS;
    static const long SOUND_DAMPEN_TIME_BETWEEN_ITEM_SPAWNS_IN_MS;

    static SoundID enterBulletTimeSoundID;
    static SoundID exitBulletTimeSoundID;
    static SoundID inBulletTimeLoopSoundID;

    int numFallingItemsInPlay;
    SoundID fallingItemSoundID;

	void DestroyBallSafetyNet(const Point2D& pt);

	static void GetEffectsForBallSize(const GameBall::BallSize& ballSize, float& shakeMagnitude,
        float& shakeLength, BBBGameController::VibrateAmount& controllerVibeAmt);
};

#endif