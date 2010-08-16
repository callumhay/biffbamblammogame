#include "GameCompleteDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameAssets.h"

const std::string GameCompleteDisplayState::GAME_COMPLETE_TEXT = "YOU WIN!";

GameCompleteDisplayState::GameCompleteDisplayState(GameDisplay* display) : DisplayState(display) {

	this->gameCompleteLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), GAME_COMPLETE_TEXT);
	this->gameCompleteLabel.SetColour(Colour(0, 1, 0));
	this->gameCompleteLabel.SetDropShadow(Colour(0, 0, 0), 0.08f);
}

GameCompleteDisplayState::~GameCompleteDisplayState() {
}

void GameCompleteDisplayState::RenderFrame(double dT) {
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	const Camera& camera = this->display->GetCamera();
	this->gameCompleteLabel.SetTopLeftCorner(
		Point2D((static_cast<float>(camera.GetWindowWidth()) - this->gameCompleteLabel.GetLastRasterWidth()) * 0.5f, 
						(static_cast<float>(camera.GetWindowHeight()) - this->gameCompleteLabel.GetHeight()) * 0.5f + this->gameCompleteLabel.GetHeight()));
	this->gameCompleteLabel.Draw();

}

void GameCompleteDisplayState::KeyPressed(SDLKey key, SDLMod modifier) {
	this->display->SetCurrentState(new MainMenuDisplayState(this->display));
}

void GameCompleteDisplayState::KeyReleased(SDLKey key, SDLMod modifier) {
}

void GameCompleteDisplayState::DisplaySizeChanged(int width, int height) {
}