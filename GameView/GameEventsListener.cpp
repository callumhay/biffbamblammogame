#include "GameEventsListener.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "LevelMesh.h"

#include "GameOverDisplayState.h"
#include "GameCompleteDisplayState.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/Onomatoplex.h"
#include "../GameModel/Projectile.h"

#include "../BlammoEngine/BlammoEngine.h"

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

	// Set the world assets
	this->display->GetAssets()->LoadWorldAssets(world.GetStyle());
}

void GameEventsListener::WorldCompletedEvent(const GameWorld& world) {
	debug_output("EVENT: World completed");
}

void GameEventsListener::LevelStartedEvent(const GameWorld& world, const GameLevel& level) {
	debug_output("EVENT: Level started");

	// Load the level assets
	this->display->GetAssets()->LoadLevelAssets(this->display->GetModel()->GetCurrentLevel());
	
	// Set up the initial game camera for the level - figure out where the camera
	// should be to maximize view of all the game pieces
	float distance = level.GetLevelUnitHeight() / (2.0f * tanf(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS * 0.5f))) + 5.0f;
	this->display->GetCamera().Reset();
	this->display->GetCamera().Move(Vector3D(0, 0, distance));
}

void GameEventsListener::LevelCompletedEvent(const GameWorld& world, const GameLevel& level) {
	debug_output("EVENT: Level completed");
}


void GameEventsListener::PaddleHitWallEvent(const Point2D& hitLoc) {
	std::string soundText = Onomatoplex::Generator::Instance()->Generate(Onomatoplex::BOUNCE, Onomatoplex::NORMAL);

	// Do a tiny shake to the camera...
	this->display->GetCamera().SetCameraShake(0.15, Vector3D(0.25, 0.0, 0.0), 20);

	// Add a smacking type effect...
	// TODO

	debug_output("EVENT: Paddle hit wall - " << soundText);
}

void GameEventsListener::BallDeathEvent(const GameBall& deadBall, int livesLeft) {
	debug_output("EVENT: Ball death, lives left: " << livesLeft);
	
	// Kill all effects that may have previously been occuring...
	this->display->GetAssets()->KillAllActiveEffects();
		
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

			this->display->GetAssets()->AddBallBounceESP(this->display->GetCamera(), ball);
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
	this->display->GetAssets()->AddBallBounceESP(this->display->GetCamera(), ball);

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
			// Typical break effect for basic breakable blocks
			this->display->GetAssets()->AddBasicBlockBreakEffect(this->display->GetCamera(), block);
			break;
		case LevelPiece::Bomb:
			// Bomb effect - big explosion!
			this->display->GetAssets()->AddBombBlockBreakEffect(this->display->GetCamera(), block);
			this->display->GetCamera().SetCameraShake(1.2, Vector3D(1.0, 0.3, 0.1), 100);
			break;
		default:
			break;
	}

	// TODO: Transmit data concerning the level of sound needed
	debug_output("EVENT: Block destroyed");
}

void GameEventsListener::LevelPieceChangedEvent(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
	this->display->GetAssets()->GetLevelMesh()->ChangePiece(pieceBefore, pieceAfter);
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
	this->display->GetAssets()->AddItemDropEffect(this->display->GetCamera(), item);

	debug_output("EVENT: Item Spawned: " << item);
}

void GameEventsListener::ItemRemovedEvent(const GameItem& item) {
	// Remove any previous item drop effect
	this->display->GetAssets()->RemoveItemDropEffect(this->display->GetCamera(), item);

	debug_output("EVENT: Item Removed: " << item);
}

void GameEventsListener::ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle) {


	debug_output("EVENT: Item Obtained by Player: " << item);
}

void GameEventsListener::ItemActivatedEvent(const GameItem& item) {
	// Activate the item's effect (if any)
	this->display->GetAssets()->SetItemEffect(item, true);

	debug_output("EVENT: Item Activated: " << item);
}

void GameEventsListener::ItemDeactivatedEvent(const GameItem& item) {
	// Deactivate the item's effect (if any)
	this->display->GetAssets()->SetItemEffect(item, false);

	debug_output("EVENT: Item Deactivated: " << item);
}

void GameEventsListener::ProjectileSpawnedEvent(const Projectile& projectile) {
	// Add the projectile's effect
	this->display->GetAssets()->AddProjectileEffect(this->display->GetCamera(), projectile); 

	debug_output("EVENT: Projectile spawned");
}

void GameEventsListener::ProjectileRemovedEvent(const Projectile& projectile) {
	// Remove the projectile's effect
	this->display->GetAssets()->RemoveProjectileEffect(this->display->GetCamera(), projectile); 
	debug_output("EVENT: Projectile removed");
}