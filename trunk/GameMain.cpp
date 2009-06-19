#include "BlammoEngine/FBObj.h"
#include "BlammoEngine/Noise.h"
#include "BlammoEngine/GeometryMaker.h"

#include "GameView/GameDisplay.h"
#include "GameView/GameViewConstants.h"
#include "GameView/GameFontAssetsManager.h"
#include "GameView/LoadingScreen.h"

#include "GameModel/GameModel.h"
#include "GameModel/GameEventManager.h"
#include "GameModel/GameModelConstants.h"
#include "GameModel/Onomatoplex.h"

#include "GameController.h"
#include "ResourceManager.h"
#include "ConfigOptions.h"

// Initialization Constants for the application
static const char* WINDOW_TITLE		= "Biff! Bam!! Blammo!?!";
static const char* ICON_FILEPATH  = "BiffBamBlammoIcon.bmp";
static const std::string RESOURCE_ZIP = "BBBResources.zip";

static GameModel *model = NULL;
static GameController *controller = NULL;
static GameDisplay *display = NULL;

static const unsigned int DEFAULT_VIDEO_FLAGS = SDL_OPENGL | SDL_SWSURFACE;
static int bitsPerPixel = 16;
static SDL_Surface* VideoSurface = NULL;

static void ResizeWindow(int w, int h) {
	display->ChangeDisplaySize(w, h);
}

static void KeyDownEventHandler(SDL_keysym* keysym) {
	switch (keysym->sym) {
	case SDLK_ESCAPE:
		display->QuitGame();
		break;
	default:
		controller->KeyDown(keysym->sym);
		break;
	}
}

static void KeyUpEventHandler(SDL_keysym* keysym) {
	controller->KeyUp(keysym->sym);
}

static void ProcessEvents() {
	//Our SDL event placeholder.
	SDL_Event event;

	// Grab all the events off the queue
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				// Handle key presses
				KeyDownEventHandler(&event.key.keysym);
				break;
			case SDL_KEYUP:
				KeyUpEventHandler(&event.key.keysym);
				break;
			case SDL_VIDEORESIZE:
				ResizeWindow(event.resize.w, event.resize.h);
				break;
			case SDL_QUIT:
				// Handle quit requests (like Ctrl-c)
				display->QuitGame();
				break;
		}
	}
}

/**
 * Initialize SDL.
 * Returns: true on success, false otherwise.
 */
static bool InitSDLWindow(int windowWidth, int windowHeight, unsigned int videoFlags) {
	// Load SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Unable to initialize SDL: " << SDL_GetError();
    return false;
  }

	SDL_WM_SetCaption(WINDOW_TITLE, WINDOW_TITLE);

  // To use OpenGL, you need to get some information first,
  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  if(!info) {
    // This should never happen...
		std::cerr << "Video query failed: " << SDL_GetError() << std::endl;
    return false;
  }
  bitsPerPixel = info->vfmt->BitsPerPixel;

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

	// TODO: fix this... (make multisampling versatile...)
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, GameDisplay::NUM_MULTISAMPLES);

  // Set the draw surface...
	VideoSurface = SDL_SetVideoMode(windowWidth, windowHeight, bitsPerPixel, videoFlags);
	if (VideoSurface == NULL) {
		std::cerr << "Unable to set video mode: " << SDL_GetError();
    return false;
  }
	
	SDL_Surface* icon = SDL_LoadBMP(ICON_FILEPATH);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 255, 0, 255));
	SDL_WM_SetIcon(icon, NULL);

	return true;
}

/**
 * Clean up the ModelViewController classes that run the game.
 */
static void CleanUpMVC() {
	if (model != NULL) {
		delete model;
		model = NULL;
	}

	if (display != NULL) {
		delete display;
		display = NULL;
	}

	if (controller != NULL) {
		delete controller;
		controller = NULL;
	}
}

/**
 * Run the main game loop - this will continuously draw the game until
 * either the game is quit or reinitialization (e.g., to switch video size) occurs.
 */
static void GameRenderLoop() {
	double frameTimeDelta = 0.0;
	const double maxDelta = 1.0 / 30.0;

	// Main render loop...
	while (!display->HasGameExited() && !display->ShouldGameReinitialize()) {
		Uint32 startOfFrameTime = SDL_GetTicks();
		
		// Don't let the game run at less than 30 fps
		if (frameTimeDelta > maxDelta) {
			frameTimeDelta = maxDelta;
		}

		// Render what's currently being displayed by the game
		display->Render(frameTimeDelta);
		SDL_GL_SwapBuffers();
		
		SDL_Delay(GameDisplay::FRAME_SLEEP_MS);

		// Process SDL events...
		ProcessEvents();
		// Controller sync...
		controller->Tick();

		// Calculate the frame delta...
		frameTimeDelta = static_cast<double>(SDL_GetTicks() - startOfFrameTime) / 1000.0;
	}
}

static void Kill() {

}

// Driver function for the game.
int main(int argc, char *argv[]) {

	// Memory dump debug info for detecting and finding memory leaks
#ifdef _DEBUG
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(388);
#endif

	// One-Time Initialization stuff **************************************

	// Establish the resource manager
	ResourceManager::InitResourceManager(RESOURCE_ZIP, argv[0]);
	
	// We show the initial loading screen on the first time initializing the game
	bool showInitialLoadingScreen = true;

	// Set the default config options - these will be read from and written to
	// the .ini file as we need them
	ConfigOptions initCfgOptions;

	// ********************************************************************

	// We continue to run the game until a definitive quit has been issued
	// in this case we will break from the loop
	bool quitGame = false;
	while (!quitGame) {

		// Read the .ini file options (used to initialize various settings in the game)
		initCfgOptions = ResourceManager::GetInstance()->ReadConfigurationOptions(true);

		// Setup the SDL window
		if (!InitSDLWindow(initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight(), DEFAULT_VIDEO_FLAGS)) {
			quitGame = true;
			break;
		}

		// Load extensions
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cout << "Error loading extensions: " << glewGetErrorString(err) << std::endl;
			quitGame = true;
			break;
		}

		// TODO: VSync: option for this?
		BlammoTime::SetVSync(0);

		// Create the MVC while showing the loading screen...
		if (showInitialLoadingScreen) {
			LoadingScreen::GetInstance()->StartShowLoadingScreen(initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight(), 5);
		}

		model = new GameModel();
		display = new GameDisplay(model, initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight());
		controller = new GameController(model, display);

		if (showInitialLoadingScreen) {
			LoadingScreen::GetInstance()->EndShowingLoadingScreen();
		}

		debug_opengl_state();

		// This will run the game until quit or reinitialization
		GameRenderLoop();

		// Set whether the game has quit or not - if the game has not
		// quit then we must be reinitializing it
		quitGame = display->HasGameExited();

		// Clear up MVC
		CleanUpMVC();

		// No longer show the initial loading screen (in the case that we are
		// repeating the loop - i.e., reinitializing the window)
		showInitialLoadingScreen = false;
	}

	// Clear up singletons
	Onomatoplex::Generator::DeleteInstance();
	GameEventManager::DeleteInstance();
	GameModelConstants::DeleteInstance();
	GameViewConstants::DeleteInstance();
	GameFontAssetsManager::DeleteInstance();
	Randomizer::DeleteInstance();
	LoadingScreen::DeleteInstance();
	Noise::DeleteInstance();
	GeometryMaker::DeleteInstance();

	SDL_Quit();

	// One-Time Deletion Stuff (only on exit) *****************************

	// Clean up all file and shader resources, ORDER MATTERS HERE!
	
	// Write whatever the current state of the configuration is back to the config (.ini) file
	initCfgOptions = ResourceManager::GetInstance()->ReadConfigurationOptions(true);
	bool iniWriteResult = ResourceManager::GetInstance()->WriteConfigurationOptionsToFile(initCfgOptions);
	assert(iniWriteResult);

	// Clean up the resource manager last
	ResourceManager::DeleteInstance();

	// ********************************************************************

	return 0;
}