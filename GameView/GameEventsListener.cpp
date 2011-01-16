/**
 * GameEventsListener.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameEventsListener.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "LevelMesh.h"
#include "LoadingScreen.h"
#include "GameOverDisplayState.h"
#include "GameCompleteDisplayState.h"
#include "LivesLeftHUD.h"
#include "BallSafetyNetMesh.h"

#include "../Blammopedia.h"

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
#include "../GameModel/PaddleRocketProjectile.h"

#include "../GameControl/GameControllerManager.h"

// Wait times before showing the same effect - these prevent the game view from displaying a whole ton
// of the same effect over and over when the ball hits a bunch of blocks/the paddle in a very small time frame
const long GameEventsListener::EFFECT_WAIT_TIME_BETWEEN_BALL_BLOCK_COLLISIONS_IN_MS		= 100;
const long GameEventsListener::EFFECT_WAIT_TIME_BETWEEN_BALL_PADDLE_COLLISIONS_IN_MS    = 75;
const long GameEventsListener::EFFECT_WAIT_TIME_BETWEEN_BALL_TESLA_COLLISIONS_IN_MS     = 60;

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
	debug_output("EVENT: Game completed");

	// Set the state to the end of game display
	this->display->SetCurrentState(new GameCompleteDisplayState(this->display));

	// Stop world background music (if it's still going)
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldBackgroundMusic);
}

void GameEventsListener::WorldStartedEvent(const GameWorld& world) {
	debug_output("EVENT: World started");
	
	// Show a loading screen for loading up the assets for the next in-game world...
	//unsigned int numLevelsInWorld = world.GetAllLevelsInWorld().size();
	const Camera& camera = this->display->GetCamera();

	LoadingScreen::GetInstance()->StartShowLoadingScreen(camera.GetWindowWidth(), camera.GetWindowHeight(), 2);
	this->display->GetAssets()->LoadWorldAssets(world);
	LoadingScreen::GetInstance()->EndShowingLoadingScreen();

	//this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldBackgroundMusic);
	this->display->AddStateToQueue(DisplayState::WorldStart);
}

void GameEventsListener::WorldCompletedEvent(const GameWorld& world) {
	UNUSED_PARAMETER(world);
	
	// Stop world background music (if it's still going)
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldBackgroundMusic);
	debug_output("EVENT: World completed");
}

void GameEventsListener::LevelStartedEvent(const GameWorld& world, const GameLevel& level) {
	UNUSED_PARAMETER(world);

	// Set up the initial game camera for the level
	//this->display->GetCamera().Reset();
	//this->display->GetCamera().SetTransform(this->display->GetModel()->GetTransformInfo()->GetCameraTransform());

	// Load the level geometry/mesh data for display...
	this->display->GetAssets()->LoadNewLevelMesh(level);

    // Any new blocks that are being revealed in this level should affect the blammopedia...
    //const std::vector<std::vector<LevelPiece*> >& levelPieces = level.GetCurrentLevelLayout();

	// Queue up the state for starting a level - this will display the level name and do proper animations, fade-ins, etc.
	this->display->AddStateToQueue(DisplayState::LevelStart);
	this->display->AddStateToQueue(DisplayState::InGame);

	debug_output("EVENT: Level started");

}

void GameEventsListener::LevelCompletedEvent(const GameWorld& world, const GameLevel& level) {
	UNUSED_PARAMETER(world);
	UNUSED_PARAMETER(level);

	// Kill all effects that may have previously been occuring...
	this->display->GetAssets()->DeactivateMiscEffects();
	
	// Queue up the state for ending a level - this will display the level name and do proper animations, fade-outs, etc.
	this->display->AddStateToQueue(DisplayState::LevelEnd);
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
	GameSoundAssets::SoundVolumeLoudness volume;
	switch(paddleSize) {
		case PlayerPaddle::SmallestSize:
			shakeMagnitude = 0.0f;
			shakeLength = 0.0f;
			volume = GameSoundAssets::VeryQuietVolume;
			controllerVibeAmt = BBBGameController::VerySoftVibration;
			break;
		case PlayerPaddle::SmallerSize:
			shakeMagnitude = 0.1f;
			shakeLength = 0.1f;
			volume = GameSoundAssets::QuietVolume;
			controllerVibeAmt = BBBGameController::SoftVibration;
			break;
		case PlayerPaddle::NormalSize:
			shakeMagnitude = 0.2f;
			shakeLength = 0.15f;
			volume = GameSoundAssets::NormalVolume;
			controllerVibeAmt = BBBGameController::MediumVibration;
			break;
		case PlayerPaddle::BiggerSize:
			shakeMagnitude = 0.33f;
			shakeLength = 0.2f;
			volume = GameSoundAssets::LoudVolume;
			controllerVibeAmt = BBBGameController::HeavyVibration;
			break;
		case PlayerPaddle::BiggestSize:
			shakeMagnitude = 0.5f;
			shakeLength = 0.25f;
			volume = GameSoundAssets::VeryLoudVolume;
			controllerVibeAmt = BBBGameController::VeryHeavyVibration;
			break;
		default:
			assert(false);
			volume = GameSoundAssets::NormalVolume;
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
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundPaddleHitWallEvent, volume);

	debug_output("EVENT: Paddle hit wall - " << soundText);
}

void GameEventsListener::PaddleHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile) {
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

void GameEventsListener::BallDiedEvent(const GameBall& deadBall) {
	debug_output("EVENT: Ball died");
	this->display->GetAssets()->GetESPAssets()->KillAllActiveBallEffects(deadBall);

	// If it's not the last ball then we play the lost ball effect...
	if (this->display->GetModel()->GetGameBalls().size() >= 2) {
		this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundPlayerLostABallEvent);
	}

}

void GameEventsListener::LastBallAboutToDieEvent(const GameBall& lastBallToDie) {
	debug_output("EVENT: Last ball is about to die.");

	this->display->GetAssets()->ActivateLastBallDeathEffects(lastBallToDie);

	// Stop any persistant masks that might currently be activated...
	this->display->GetAssets()->GetSoundAssets()->StopAllActiveMasks(true);

	// Play the sound of the ball spiralling to its most horrible death
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundLastBallSpiralingToDeathMask);

	// Clear out the timers - they no longer matter since the ball is doomed
	this->display->GetAssets()->GetItemAssets()->ClearTimers();
}

void GameEventsListener::LastBallExploded(const GameBall& explodedBall) {
	debug_output("EVENT: Last ball exploded.");

	// Add a cool effect for when the ball explodes
	this->display->GetAssets()->GetESPAssets()->AddBallExplodedEffect(&explodedBall);

	// Stop the spiraling sound mask
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldSoundLastBallSpiralingToDeathMask);
	// Add the sound for the last ball exploding
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundLastBallExplodedEvent, GameSoundAssets::LoudVolume);
}

void GameEventsListener::AllBallsDeadEvent(int livesLeft) {
	debug_output("EVENT: Ball death, lives left: " << livesLeft);
	
	// Kill all effects that may have previously been occuring...
	this->display->GetAssets()->DeactivateLastBallDeathEffects();
	this->display->GetAssets()->GetESPAssets()->KillAllActiveEffects();
		
	// Check to see if it's game over, and switch the display state appropriately
	if (this->display->GetModel()->IsGameOver()) {
		
		// Kill absolutely all remaining visual effects...
		this->display->GetAssets()->DeactivateMiscEffects();
		
		// Stop world background music (if it's still going)
		this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldBackgroundMusic);

		this->display->SetCurrentState(new GameOverDisplayState(this->display));
	}
}

void GameEventsListener::BallSpawnEvent(const GameBall& spawnedBall) {
	UNUSED_PARAMETER(spawnedBall);
	debug_output("EVENT: Ball respawning");
}

void GameEventsListener::BallShotEvent(const GameBall& shotBall) {
	UNUSED_PARAMETER(shotBall);
	debug_output("EVENT: Ball shot");
}

void GameEventsListener::ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block) {

	// Add any visual effects required for when a projectile hits the block
	this->display->GetAssets()->GetESPAssets()->AddBlockHitByProjectileEffect(projectile, block);

	debug_output("EVENT: Projectile-block collision");
}

void GameEventsListener::BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) {
	long currSystemTime = BlammoTime::GetSystemTimeInMillisecs();
	bool doEffects = (currSystemTime - this->timeSinceLastBallBlockCollisionEventInMS) > GameEventsListener::EFFECT_WAIT_TIME_BETWEEN_BALL_BLOCK_COLLISIONS_IN_MS;

	if (doEffects) {
		GameSoundAssets::SoundVolumeLoudness volume = GameSoundAssets::NormalVolume;

		// Add the visual effect for when the ball hits a block
		// We don't do bounce effects for the invisiball... cause then the player would know where it is easier
		if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
			((ball.GetBallType() & GameBall::UberBall) != GameBall::UberBall || !block.UberballBlastsThrough())) {
				this->display->GetAssets()->GetESPAssets()->AddBounceLevelPieceEffect(ball, block);
		}

		// We shake things up if the ball is uber and the block is indestructible...
		if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
			  (ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall && !block.UberballBlastsThrough()) {

			float shakeMagnitude, shakeLength;
			BBBGameController::VibrateAmount controllerVibeAmt;
			GameEventsListener::GetVolAndShakeForBallSize(ball.GetBallSize(), shakeMagnitude, shakeLength, volume, controllerVibeAmt);
			
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

		this->display->GetAssets()->GetSoundAssets()->PlayBallHitBlockEvent(ball, block, volume);
	}

	this->timeSinceLastBallBlockCollisionEventInMS = currSystemTime;
	debug_output("EVENT: Ball-block collision");
}

void GameEventsListener::BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) {
	long currSystemTime = BlammoTime::GetSystemTimeInMillisecs();
	bool doEffect = (currSystemTime - this->timeSinceLastBallPaddleCollisionEventInMS) > 
									EFFECT_WAIT_TIME_BETWEEN_BALL_PADDLE_COLLISIONS_IN_MS;

	if (doEffect) {
		// Add the visual effect for when the ball hits the paddle
		this->display->GetAssets()->GetESPAssets()->AddBouncePaddleEffect(ball, paddle);

		bool ballIsUber = (ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall;
		// Loudness is determined by the current state of the ball...
		GameSoundAssets::SoundVolumeLoudness loudness = GameSoundAssets::NormalVolume;
		BBBGameController::VibrateAmount vibration = BBBGameController::SoftVibration;

		// We shake things up if the ball is uber...
		if (ballIsUber) {
			this->display->GetCamera().SetCameraShake(0.2f, Vector3D(0.8f, 0.2f, 0.0f), 100);
			loudness = GameSoundAssets::VeryLoudVolume;
			vibration = BBBGameController::HeavyVibration;
		}

		if (ball.GetBallSize() > GameBall::NormalSize) {
			loudness = GameSoundAssets::VeryLoudVolume;
			vibration = BBBGameController::MediumVibration;
		}
		else if (ball.GetBallSize() < GameBall::NormalSize) {
			loudness = GameSoundAssets::QuietVolume;
			vibration = BBBGameController::VerySoftVibration;
		}

		GameControllerManager::GetInstance()->VibrateControllers(0.2f, vibration, vibration);

		// Play the sound for when the ball hits the paddle
		if ((paddle.GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle && 
				(paddle.GetPaddleType() & PlayerPaddle::ShieldPaddle) == NULL) {
			this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundStickyBallPaddleCollisionEvent, loudness);
		}
		else if ((paddle.GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
			// sound for paddle shield hit...
		}
		else {
			this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBallPaddleCollisionEvent, loudness);
		}
	}

	this->timeSinceLastBallPaddleCollisionEventInMS = currSystemTime;
	debug_output("EVENT: Ball-paddle collision");
}

void GameEventsListener::BallBallCollisionEvent(const GameBall& ball1, const GameBall& ball2) {
	// Add the effect for ball-ball collision
	this->display->GetAssets()->GetESPAssets()->AddBounceBallBallEffect(ball1, ball2);

	// ... and the sound 
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBallBallCollisionEvent);

	debug_output("EVENT: Ball-ball collision");
}

void GameEventsListener::BallPortalBlockTeleportEvent(const GameBall& ball, const PortalBlock& enterPortal) {
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundPortalTeleportEvent);
	this->display->GetAssets()->GetESPAssets()->AddPortalTeleportEffect(ball.GetBounds().Center(), enterPortal);
	debug_output("EVENT: Ball teleported by portal block");
}

void GameEventsListener::ProjectilePortalBlockTeleportEvent(const Projectile& projectile, const PortalBlock& enterPortal) {
	switch (projectile.GetType()) {
        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
			// TODO?
			break;

		case Projectile::PaddleRocketBulletProjectile:
		case Projectile::CollateralBlockProjectile: {
				Point2D projectileTeleportPos = projectile.GetPosition() + projectile.GetHalfHeight() * projectile.GetVelocityDirection();
				this->display->GetAssets()->GetESPAssets()->AddPortalTeleportEffect(projectileTeleportPos, enterPortal);
			}
			break;

		default:
			assert(false);
			break;
	}
	debug_output("EVENT: Projectile teleported by portal block");
}

void GameEventsListener::BallFiredFromCannonEvent(const GameBall& ball, const CannonBlock& cannonBlock) {
	UNUSED_PARAMETER(ball);

	// Add the blast effect of the ball exiting the cannon
	this->display->GetAssets()->GetESPAssets()->AddCannonFireEffect(cannonBlock);
	// Stop the sound of the cannon rotating
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldSoundCannonBlockRotatingMask);
	// .. and the sound for the blast
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundCannonBlockFiredEvent, GameSoundAssets::LoudVolume);

	debug_output("EVENT: Ball fired out of cannon block");
}


void GameEventsListener::RocketEnteredCannonEvent(const PaddleRocketProjectile& rocket, const CannonBlock& cannonBlock) {
	UNUSED_PARAMETER(rocket);
	UNUSED_PARAMETER(cannonBlock);

	// Suspend certain elements of the rocket projectile until it's fired back out of the cannon...
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldSoundRocketMovingMask);
	// Start the sound of the cannon rotating
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundCannonBlockRotatingMask);
	
	debug_output("EVENT: Rocket entered cannon block");
}

void GameEventsListener::RocketFiredFromCannonEvent(const PaddleRocketProjectile& rocket, const CannonBlock& cannonBlock) {
	UNUSED_PARAMETER(rocket);

	// Add the blast effect of the rocket exiting the cannon
	this->display->GetAssets()->GetESPAssets()->AddCannonFireEffect(cannonBlock);
	// Stop the sound of the cannon rotating
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldSoundCannonBlockRotatingMask);
	// .. and the sound for the blast
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundCannonBlockFiredEvent, GameSoundAssets::LoudVolume);
	// ... and for the rocket moving mask again
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundRocketMovingMask);

	debug_output("EVENT: Rocket fired out of cannon block");
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
		this->display->GetCamera().SetCameraShake(1.2, Vector3D(0.75, 0.1, 0.1), 40);
		GameControllerManager::GetInstance()->VibrateControllers(0.8f, BBBGameController::SoftVibration, BBBGameController::SoftVibration);
	}

	this->timeSinceLastBallTeslaCollisionEventInMS = currSystemTime;
	debug_output("EVENT: Ball hit tesla lightning arc");
}

void GameEventsListener::BlockDestroyedEvent(const LevelPiece& block) {
	bool wasFrozen = block.HasStatus(LevelPiece::IceCubeStatus);

	// Add the effects based on the type of block that is being destroyed...
	switch (block.GetType()) {
		
		case LevelPiece::Breakable:
		case LevelPiece::BreakableTriangle:
		case LevelPiece::Solid:
		case LevelPiece::SolidTriangle:
		case LevelPiece::Tesla:
        case LevelPiece::Switch:
			if (wasFrozen) {
				// Add ice break effect
				this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, block.GetColour());
				//this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundFrozenBlockDestroyedEvent);
			}
			else {
				// Typical break effect for basic breakable blocks
				this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
				// Sound for basic breakable blocks
				this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBasicBlockDestroyedEvent);
			}
			break;

		case LevelPiece::ItemDrop:
			if (wasFrozen) {
				// Add ice break effect
				this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, Colour(0.9f, 0.45f, 0.0f));
				//this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundFrozenBlockDestroyedEvent);
			}
			else {
				// Typical break effect
				this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
				// Sound for basic breakable blocks
				this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBasicBlockDestroyedEvent);
			}
			this->display->GetAssets()->GetCurrentLevelMesh()->RemovePiece(block);
			break;

		case LevelPiece::Bomb:
			if (wasFrozen) {
				// Add ice break effect
				this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, Colour(0.66f, 0.66f, 0.66f));
				//this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundFrozenBlockDestroyedEvent);
			}
			else {
				// Bomb effect - big explosion!
				this->display->GetAssets()->GetESPAssets()->AddBombBlockBreakEffect(block);
				this->display->GetCamera().SetCameraShake(1.2f, Vector3D(1.0f, 0.3f, 0.1f), 110);
				GameControllerManager::GetInstance()->VibrateControllers(1.0f, BBBGameController::HeavyVibration, BBBGameController::HeavyVibration);

				// Sound for bomb explosion
				this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBombBlockDestroyedEvent, GameSoundAssets::LoudVolume);
			}
			break;

		case LevelPiece::Ink: {
				const PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();

				// We do not do any ink blotches while in ball or paddle camera modes, also, if the ink block is frozen
				// then it just shatters...
				bool inkSplatter = !(paddle->GetIsPaddleCameraOn() || GameBall::GetIsBallCameraOn()) && !wasFrozen;

				if (wasFrozen) {
					// Add ice break effect
					this->display->GetAssets()->GetESPAssets()->AddIceCubeBlockBreakEffect(block, GameViewConstants::GetInstance()->INK_BLOCK_COLOUR);
					//this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundFrozenBlockDestroyedEvent);
				}
				else {
					// Emit goo from ink block and make onomata effects
					this->display->GetAssets()->GetESPAssets()->AddInkBlockBreakEffect(this->display->GetCamera(), block, *this->display->GetModel()->GetCurrentLevel(), inkSplatter);
					this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundInkBlockDestroyedEvent);

					if (inkSplatter) {
						// Cover camera in ink with a fullscreen splatter effect
						this->display->GetAssets()->GetFBOAssets()->ActivateInkSplatterEffect();
					}
				}
			}
			break;

		case LevelPiece::Prism:
		case LevelPiece::PrismTriangle:
			this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
			break;

		case LevelPiece::Cannon: {
				this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
				this->display->GetAssets()->GetCurrentLevelMesh()->RemovePiece(block);
			}
			break;

		case LevelPiece::Collateral: {
				// Don't show any effects / play any sounds if the ball is dead/dying
				if (this->display->GetModel()->GetCurrentStateType() != GameState::BallDeathStateType) {
					this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(block);
					this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundCollateralBlockDestroyedEvent);
				}
				this->display->GetAssets()->GetCurrentLevelMesh()->RemovePiece(block);
			}
			break;

		default:
			break;
	}

	// TODO: Transmit data concerning the level of sound needed
	debug_output("EVENT: Block destroyed");
}

void GameEventsListener::BallSafetyNetCreatedEvent() {
	// Play the sound
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBallSafetyNetCreatedEvent);

	// Tell the level mesh about it so it can show any effects for the creation
	// of the safety net mesh
	this->display->GetAssets()->GetCurrentLevelMesh()->BallSafetyNetCreated();

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
	// Play the sound
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBallSafetyNetDestroyedEvent);

	// Tell the level mesh about it so it can show any effects for the destruction
	// of the safety net mesh
	this->display->GetAssets()->GetCurrentLevelMesh()->BallSafetyNetDestroyed(pt);

	// Particle effects for when the safety net is destroyed
	this->display->GetAssets()->GetESPAssets()->AddBallSafetyNetDestroyedEffect(pt);
}

void GameEventsListener::LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
	this->display->GetAssets()->GetCurrentLevelMesh()->ChangePiece(pieceBefore, pieceAfter);
	debug_output("EVENT: LevelPiece changed");
}

void GameEventsListener::LevelPieceStatusAddedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& addedStatus) {
	this->display->GetAssets()->GetCurrentLevelMesh()->LevelPieceStatusAdded(piece, addedStatus);
	debug_output("EVENT: LevelPiece status added");
}

void GameEventsListener::LevelPieceStatusRemovedEvent(const LevelPiece& piece, const LevelPiece::PieceStatus& removedStatus) {
	UNUSED_PARAMETER(removedStatus);
	this->display->GetAssets()->GetCurrentLevelMesh()->LevelPieceAllStatusRemoved(piece);
	debug_output("EVENT: LevelPiece status removed");
}

void GameEventsListener::LevelPieceAllStatusRemovedEvent(const LevelPiece& piece) {
	this->display->GetAssets()->GetCurrentLevelMesh()->LevelPieceAllStatusRemoved(piece);
	debug_output("EVENT: LevelPiece all status removed");
}

void GameEventsListener::ScoreChangedEvent(int amt) {
	debug_output("EVENT: Score Change: " << amt);
}

void GameEventsListener::ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier) {
	debug_output("EVENT: Score Multiplier Change - Old Value: " << oldMultiplier << " New Value: " << newMultiplier); 
}

void GameEventsListener::ItemSpawnedEvent(const GameItem& item) {
	// Play item spawn sound
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundItemSpawnedEvent);

	// We don't show the stars coming off the dropping items if it gets in the way of playing
	// the game - e.g., when in paddle camera mode
	const PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();
	bool showItemDropStars = !paddle->GetIsPaddleCameraOn() && !GameBall::GetIsBallCameraOn();

	// Spawn an item drop effect for the item...
	this->display->GetAssets()->GetESPAssets()->AddItemDropEffect(item, showItemDropStars);

	// Play the item 'moving' mask - plays as the item falls towards the paddle until it leaves play
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundItemMovingMask, GameSoundAssets::VeryQuietVolume);

	debug_output("EVENT: Item Spawned: " << item);
}

void GameEventsListener::ItemRemovedEvent(const GameItem& item) {
	// Stop the item moving mask sound
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldSoundItemMovingMask);

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

	// Play the appropriate sound based on the item acquired by the player
	GameSoundAssets* soundAssets = this->display->GetAssets()->GetSoundAssets();
	switch (item.GetItemDisposition()) {
		case GameItem::Good:
			soundAssets->PlayWorldSound(GameSoundAssets::WorldSoundPowerUpItemActivatedEvent);
			break;

		case GameItem::Neutral:
			soundAssets->PlayWorldSound(GameSoundAssets::WorldSoundPowerNeutralItemActivatedEvent);
			break;

		case GameItem::Bad:
			soundAssets->PlayWorldSound(GameSoundAssets::WorldSoundPowerDownItemActivatedEvent);
			break;

		default:
			assert(false);
			break;
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
    this->display->GetAssets()->GetCurrentLevelMesh()->SwitchActivated(&switchBlock, this->display->GetModel()->GetCurrentLevel());

    //GameSoundAssets* soundAssets = this->display->GetAssets()->GetSoundAssets();
    //this->PlayWorldSound(GameSoundAssets::WorldSoundActionSwitchBlockActivated,  GameSoundAssets::NormalVolume);

    debug_output("EVENT: Switch block activated");
}

void GameEventsListener::ProjectileSpawnedEvent(const Projectile& projectile) {
	// Tell the assets - this will spawn the appropriate sprites/projectiles and effects
	this->display->GetAssets()->AddProjectile(*this->display->GetModel(), projectile);
	debug_output("EVENT: Projectile spawned");
}

void GameEventsListener::ProjectileRemovedEvent(const Projectile& projectile) {
	// Remove the projectile's effect
	this->display->GetAssets()->RemoveProjectile(projectile);
	debug_output("EVENT: Projectile removed");
}

void GameEventsListener::RocketExplodedEvent(const PaddleRocketProjectile& rocket) {
	UNUSED_PARAMETER(rocket);
	this->display->GetAssets()->RocketExplosion(this->display->GetCamera());
	debug_output("EVENT: Rocket exploded");
}

void GameEventsListener::BeamSpawnedEvent(const Beam& beam) {
	// Add an effect for the new beam...
	this->display->GetAssets()->GetESPAssets()->AddBeamEffect(beam);

	// Add the appropriate sounds for the beam
	switch (beam.GetBeamType()) {
		case Beam::PaddleLaserBeam:
			this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundLaserBeamFiringMask);
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
	debug_output("EVENT: Beam changed");
}

void GameEventsListener::BeamRemovedEvent(const Beam& beam) {
	// Remove the effect for the beam...
	this->display->GetAssets()->GetESPAssets()->RemoveBeamEffect(beam);

	// Removed the appropriate sounds for the beam
	switch (beam.GetBeamType()) {
		case Beam::PaddleLaserBeam:
			this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldSoundLaserBeamFiringMask);
			break;
		default:
			assert(false);
			break;
	}

	debug_output("EVENT: Beam removed");
}

void GameEventsListener::TeslaLightningBarrierSpawnedEvent(const TeslaBlock& newlyOnTeslaBlock, const TeslaBlock& previouslyOnTeslaBlock) {
	Vector3D negHalfLevelDim(-0.5 * this->display->GetModel()->GetLevelUnitDimensions(), 0.0f);
	this->display->GetAssets()->GetESPAssets()->AddTeslaLightningBarrierEffect(newlyOnTeslaBlock, previouslyOnTeslaBlock, negHalfLevelDim);
	debug_output("EVENT: Tesla lightning barrier spawned");
}

void GameEventsListener::TeslaLightningBarrierRemovedEvent(const TeslaBlock& newlyOffTeslaBlock, const TeslaBlock& stillOnTeslaBlock) {
	this->display->GetAssets()->GetESPAssets()->RemoveTeslaLightningBarrierEffect(newlyOffTeslaBlock, stillOnTeslaBlock);
	debug_output("EVENT: Tesla lightning barrier removed");
}

void GameEventsListener::LivesChangedEvent(int livesLeftBefore, int livesLeftAfter) {
	
	// Tell the life HUD about the change in lives
	int lifeDelta  = abs(livesLeftAfter - livesLeftBefore);
	bool livesLost = livesLeftAfter < livesLeftBefore;
	
	LivesLeftHUD* lifeHUD = this->display->GetAssets()->GetLifeHUD();
	if (livesLost) {
		lifeHUD->LivesLost(lifeDelta);
	}
	else {
		lifeHUD->LivesGained(lifeDelta);
	}

	debug_output("EVENT: Lives changed - Before: " << livesLeftBefore << " After: " << livesLeftAfter);
}

void GameEventsListener::BlockIceShatteredEvent(const LevelPiece& block) {
	this->display->GetAssets()->GetESPAssets()->AddIceBitsBreakEffect(block);
	debug_output("EVENT: Ice shattered");
}

/**
 * Calculate the volume and shake for a ball of a given size (if we want the size to have an effect on such things).
 */
void GameEventsListener::GetVolAndShakeForBallSize(const GameBall::BallSize& ballSize, float& shakeMagnitude,
																								   float& shakeLength, GameSoundAssets::SoundVolumeLoudness& volume,
																								   BBBGameController::VibrateAmount& controllerVibeAmt) {
	switch(ballSize) {

		case GameBall::SmallestSize:
			shakeMagnitude = 0.1f;
			shakeLength = 0.05f;
			volume = GameSoundAssets::VeryQuietVolume;
			controllerVibeAmt = BBBGameController::VerySoftVibration;
			break;

		case GameBall::SmallerSize:
			shakeMagnitude = 0.15f;
			shakeLength = 0.1f;
			volume = GameSoundAssets::QuietVolume;
			controllerVibeAmt = BBBGameController::SoftVibration;
			break;

		case GameBall::NormalSize:
			shakeMagnitude = 0.2f;
			shakeLength = 0.15f;
			volume = GameSoundAssets::NormalVolume;
			controllerVibeAmt = BBBGameController::MediumVibration;
			break;

		case GameBall::BiggerSize:
			shakeMagnitude = 0.33f;
			shakeLength = 0.2f;
			volume = GameSoundAssets::LoudVolume;
			controllerVibeAmt = BBBGameController::HeavyVibration;
			break;

		case GameBall::BiggestSize:
			shakeMagnitude = 0.5f;
			shakeLength = 0.25f;
			volume = GameSoundAssets::VeryLoudVolume;
			controllerVibeAmt = BBBGameController::VeryHeavyVibration;
			break;

		default:
			assert(false);
			shakeMagnitude = 0.0f;
			shakeLength = 0.0f;
			volume = GameSoundAssets::NormalVolume;
			controllerVibeAmt = BBBGameController::NoVibration;
			break;

	}
}