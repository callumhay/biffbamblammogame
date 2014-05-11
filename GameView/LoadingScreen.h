/**
 * LoadingScreen.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LOADINGSCREEN_H__
#define __LOADINGSCREEN_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"

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
	void UpdateLoadingScreenWithRandomLoadStr();
    void UpdateLoadingScreen(const std::string& loadingStr);

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
    Texture2D* starryBG;

	LoadingScreen();
	~LoadingScreen();

	inline static void InitOpenGLForLoadingScreen();
	
	void RenderLoadingScreen();
	void DrawLoadingBar();	

    DISALLOW_COPY_AND_ASSIGN(LoadingScreen);
};

inline void LoadingScreen::UpdateLoadingScreenWithRandomLoadStr() {
    this->UpdateLoadingScreen(ABSURD_LOADING_DESCRIPTION);
}

#endif