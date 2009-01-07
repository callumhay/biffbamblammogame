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

void GameEventsListener::BallBlockCollisionEvent(const GameBall& ball, const LevelPiece& blockBefore, const LevelPiece& blockAfter) {
	assert(blockBefore.GetHeightIndex() == blockAfter.GetHeightIndex());
	assert(blockBefore.GetWidthIndex() == blockAfter.GetWidthIndex());

	// Add the visual effect for when the ball hits a block (if it isn't an uber ball and a solid block)
	if ((ball.GetBallType() & GameBall::UberBall) != GameBall::UberBall ||
		blockAfter.GetType() == LevelPiece::Solid) {

		this->display->GetAssets()->AddBallBounceESP(ball);
	}

	debug_output("EVENT: Ball-block collision");
	this->display->GetAssets()->GetLevelMesh()->ChangePiece(blockBefore, blockAfter);
}

void GameEventsListener::BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle) {

	// Add the visual effect for when the ball hits the paddle
	this->display->GetAssets()->AddBallBounceESP(ball);

	debug_output("EVENT: Ball-paddle collision");
}

void GameEventsListener::BlockDestroyedEvent(const LevelPiece& block) {
	// Add the visual effect for when a ball breaks a typical block
	this->display->GetAssets()->AddBasicBlockBreakEffect(block);

	// TODO: Transmit data concerning the level of sound needed
	std::string soundText = Onomatoplex::Generator::Instance()->Generate(Onomatoplex::EXPLOSION, Onomatoplex::NORMAL);
	debug_output("EVENT: Block destroyed - " << soundText);
}

void GameEventsListener::ScoreChangedEvent(int amt) {
	debug_output("EVENT: Score Change: " << amt);
}

void GameEventsListener::ScoreMultiplierChangedEvent(int oldMultiplier, int newMultiplier) {
	debug_output("EVENT: Score Multiplier Change - Old Value: " << oldMultiplier << " New Value: " << newMultiplier); 
}

void GameEventsListener::ItemSpawnedEvent(const GameItem& item) {
	debug_output("EVENT: Item Spawned: " << item);
}

void GameEventsListener::ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle) {
	debug_output("EVENT: Item Obtained by Player: " << item);
}