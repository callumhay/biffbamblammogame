/**
 * GameOverDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameOverDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameAssets.h"

const char* GameOverDisplayState::GAME_OVER_TEXT = "Game Over";

GameOverDisplayState::GameOverDisplayState(GameDisplay* display) : DisplayState(display) {
	this->gameOverLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), GAME_OVER_TEXT);
	this->gameOverLabel.SetColour(Colour(1, 0, 0));
	this->gameOverLabel.SetDropShadow(Colour(1, 1, 1), 0.08f);
}

GameOverDisplayState::~GameOverDisplayState() {
}

void GameOverDisplayState::RenderFrame(double dT) {
	UNUSED_PARAMETER(dT);
	const Camera& camera = this->display->GetCamera();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	this->gameOverLabel.SetTopLeftCorner((static_cast<float>(camera.GetWindowWidth()) - this->gameOverLabel.GetLastRasterWidth()) * 0.5f, 
                                         (static_cast<float>(camera.GetWindowHeight()) - this->gameOverLabel.GetHeight()) * 0.5f + this->gameOverLabel.GetHeight());
	this->gameOverLabel.Draw();
}

void GameOverDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
	UNUSED_PARAMETER(pressedButton);

	// Allow the player to exit the game over screen...
	this->display->SetCurrentState(new MainMenuDisplayState(this->display));
}