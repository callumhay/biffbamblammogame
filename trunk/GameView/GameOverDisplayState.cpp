#include "GameOverDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameAssets.h"

const std::string GameOverDisplayState::GAME_OVER_TEXT = "Game Over";

GameOverDisplayState::GameOverDisplayState(GameDisplay* display) : DisplayState(display) {
	this->gameOverLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), GAME_OVER_TEXT);
	this->gameOverLabel.SetColour(Colour(1, 0, 0));
	this->gameOverLabel.SetDropShadow(Colour(1, 1, 1), 0.08f);
}

GameOverDisplayState::~GameOverDisplayState() {
}

void GameOverDisplayState::RenderFrame(double dT) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	this->gameOverLabel.SetTopLeftCorner(Point2D((static_cast<float>(this->display->GetDisplayWidth()) - this->gameOverLabel.GetLastRasterWidth()) * 0.5f, 
																			         (static_cast<float>(this->display->GetDisplayHeight()) - this->gameOverLabel.GetHeight()) * 0.5f + this->gameOverLabel.GetHeight()));
	this->gameOverLabel.Draw();
}

void GameOverDisplayState::KeyPressed(SDLKey key) {
	// Allow the player to exit the game over screen...
	this->display->SetCurrentState(new MainMenuDisplayState(this->display));
}

void GameOverDisplayState::DisplaySizeChanged(int width, int height) {
}