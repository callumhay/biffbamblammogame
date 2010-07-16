#include "StartGameDisplayState.h"
#include "InGameDisplayState.h"

#include "GameDisplay.h"

#include "../GameModel/GameModel.h"

StartGameDisplayState::StartGameDisplayState(GameDisplay* display) : DisplayState(display) {
	// Load all the initial stuffs for the game
	this->display->GetModel()->BeginOrRestartGame();
}

StartGameDisplayState::~StartGameDisplayState() {
}

void StartGameDisplayState::RenderFrame(double dT) {
	
	// TODO: Add an actual animation here...
	this->display->SetCurrentState(new InGameDisplayState(this->display));
}

void StartGameDisplayState::KeyPressed(SDLKey key, SDLMod modifier) {
	// Allow the user to skip the animation by pressing a key
	this->display->SetCurrentState(new InGameDisplayState(this->display));
}

void StartGameDisplayState::KeyReleased(SDLKey key, SDLMod modifier) {
}

void StartGameDisplayState::DisplaySizeChanged(int width, int height) {
}