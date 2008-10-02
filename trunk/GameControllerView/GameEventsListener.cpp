#include "GameEventsListener.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "LevelMesh.h"
#include "Onomatoplex.h"

#include "GameOverDisplayState.h"
#include "GameCompleteDisplayState.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/PlayerPaddle.h"

#include "../Utils/Includes.h"

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
	this->display->GetAssets()->LoadLevelAssets(world.GetStyle(), this->display->GetModel()->GetCurrentLevel());
	
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
	debug_output("EVENT: Paddle hit wall - " << soundText);
}

void GameEventsListener::BallDeathEvent(const GameBall& deadBall, int livesLeft) {
	debug_output("EVENT: Ball death, lives left: " << livesLeft);
	
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

void GameEventsListener::BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& block) {

	debug_output("EVENT: Ball-block collision");
	this->display->GetAssets()->GetLevelMesh()->ChangePiece(block);
}

void GameEventsListener::BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) {

	// Use ball speed to determine amount of bounce sound
	GameBall::BallSpeed ballSpd = ball.GetSpeed();
	std::string soundText = "";

	switch (ballSpd) {
		case GameBall::ZeroSpeed :
			// How the hell can there be a collision with no speed?
			assert(false);
			break;
		case GameBall::SlowSpeed :
			soundText = Onomatoplex::Generator::Instance()->Generate(Onomatoplex::BOUNCE, Onomatoplex::NORMAL);
			break;
		case GameBall::NormalSpeed :
			soundText = Onomatoplex::Generator::Instance()->Generate(Onomatoplex::BOUNCE, Onomatoplex::GOOD);
			break;
		case GameBall::FastSpeed :
			soundText = Onomatoplex::Generator::Instance()->Generate(Onomatoplex::BOUNCE, Onomatoplex::AWESOME);
			break;
		default :
			assert(false);
			break;
	}
	debug_output("EVENT: Ball-paddle collision - " << soundText);
}

void GameEventsListener::BlockDestroyedEvent(const LevelPiece& block) {

	// TODO: Transmit data concerning the level of sound needed
	std::string soundText = Onomatoplex::Generator::Instance()->Generate(Onomatoplex::EXPLOSION, Onomatoplex::NORMAL);
	debug_output("EVENT: Block destroyed - " << soundText);
}

void GameEventsListener::ScoreChangedEvent(int amt) {
	debug_output("Score Change: " << amt);
}

void GameEventsListener::ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier) {
	debug_output("Score Multiplier Change - Old Value: " << oldMultiplier << " New Value: " << newMultiplier); 
}