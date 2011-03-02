/**
 * GameCompleteDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameCompleteDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameAssets.h"

const char* GameCompleteDisplayState::GAME_COMPLETE_TEXT = "YOU WIN!";

GameCompleteDisplayState::GameCompleteDisplayState(GameDisplay* display) : DisplayState(display) {

	this->gameCompleteLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), GAME_COMPLETE_TEXT);
	this->gameCompleteLabel.SetColour(Colour(0, 1, 0));
	this->gameCompleteLabel.SetDropShadow(Colour(0, 0, 0), 0.08f);
}

GameCompleteDisplayState::~GameCompleteDisplayState() {
}

void GameCompleteDisplayState::RenderFrame(double dT) {
	UNUSED_PARAMETER(dT);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	const Camera& camera = this->display->GetCamera();
	this->gameCompleteLabel.SetTopLeftCorner(
		Point2D((static_cast<float>(camera.GetWindowWidth()) - this->gameCompleteLabel.GetLastRasterWidth()) * 0.5f, 
						(static_cast<float>(camera.GetWindowHeight()) - this->gameCompleteLabel.GetHeight()) * 0.5f + this->gameCompleteLabel.GetHeight()));
	this->gameCompleteLabel.Draw();

}

void GameCompleteDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
	UNUSED_PARAMETER(pressedButton);
	this->display->SetCurrentState(new MainMenuDisplayState(this->display));
}