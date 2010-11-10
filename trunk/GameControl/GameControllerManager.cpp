
#include "GameControllerManager.h"
#include "KeyboardSDLController.h"
#include "XBox360Controller.h"

GameControllerManager* GameControllerManager::instance = NULL;

const size_t GameControllerManager::KEYBOARD_SDL_INDEX = 0;
const size_t GameControllerManager::XBOX_360_INDEX		 = 1;

GameControllerManager::GameControllerManager() : model(NULL), display(NULL) {
	this->gameControllers.resize(2, NULL);
}

GameControllerManager::~GameControllerManager() {
	// Clean up all the controllers...
	for (std::list<BBBGameController*>::iterator iter = this->loadedGameControllers.begin(); iter != this->loadedGameControllers.end(); ++iter) {
		BBBGameController* currController = *iter;
		delete currController;
		currController = NULL;
	}
	this->gameControllers.clear();
	this->loadedGameControllers.clear();
}

void GameControllerManager::InitAllControllers(GameModel* model, GameDisplay* display) {
	assert(model != NULL);
	assert(display != NULL);
	this->model   = model;
	this->display = display;
	this->GetSDLKeyboardGameController();
	this->GetXBox360Controller(0);
}

/**
 * Builds a keyboard controller when using the SDL library.
 */
BBBGameController* GameControllerManager::GetSDLKeyboardGameController() {
	assert(this->model != NULL);
	assert(this->display != NULL);

	if (this->gameControllers[KEYBOARD_SDL_INDEX] == NULL) {
		BBBGameController* sdlKeyboard = new KeyboardSDLController(this->model, this->display);
		this->gameControllers[KEYBOARD_SDL_INDEX] = sdlKeyboard;
		this->loadedGameControllers.push_back(sdlKeyboard);
	}
	return this->gameControllers[KEYBOARD_SDL_INDEX];
}

/**
 * Builds a XBox 360 Game controller when in windows using the XInput library.
 */
BBBGameController* GameControllerManager::GetXBox360Controller(int controllerNum) {
	assert(this->model != NULL);
	assert(this->display != NULL);

#ifdef _WIN32
	if (this->gameControllers[XBOX_360_INDEX] == NULL) {
		// Check for a connected XBox 360 controller of the given controller number
		if (XBox360Controller::IsConnected(controllerNum)) {
			BBBGameController* xBox360Controller = new XBox360Controller(this->model, this->display, controllerNum);
		this->gameControllers[XBOX_360_INDEX] = xBox360Controller;
		this->loadedGameControllers.push_back(xBox360Controller);
		}
	}
#endif

	return this->gameControllers[XBOX_360_INDEX];
}

/**
 * Trys to load any newly introduced plug-and-play controllers.
 */
void GameControllerManager::TryToLoadPlugAndPlayControllers() {
	assert(this->model != NULL);
	assert(this->display != NULL);

	if (this->ControllersCanStillPlugAndPlay()) {
		// Currently the only plug-and-play controller we care about is the XBox360 controller
		GameControllerManager::GetXBox360Controller(0);
	}
}

