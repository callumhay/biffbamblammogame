#ifndef __GAMECONTROLLERMANAGER_H__
#define __GAMECONTROLLERMANAGER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "BBBGameController.h"

class GameModel;
class GameDisplay;

/**
 * Builds various control types for use with a game and does so while
 * keeping the implementation completely obfuscated.
 */
class GameControllerManager {
public:
	static GameControllerManager* GetInstance() {
		if (GameControllerManager::instance == NULL) {
			GameControllerManager::instance = new GameControllerManager();
		}
		return GameControllerManager::instance;
	}

	static void DeleteInstance() {
		if (GameControllerManager::instance != NULL) {
			delete GameControllerManager::instance;
			GameControllerManager::instance = NULL;
		}
	}

	void InitAllControllers(GameModel* model, GameDisplay* display);
	BBBGameController* GetSDLKeyboardGameController(GameModel* model, GameDisplay* display);
	//BBBGameController* GetSDLJoystickGameController();
	BBBGameController* GetXBox360Controller(GameModel* model, GameDisplay* display, int controllerNum);

	bool ControllersCanStillPlugAndPlay() const;
	void TryToLoadPlugAndPlayControllers(GameModel* model, GameDisplay* display);

	//const std::list<BBBGameController*>& GetLoadedGameControllers() const;

	bool ProcessControllers();
	void SyncControllers(double dT);
	void VibrateControllers(double lengthInSeconds, const BBBGameController::VibrateAmount& leftMotorAmt, 
											    const BBBGameController::VibrateAmount& rightMotorAmt);

private:
	static GameControllerManager* instance;

	GameControllerManager();
	~GameControllerManager();

	static const size_t KEYBOARD_SDL_INDEX;
	static const size_t XBOX_360_INDEX;
	std::vector<BBBGameController*> gameControllers;
	std::list<BBBGameController*> loadedGameControllers;

	DISALLOW_COPY_AND_ASSIGN(GameControllerManager);
};

inline bool GameControllerManager::ControllersCanStillPlugAndPlay() const {
	return (this->gameControllers[XBOX_360_INDEX] == NULL);
}

/**
 * Process the current state on all controllers in this manager.
 * Returns: true on a quit signal from any of the controllers.
 */
inline bool GameControllerManager::ProcessControllers() {
	bool quit = false;
	BBBGameController* currController;
	for (std::list<BBBGameController*>::iterator iter = this->loadedGameControllers.begin(); iter != this->loadedGameControllers.end(); ++iter) {
		currController = *iter;
		quit |= currController->ProcessState();
	}

	// TODO: Check for other controllers here...

	return quit;
}

inline void GameControllerManager::SyncControllers(double dT) {
	BBBGameController* currController;
	for (std::list<BBBGameController*>::iterator iter = this->loadedGameControllers.begin(); iter != this->loadedGameControllers.end(); ++iter) {
		currController = *iter;
		currController->Sync(dT);
	}
}

inline void GameControllerManager::VibrateControllers(double lengthInSeconds, 
																											const BBBGameController::VibrateAmount& leftMotorAmt, 
																											const BBBGameController::VibrateAmount& rightMotorAmt) {
	BBBGameController* currController;
	for (std::list<BBBGameController*>::iterator iter = this->loadedGameControllers.begin(); iter != this->loadedGameControllers.end(); ++iter) {
		currController = *iter;
		currController->Vibrate(lengthInSeconds, leftMotorAmt, rightMotorAmt);
	}
}


//inline const std::list<BBBGameController*>& GameControllerManager::GetLoadedGameControllers() const {
//	return this->loadedGameControllers;
//}

#endif // __GAMECONTROLLERMANAGER_H__