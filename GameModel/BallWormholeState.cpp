#include "BallWormholeState.h"
#include "GameModel.h"

BallWormholeState::BallWormholeState(GameModel* gm) : GameState(gm), wormholeBall(NULL) {
	// Save the initial states of all relevant objects in the current game model -
	// these objects will be treated very differently while the wormhole state is active...

	// Save the state of the game balls
	std::list<GameBall*>& gameBallList = this->gameModel->GetGameBalls();
	this->originalBalls = gameBallList;

	// Grab the ball with the camera in it
	const GameBall* currentBallCamBall = GameBall::GetBallCameraBall();
	assert(currentBallCamBall != NULL);

	// Clear the balls from the game model and add a single new ball with a position at the start 
	// of the wormhole (the origin) and the proper properties
	gameBallList.clear();

	this->wormholeBall = new GameBall(*currentBallCamBall);
	this->wormholeBall->SetCenterPosition(Point3D(0, 0, 0));
	this->wormholeBall->SetColour(ColourRGBA(1,1,1,1));
	gameBallList.push_back(this->wormholeBall);

	// Handle the camera in the display state... not in the transform mgr!!!
}

BallWormholeState::~BallWormholeState() {
	// Restore the state of the ball(s) as it was before going into the wormhole states
	assert(this->originalBalls.size() > 0);
	this->gameModel->GetGameBalls() = this->originalBalls;

	// Clean up the wormhole ball
	assert(this->wormholeBall != NULL);
	delete this->wormholeBall;
	this->wormholeBall = NULL;
}

void BallWormholeState::Tick(double seconds) {
	UNUSED_PARAMETER(seconds);

	// We can be in several 'sub' states:
	// a) entering the wormhole
	// b) travelling through the wormhole tunnels
	// c) exiting the wormhole



}

// The player can manipulate the direction of the ball as it moves through
// the wormhole by pushing it up, down, left or right in order to accomplish
// tasks and solve puzzles...
void BallWormholeState::MovePaddleKeyPressed(const PlayerPaddle::PaddleMovement& paddleMovement) {
	UNUSED_PARAMETER(paddleMovement);

	// TODO
}