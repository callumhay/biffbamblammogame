/**
 * WindowManager.h
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
    void ShowCursor(bool show);

    bool GetIsFullscreen() const;
    int GetWidth() const;
    int GetHeight() const;

	void GetPossibleResolutionsList(std::vector<std::string>& resolutions) const;
    static void GetPossibleResolutionsList(std::vector<std::pair<int, int> >& resolutionWHPairs);

private:
	static WindowManager* instance;

	// Constants used to setup the window
	static const char* WINDOW_TITLE;
	static const char* ICON_FILEPATH;
	static const unsigned int DEFAULT_VIDEO_FLAGS;
	
	SDL_Surface* videoSurface;
	int bitsPerPixel;
    bool inFullscreenMode;
	
	WindowManager();
	~WindowManager();
};

inline void WindowManager::ShowCursor(bool show) {
    SDL_ShowCursor(show ? 1 : 0);
}

inline bool WindowManager::GetIsFullscreen() const {
    return this->inFullscreenMode;
}

inline int WindowManager::GetWidth() const {
    return this->videoSurface->w;
}

inline int WindowManager::GetHeight() const {
    return this->videoSurface->h;
}

#endif