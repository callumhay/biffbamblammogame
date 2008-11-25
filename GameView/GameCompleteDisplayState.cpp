#include "GameCompleteDisplayState.h"
#include "MainMenuDisplayState.h"

#include "GameDisplay.h"
#include "GameAssets.h"

#include "../BlammoEngine/BlammoEngine.h"

const std::string GameCompleteDisplayState::GAME_COMPLETE_TEXT = "YOU WIN!";

GameCompleteDisplayState::GameCompleteDisplayState(GameDisplay* display) : DisplayState(display) {

	this->gameCompleteLabel = TextLabel2D(this->display->GetAssets()->GetFont(GameAssets::ExplosionBoom, GameAssets::Huge), GAME_COMPLETE_TEXT);
	this->gameCompleteLabel.SetColour(Colour(0, 1, 0));
	this->gameCompleteLabel.SetDropShadow(Colour(0, 0, 0), 0.08f);
}

GameCompleteDisplayState::~GameCompleteDisplayState() {
}

void GameCompleteDisplayState::RenderFrame(double dT) {
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	this->gameCompleteLabel.SetTopLeftCorner(
		Point2D((static_cast<float>(this->display->GetDisplayWidth()) - this->gameCompleteLabel.GetLastRasterWidth()) * 0.5f, 
						(static_cast<float>(this->display->GetDisplayHeight()) - this->gameCompleteLabel.GetHeight()) * 0.5f + this->gameCompleteLabel.GetHeight()));
	this->gameCompleteLabel.Draw();

}

void GameCompleteDisplayState::KeyPressed(unsigned char key) {
	this->display->SetCurrentState(new MainMenuDisplayState(this->display));
}

void GameCompleteDisplayState::DisplaySizeChanged(int width, int height) {
}