/**
 * WindowManager.cpp
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

#include "WindowManager.h"

WindowManager* WindowManager::instance = NULL;

const char* WindowManager::WINDOW_TITLE								= "Biff! Bam!! Blammo!?!";
const char* WindowManager::ICON_FILEPATH							= "BiffBamBlammoIcon.bmp";
const unsigned int WindowManager::DEFAULT_VIDEO_FLAGS = SDL_OPENGL | SDL_SWSURFACE;

WindowManager::WindowManager() : videoSurface(NULL), bitsPerPixel(16), inFullscreenMode(false) {
}

WindowManager::~WindowManager() {
	this->Shutdown();
}

/**
 * Initializes the draw window for the game - this will establish
 * the entire video/window system for drawing everything at the given resolution.
 */
bool WindowManager::Init(int width, int height, bool isFullscreen) {
	// Make sure any previous SDL initialization is not active
	if (this->videoSurface != NULL) {
		assert(false);
		return false;
	}

    // Grab a proper list of screen resolutions
    std::vector<std::pair<int, int> > resPairs;
    this->GetPossibleResolutionsList(resPairs);

    // If we can't find the width and height in the list then we set it to whatever the
    // closest pair is...
    int chosenIdx = 0;
    for (std::vector<std::pair<int, int> >::const_iterator iter = resPairs.begin(); iter != resPairs.end(); ++iter) {
        const std::pair<int, int>& currPair = *iter;

        if (width == currPair.first && height == currPair.second) {
            break;
        }
        else if (width <= currPair.first) {
            break;
        }

        chosenIdx++;
    }
    chosenIdx = std::min<int>(chosenIdx, static_cast<int>(resPairs.size())-1);

    int determinedWidth  = resPairs[chosenIdx].first;
    int determinedHeight = resPairs[chosenIdx].second;
    assert(determinedWidth > 0 && determinedHeight > 0);

	// Load SDL, make sure the window is centered
	putenv("SDL_VIDEO_CENTERED=1");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) != 0) {
        debug_output("Unable to initialize SDL: " << SDL_GetError());
		    std::cerr << "Unable to initialize SDL: " << SDL_GetError();
        return false;
    }
	
	// Don't show the mouse cursor if we're in full screen mode
    this->ShowCursor(!isFullscreen);

    SDL_WM_SetCaption(WindowManager::WINDOW_TITLE, WindowManager::WINDOW_TITLE);

    // To use OpenGL, you need to get some information first,
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    if (!info) {
    // This should never happen...
	    debug_output("Video query failed: " << SDL_GetError() << std::endl);
	    std::cerr << "Video query failed: " << SDL_GetError() << std::endl;
    return false;
    }
    this->bitsPerPixel = info->vfmt->BitsPerPixel;

    // Set colour bits...
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  
	// Set colour depth
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // Double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// .. OTHER STUFF HERE

	// Multisampling ?
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, GameDisplay::NUM_MULTISAMPLES);

    // Set the draw surface...
	unsigned int videoFlags = WindowManager::DEFAULT_VIDEO_FLAGS | (isFullscreen ? SDL_FULLSCREEN : NULL);
	this->videoSurface = SDL_SetVideoMode(determinedWidth, determinedHeight, bitsPerPixel, videoFlags);
    
	if (this->videoSurface == NULL) {

		debug_output("Unable to set video mode: " << SDL_GetError());
		std::cerr << "Unable to set video mode: " << SDL_GetError();
        return false;
    }
    this->inFullscreenMode = isFullscreen;
	
    // Draw black
    glViewport(0, 0, determinedWidth, determinedHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapBuffers();

	SDL_Surface* icon = SDL_LoadBMP(WindowManager::ICON_FILEPATH);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 255, 0, 255));
	SDL_WM_SetIcon(icon, NULL);

	return true;
}

/**
 * Shut down the windowing system. Call this before calling Init again.
 */
void WindowManager::Shutdown() {
	SDL_Quit();                 // This will obliterate all surfaces and other SDL objects
	this->videoSurface = NULL;	// To tell this object that video is dead
}

/**
 * Toggles full screen mode on/off for the render window.
 */
bool WindowManager::ToggleFullscreen() {
	if (this->videoSurface == NULL) {
		assert(false);
		return false;
	}
	
	int success = SDL_WM_ToggleFullScreen(this->videoSurface);
    if (success > 0) {
        this->inFullscreenMode = !this->inFullscreenMode;
        return true;
    }

	return false;
}

/**
 * Obtain a list of possible resolutions for the display device
 * being used to display the game.
 */
void WindowManager::GetPossibleResolutionsList(std::vector<std::string>& resolutions) const {

    std::vector<std::pair<int, int> > resPairs;
    this->GetPossibleResolutionsList(resPairs);

    resolutions.clear();
    resolutions.reserve(resPairs.size());

    std::stringstream resolutionStrStream;
    for (std::vector<std::pair<int, int> >::const_iterator iter = resPairs.begin(); iter != resPairs.end(); ++iter) {
        const std::pair<int, int>& resPair = *iter;
        resolutionStrStream.str("");
        resolutionStrStream << resPair.first << " x " << resPair.second;
        resolutions.push_back(resolutionStrStream.str());
    }

    assert(resPairs.size() == resolutions.size());
}

void WindowManager::GetPossibleResolutionsList(std::vector<std::pair<int, int> >& resolutionWHPairs) {
    std::set<std::pair<int, int> > resolutionSet;

    // cross-compatibility issue
#ifdef WIN32
    DEVMODE devMode;
    DWORD count = 0;

    while (EnumDisplaySettings(NULL, count, &devMode) != NULL) {
        // Don't accept resolutions under 800 width or 600 height
        if (devMode.dmPelsWidth < 800 || devMode.dmPelsHeight < 600) {
            count++;
            continue;
        }

        // We use the set as a detector for unique resolutions, and we only insert
        // unique ones into the vector that we will be returning
        resolutionSet.insert(std::make_pair(devMode.dmPelsWidth, devMode.dmPelsHeight));
        count++;
    }
#else
    #error No cross platform code implemented for determining allowed screen resolutions!
    assert(false);
#endif

    resolutionWHPairs.clear();
    resolutionWHPairs.reserve(resolutionSet.size());
    resolutionWHPairs.insert(resolutionWHPairs.end(), resolutionSet.begin(), resolutionSet.end());
    sort(resolutionWHPairs.begin(), resolutionWHPairs.end(), SortingFunctions::ResolutionPairCompare);
}