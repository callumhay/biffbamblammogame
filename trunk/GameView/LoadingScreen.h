/**
 * LoadingScreen.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LOADINGSCREEN_H__
#define __LOADINGSCREEN_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"

class FBObj;
class CgFxBloom;

/**
 * Static class that will spawn a thread to render a loading screen for
 * the game.
 */
class LoadingScreen {
public:
	static const char* ABSURD_LOADING_DESCRIPTION;

	std::vector<std::string> absurdLoadingDescriptions;

	// Obtain the singleton
	static LoadingScreen* GetInstance() {
		if (LoadingScreen::instance == NULL) {
			LoadingScreen::instance = new LoadingScreen();
		}
		return LoadingScreen::instance;
	}
	
	// Delete the singleton
	static void DeleteInstance() {
		if (LoadingScreen::instance != NULL) {
			delete LoadingScreen::instance;
			LoadingScreen::instance = NULL;
		}
	}

	void StartShowLoadingScreen(int width, int height, unsigned int numExpectedUpdates);
	void UpdateLoadingScreen(std::string loadingStr);
	void EndShowingLoadingScreen();

private:
	static LoadingScreen* instance;

	bool loadingScreenOn;							// Whether or not the loading screen is currently being shown
	int width, height;								// The width and height of the window

	unsigned int numExpectedUpdates;	// The number of expected updates (loads) during a full loading screen
	unsigned int numCallsToUpdate;		// The number of actual calls to update the load screen so far
	unsigned int lastRandomAbsurdity;

	// Display-related members
	static const float GAP_PIXELS;						// Gap between text and items on the load screen
	
	static const int LOADING_BAR_WIDTH;				// Width of the loading bar in pixels
	static const int LOADING_BAR_HEIGHT;			// Height of the loading bar in pixels

	TextLabel2D itemLoadingLabel;			// Text label for saying what is being currently loaded

	FBObj* loadingScreenFBO;
	CgFxBloom* bloomEffect;
    Texture2D* starryBG;

	LoadingScreen();
	~LoadingScreen();

	inline static void InitOpenGLForLoadingScreen();
	
	void RenderLoadingScreen();
	void SetupFullscreenEffect(int width, int height);
	void DrawLoadingBar();	
};
#endif