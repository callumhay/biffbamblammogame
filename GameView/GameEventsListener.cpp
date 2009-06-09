#include "GameEventsListener.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "LevelMesh.h"
#include "LoadingScreen.h"
#include "GameOverDisplayState.h"
#include "GameCompleteDisplayState.h"
#include "LivesLeftHUD.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/Onomatoplex.h"
#include "../GameModel/Projectile.h"

GameEventsListener::GameEventsListener(GameDisplay* d) : display(d) {
	assert(d != NULL);
}

GameEventsListener::~GameEventsListener() {
}

void GameEventsListener::GameCompletedEvent() {
	debug_output("EVENT: Game completed");

	// Set the state to the end of game display
	this->display->SetCurrentState(new GameCompleteDisplayState(this->display));
}

void GameEventsListener::WorldStartedEvent(const GameWorld& world) {
	debug_output("EVENT: World started");
	
	// Show a loading screen for loading up the assets for the next in-game world...
	unsigned int numLevelsInWorld = world.GetAllLevelsInWorld().size();
	LoadingScreen::GetInstance()->StartShowLoadingScreen(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), numLevelsInWorld + 1);
	this->display->GetAssets()->LoadWorldAssets(&world);
	LoadingScreen::GetInstance()->EndShowingLoadingScreen();
}

void GameEventsListener::WorldCompletedEvent(const GameWorld& world) {
	debug_output("EVENT: World completed");
}

void GameEventsListener::LevelStartedEvent(const GameWorld& world, const GameLevel& level) {
	debug_output("EVENT: Level started");

	// Set up the initial game camera for the level - figure out where the camera
	// should be to maximize view of all the game pieces
	float distance = level.GetLevelUnitHeight() / (2.0f * tanf(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS * 0.5f))) + 5.0f;
	this->display->GetCamera().Reset();
	this->display->GetCamera().Move(Vector3D(0, 0, distance));
}

void GameEventsListener::LevelCompletedEvent(const GameWorld& world, const GameLevel& level) {
	debug_output("EVENT: Level completed");

	// Kill all effects that may have previously been occuring...
	this->display->GetAssets()->GetESPAssets()->KillAllActiveEffects();
}


void GameEventsListener::PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc) {
	std::string soundText = Onomatoplex::Generator::GetInstance()->Generate(Onomatoplex::BOUNCE, Onomatoplex::NORMAL);

	// Do a camera shake based on the size of the paddle...
	PlayerPaddle::PaddleSize paddleSize = paddle.GetPaddleSize();
	float shakeMagnitude = 0.0f;
	float shakeLength = 0.0f;
	switch(paddleSize) {
		case PlayerPaddle::SmallestSize:
			shakeMagnitude = 0.0f;
			shakeLength = 0.0f;
			break;
		case PlayerPaddle::SmallerSize:
			shakeMagnitude = 0.1f;
			shakeLength = 0.1f;
			break;
		case PlayerPaddle::NormalSize:
			shakeMagnitude = 0.2f;
			shakeLength = 0.15f;
			break;
		case PlayerPaddle::BiggerSize:
			shakeMagnitude = 0.33f;
			shakeLength = 0.2f;
			break;
		case PlayerPaddle::BiggestSize:
			shakeMagnitude = 0.5f;
			shakeLength = 0.25f;
			break;
		default:
			assert(false);
			break;
	}
	this->display->GetCamera().SetCameraShake(shakeLength, Vector3D(shakeMagnitude, 0.0, 0.0), 20);
	
	// Add a smacking type effect...
	this->display->GetAssets()->GetESPAssets()->AddPaddleHitWallEffect(paddle, hitLoc);

	debug_output("EVENT: Paddle hit wall - " << soundText);
}

void GameEventsListener::BallDiedEvent(const GameBall& deadBall) {
	debug_output("EVENT: Ball died");
	this->display->GetAssets()->GetESPAssets()->KillAllActiveBallEffects(deadBall);
}

void GameEventsListener::AllBallsDeadEvent(int livesLeft) {
	debug_output("EVENT: Ball death, lives left: " << livesLeft);
	
	// Kill all effects that may have previously been occuring...
	this->display->GetAssets()->GetESPAssets()->KillAllActiveEffects();
		
	// Check to see if it's game over, and switch the display state appropriately
	if (this->display->GetModel()->IsGameOver()) {
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
	debug_output("EVENT: Projectile-block collision");
}

void GameEventsListener::BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) {
	// Add the visual effect for when the ball hits a block
	// We don't do bounce effects for the invisiball... cause then the player would know where it is easier
	if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
		((ball.GetBallType() & GameBall::UberBall) != GameBall::UberBall || !block.UberballBlastsThrough())) {

			this->display->GetAssets()->GetESPAssets()->AddBallBounceEffect(this->display->GetCamera(), ball);
	}

	// We shake things up if the ball is uber and the block is indestructible...
	if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
		  (ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall && !block.CanBeDestroyed()) {

		this->display->GetCamera().SetCameraShake(0.2, Vector3D(0.8, 0.1, 0.0), 100);
	}

	debug_output("EVENT: Ball-block collision");
}

void GameEventsListener::BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) {

	// Add the visual effect for when the ball hits the paddle
	this->display->GetAssets()->GetESPAssets()->AddBallBounceEffect(this->display->GetCamera(), ball);

	// We shake things up if the ball is uber...
	if ((ball.GetBallType() & GameBall::InvisiBall) != GameBall::InvisiBall &&
		  (ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall) {
		this->display->GetCamera().SetCameraShake(0.2, Vector3D(0.8, 0.2, 0.0), 100);
	}

	debug_output("EVENT: Ball-paddle collision");
}

void GameEventsListener::BlockDestroyedEvent(const LevelPiece& block) {
	
	// Add the effects based on the type of block that is being destroyed...
	switch (block.GetType()) {
		
		case LevelPiece::Breakable:
		case LevelPiece::BreakableTriangle:
			// Typical break effect for basic breakable blocks
			this->display->GetAssets()->GetESPAssets()->AddBasicBlockBreakEffect(this->display->GetCamera(), block);
			break;

		case LevelPiece::Bomb:
			// Bomb effect - big explosion!
			this->display->GetAssets()->GetESPAssets()->AddBombBlockBreakEffect(this->display->GetCamera(), block);
			this->display->GetCamera().SetCameraShake(1.2, Vector3D(1.0, 0.3, 0.1), 100);
			break;

		case LevelPiece::Ink:
			// Emit goo from ink block and make onomata effects
			this->display->GetAssets()->GetESPAssets()->AddInkBlockBreakEffect(this->display->GetCamera(), block, *this->display->GetModel()->GetCurrentLevel());
			// TODO: Cover camera in ink with a fullscreen splatter effect
			break;
		default:
			break;
	}

	// TODO: Transmit data concerning the level of sound needed
	debug_output("EVENT: Block destroyed");
}

void GameEventsListener::BallSafetyNetCreatedEvent() {
	// Tell the level mesh about it so it can show any effects for the creation
	// of the safety net mesh
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->GetLevelMesh(currLevel)->BallSafetyNetCreated();

	debug_output("EVENT: Ball safety net created");
}

void GameEventsListener::BallSafetyNetDestroyedEvent(const GameBall& ball) {
	
	// Tell the level mesh about it so it can show any effects for the destruction
	// of the safety net mesh
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->GetLevelMesh(currLevel)->BallSafetyNetDestroyed(ball);

	// Particle effects for when the safety net is destroyed
	this->display->GetAssets()->GetESPAssets()->AddBallSafetyNetDestroyedEffect(ball);

	debug_output("EVENT: Ball safety net destroyed");
}

void GameEventsListener::LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->GetLevelMesh(currLevel)->ChangePiece(pieceBefore, pieceAfter);
	debug_output("EVENT: LevelPiece changed");
}

void GameEventsListener::ScoreChangedEvent(int amt) {
	debug_output("EVENT: Score Change: " << amt);
}

void GameEventsListener::ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier) {
	debug_output("EVENT: Score Multiplier Change - Old Value: " << oldMultiplier << " New Value: " << newMultiplier); 
}

void GameEventsListener::ItemSpawnedEvent(const GameItem& item) {
	// Spawn an item drop effect for the item...
	this->display->GetAssets()->GetESPAssets()->AddItemDropEffect(this->display->GetCamera(), item);

	debug_output("EVENT: Item Spawned: " << item);
}

void GameEventsListener::ItemRemovedEvent(const GameItem& item) {
	// Remove any previous item drop effect
	this->display->GetAssets()->GetESPAssets()->RemoveItemDropEffect(this->display->GetCamera(), item);

	debug_output("EVENT: Item Removed: " << item);
}

void GameEventsListener::ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle) {
	
	this->display->GetAssets()->GetESPAssets()->AddItemAcquiredEffect(this->display->GetCamera(), paddle, item);
	debug_output("EVENT: Item Obtained by Player: " << item);
}

void GameEventsListener::ItemActivatedEvent(const GameItem& item) {
	
	// Activate the item's effects (if any)
	this->display->GetAssets()->ActivateItemEffects(*this->display->GetModel(), item, this->display->GetCamera());

	debug_output("EVENT: Item Activated: " << item);
}

void GameEventsListener::ItemDeactivatedEvent(const GameItem& item) {

	// Deactivate the item's effects (if any)
	this->display->GetAssets()->DeactivateItemEffects(*this->display->GetModel(), item);

	debug_output("EVENT: Item Deactivated: " << item);
}

void GameEventsListener::ProjectileSpawnedEvent(const Projectile& projectile) {
	// Add the projectile's effect
	this->display->GetAssets()->GetESPAssets()->AddProjectileEffect(this->display->GetCamera(), projectile); 

	// Add any other view-related effects for the given projectile
	switch (projectile.GetType()) {
		case Projectile::PaddleLaserProjectile:
			// Have the laser gun attachment move downwards in reaction to the laser being shot
			this->display->GetAssets()->AnimatePaddleLaserAttachment(*this->display->GetModel()->GetPlayerPaddle());
			break;
		default:
			break;
	}

	debug_output("EVENT: Projectile spawned");
}

void GameEventsListener::ProjectileRemovedEvent(const Projectile& projectile) {
	// Remove the projectile's effect
	this->display->GetAssets()->GetESPAssets()->RemoveProjectileEffect(this->display->GetCamera(), projectile);

	debug_output("EVENT: Projectile removed");
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
