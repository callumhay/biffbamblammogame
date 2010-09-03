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

#include "../GameModel/GameModel.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/Onomatoplex.h"
#include "../GameModel/Projectile.h"
#include "../GameModel/Beam.h"

#include "../GameSound/GameSoundAssets.h"

GameEventsListener::GameEventsListener(GameDisplay* d) : display(d) {
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
	unsigned int numLevelsInWorld = world.GetAllLevelsInWorld().size();
	const Camera& camera = this->display->GetCamera();

	LoadingScreen::GetInstance()->StartShowLoadingScreen(camera.GetWindowWidth(), camera.GetWindowHeight(), 2);
	this->display->GetAssets()->LoadWorldAssets(world);
	LoadingScreen::GetInstance()->EndShowingLoadingScreen();

	//this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldBackgroundMusic);
	this->display->AddStateToQueue(DisplayState::WorldStart);
}

void GameEventsListener::WorldCompletedEvent(const GameWorld& world) {
	debug_output("EVENT: World completed");

	// Stop world background music (if it's still going)
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldBackgroundMusic);

}

void GameEventsListener::LevelStartedEvent(const GameWorld& world, const GameLevel& level) {
	debug_output("EVENT: Level started");

	// Set up the initial game camera for the level
	//this->display->GetCamera().Reset();
	//this->display->GetCamera().SetTransform(this->display->GetModel()->GetTransformInfo()->GetCameraTransform());

	// Load the level geometry/mesh data for display...
	this->display->GetAssets()->LoadNewLevelMesh(level);

	// Queue up the state for starting a level - this will display the level name and do proper animations, fade-ins, etc.
	this->display->AddStateToQueue(DisplayState::LevelStart);
	this->display->AddStateToQueue(DisplayState::InGame);
}

void GameEventsListener::LevelCompletedEvent(const GameWorld& world, const GameLevel& level) {
	debug_output("EVENT: Level completed");

	// Kill all effects that may have previously been occuring...
	this->display->GetAssets()->DeactivateMiscEffects();
	
	// Queue up the state for ending a level - this will display the level name and do proper animations, fade-outs, etc.
	this->display->AddStateToQueue(DisplayState::LevelEnd);
	this->display->SetCurrentStateAsNextQueuedState();
}


void GameEventsListener::PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc) {
	std::string soundText = Onomatoplex::Generator::GetInstance()->Generate(Onomatoplex::BOUNCE, Onomatoplex::NORMAL);

	// Do a camera shake based on the size of the paddle...
	PlayerPaddle::PaddleSize paddleSize = paddle.GetPaddleSize();
	float shakeMagnitude = 0.0f;
	float shakeLength = 0.0f;
	GameSoundAssets::SoundVolumeLoudness volume;
	switch(paddleSize) {
		case PlayerPaddle::SmallestSize:
			shakeMagnitude = 0.0f;
			shakeLength = 0.0f;
			volume = GameSoundAssets::VeryQuietVolume;
			break;
		case PlayerPaddle::SmallerSize:
			shakeMagnitude = 0.1f;
			shakeLength = 0.1f;
			volume = GameSoundAssets::QuietVolume;
			break;
		case PlayerPaddle::NormalSize:
			shakeMagnitude = 0.2f;
			shakeLength = 0.15f;
			volume = GameSoundAssets::NormalVolume;
			break;
		case PlayerPaddle::BiggerSize:
			shakeMagnitude = 0.33f;
			shakeLength = 0.2f;
			volume = GameSoundAssets::LoudVolume;
			break;
		case PlayerPaddle::BiggestSize:
			shakeMagnitude = 0.5f;
			shakeLength = 0.25f;
			volume = GameSoundAssets::VeryLoudVolume;
			break;
		default:
			assert(false);
			break;
	}
	this->display->GetCamera().SetCameraShake(shakeLength, Vector3D(shakeMagnitude, 0.0, 0.0), 20);
	
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
	// TODO ?
	debug_output("EVENT: Paddle shield hit by projectile");
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
	debug_output("EVENT: Ball respawning");
}

void GameEventsListener::BallShotEvent(const GameBall& shotBall) {
	debug_output("EVENT: Ball shot");
}

void GameEventsListener::ProjectileBlockCollisionEvent(const Projectile& projectile, const LevelPiece& block) {

	// Add any visual effects required for when a projectile hits the block
	this->display->GetAssets()->GetESPAssets()->AddBlockHitByProjectileEffect(projectile, block);

	debug_output("EVENT: Projectile-block collision");
}

void GameEventsListener::BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) {
	// Add the visual effect for when the ball hits a block
	// We don't do bounce effects for the invisiball... cause then the player would know where it is easier
	if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
		((ball.GetBallType() & GameBall::UberBall) != GameBall::UberBall || !block.UberballBlastsThrough())) {
			this->display->GetAssets()->GetESPAssets()->AddBounceLevelPieceEffect(ball, block);
	}

	// We shake things up if the ball is uber and the block is indestructible...
	if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
		  (ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall && !block.CanBeDestroyedByBall()) {

		this->display->GetCamera().SetCameraShake(0.2, Vector3D(0.8, 0.1, 0.0), 100);
	}

	this->display->GetAssets()->GetSoundAssets()->PlayBallHitBlockEvent(ball, block);

	debug_output("EVENT: Ball-block collision");
}

void GameEventsListener::BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) {

	// Add the visual effect for when the ball hits the paddle
	this->display->GetAssets()->GetESPAssets()->AddBouncePaddleEffect(ball, paddle);

	bool ballIsUber = (ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall;
	// Loudness is determined by the current state of the ball...
	GameSoundAssets::SoundVolumeLoudness loudness = GameSoundAssets::NormalVolume;

	// We shake things up if the ball is uber...
	if (ballIsUber) {
		this->display->GetCamera().SetCameraShake(0.2, Vector3D(0.8, 0.2, 0.0), 100);
		loudness = GameSoundAssets::VeryLoudVolume;
	}

	if (ball.GetBallSize() > GameBall::NormalSize) {
		loudness = GameSoundAssets::VeryLoudVolume;
	}
	else if (ball.GetBallSize() < GameBall::NormalSize) {
		loudness = GameSoundAssets::QuietVolume;
	}

	// Play the sound for when the ball hits the paddle
	if ((paddle.GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
		this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundStickyBallPaddleCollisionEvent, loudness);
	}
	else {
		this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBallPaddleCollisionEvent, loudness);
	}

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
	// Add the blast effect of the ball exiting the cannon
	this->display->GetAssets()->GetESPAssets()->AddCannonFireEffect(ball, cannonBlock);
	// Stop the sound of the cannon rotating
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldSoundCannonBlockRotatingMask);
	// .. and the sound for it
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundCannonBlockFiredEvent, GameSoundAssets::LoudVolume);


	debug_output("EVENT: Ball fired out of cannon block");
}

void GameEventsListener::BallHitTeslaLightningArcEvent(const GameBall& ball, const TeslaBlock& teslaBlock1, const TeslaBlock& teslaBlock2) {
	// Add the effect(s) for when the ball hits the lightning
	this->display->GetAssets()->GetESPAssets()->AddBallHitLightningArcEffect(ball);
	// Add a tiny camera shake
	this->display->GetCamera().SetCameraShake(1.2, Vector3D(0.75, 0.1, 0.1), 40);

	debug_output("EVENT: Ball hit tesla lightning arc");
}

void GameEventsListener::BlockDestroyedEvent(const LevelPiece& block) {
	
	// Add the effects based on the type of block that is being destroyed...
	switch (block.GetType()) {
		
		case LevelPiece::Breakable:
		case LevelPiece::BreakableTriangle:
		case LevelPiece::Solid:
		case LevelPiece::SolidTriangle:
		case LevelPiece::Tesla:
			// Typical break effect for basic breakable blocks
			this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(this->display->GetCamera(), block);
			// Sound for basic breakable blocks
			this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBasicBlockDestroyedEvent);
			break;

		case LevelPiece::Bomb:
			// Bomb effect - big explosion!
			this->display->GetAssets()->GetESPAssets()->AddBombBlockBreakEffect(this->display->GetCamera(), block);
			this->display->GetCamera().SetCameraShake(1.2, Vector3D(1.0, 0.3, 0.1), 100);

			// Sound for bomb explosion
			this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBombBlockDestroyedEvent, GameSoundAssets::LoudVolume);
			break;

		case LevelPiece::Ink: {
				const PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();

				// We do not do any ink blotches while in ball or paddle camera modes
				bool specialCamModeOn = paddle->GetIsPaddleCameraOn() || GameBall::GetIsBallCameraOn();
				// Emit goo from ink block and make onomata effects
				this->display->GetAssets()->GetESPAssets()->AddInkBlockBreakEffect(this->display->GetCamera(), block, *this->display->GetModel()->GetCurrentLevel(), !specialCamModeOn);
				if (!specialCamModeOn) {
					// Cover camera in ink with a fullscreen splatter effect
					this->display->GetAssets()->GetFBOAssets()->ActivateInkSplatterEffect();
				}

				this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundInkBlockDestroyedEvent);
			}
			break;

		case LevelPiece::Prism:
		case LevelPiece::PrismTriangle:
			this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(this->display->GetCamera(), block);
			break;

		case LevelPiece::Cannon: {
				this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(this->display->GetCamera(), block);
				const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
				this->display->GetAssets()->GetCurrentLevelMesh()->RemovePiece(block);
			}
			break;

		case LevelPiece::Collateral: {
				// Don't show any effects / play any sounds if the ball is dead/dying
				if (this->display->GetModel()->GetCurrentStateType() != GameState::BallDeathStateType) {
					this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(this->display->GetCamera(), block);
					this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundCollateralBlockDestroyedEvent);
				}
				const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
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

// Private helper for when the safety net is destroyed
void GameEventsListener::DestroyBallSafetyNet(const Point2D& pt) {
	// Play the sound
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundBallSafetyNetDestroyedEvent);

	// Tell the level mesh about it so it can show any effects for the destruction
	// of the safety net mesh
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->GetCurrentLevelMesh()->BallSafetyNetDestroyed(pt);

	// Particle effects for when the safety net is destroyed
	this->display->GetAssets()->GetESPAssets()->AddBallSafetyNetDestroyedEffect(pt);
}

void GameEventsListener::LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->GetCurrentLevelMesh()->ChangePiece(pieceBefore, pieceAfter);
	debug_output("EVENT: LevelPiece changed");
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
	this->display->GetAssets()->GetESPAssets()->AddItemDropEffect(this->display->GetCamera(), item, showItemDropStars);

	// Play the item 'moving' mask - plays as the item falls towards the paddle until it leaves play
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldSoundItemMovingMask, GameSoundAssets::VeryQuietVolume);

	debug_output("EVENT: Item Spawned: " << item);
}

void GameEventsListener::ItemRemovedEvent(const GameItem& item) {
	// Stop the item moving mask sound
	this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldSoundItemMovingMask);

	// Remove any previous item drop effect
	this->display->GetAssets()->GetESPAssets()->RemoveItemDropEffect(this->display->GetCamera(), item);

	debug_output("EVENT: Item Removed: " << item);
}

void GameEventsListener::ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle) {
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
	this->display->GetAssets()->ActivateItemEffects(*this->display->GetModel(), item, this->display->GetCamera());

	debug_output("EVENT: Item Activated: " << item);
}

void GameEventsListener::ItemDeactivatedEvent(const GameItem& item) {

	// Deactivate the item's effects (if any)
	this->display->GetAssets()->DeactivateItemEffects(*this->display->GetModel(), item);

	debug_output("EVENT: Item Deactivated: " << item);
}

void GameEventsListener::ItemTimerStartedEvent(const GameItemTimer& itemTimer){
	this->display->GetAssets()->GetItemAssets()->TimerStarted(&itemTimer);

	debug_output("EVENT: Item timer started: " << itemTimer);
}

void GameEventsListener::ItemTimerStoppedEvent(const GameItemTimer& itemTimer) {
	this->display->GetAssets()->GetItemAssets()->TimerStopped(&itemTimer);

	debug_output("EVENT: Item timer stopped/expired: " << itemTimer);
}

void GameEventsListener::ProjectileSpawnedEvent(const Projectile& projectile) {
	// Tell the assets - this will spawn the appropriate sprites/projectiles and effects
	this->display->GetAssets()->AddProjectile(*this->display->GetModel(), projectile);
	debug_output("EVENT: Projectile spawned");
}

void GameEventsListener::ProjectileRemovedEvent(const Projectile& projectile) {
	// Remove the projectile's effect
	this->display->GetAssets()->RemoveProjectile(this->display->GetCamera(), *this->display->GetModel(), projectile);
	debug_output("EVENT: Projectile removed");
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
