/**
 * WindowManager.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "WindowManager.h"

WindowManager* WindowManager::instance = NULL;

const char* WindowManager::WINDOW_TITLE								= "Biff! Bam!! Blammo!?!";
const char* WindowManager::ICON_FILEPATH							= "BiffBamBlammoIcon.bmp";
const unsigned int WindowManager::DEFAULT_VIDEO_FLAGS = SDL_OPENGL | SDL_SWSURFACE;

WindowManager::WindowManager() : videoSurface(NULL), bitsPerPixel(16) {
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

	// Load SDL, make sure the window is centered
	putenv("SDL_VIDEO_CENTERED=1");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    debug_output("Unable to initialize SDL: " << SDL_GetError());
		std::cerr << "Unable to initialize SDL: " << SDL_GetError();
    return false;
  }
	
	// Load the SDL Audio Mixer
  static const int audio_rate = 44100;
  static const Uint16 audio_format = AUDIO_S16SYS; // 16-bit stereo
  static const int audio_channels = 2;
  static const int audio_buffers = 2048;

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
		debug_output("Unable to open SDL audio: " << SDL_GetError());
		std::cerr << "Unable to open SDL audio: " << SDL_GetError();
    return false;
  }
	
	int resultAudioRate, resultAudioChannels;
	Uint16 resultAudioFormat;
	Mix_QuerySpec(&resultAudioRate, &resultAudioFormat, &resultAudioChannels);
	debug_output("SDL Audio Information:" << std::endl << 
		           "Audio rate: " << resultAudioRate << "Hz" << std::endl <<
							 "Audio channels: " << resultAudioChannels << std::endl);
		           

	// Don't show the mouse cursor unless we're in debug mode
#ifdef _DEBUG
	SDL_ShowCursor(1);
#else
	SDL_ShowCursor(1);
    // TODO: Show the cursor as a graphic/effect of some sort in the menu and game
    // and hide the normal one...
    //SDL_ShowCursor(0);
#endif

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
	this->videoSurface = SDL_SetVideoMode(width, height, bitsPerPixel, videoFlags);
	if (this->videoSurface == NULL) {
		debug_output("Unable to set video mode: " << SDL_GetError());
		std::cerr << "Unable to set video mode: " << SDL_GetError();
    return false;
  }
	
	SDL_Surface* icon = SDL_LoadBMP(WindowManager::ICON_FILEPATH);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 255, 0, 255));
	SDL_WM_SetIcon(icon, NULL);

	return true;
}

/**
 * Shut down the windowing system. Call this before calling Init again.
 */
void WindowManager::Shutdown() {
	Mix_CloseAudio();						// Close/clean-up all SDL audio/mixer stuff
	SDL_Quit();									// This will obliterate all surfaces and other SDL objects
	this->videoSurface = NULL;	// To tell this object that video is dead
}

/**
 * Toggles fullscreen mode on/off for the render window.
 */
bool WindowManager::ToggleFullscreen() {
	if (this->videoSurface == NULL) {
		assert(false);
		return false;
	}
	
	int success = SDL_WM_ToggleFullScreen(this->videoSurface);
	return (success == 1);
}




/**
 * Obtain a list of possible resolutions for the display device
 * being used to display the game.
 */
std::vector<std::string> WindowManager::GetPossibleResolutionsList() {
	std::set<std::string> resolutionSet;
	std::vector<std::string> resolutionVec;

	// cross-compatibility issue
#ifdef WIN32
	DEVMODE devMode;
	DWORD count = 0;
	
	while (EnumDisplaySettings(NULL, count, &devMode) != NULL) {
		std::stringstream resolutionStrStream;
		resolutionStrStream << devMode.dmPelsWidth << " x " << devMode.dmPelsHeight;
        // Don't accept resolutions under 800 width or 600 height
        if (devMode.dmPelsWidth < 800 || devMode.dmPelsHeight < 600) {
            count++;
            continue;
        }

		// We use the set as a detector for unique resolutions, and we only insert
		// unique ones into the vector that we will be returning
		resolutionSet.insert(resolutionStrStream.str());

		count++;
	}
#endif

	resolutionVec.insert(resolutionVec.begin(), resolutionSet.begin(), resolutionSet.end());
	sort(resolutionVec.begin(), resolutionVec.end(), SortingFunctions::ResolutionCompare);
	
	return resolutionVec;
}