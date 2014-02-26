/**
 * BallWormholeState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
void BallWormholeState::MoveKeyPressedForPaddle(int dir, float magnitudePercent) {
	UNUSED_PARAMETER(dir);
    UNUSED_PARAMETER(magnitudePercent);
	// TODO
}