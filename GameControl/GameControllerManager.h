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
	bool ControllersCanStillPlugAndPlay() const;
	void TryToLoadPlugAndPlayControllers();

	//const std::list<BBBGameController*>& GetLoadedGameControllers() const;

	bool ProcessControllers();
	void SyncControllers(double dT);
	void VibrateControllers(double lengthInSeconds, const BBBGameController::VibrateAmount& leftMotorAmt, 
											    const BBBGameController::VibrateAmount& rightMotorAmt);

private:
	static GameControllerManager* instance;

	GameModel* model;
	GameDisplay* display;

	GameControllerManager();
	~GameControllerManager();

	BBBGameController* GetSDLKeyboardGameController();
	//BBBGameController* GetSDLJoystickGameController();
	BBBGameController* GetXBox360Controller(int controllerNum);

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
	for (std::list<BBBGameController*>::iterator iter = this->loadedGameControllers.begin(); iter != this->loadedGameControllers.end();) {
		currController = *iter;
		// Make sure the controller is still connected...
		if (currController->IsConnected()) {
			quit |= currController->ProcessState();
			++iter;
		}
		else {
			// Remove the controller from all lists/maps of game controllers and delete it
			for (std::vector<BBBGameController*>::iterator iter2 = this->gameControllers.begin(); iter2 != this->gameControllers.end(); ++iter2) {
				if (*iter2 == currController) {
					(*iter2) = NULL;
					break;
				}
			}
			delete currController;
			currController = NULL;
			
			iter = this->loadedGameControllers.erase(iter); 
		}
	}

	// Check for other controllers that may have been plugged in since last time we processed them
	this->TryToLoadPlugAndPlayControllers();

	return quit;
}

inline void GameControllerManager::SyncControllers(double dT) {
	static size_t currFrameID = 0;
	BBBGameController* currController;
	for (std::list<BBBGameController*>::iterator iter = this->loadedGameControllers.begin(); iter != this->loadedGameControllers.end(); ++iter) {
		currController = *iter;
		currController->Sync(currFrameID, dT);
	}
	currFrameID++;
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