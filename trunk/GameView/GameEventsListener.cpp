/**
 * GameEventsListener.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

// GameDisplay Includes
#include "GameEventsListener.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "LevelMesh.h"
#include "LoadingScreen.h"
#include "GameOverDisplayState.h"
#include "GameCompleteDisplayState.h"
#include "LivesLeftHUD.h"
#include "PointsHUD.h"
#include "BallBoostHUD.h"
#include "BallReleaseHUD.h"
#include "BallSafetyNetMesh.h"
#include "BossMesh.h"

// GameModel Includes
#include "../GameModel/GameModel.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/Onomatoplex.h"
#include "../GameModel/Projectile.h"
#include "../GameModel/Beam.h"
#include "../GameModel/RandomItem.h"
#include "../GameModel/TeslaBlock.h"
#include "../GameModel/CannonBlock.h"
#include "../GameModel/RegenBlock.h"
#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/BallBoostModel.h"
#include "../GameModel/BossWeakpoint.h"
#include "../GameModel/PowerChargeEffectInfo.h"
#include "../GameModel/ExpandingHaloEffectInfo.h"
#include "../GameModel/SparkBurstEffectInfo.h"
#include "../GameModel/ElectricitySpasmEffectInfo.h"
#include "../GameModel/PuffOfSmokeEffectInfo.h"
#include "../GameModel/ShockwaveEffectInfo.h"
#include "../GameModel/FullscreenFlashEffectInfo.h"
#include "../GameModel/DebrisEffectInfo.h"
#include "../GameModel/LaserBeamSightsEffectInfo.h"

// GameControl Includes
#include "../GameControl/GameControllerManager.h"

// Base/Global Includes
#include "../Blammopedia.h"

// Wait times before showing the same effect - these prevent the game view from displaying a whole ton
// of the same effect over and over when the ball hits a bunch of blocks/the paddle in a very small time frame
const long GameEventsListener::EFFECT_WAIT_TIME_BETWEEN_BALL_BLOCK_COLLISIONS_IN_MS		= 100;
const long GameEventsListener::EFFECT_WAIT_TIME_BETWEEN_BALL_PADDLE_COLLISIONS_IN_MS    = 125;
const long GameEventsListener::EFFECT_WAIT_TIME_BETWEEN_BALL_TESLA_COLLISIONS_IN_MS     = 60;

SoundID GameEventsListener::enterBulletTimeSoundID  = INVALID_SOUND_ID;
SoundID GameEventsListener::exitBulletTimeSoundID   = INVALID_SOUND_ID;
SoundID GameEventsListener::inBulletTimeLoopSoundID = INVALID_SOUND_ID;

GameEventsListener::GameEventsListener(GameDisplay* d) : 
display(d), 
timeSinceLastBallBlockCollisionEventInMS(0),
timeSinceLastBallPaddleCollisionEventInMS(0),
timeSinceLastBallTeslaCollisionEventInMS(0) {
	assert(d != NULL);
}

GameEventsListener::~GameEventsListener() {
}

void GameEventsListener::GameCompletedEvent() {
	// Queue the game complete state
    this->display->AddStateToQueue(DisplayState::GameComplete);

    debug_output("EVENT: Game completed");
}

void GameEventsListener::WorldStartedEvent(const GameWorld& world) {

    // Load the new movement/world's assets...
    const Camera& camera = this->display->GetCamera();
	LoadingScreen::GetInstance()->StartShowLoadingScreen(camera.GetWindowWidth(), camera.GetWindowHeight(), 3);
	this->display->GetAssets()->LoadWorldAssets(world);
    LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading movement sounds...");
    this->display->GetSound()->LoadWorldSounds(world.GetStyle());
	LoadingScreen::GetInstance()->EndShowingLoadingScreen();

    debug_output("EVENT: Movement/World started");
}

void GameEventsListener::WorldCompletedEvent(const GameWorld& world) {
	UNUSED_PARAMETER(world);

	debug_output("EVENT: Movement/World completed");
}

void GameEventsListener::LevelStartedEvent(const GameWorld& world, const GameLevel& level) {
	UNUSED_PARAMETER(world);

    this->teslaLightningSoundIDs.clear();

	// Load the level geometry/mesh data for display...
	this->display->GetAssets()->LoadNewLevelMesh(level);
    this->display->GetAssets()->ReinitializeAssets();

    // Any new blocks that are being revealed in this level should affect the blammopedia...
    //const std::vector<std::vector<LevelPiece*> >& levelPieces = level.GetCurrentLevelLayout();

	// Queue up the state for starting a level - this will display the level name and do proper animations, fade-ins, etc.
	this->display->AddStateToQueue(DisplayState::LevelStart);

    // Queue the tutorial in-game state if this is the first world and first level of the game,
    // queue the boss in-game state if the level is a boss level, otherwise it's just a typical level and
    // we queue the regular in-game display state
    GameModel* model = this->display->GetModel();
    if (model->IsCurrentLevelTheTutorialLevel()) {
        // Cache the difficulty and change it to medium/normal for the tutorial level
        this->display->SetCachedDifficulty(model->GetDifficulty());
        model->SetDifficulty(GameModel::MediumDifficulty);
        this->display->AddStateToQueue(DisplayState::InTutorialGame);
    }
    else if (model->IsCurrentLevelABossLevel()) {
        this->display->AddStateToQueue(DisplayState::InGameBossLevel);
    }
    else {
        // Regular in-game level state
	    this->display->AddStateToQueue(DisplayState::InGame);
    }

    // Update the minimum 3D sound distance based on how far the camera is away from the current level
    // TODO this->display->GetSound()->SetMin3DSoundDistance(model->GetTransformModel()->GetCameraTranslation()[2]);

    debug_output("EVENT: Level started");
}

void GameEventsListener::LevelAlmostCompleteEvent(const GameLevel& level) {
	UNUSED_PARAMETER(level);

    this->display->GetAssets()->GetCurrentLevelMesh()->LevelIsAlmostComplete();

	debug_output("EVENT: Level almost complete");
}

void GameEventsListener::LevelCompletedEvent(const GameWorld& world, const GameLevel& level) {
	UNUSED_PARAMETER(world);
	UNUSED_PARAMETER(level);

	// Queue up the state for ending a level - this will display the level name and do
    // proper animations, fade-outs, etc. In the case of a boss level being completed, we
    // display a "movement complete" state instead.
    if (level.GetHasBoss()) {
        // For now, every boss level should be the last level in a world
        assert(world.GetLastLevelIndex() == static_cast<int>(level.GetLevelIndex()));
        this->display->AddStateToQueue(DisplayState::BossLevelCompleteSummary);

        // TODO: Bring the player to the world select screen and unlock the latest world.. unless this was the last level,
        // in which case we go the state for the end of the game...
        //this->display->AddStateToQueue(DisplayState::WorldUnlock);

        // If it's not the last world, then we add a world start screen after the boss level complete summary
        if (this->display->GetModel()->GetLastWorldIndex() != world.GetWorldIndex()) {
            this->display->AddStateToQueue(DisplayState::WorldStart);
        }
    }
    else {
        this->display->AddStateToQueue(DisplayState::LevelEnd);
	    this->display->AddStateToQueue(DisplayState::LevelCompleteSummary);
    }

	this->display->SetCurrentStateAsNextQueuedState();

	debug_output("EVENT: Level completed");
}


void GameEventsListener::PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc) {
	std::string soundText = Onomatoplex::Generator::GetInstance()->Generate(Onomatoplex::BOUNCE, Onomatoplex::NORMAL);

	// Do a camera shake based on the size of the paddle...
	BBBGameController::VibrateAmount controllerVibeAmt = BBBGameController::NoVibration;
	PlayerPaddle::PaddleSize paddleSize = paddle.GetPaddleSize();
	float shakeMagnitude = 0.0f;
	float shakeLength = 0.0f;

	switch(paddleSize) {

		case PlayerPaddle::SmallestSize:
			shakeMagnitude = 0.025f;
			shakeLength = 0.025f;
			controllerVibeAmt = BBBGameController::VerySoftVibration;
			break;

		case PlayerPaddle::SmallerSize:
			shakeMagnitude = 0.05f;
			shakeLength = 0.05f;
			controllerVibeAmt = BBBGameController::SoftVibration;
			break;

		case PlayerPaddle::NormalSize:
			shakeMagnitude = 0.1f;
			shakeLength = 0.10f;
			controllerVibeAmt = BBBGameController::SoftVibration;
			break;

		case PlayerPaddle::BiggerSize:
			shakeMagnitude = 0.25f;
			shakeLength = 0.125f;
            controllerVibeAmt = BBBGameController::MediumVibration;
			break;

		case PlayerPaddle::BiggestSize:
			shakeMagnitude = 0.35f;
			shakeLength = 0.15f;
			controllerVibeAmt = BBBGameController::HeavyVibration;
			break;
		default:
			assert(false);
			break;
	}
	
	// Make the camera shake...
	this->display->GetCamera().SetCameraShake(shakeLength, Vector3D(shakeMagnitude, 0.0, 0.0), 20);
	
	// Make the controller shake (if it can)...
	BBBGameController::VibrateAmount leftVibeAmt = BBBGameController::VerySoftVibration;
	BBBGameController::VibrateAmount rightVibeAmt = BBBGameController::VerySoftVibration;
	if (hitLoc[0] > 0) {
		// On the right side...
		rightVibeAmt = controllerVibeAmt;
	}
	else {
		// On the left side...
		leftVibeAmt = controllerVibeAmt;
	}
	GameControllerManager::GetInstance()->VibrateControllers(shakeLength, leftVibeAmt, rightVibeAmt);

	// Add a smacking type effect...
	this->display->GetAssets()->GetESPAssets()->AddPaddleHitWallEffect(paddle, hitLoc);

	// Add the sound for the smacking against the wall
	this->display->GetSound()->PlaySoundAtPosition(GameSound::PaddleHitWallEvent, false, Point3D(hitLoc, 0.0f));

	debug_output("EVENT: Paddle hit wall - " << soundText);
}

void GameEventsListener::PaddleHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile) {
	
    // No hurting effects if paddle has sticky effect and the projectile is a mine...
    if (paddle.HasPaddleType(PlayerPaddle::StickyPaddle) &&
        (projectile.GetType() == Projectile::PaddleMineBulletProjectile ||
         projectile.GetType() == Projectile::MineTurretBulletProjectile)) {
        return;
    }

    // Add the hurting effects...
	this->display->GetAssets()->PaddleHurtByProjectile(paddle, projectile);

	debug_output("EVENT: Paddle hit by projectile");
}

void GameEventsListener::PaddleShieldHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile) {
	UNUSED_PARAMETER(paddle);
	UNUSED_PARAMETER(projectile);
	// TODO ?
	debug_output("EVENT: Paddle shield hit by projectile");
}

void GameEventsListener::ProjectileDeflectedByPaddleShieldEvent(const Projectile& projectile, const PlayerPaddle& paddle) {
	UNUSED_PARAMETER(paddle);
	UNUSED_PARAMETER(projectile);
	// TODO ?
	debug_output("EVENT: Paddle shield deflected projectile");
}

void GameEventsListener::PaddleHitByBeamEvent(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment) {
    this->display->GetAssets()->PaddleHurtByBeam(paddle, beam, beamSegment);
    debug_output("EVENT: Paddle hit by beam");
}

void GameEventsListener::PaddleShieldHitByBeamEvent(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment) {
    UNUSED_PARAMETER(paddle);
    UNUSED_PARAMETER(beam);
    UNUSED_PARAMETER(beamSegment);
    // TODO?
	debug_output("EVENT: Paddle shield deflected projectile");
}

void GameEventsListener::BallDiedEvent(const GameBall& deadBall) {
	debug_output("EVENT: Ball died");
	this->display->GetAssets()->GetESPAssets()->KillAllActiveBallEffects(deadBall);

	// If it's not the last ball then we play the lost ball effect...
	if (this->display->GetModel()->GetGameBalls().size() >= 2) {
		this->display->GetSound()->PlaySoundAtPosition(GameSound::PlayerLostABallButIsStillAliveEvent, false, deadBall.GetCenterPosition());
	}
}

void GameEventsListener::LastBallAboutToDieEvent(const GameBall& lastBallToDie) {
	debug_output("EVENT: Last ball is about to die.");

	this->display->GetAssets()->ActivateLastBallDeathEffects(lastBallToDie);

	// Setup the sound to quiet everything else and play the sound of the ball spiralling to its most horrible death
    GameSound* sound = this->display->GetSound();
    
    // Kill any sound effects that might be active
    sound->StopAllEffects();

    // Stop playing specific loops
    sound->StopSound(inBulletTimeLoopSoundID);

    //sound->SetAllPlayingSoundVolume(0.5f, 0.5);
	sound->AttachAndPlaySound(&lastBallToDie, GameSound::LastBallSpiralingToDeathLoop, true);

	// Clear out the timers - they no longer matter since the ball is doomed
	this->display->GetAssets()->GetItemAssets()->ClearTimers();
}

void GameEventsListener::LastBallExploded(const GameBall& explodedBall) {
	debug_output("EVENT: Last ball exploded.");

	// Add a cool effect for when the ball explodes
	this->display->GetAssets()->GetESPAssets()->AddBallExplodedEffect(&explodedBall);

	// Stop the spiraling sound loop, restore all previous sounds to their proper volume and add the sound for the last ball exploding
    GameSound* sound = this->display->GetSound();
    sound->StopAllEffects();
    sound->DetachAndStopAllSounds(&explodedBall);

    sound->PlaySoundAtPosition(GameSound::LastBallExplodedEvent, false, explodedBall.GetCenterPosition());
}

void GameEventsListener::AllBallsDeadEvent(int livesLeft) {
    UNUSED_PARAMETER(livesLeft);
	debug_output("EVENT: Ball death, lives left: " << livesLeft);

	// Kill all effects that may have previously been occuring...
	this->display->GetAssets()->DeactivateLastBallDeathEffects();
	this->display->GetAssets()->GetESPAssets()->KillAllActiveEffects(false);
		
    // Clean up all sound effects
    GameSound* sound = this->display->GetSound();
    sound->StopAllEffects();

	// Check to see if it's game over, and switch the display state appropriately
	if (this->display->GetModel()->IsGameOver()) {
		
		// Kill absolutely all remaining visual effects...
		this->display->GetAssets()->DeactivateMiscEffects();
		
        // Game over.
		this->display->SetCurrentState(new GameOverDisplayState(this->display));
	}
}

void GameEventsListener::BallSpawnEvent(const GameBall& spawnedBall) {
	UNUSED_PARAMETER(spawnedBall);
	debug_output("EVENT: Ball respawning");
}

void GameEventsListener::BallShotEvent(const GameBall& shotBall) {
	UNUSED_PARAMETER(shotBall);
    this->display->GetAssets()->GetBallReleaseHUD()->BallReleased();
	debug_output("EVENT: Ball shot");
}

void GameEventsListener::ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block) {

	// Add any visual effects required for when a projectile hits the block
	this->display->GetAssets()->GetESPAssets()->AddBlockHitByProjectileEffect(projectile, block);

	debug_output("EVENT: Projectile-block collision");
}

void GameEventsListener::ProjectileSafetyNetCollisionEvent(const Projectile& projectile, const SafetyNet& safetyNet) {
    UNUSED_PARAMETER(safetyNet);

    // Add any visual effects for the collision
    this->display->GetAssets()->GetESPAssets()->AddSafetyNetHitByProjectileEffect(projectile);

    debug_output("EVENT: Projectile-safety net collision");
}

void GameEventsListener::ProjectileBossCollisionEvent(const Projectile& projectile, const Boss& boss, const BossBodyPart& collisionPart) {
    UNUSED_PARAMETER(boss);

    // Add visual effects for the collision
    this->display->GetAssets()->GetESPAssets()->AddBossHitByProjectileEffect(projectile, collisionPart);

    debug_output("EVENT: Projectile-boss collision");
}

void GameEventsListener::BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) {
	long currSystemTime = BlammoTime::GetSystemTimeInMillisecs();
	bool doEffects = (currSystemTime - this->timeSinceLastBallBlockCollisionEventInMS) > GameEventsListener::EFFECT_WAIT_TIME_BETWEEN_BALL_BLOCK_COLLISIONS_IN_MS;

	if (doEffects) {

		// Add the visual effect for when the ball hits a block
		// We don't do bounce effects for the invisiball... cause then the player would know where it is easier
		if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
			((ball.GetBallType() & GameBall::UberBall) != GameBall::UberBall || !block.BallBlastsThrough(ball))) {
				this->display->GetAssets()->GetESPAssets()->AddBounceLevelPieceEffect(ball, block);
		}

		// We shake things up if the ball is uber and the block is indestructible...
		if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
			(ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall && !block.BallBlastsThrough(ball)) {

			float shakeMagnitude, shakeLength;
			BBBGameController::VibrateAmount controllerVibeAmt;
			GameEventsListener::GetEffectsForBallSize(ball.GetBallSize(), shakeMagnitude, shakeLength, controllerVibeAmt);
			
			this->display->GetCamera().SetCameraShake(shakeLength, Vector3D(8 * shakeMagnitude, shakeMagnitude, 0.0), 100);
			
			// Make the controller shake by side (based on where the ball is)
			BBBGameController::VibrateAmount leftVibeAmt = BBBGameController::VerySoftVibration;
			BBBGameController::VibrateAmount rightVibeAmt = BBBGameController::VerySoftVibration;
			if (ball.GetCenterPosition2D()[0] > 0) {
				// On the right side...
				rightVibeAmt = controllerVibeAmt;
			}
			else {
				// On the left side...
				leftVibeAmt = controllerVibeAmt;
			}

			GameControllerManager::GetInstance()->VibrateControllers(shakeLength, leftVibeAmt, rightVibeAmt);
		}

        if (block.ProducesBounceEffectsWithBallWhenHit(ball)) {
            Point3D approxCollisionPos = ball.GetCenterPosition() + ball.GetBounds().Radius() * 
                Vector3D::Normalize(block.GetPosition3D() - ball.GetCenterPosition());
		    this->display->GetSound()->PlaySoundAtPosition(GameSound::BallBlockCollisionEvent, false, approxCollisionPos);
        }
	}

	this->timeSinceLastBallBlockCollisionEventInMS = currSystemTime;
	debug_output("EVENT: Ball-block collision");
}

void GameEventsListener::BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) {
	long currSystemTime = BlammoTime::GetSystemTimeInMillisecs();
	bool doEffect = (currSystemTime - this->timeSinceLastBallPaddleCollisionEventInMS) > 
									EFFECT_WAIT_TIME_BETWEEN_BALL_PADDLE_COLLISIONS_IN_MS;

	if (doEffect) {
		// Play the sound for when the ball hits the paddle
        Point3D collisionPtEstimate = ball.GetCenterPosition() + ball.GetBounds().Radius() * 
            Vector3D::Normalize(Point3D(paddle.GetCenterPosition()) - ball.GetCenterPosition());
        GameSound* sound = this->display->GetSound();
        if (paddle.HasPaddleType(PlayerPaddle::ShieldPaddle)) {
            sound->PlaySoundAtPosition(GameSound::BallShieldPaddleCollisionEvent, false, collisionPtEstimate);
		}
		else if (paddle.HasPaddleType(PlayerPaddle::StickyPaddle)) {
            sound->PlaySoundAtPosition(GameSound::BallStickyPaddleCollisionEvent, false, collisionPtEstimate);
		}
		else {
			sound->PlaySoundAtPosition(GameSound::GetRandomBallPaddleCollisionEventSoundType(), false, collisionPtEstimate);
		}

		// Add the visual effect for when the ball hits the paddle
		this->display->GetAssets()->GetESPAssets()->AddBouncePaddleEffect(ball, paddle);

		bool ballIsUber = (ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall;
		BBBGameController::VibrateAmount vibrationLeft  = BBBGameController::VerySoftVibration;
        BBBGameController::VibrateAmount vibrationRight = BBBGameController::SoftVibration;

		if (ballIsUber) {
            // We shake things up if the ball is uber...
			this->display->GetCamera().SetCameraShake(0.2f, Vector3D(0.6f, 0.2f, 0.0f), 90);
			vibrationLeft  = BBBGameController::MediumVibration;
            vibrationRight = BBBGameController::HeavyVibration;
		}

		if (ball.GetBallSize() > GameBall::NormalSize) {
			vibrationLeft  = BBBGameController::SoftVibration;
            vibrationRight = BBBGameController::MediumVibration;
		}
		else if (ball.GetBallSize() < GameBall::NormalSize) {
			vibrationLeft = BBBGameController::VerySoftVibration;
            vibrationRight = BBBGameController::VerySoftVibration;
		}

		GameControllerManager::GetInstance()->VibrateControllers(0.15f, vibrationLeft, vibrationRight);
	}

	this->timeSinceLastBallPaddleCollisionEventInMS = currSystemTime;
	debug_output("EVENT: Ball-paddle collision");
}

void GameEventsListener::BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2) {
	// Play the sound for the collision
    Point3D collisionPtEstimate = ball1.GetCenterPosition() + ball1.GetBounds().Radius() * Vector3D::Normalize(ball2.GetCenterPosition() - ball1.GetCenterPosition());
	this->display->GetSound()->PlaySoundAtPosition(GameSound::BallBallCollisionEvent, false, collisionPtEstimate);
    
    // Add the effect for ball-ball collision
	this->display->GetAssets()->GetESPAssets()->AddBounceBallBallEffect(ball1, ball2);

	debug_output("EVENT: Ball-ball collision");
}

void GameEventsListener::BallPortalBlockTeleportEvent(const GameBall& ball, const PortalBlock& enterPortal) {
    // Play the ball teleportation sound
    this->display->GetSound()->PlaySoundAtPosition(GameSound::PortalTeleportEvent, false, enterPortal.GetPosition3D());

    // Add the teleportation effect
	this->display->GetAssets()->GetESPAssets()->AddPortalTeleportEffect(ball.GetBounds().Center(), enterPortal);

	debug_output("EVENT: Ball teleported by portal block");
}

void GameEventsListener::ProjectilePortalBlockTeleportEvent(const Projectile& projectile, const PortalBlock& enterPortal) {
	
    switch (projectile.GetType()) {

        case Projectile::BossLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			// TODO: Maybe a rotating sparkle or something?
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
		case Projectile::CollateralBlockProjectile: {
		    Point2D projectileTeleportPos = projectile.GetPosition() + projectile.GetHalfHeight() * projectile.GetVelocityDirection();
		    this->display->GetAssets()->GetESPAssets()->AddPortalTeleportEffect(projectileTeleportPos, enterPortal);
            break;
        }

		default:
			assert(false);
			break;
	}
	debug_output("EVENT: Projectile teleported by portal block");
}

void GameEventsListener::BallEnteredCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock) {
    UNUSED_PARAMETER(ball);
    UNUSED_PARAMETER(cannonBlock);

    // Start the sound of the cannon rotating
    this->display->GetSound()->AttachAndPlaySound(&cannonBlock, GameSound::CannonBlockRotatingLoop, true);

    debug_output("EVENT: Ball entered cannon");
}

void GameEventsListener::BallFiredFromCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock) {
	UNUSED_PARAMETER(ball);

    // Stop the sound of the cannon rotating and add a sound for the blast
	GameSound* sound = this->display->GetSound();
    sound->DetachAndStopAllSounds(&cannonBlock);
	sound->PlaySoundAtPosition(GameSound::CannonBlockFiredEvent, false, cannonBlock.GetPosition3D());

	// Add the blast effect of the ball exiting the cannon
	this->display->GetAssets()->GetESPAssets()->AddCannonFireEffect(
        Point3D(cannonBlock.GetEndOfBarrelPoint()), cannonBlock.GetCurrentCannonDirection());

	debug_output("EVENT: Ball fired out of cannon");
}

void GameEventsListener::ProjectileEnteredCannonEvent(const Projectile& projectile, const CannonBlock& cannonBlock) {
	UNUSED_PARAMETER(cannonBlock);
    
    GameSound* sound = this->display->GetSound();
    // Make sure the object no longer is making any noise (since it's now loaded into the cannon)
	sound->SetPauseForAllAttachedSounds(&projectile, true);
    // Start the sound of the cannon rotating
    sound->AttachAndPlaySound(&cannonBlock, GameSound::CannonBlockRotatingLoop, true);

    // Reset the projectile's effects
    this->display->GetAssets()->GetESPAssets()->ResetProjectileEffects(projectile);

	debug_output("EVENT: Projectile entered cannon");
}

void GameEventsListener::ProjectileFiredFromCannonEvent(const Projectile& projectile, const CannonBlock& cannonBlock) {

	// Add the blast effect of the rocket exiting the cannon
	this->display->GetAssets()->GetESPAssets()->AddCannonFireEffect(
        Point3D(cannonBlock.GetEndOfBarrelPoint()), cannonBlock.GetCurrentCannonDirection());
	
    // Stop the sound of the cannon rotating and add a sound for the blast
	GameSound* sound = this->display->GetSound();
    sound->DetachAndStopAllSounds(&cannonBlock);
	sound->PlaySoundAtPosition(GameSound::CannonBlockFiredEvent, false, cannonBlock.GetPosition3D());

    // Reset the projectile effects
    this->display->GetAssets()->GetESPAssets()->ResetProjectileEffects(projectile);

    // Resume all the projectile's sounds (if any)
    sound->SetPauseForAllAttachedSounds(&projectile, false);

	debug_output("EVENT: Projectile fired out of cannon");
}

void GameEventsListener::BallHitTeslaLightningArcEvent(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2) {
	UNUSED_PARAMETER(teslaBlock1);
	UNUSED_PARAMETER(teslaBlock2);

	long currSystemTime = BlammoTime::GetSystemTimeInMillisecs();
	bool doEffect = (currSystemTime - this->timeSinceLastBallTeslaCollisionEventInMS) > 
									EFFECT_WAIT_TIME_BETWEEN_BALL_TESLA_COLLISIONS_IN_MS;

	if (doEffect) {
		// Add the effect(s) for when the ball hits the lightning
		this->display->GetAssets()->GetESPAssets()->AddBallHitLightningArcEffect(ball);

		// Add a tiny camera and controller shake
		//this->display->GetCamera().SetCameraShake(1.2, Vector3D(0.75, 0.1, 0.1), 40);
        GameControllerManager::GetInstance()->VibrateControllers(0.08f, BBBGameController::VerySoftVibration, BBBGameController::VerySoftVibration);
	}

	this->timeSinceLastBallTeslaCollisionEventInMS = currSystemTime;
	debug_output("EVENT: Ball hit tesla lightning arc");
}

void GameEventsListener::FireBallCancelledByIceBallEvent(const GameBall& ball) {
    this->display->GetAssets()->GetESPAssets()->AddFireballCancelledEffect(&ball);
	debug_output("EVENT: Fireball cancelled by Iceball");
}

void GameEventsListener::IceBallCancelledByFireBallEvent(const GameBall& ball) {
    this->display->GetAssets()->GetESPAssets()->AddIceballCancelledEffect(&ball);
	debug_output("EVENT: Iceball cancelled by Fireball");
}

void GameEventsListener::BlockDestroyedEvent(const LevelPiece& block, const LevelPiece::DestructionMethod& method) {
    GameSound* sound = this->display->GetSound();

    if (method == LevelPiece::DisintegrationDestruction) {
        this->display->GetAssets()->GetESPAssets()->AddBlockDisintegrationEffect(block);
    }
    else {
        bool wasFrozen = block.HasStatus(LevelPiece::IceCubeStatus);

	    // Add the effects based on the type of block that is being destroyed, its status and method of destruction...
	    switch (block.GetType()) {
    		
		    case LevelPiece::Breakable:
		    case LevelPiece::BreakableTriangle:
		    case LevelPiece::Solid:
		    case LevelPiece::SolidTriangle:
		    case LevelPiece::Tesla:
            case LevelPiece::Switch:
            case LevelPiece::OneWay:
            case LevelPiece::NoEntry:
			    if (wasFrozen) {
				    // Add ice break effect
				    this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, block.GetColour());
			    }
			    else {
				    // Typical break effect for basic breakable blocks
				    this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
				    sound->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, block.GetPosition3D());
			    }
			    break;

            case LevelPiece::Regen: {
                const RegenBlock& regenBlock = static_cast<const RegenBlock&>(block);
                this->display->GetAssets()->GetESPAssets()->AddRegenBlockSpecialBreakEffect(regenBlock); // Make the counter on the block go exploding out
			    if (wasFrozen) {
				    // Add ice break effect
				    this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, block.GetColour());
			    }
			    else {
				    // Typical break effect for basic breakable blocks
				    this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
				    sound->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, block.GetPosition3D());
			    }
			    break;
            }

            case LevelPiece::LaserTurret:
            case LevelPiece::RocketTurret:
            case LevelPiece::MineTurret:
			    if (wasFrozen) {
				    // Add ice break effect
				    this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, block.GetColour());
			    }
			    else {
				    // Typical break effect for basic breakable blocks
				    this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
				    sound->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, block.GetPosition3D());
			    }
			    break;

		    case LevelPiece::ItemDrop:
			    if (wasFrozen) {
				    // Add ice break effect
				    this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, Colour(0.9f, 0.45f, 0.0f));
			    }
			    else {
				    // Typical break effect
				    this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
				    sound->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, block.GetPosition3D());
			    }
			    break;

            case LevelPiece::AlwaysDrop:
			    if (wasFrozen) {
				    // Add ice break effect
				    this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, block.GetColour());
			    }
			    else {
				    // Typical break effect for basic breakable blocks
				    this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
				    sound->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, block.GetPosition3D());
			    }
                break;

		    case LevelPiece::Bomb:
			    if (wasFrozen) {
				    // Add ice break effect
				    this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, Colour(0.66f, 0.66f, 0.66f));
			    }
			    else {
				    // Bomb effect - big explosion!
				    this->display->GetAssets()->GetESPAssets()->AddBombBlockBreakEffect(block);
				    this->display->GetCamera().SetCameraShake(1.2f, Vector3D(1.0f, 0.3f, 0.1f), 110);
				    GameControllerManager::GetInstance()->VibrateControllers(1.0f, BBBGameController::HeavyVibration, BBBGameController::HeavyVibration);
                    sound->PlaySoundAtPosition(GameSound::BombBlockDestroyedEvent, false, block.GetPosition3D());
			    }
			    break;

		    case LevelPiece::Ink: {
		        const PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();

		        // We do not do any ink blotches while in ball or paddle camera modes, also, if the ink block is frozen
		        // then it just shatters...
		        bool inkSplatter = !(paddle->GetIsPaddleCameraOn() || GameBall::GetIsBallCameraOn()) && !wasFrozen &&
                    method != LevelPiece::BombDestruction && method != LevelPiece::RocketDestruction && 
                    method != LevelPiece::MineDestruction;

		        if (wasFrozen) {
		    	    // Add ice break effect
		    	    this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, GameViewConstants::GetInstance()->INK_BLOCK_COLOUR);
		        }
		        else {
		    	    // Emit goo from ink block and make onomata effects
		    	    this->display->GetAssets()->GetESPAssets()->AddInkBlockBreakEffect(
                        this->display->GetCamera(), block, *this->display->GetModel()->GetCurrentLevel(), inkSplatter);
		    	    sound->PlaySoundAtPosition(GameSound::InkBlockDestroyedEvent, false, block.GetPosition3D());

		    	    if (inkSplatter) {
		    		    // Cover camera in ink with a fullscreen splatter effect
		    		    this->display->GetAssets()->GetFBOAssets()->ActivateInkSplatterEffect();
		    	    }
		        }
			    break;
            }

		    case LevelPiece::Prism:
		    case LevelPiece::PrismTriangle:
			    this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
                sound->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, block.GetPosition3D());
			    break;

		    case LevelPiece::Cannon: {
                this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
                sound->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, block.GetPosition3D());
			    break;
            }

		    case LevelPiece::Collateral: {
                if (wasFrozen) {
				    // Add ice break effect
				    this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, Colour(0.5f, 0.5f, 0.5f));
			    }
                else {
                    
                    
			        // Don't show any effects / play any sounds if the ball is dead/dying or if the block is off screen...
                    const GameModel* gameModel = this->display->GetModel();
                    if (!gameModel->IsOutOfGameBounds(block.GetCenter()) && 
                        gameModel->GetCurrentStateType() != GameState::BallDeathStateType) {

				        this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);

                        if ((gameModel->GetCurrentStateType() == GameState::BallInPlayStateType ||
                            gameModel->GetCurrentStateType() == GameState::BallOnPaddleStateType)) {
				            sound->PlaySoundAtPosition(GameSound::BasicBlockDestroyedEvent, false, block.GetPosition3D());
                        }

			        }
                }
                break;
            }

		    default:
			    break;
	    }
    }

    // Blocks implemented in particular ways have to be disposed of in particular ways... ugh.
    switch (block.GetType()) {

        case LevelPiece::Regen:
        case LevelPiece::LaserTurret:
        case LevelPiece::RocketTurret:
        case LevelPiece::MineTurret:
		case LevelPiece::ItemDrop:
        case LevelPiece::AlwaysDrop:
		case LevelPiece::Cannon:
		case LevelPiece::Collateral:
			this->display->GetAssets()->GetCurrentLevelMesh()->RemovePiece(block);
            break;

	    case LevelPiece::Breakable:
		case LevelPiece::BreakableTriangle:
		case LevelPiece::Solid:
		case LevelPiece::SolidTriangle:
		case LevelPiece::Tesla:
        case LevelPiece::Switch:
        case LevelPiece::OneWay:
        case LevelPiece::NoEntry:
		case LevelPiece::Bomb:
		case LevelPiece::Ink:
		case LevelPiece::Prism:
		case LevelPiece::PrismTriangle:
        case LevelPiece::Empty:
        case LevelPiece::Portal:
		default:
			break;
    }

	debug_output("EVENT: Block destroyed");
}

void GameEventsListener::BallSafetyNetCreatedEvent() {

    // Tell the level mesh about it so it can show any effects for the creation
	// of the safety net mesh
	this->display->GetAssets()->BallSafetyNetCreated();

    // Play the sound for the creation of the net
    const Point2D& safetyNetCenterPos = this->display->GetAssets()->GetBallSafetyNetPosition();
    this->display->GetSound()->PlaySoundAtPosition(GameSound::BallSafetyNetCreatedEvent, false, Point3D(safetyNetCenterPos, 0.0f));

	debug_output("EVENT: Ball safety net created");
}

void GameEventsListener::BallSafetyNetDestroyedEvent(const GameBall& ball) {
	Point2D collisionCenter = ball.GetCenterPosition2D() - Vector2D(0, ball.GetBounds().Radius() + BallSafetyNetMesh::SAFETY_NET_HEIGHT/2.0f);
	this->DestroyBallSafetyNet(collisionCenter);
	debug_output("EVENT: Ball safety net destroyed by ball");
}

void GameEventsListener::BallSafetyNetDestroyedEvent(const PlayerPaddle& paddle) {
	this->DestroyBallSafetyNet(paddle.GetCenterPosition());
	debug_output("EVENT: Ball safety net destroyed by paddle");
}

void GameEventsListener::BallSafetyNetDestroyedEvent(const Projectile& projectile) {
	this->DestroyBallSafetyNet(projectile.GetPosition());
	debug_output("EVENT: Ball safety net destroyed by projectile");
}


// Private helper for when the safety net is destroyed
void GameEventsListener::DestroyBallSafetyNet(const Point2D& pt) {

	this->display->GetSound()->PlaySoundAtPosition(GameSound::BallSafetyNetDestroyedEvent, false, Point3D(pt, 0.0f));

	// Tell the level mesh about it so it can show any effects for the destruction
	// of the safety net mesh
    GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
    assert(currLevel != NULL);
	this->display->GetAssets()->BallSafetyNetDestroyed(*currLevel, pt);

	// Particle effects for when the safety net is destroyed
	this->display->GetAssets()->GetESPAssets()->AddBallSafetyNetDestroyedEffect(pt);
}

void GameEventsListener::LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
    // Stop all sounds for the piece that has changed...
    this->display->GetSound()->DetachAndStopAllSounds(&pieceBefore);

	this->display->GetAssets()->GetCurrentLevelMesh()->ChangePiece(pieceBefore, pieceAfter);
	debug_output("EVENT: LevelPiece changed");
}

void GameEventsListener::LevelPieceStatusAddedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus) {
    switch (addedStatus) {
        case LevelPiece::IceCubeStatus:
            this->display->GetSound()->PlaySoundAtPosition(GameSound::BlockFrozenEvent, false, piece.GetPosition3D());
            break;
        case LevelPiece::OnFireStatus:
            this->display->GetSound()->AttachAndPlaySound(&piece, GameSound::BlockOnFireLoop, true);
            break;
        default:
            break;
    }

	this->display->GetAssets()->GetCurrentLevelMesh()->LevelPieceStatusAdded(piece, addedStatus);
	debug_output("EVENT: LevelPiece status added");
}

void GameEventsListener::LevelPieceStatusRemovedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus) {
    switch (removedStatus) {
        case LevelPiece::OnFireStatus:
            this->display->GetSound()->DetachAndStopSound(&piece, GameSound::BlockOnFireLoop);
            break;
        default:
            break;
    }

	this->display->GetAssets()->GetCurrentLevelMesh()->LevelPieceAllStatusRemoved(piece);
	debug_output("EVENT: LevelPiece status removed");
}

void GameEventsListener::LevelPieceAllStatusRemovedEvent(const LevelPiece& piece) {
    this->display->GetSound()->DetachAndStopSound(&piece, GameSound::BlockOnFireLoop);

	this->display->GetAssets()->GetCurrentLevelMesh()->LevelPieceAllStatusRemoved(piece);
	debug_output("EVENT: LevelPiece all status removed");
}

void GameEventsListener::ItemSpawnedEvent(const GameItem& item) {
    GameSound* sound = this->display->GetSound();

	// Play item spawn sound
	sound->PlaySoundAtPosition(GameSound::ItemSpawnedEvent, false, item.GetPosition3D());

	// We don't show the stars coming off the dropping items if it gets in the way of playing
	// the game - e.g., when in paddle camera mode
	const PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();
	bool showParticles = !paddle->GetIsPaddleCameraOn() && !GameBall::GetIsBallCameraOn();

	// Spawn an item drop effect for the item...
	this->display->GetAssets()->GetESPAssets()->AddItemDropEffect(item, showParticles);

	// Play the item moving loop - plays as the item falls towards the paddle until it leaves play
    SoundID itemMovingSoundID = sound->AttachAndPlaySound(&item, GameSound::ItemMovingLoop, true);
    sound->SetSoundVolume(itemMovingSoundID, 0.5f);

	debug_output("EVENT: Item Spawned: " << item);
}

void GameEventsListener::ItemRemovedEvent(const GameItem& item) {
	// Stop the item moving sounds
    this->display->GetSound()->DetachAndStopAllSounds(&item);

	// Remove any previous item drop effect
	this->display->GetAssets()->GetESPAssets()->RemoveItemDropEffect(item);

	debug_output("EVENT: Item Removed: " << item);
}

void GameEventsListener::ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle) {
    // Tell blammopedia to unlock the item (if it hasn't already)
    Blammopedia* blammopedia = ResourceManager::GetInstance()->GetBlammopedia();
    blammopedia->UnlockItem(item.GetItemType());
    
    // Add the visual effect for acquiring an item
	this->display->GetAssets()->GetESPAssets()->AddItemAcquiredEffect(this->display->GetCamera(), paddle, item);
	debug_output("EVENT: Item Obtained by Player: " << item);
}

void GameEventsListener::ItemActivatedEvent(const GameItem& item) {

    bool playItemActivateGeneralSound = (item.GetItemType() != GameItem::LifeUpItem);
    if (playItemActivateGeneralSound) {

        // Play the appropriate general sound based on the disposition type of item acquired by the player
	    GameSound* sound = this->display->GetSound();
	    switch (item.GetItemDisposition()) {

		    case GameItem::Good:
                sound->PlaySoundAtPosition(GameSound::PowerUpItemActivatedEvent, false, item.GetPosition3D());
			    break;

		    case GameItem::Neutral:
			    sound->PlaySoundAtPosition(GameSound::PowerNeutralItemActivatedEvent, false, item.GetPosition3D());
			    break;

		    case GameItem::Bad:
			    sound->PlaySoundAtPosition(GameSound::PowerDownItemActivatedEvent, false, item.GetPosition3D());
			    break;

		    default:
			    assert(false);
			    break;
	    }
    }

	// Activate the item's effects (if any)
	this->display->GetAssets()->ActivateItemEffects(*this->display->GetModel(), item);

	debug_output("EVENT: Item activated: " << item);
}

void GameEventsListener::ItemDeactivatedEvent(const GameItem& item) {

	// Deactivate the item's effects (if any)
	this->display->GetAssets()->DeactivateItemEffects(*this->display->GetModel(), item);

	debug_output("EVENT: Item deactivated: " << item);
}

void GameEventsListener::RandomItemActivatedEvent(const RandomItem& randomItem, const GameItem& actualItem) {
	UNUSED_PARAMETER(randomItem);

	// Cause the small HUD-like thingy for showing what the random item has become to the player...
	this->display->GetAssets()->ActivateRandomItemEffects(*this->display->GetModel(), actualItem);
	debug_output("EVENT: Random item activated");
}

void GameEventsListener::ItemTimerStartedEvent(const GameItemTimer& itemTimer){
	this->display->GetAssets()->GetItemAssets()->TimerStarted(&itemTimer);

	debug_output("EVENT: Item timer started: " << itemTimer);
}

void GameEventsListener::ItemTimerStoppedEvent(const GameItemTimer& itemTimer) {
	this->display->GetAssets()->GetItemAssets()->TimerStopped(&itemTimer);

	debug_output("EVENT: Item timer stopped/expired: " << itemTimer);
}

void GameEventsListener::ItemDropBlockItemChangeEvent(const ItemDropBlock& dropBlock) {
	const GameItemAssets* itemAssets = this->display->GetAssets()->GetItemAssets();
	assert(itemAssets != NULL);
	this->display->GetAssets()->GetCurrentLevelMesh()->UpdateItemDropBlock(*itemAssets, dropBlock);

	debug_output("EVENT: Item drop block item type changed");
}

void GameEventsListener::SwitchBlockActivatedEvent(const SwitchBlock& switchBlock) {

    this->display->GetSound()->PlaySoundAtPosition(GameSound::SwitchBlockActivated, false, switchBlock.GetPosition3D());
    this->display->GetAssets()->GetCurrentLevelMesh()->SwitchActivated(&switchBlock, this->display->GetModel()->GetCurrentLevel());

    debug_output("EVENT: Switch block activated");
}

void GameEventsListener::BulletTimeStateChangedEvent(const BallBoostModel& boostModel) {
    this->display->GetAssets()->GetFBOAssets()->UpdateBulletTimeState(boostModel);

    if (boostModel.GetBulletTimeState() == BallBoostModel::BulletTimeFadeIn ||
        boostModel.GetBulletTimeState() == BallBoostModel::BulletTimeFadeOut ) {
        // Reset the effects for the bullet time direction
        this->display->GetAssets()->GetESPAssets()->ResetBulletTimeBallBoostEffects();
    }

    GameSound* sound = this->display->GetSound();

    switch (boostModel.GetBulletTimeState()) {

        case BallBoostModel::BulletTimeFadeIn: {
            // Sounds and sound effects for going into bullet time mode
            enterBulletTimeSoundID = sound->PlaySound(GameSound::EnterBulletTimeEvent, false, false);
            sound->StopSound(exitBulletTimeSoundID);
            
            std::set<SoundID> ignoreSoundSet;
            ignoreSoundSet.insert(enterBulletTimeSoundID);
            sound->ToggleSoundEffect(GameSound::BulletTimeEffect, true, ignoreSoundSet);

            inBulletTimeLoopSoundID = sound->PlaySound(GameSound::InBulletTimeLoop, true, false);

            break;
        }

        case BallBoostModel::BulletTimeFadeOut: {
            // Sounds for exiting bullet time mode
            exitBulletTimeSoundID = sound->PlaySound(GameSound::ExitBulletTimeEvent, false, false);
            
            sound->StopSound(enterBulletTimeSoundID);
            sound->StopSound(inBulletTimeLoopSoundID);

            break;
        }
        case BallBoostModel::NotInBulletTime: {
            // Turn off the bullet time sound effects
            sound->ToggleSoundEffect(GameSound::BulletTimeEffect, false);
            sound->StopSound(enterBulletTimeSoundID);
            sound->StopSound(inBulletTimeLoopSoundID);
            break;
        }

        default:
            break;
    }
}

void GameEventsListener::BallBoostExecutedEvent(const BallBoostModel& boostModel) {
    UNUSED_PARAMETER(boostModel);

    this->display->GetSound()->PlaySound(GameSound::BallBoostEvent, false);
    this->display->GetAssets()->GetESPAssets()->AddBallBoostEffect(boostModel);
    
    debug_output("EVENT: Ball boost executed");
}

void GameEventsListener::BallBoostGainedEvent() {
    BallBoostHUD* boostHUD = this->display->GetAssets()->GetBoostHUD();

    // Don't show any effects if the ball is in the throes of death
    if (this->display->GetModel()->GetCurrentStateType() != GameState::BallDeathStateType) {

        // Effects on the ball that might bring attention to the newly available boost...
        // NOTE: Make sure this happens BEFORE telling the HUD that it's gained the boost -
        // that way we get the correct colour from it
        const std::list<GameBall*>& balls = this->display->GetModel()->GetGameBalls();
        Point3D avgPos(0,0,0);
        for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
            
            GameBall* currBall = *iter;
            avgPos += Vector3D(currBall->GetCenterPosition2D()[0], currBall->GetCenterPosition2D()[1], 0.0f);

            this->display->GetAssets()->GetESPAssets()->AddBallAcquiredBoostEffect(*currBall, boostHUD->GetCurrentBoostColour());
        }
        avgPos /= static_cast<float>(balls.size());

        this->display->GetSound()->PlaySoundAtPosition(GameSound::BallBoostGainedEvent, false, avgPos);
    }
    
    // Make the HUD change for indicating the number of boosts that the player has
    boostHUD->BoostGained();
    debug_output("EVENT: Ball boost gained");
}

void GameEventsListener::BallBoostLostEvent(bool allBoostsLost) {
    if (allBoostsLost) {
        this->display->GetAssets()->GetBoostHUD()->AllBoostsLost(this->display->GetModel()->GetBallBoostModel());
        debug_output("EVENT: All ball boosts lost");
    }
    else {
        this->display->GetAssets()->GetBoostHUD()->BoostLost();
        debug_output("EVENT: Ball boost lost");
    }
}

void GameEventsListener::ProjectileSpawnedEvent(const Projectile& projectile) {
	// Tell the assets - this will spawn the appropriate sprites/projectiles and effects
	this->display->GetAssets()->AddProjectile(*this->display->GetModel(), projectile);
}

void GameEventsListener::ProjectileRemovedEvent(const Projectile& projectile) {
	// Remove the projectile's effect
	this->display->GetAssets()->RemoveProjectile(projectile);
}

void GameEventsListener::RocketExplodedEvent(const RocketProjectile& rocket) {
	this->display->GetAssets()->RocketExplosion(rocket, this->display->GetCamera());
	debug_output("EVENT: Rocket exploded");
}

void GameEventsListener::MineExplodedEvent(const MineProjectile& mine) {
    this->display->GetAssets()->MineExplosion(mine, this->display->GetCamera());
	debug_output("EVENT: Mine exploded");
}

void GameEventsListener::RemoteControlRocketFuelWarningEvent(const PaddleRemoteControlRocketProjectile& rocket) {

    // Vibrate the controller a bit to indicate a dying rocket...
    if (rocket.GetCurrentFuelAmount() < 10.0f) {
        GameControllerManager::GetInstance()->VibrateControllers(0.075, 
            Randomizer::GetInstance()->RandomTrueOrFalse() ? BBBGameController::SoftVibration : BBBGameController::MediumVibration, 
            Randomizer::GetInstance()->RandomTrueOrFalse() ? BBBGameController::SoftVibration : BBBGameController::MediumVibration);
    }
    else {
        GameControllerManager::GetInstance()->VibrateControllers(0.1, 
            Randomizer::GetInstance()->RandomTrueOrFalse() ? BBBGameController::SoftVibration : BBBGameController::VerySoftVibration,
            Randomizer::GetInstance()->RandomTrueOrFalse() ? BBBGameController::SoftVibration : BBBGameController::VerySoftVibration);
    }

	debug_output("EVENT: Remote controlled rocket warning.");
}

void GameEventsListener::RemoteControlRocketThrustAppliedEvent(const PaddleRemoteControlRocketProjectile& rocket) {
    
    // Let the rocket mesh know about the thrusting...
    this->display->GetAssets()->ApplyRocketThrust(rocket);

    debug_output("EVENT: Remote controlled rocket thrust applied.");
}

void GameEventsListener::BeamSpawnedEvent(const Beam& beam) {
	// Add an effect for the new beam...
	this->display->GetAssets()->GetESPAssets()->AddBeamEffect(beam);

	// Add the appropriate sounds for the beam
	switch (beam.GetType()) {
		
        case Beam::PaddleBeam:
			this->display->GetSound()->AttachAndPlaySound(&beam, GameSound::LaserBeamFiringLoop, true);
			break;
        case Beam::BossBeam:
            this->display->GetSound()->AttachAndPlaySound(&beam, GameSound::LaserBeamFiringLoop, true);
            break;

		default:
			assert(false);
			break;
	}

	debug_output("EVENT: Beam spawned");
}

void GameEventsListener::BeamChangedEvent(const Beam& beam) {
	// Update the effect for the now changed beam...
	this->display->GetAssets()->GetESPAssets()->UpdateBeamEffect(beam);

    // Pause/un-pause the beam sound effects/loop based on whether the beam is 'active' or not
    bool beamIsPaused = (beam.GetBeamParts().empty() || beam.GetBeamAlpha() <= 0.0f);
    this->display->GetSound()->SetPauseForAllAttachedSounds(&beam, beamIsPaused);

	debug_output("EVENT: Beam changed");
}

void GameEventsListener::BeamRemovedEvent(const Beam& beam) {
	// Remove the effect for the beam...
	this->display->GetAssets()->GetESPAssets()->RemoveBeamEffect(beam, true);

	// Removed the appropriate sounds for the beam
	switch (beam.GetType()) {

		case Beam::PaddleBeam:
        case Beam::BossBeam:
			this->display->GetSound()->DetachAndStopAllSounds(&beam);
			break;
        
		default:
			assert(false);
			break;
	}

	debug_output("EVENT: Beam removed");
}

void GameEventsListener::TeslaLightningBarrierSpawnedEvent(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock) {
    GameSound* sound = this->display->GetSound();

    // Attach a sound for the lightning...
    Point3D midPt = Point3D::GetMidPoint(newlyOnTeslaBlock.GetPosition3D(), previouslyOnTeslaBlock.GetPosition3D());
    SoundID lightningSoundID = sound->PlaySoundAtPosition(GameSound::TeslaLightningArcLoop, true, midPt);
    sound->SetSoundVolume(lightningSoundID, 0.33f);
    this->teslaLightningSoundIDs.insert(std::make_pair(std::make_pair(&newlyOnTeslaBlock, &previouslyOnTeslaBlock), lightningSoundID));

	Vector3D negHalfLevelDim(-0.5 * this->display->GetModel()->GetLevelUnitDimensions(), 0.0f);
	this->display->GetAssets()->GetESPAssets()->AddTeslaLightningBarrierEffect(newlyOnTeslaBlock, previouslyOnTeslaBlock, negHalfLevelDim);
	debug_output("EVENT: Tesla lightning barrier spawned");
}

void GameEventsListener::TeslaLightningBarrierRemovedEvent(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock) {
    
    // Find and stop the sound for the lightning arc
    TeslaLightningSoundIDMapIter findIter = this->teslaLightningSoundIDs.find(std::make_pair(&newlyOffTeslaBlock, &stillOnTeslaBlock));
    if (findIter == this->teslaLightningSoundIDs.end()) {
        findIter = this->teslaLightningSoundIDs.find(std::make_pair(&stillOnTeslaBlock, &newlyOffTeslaBlock));
    }
    if (findIter != this->teslaLightningSoundIDs.end()) {
        this->display->GetSound()->StopSound(findIter->second);
        this->teslaLightningSoundIDs.erase(findIter);
    }
    else {
        // This should never happen... there should always be a sound ID for each lightning arc!
        assert(false);
    }

	this->display->GetAssets()->GetESPAssets()->RemoveTeslaLightningBarrierEffect(newlyOffTeslaBlock, stillOnTeslaBlock);
	debug_output("EVENT: Tesla lightning barrier removed");
}

void GameEventsListener::LivesChangedEvent(int livesLeftBefore, int livesLeftAfter) {
	LivesLeftHUD* lifeHUD = this->display->GetAssets()->GetLifeHUD();

    if (livesLeftBefore == 0) {
        lifeHUD->Reinitialize();
    }

	// Tell the life HUD about the change in lives
	int lifeDelta  = abs(livesLeftAfter - livesLeftBefore);
	bool livesLost = livesLeftAfter < livesLeftBefore;
	
	if (livesLost) {
		lifeHUD->LivesLost(lifeDelta);
	}
	else {
		lifeHUD->LivesGained(lifeDelta);
	}

	debug_output("EVENT: Lives changed - Before: " << livesLeftBefore << " After: " << livesLeftAfter);
}

void GameEventsListener::BlockIceShatteredEvent(const LevelPiece& block) {
    
    this->display->GetSound()->PlaySoundAtPosition(GameSound::IceShatterEvent, false, block.GetPosition3D());
	this->display->GetAssets()->GetESPAssets()->AddIceBitsBreakEffect(block);

	debug_output("EVENT: Ice shattered");
}

void GameEventsListener::BlockIceCancelledWithFireEvent(const LevelPiece& block) {

    this->display->GetSound()->PlaySoundAtPosition(GameSound::IceMeltedEvent, false, block.GetPosition3D());
    this->display->GetAssets()->GetESPAssets()->AddIceMeltedByFireEffect(block);

    debug_output("EVENT: Frozen block cancelled-out");
}

void GameEventsListener::BlockFireCancelledWithIceEvent(const LevelPiece& block) {

    this->display->GetSound()->PlaySoundAtPosition(GameSound::FireFrozeEvent, false, block.GetPosition3D());
    this->display->GetAssets()->GetESPAssets()->AddFirePutOutByIceEffect(block);

    debug_output("EVENT: Block on fire cancelled-out");
}

void GameEventsListener::ReleaseTimerStartedEvent() {
    this->display->GetAssets()->GetBallReleaseHUD()->TimerStarted();
    debug_output("EVENT: Ball release timer started");
}

void GameEventsListener::PointNotificationEvent(const PointAward& pointAward) {

    PointsHUD* pointsHUD = this->display->GetAssets()->GetPointsHUD();
    pointsHUD->PostPointNotification(pointAward);

    debug_output("EVENT: Point notification");
}

void GameEventsListener::NumStarsChangedEvent(const PointAward* pointAward, int oldNumStars, int newNumStars) {
    UNUSED_PARAMETER(oldNumStars);
    
    // Have a sound for when a star is acquired!
    if (oldNumStars < newNumStars && pointAward != NULL) {
        GameSound* sound = this->display->GetSound();
        GameESPAssets* espAssets = this->display->GetAssets()->GetESPAssets();

        // Check for the special case, where the player has acquired all stars in the level...
        if (newNumStars == 5) {
            sound->PlaySound(GameSound::FiveStarsAcquiredEvent, false);
        }
        else {
            sound->PlaySound(GameSound::StarAcquiredEvent, false);
        }
        espAssets->AddStarAcquiredEffect(pointAward->GetLocation());
    }
    
    PointsHUD* pointHUD = this->display->GetAssets()->GetPointsHUD();
    pointHUD->SetNumStars(this->display->GetCamera(), newNumStars);

    debug_output("EVENT: Number of stars changed");
}

void GameEventsListener::DifficultyChangedEvent(const GameModel::Difficulty& newDifficulty) {
    UNUSED_PARAMETER(newDifficulty);

    debug_output("EVENT: Difficulty changed: " << newDifficulty);
}

void GameEventsListener::ScoreChangedEvent(int newScore) {
    PointsHUD* pointsHUD = this->display->GetAssets()->GetPointsHUD();
    pointsHUD->SetScore(newScore);

	debug_output("EVENT: Score Change: " << newScore);
}

void GameEventsListener::ScoreMultiplierCounterChangedEvent(int oldCounterValue, int newCounterValue) {
    
    if (oldCounterValue < newCounterValue && newCounterValue > 0) {
        // Play sounds as we count up towards higher multipliers by destroying more blocks...
        GameSound* sound = this->display->GetSound();
        switch (newCounterValue) {
            case 1:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc1, false);
                break;
            case 2:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc2, false);
                break;
            case 3:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc3, false);
                break;
            case 4:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc4, false);
                break;
            case 5:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc5, false);
                break;
            case 6:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc6, false);
                break;
            case 7:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc7, false);
                break;
            case 8:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc8, false);
                break;
            case 9:
                sound->PlaySound(GameSound::BlockBrokenMultiplierCounterInc9, false);
                break;

            default:
                // Just ignore after we hit 9...
                break;
        }
    }
    
    PointsHUD* pointsHUD = this->display->GetAssets()->GetPointsHUD();
    pointsHUD->SetMultiplierCounter(newCounterValue);

    debug_output("EVENT: Multiplier Counter Change: " << newCounterValue);
}

void GameEventsListener::ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier,
                                                     const Point2D& position) {
    
    PointsHUD* pointsHUD = this->display->GetAssets()->GetPointsHUD();
    pointsHUD->SetMultiplier(newMultiplier);

    // Indicate the change in multiplier where it happens in the level...
    GameSound* sound = this->display->GetSound();
    if (newMultiplier > 1 && oldMultiplier != newMultiplier) {

        // Play a sound for the increase in multiplier
        if (newMultiplier == 2) {
            sound->PlaySound(GameSound::ScoreMultiplierIncreasedTo2Event, false);
        }
        else if (newMultiplier == 3) {
            sound->PlaySound(GameSound::ScoreMultiplierIncreasedTo3Event, false);

        }
        else if (newMultiplier == 4) {
            sound->PlaySound(GameSound::ScoreMultiplierIncreasedTo4Event, false);
        }
        
        // Don't display the effect if we're in ball or paddle camera mode...
        if (!this->display->GetModel()->GetPlayerPaddle()->GetIsPaddleCameraOn() && !GameBall::GetIsBallCameraOn()) {

            this->display->GetAssets()->GetESPAssets()->AddMultiplierComboEffect(newMultiplier, position, 
                *this->display->GetModel()->GetPlayerPaddle());
        }
    }
    else if (newMultiplier == 1 && oldMultiplier > 1) {
        // The player lost their multiplier, play a sound for that
        sound->PlaySound(GameSound::ScoreMultiplierLostEvent, false);
    }

	debug_output("EVENT: Score Multiplier Change: " << oldMultiplier << " -> " << newMultiplier); 
}

void GameEventsListener::LaserTurretAIStateChangedEvent(const LaserTurretBlock& block,
                                                        LaserTurretBlock::TurretAIState oldState,
                                                        LaserTurretBlock::TurretAIState newState) {

    this->display->GetAssets()->GetCurrentLevelMesh()->LaserTurretAIStateChanged(&block, oldState, newState);
    debug_output("EVENT: Laser Turret AI State Changed.");
}

void GameEventsListener::LaserFiredByTurretEvent(const LaserTurretBlock& block) {
   this->display->GetAssets()->GetCurrentLevelMesh()->LaserFired(&block);
   debug_output("EVENT: Laser Turret AI State Changed.");
}

void GameEventsListener::RocketTurretAIStateChangedEvent(const RocketTurretBlock& block,
                                                         RocketTurretBlock::TurretAIState oldState,
                                                         RocketTurretBlock::TurretAIState newState) {

    this->display->GetAssets()->GetCurrentLevelMesh()->RocketTurretAIStateChanged(&block, oldState, newState);
    debug_output("EVENT: Rocket Turret AI state changed.");
}

void GameEventsListener::RocketFiredByTurretEvent(const RocketTurretBlock& block) {
    this->display->GetAssets()->GetCurrentLevelMesh()->RocketFired(&block);

    Point3D endOfBarrelPt;
    Vector2D barrelDir;
    block.GetBarrelInfo(endOfBarrelPt, barrelDir);

	this->display->GetAssets()->GetESPAssets()->AddCannonFireEffect(endOfBarrelPt, barrelDir);

    debug_output("EVENT: Rocket Turret fired a rocket.");
}

void GameEventsListener::MineTurretAIStateChangedEvent(const MineTurretBlock& block,
                                                       MineTurretBlock::TurretAIState oldState,
                                                       MineTurretBlock::TurretAIState newState) {

    this->display->GetAssets()->GetCurrentLevelMesh()->MineTurretAIStateChanged(&block, oldState, newState);
    debug_output("EVENT: Mine Turret AI state changed.");
}

void GameEventsListener::MineFiredByTurretEvent(const MineTurretBlock& block) {
    this->display->GetAssets()->GetCurrentLevelMesh()->MineFired(&block);

    /*
    // TODO...
    Point3D endOfBarrelPt;
    Vector2D barrelDir;
    block.GetBarrelInfo(endOfBarrelPt, barrelDir);

	this->display->GetAssets()->GetESPAssets()->AddCannonFireEffect(endOfBarrelPt, barrelDir);
    */

    debug_output("EVENT: Mine Turret fired a mine.");
}

void GameEventsListener::RegenBlockLifeChangedEvent(const RegenBlock& block, float lifePercentBefore) {
    this->display->GetAssets()->GetCurrentLevelMesh()->UpdateRegenBlock(&block, lifePercentBefore > block.GetCurrentLifePercent());
}

void GameEventsListener::RegenBlockPreturbedEvent(const RegenBlock& block) {
    if (block.HasInfiniteLife()) {
        this->display->GetAssets()->GetCurrentLevelMesh()->UpdateRegenBlock(&block, true);
    }
}

void GameEventsListener::BossHurtEvent(const BossWeakpoint* hurtPart) {
    Collision::AABB2D partAABB = hurtPart->GetLocalBounds().GenerateAABBFromLines();

    this->display->GetAssets()->GetESPAssets()->AddBossHurtEffect(hurtPart->GetTranslationPt2D(), 
        partAABB.GetWidth(), partAABB.GetHeight());
    this->display->GetAssets()->GetCurrentLevelMesh()->BossHurt();

    this->display->GetModel()->ClearSpecificBeams(Beam::BossBeam);

    debug_output("EVENT: Boss is hurt.");
}

void GameEventsListener::BossAngryEvent(const Boss* boss, const BossBodyPart* angryPart) {
    UNUSED_PARAMETER(boss);

    Collision::AABB2D partAABB = angryPart->GetLocalBounds().GenerateAABBFromLines();

    this->display->GetAssets()->GetESPAssets()->AddBossAngryEffect(angryPart->GetTranslationPt2D(),
        partAABB.GetWidth(), partAABB.GetHeight());
    debug_output("EVENT: Boss is angry.");
}


void GameEventsListener::EffectEvent(const BossEffectEventInfo& effectEvent) {
    BossMesh* bossMesh = this->display->GetAssets()->GetCurrentLevelMesh()->GetBossMesh();
    assert(bossMesh != NULL);

    switch (effectEvent.GetType()) {
        
        case BossEffectEventInfo::PowerChargeInfo: {
            const PowerChargeEffectInfo& powerChargeInfo =
                static_cast<const PowerChargeEffectInfo&>(effectEvent);
            bossMesh->AddBossPowerChargeEffect(powerChargeInfo);
            break;
        }

        case BossEffectEventInfo::ExpandingHaloInfo: {
            const ExpandingHaloEffectInfo& expandingHaloInfo =
                static_cast<const ExpandingHaloEffectInfo&>(effectEvent);
            bossMesh->AddBossExpandingHaloEffect(expandingHaloInfo);
            break;
        }

        case BossEffectEventInfo::SparkBurstInfo: {
            const SparkBurstEffectInfo& sparkBurstInfo =
                static_cast<const SparkBurstEffectInfo&>(effectEvent);
            bossMesh->AddBossSparkBurstEffect(sparkBurstInfo);
            break;
        }

        case BossEffectEventInfo::ElectricitySpasmInfo: {
            const ElectricitySpasmEffectInfo& electricitySpasmInfo =
                static_cast<const ElectricitySpasmEffectInfo&>(effectEvent);
            bossMesh->AddElectricitySpasmEffect(electricitySpasmInfo);
            break;
        }

        case BossEffectEventInfo::PuffOfSmokeInfo: {
            const PuffOfSmokeEffectInfo& puffInfo =
                static_cast<const PuffOfSmokeEffectInfo&>(effectEvent);
            this->display->GetAssets()->GetESPAssets()->AddPuffOfSmokeEffect(puffInfo);            
            break;
        }

        case BossEffectEventInfo::ShockwaveInfo: {
            const ShockwaveEffectInfo& shockwaveInfo =
                static_cast<const ShockwaveEffectInfo&>(effectEvent);
            this->display->GetAssets()->GetESPAssets()->AddShockwaveEffect(shockwaveInfo);  
            break;
        }

        case BossEffectEventInfo::FullscreenFlashInfo: {
            const FullscreenFlashEffectInfo& flashInfo = 
                static_cast<const FullscreenFlashEffectInfo&>(effectEvent);
            this->display->GetAssets()->FullscreenFlashExplosion(flashInfo, this->display->GetCamera());
            break;
        }

        case BossEffectEventInfo::DebrisInfo: {
            const DebrisEffectInfo& debrisInfo = static_cast<const DebrisEffectInfo&>(effectEvent);
            this->display->GetAssets()->GetESPAssets()->AddDebrisEffect(debrisInfo);
            break;
        }

        case BossEffectEventInfo::LaserBeamSightsInfo: {
            const LaserBeamSightsEffectInfo& beamSightsInfo = static_cast<const LaserBeamSightsEffectInfo&>(effectEvent);
            bossMesh->AddLaserBeamSightsEffect(beamSightsInfo);
            break;
        }

        default:
            assert(false);
            break;
    }
}

/**
 * Calculate the volume and shake for a ball of a given size (if we want the size to have an effect on such things).
 */
void GameEventsListener::GetEffectsForBallSize(const GameBall::BallSize& ballSize, float& shakeMagnitude,
                                               float& shakeLength, BBBGameController::VibrateAmount& controllerVibeAmt) {

	switch(ballSize) {

		case GameBall::SmallestSize:
			shakeMagnitude = 0.1f;
			shakeLength = 0.05f;
			//volume = GameSoundAssets::VeryQuietVolume;
			controllerVibeAmt = BBBGameController::VerySoftVibration;
			break;

		case GameBall::SmallerSize:
			shakeMagnitude = 0.15f;
			shakeLength = 0.1f;
			//volume = GameSoundAssets::QuietVolume;
			controllerVibeAmt = BBBGameController::SoftVibration;
			break;

		case GameBall::NormalSize:
			shakeMagnitude = 0.2f;
			shakeLength = 0.15f;
			//volume = GameSoundAssets::NormalVolume;
			controllerVibeAmt = BBBGameController::MediumVibration;
			break;

		case GameBall::BiggerSize:
			shakeMagnitude = 0.33f;
			shakeLength = 0.2f;
			//volume = GameSoundAssets::LoudVolume;
			controllerVibeAmt = BBBGameController::HeavyVibration;
			break;

		case GameBall::BiggestSize:
			shakeMagnitude = 0.5f;
			shakeLength = 0.25f;
			//volume = GameSoundAssets::VeryLoudVolume;
			controllerVibeAmt = BBBGameController::VeryHeavyVibration;
			break;

		default:
			assert(false);
			shakeMagnitude = 0.0f;
			shakeLength = 0.0f;
			//volume = GameSoundAssets::NormalVolume;
			controllerVibeAmt = BBBGameController::NoVibration;
			break;

	}
}