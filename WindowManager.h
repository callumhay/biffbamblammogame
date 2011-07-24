/**
 * WindowManager.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __WINDOWMANAGER_H__
#define __WINDOWMANAGER_H__

#include "BlammoEngine/BasicIncludes.h"

/**
 * Window manager singleton class - this class manages the windowing
 * system used by the game. Acts as a abstraction away from whatever
 * implementation is being used and provides only the functionality
 * needed for the game.
 */
class WindowManager {

public:
	static WindowManager* GetInstance() {
		if (WindowManager::instance == NULL) {
			WindowManager::instance = new WindowManager();
		}
		return WindowManager::instance;
	}

	static void DeleteInstance() {
		if (WindowManager::instance != NULL) {
			delete WindowManager::instance;
			WindowManager::instance = NULL;
		}
	}

	bool Init(int width, int height, bool isFullscreen);
	void Shutdown();

	bool ToggleFullscreen();

	std::vector<std::string> GetPossibleResolutionsList();

private:
	static WindowManager* instance;

	// Constants used to setup the window
	static const char* WINDOW_TITLE;
	static const char* ICON_FILEPATH;
	static const unsigned int DEFAULT_VIDEO_FLAGS;
	
	SDL_Surface* videoSurface;
	int bitsPerPixel;
	

	WindowManager();
	~WindowManager();

};
#endif