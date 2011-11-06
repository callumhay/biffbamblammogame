/**
 * GameMain.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

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

#include "GameSound/GameSoundAssets.h"

#include "GameControl/GameControllerManager.h"

#include "ResourceManager.h"
#include "WindowManager.h"
#include "ConfigOptions.h"

// Initialization Constants for the application
static const char* RESOURCE_ZIP = "BBBResources.zip";

static GameModel *model = NULL;
static GameDisplay *display = NULL;
static GameSoundAssets* sound = NULL;

/**
 * Clean up the ModelViewController classes that run the game.
 */
static void CleanUpMVC() {
	GameControllerManager::DeleteInstance();

	if (model != NULL) {
		delete model;
		model = NULL;
	}

	if (display != NULL) {
		delete display;
		display = NULL;
	}

	if (sound != NULL) {
		delete sound;
		sound = NULL;
	}
}

/**
 * Run the main game loop - this will continuously draw the game until
 * either the game is quit or reinitialization (e.g., to switch video size) occurs.
 */
static void GameRenderLoop() {
	double frameTimeDelta = 0.0;
	const double maxDelta = 1.0 / 15.0;
	bool quitGame = false;

	// Main render loop...
	while (!display->HasGameExited() && !display->ShouldGameReinitialize()) {
		Uint32 startOfFrameTime = SDL_GetTicks();

		// Synchronize the controller state with the current game loop
		GameControllerManager::GetInstance()->SyncControllers(frameTimeDelta);

		// Don't let the game run at less than 30 fps
		if (frameTimeDelta > maxDelta) {
			frameTimeDelta = maxDelta;
		}

		// Render what's currently being displayed by the game
		display->Render(frameTimeDelta);
		SDL_GL_SwapBuffers();

		SDL_Delay(GameDisplay::FRAME_SLEEP_MS);

		// Process events (like input from controllers/windows)...
		// TODO: Signal a quit here via a boolean
		quitGame = GameControllerManager::GetInstance()->ProcessControllers();
		if (quitGame) {
			display->QuitGame();
		}

		// Calculate the frame delta...
		frameTimeDelta = static_cast<double>(SDL_GetTicks() - startOfFrameTime) / 1000.0;
	}
}

/**
 * Kills everything related to the graphics (opengl) and the window.
 */
static void KillGraphicsAndWindow(bool gameIsQuiting) {
	// ORDER OF CLEAN-UP MATTERS - Destroy the MVC before all else!

	// Clear up MVC
	CleanUpMVC();

	// Clear up singletons
	LoadingScreen::DeleteInstance();
	Noise::DeleteInstance();
	GeometryMaker::DeleteInstance();
	LoadingScreen::DeleteInstance();
	GameFontAssetsManager::DeleteInstance();

	// Only delete the resource manager if we aren't quitting (we need the
	// resource manager to write out the config file otherwise - it needs to be
	// the last thing destroyed)
	if (!gameIsQuiting) {
		ResourceManager::DeleteInstance();
	}

	// Shut down the window
	WindowManager::GetInstance()->Shutdown();
}

// Driver function for the game.
int main(int argc, char *argv[]) {
	UNUSED_PARAMETER(argc);

	// Memory dump debug info for detecting and finding memory leaks
#ifdef _DEBUG
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(26466);
#endif

	// One-Time Initialization stuff **************************************

    // Set the base directory to load files from
    assert(argc > 0 && argv != NULL);
    ResourceManager::SetLoadDir(argv[0]);

	// Set the default config options - these will be read from and written to
	// the .ini file as we need them
	ConfigOptions initCfgOptions;
	// ********************************************************************

	// We continue to run the game until a definitive quit has been issued
	// in this case we will break from the loop
	bool quitGame = false;
	while (!quitGame) {

		// Read the .ini file options (used to initialize various settings in the game)
		initCfgOptions = ResourceManager::ReadConfigurationOptions(true);

		// Setup the window - this initializes OpenGL so that OGL calls may be made after this
		if (!WindowManager::GetInstance()->Init(initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight(), initCfgOptions.GetIsFullscreenOn())) {
			quitGame = true;
			break;
		}

		// Establish the resource manager
		ResourceManager::InitResourceManager(ResourceManager::GetLoadDir() + std::string(RESOURCE_ZIP), argv[0]);

		// Load extensions
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cout << "Error loading extensions: " << glewGetErrorString(err) << std::endl;
			quitGame = true;
			break;
		}

		// TODO: VSync: option for this?
		BlammoTime::SetVSync(initCfgOptions.GetIsVSyncOn());

		// Create the MVC while showing the loading screen...
		LoadingScreen::GetInstance()->StartShowLoadingScreen(initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight(), 7);
		// Set the volume from the initial configuration options
		GameSoundAssets::SetGameVolume(initCfgOptions.GetVolume());
        // Set the controller sensitivity from the initial configuration options
        //GameControllerManager::GetInstance()->SetControllerSensitivity(initCfgOptions.GetControllerSensitivity());

		// Load the blammopedia...
		LoadingScreen::GetInstance()->UpdateLoadingScreen("Loading blammopedia...");
        if (!ResourceManager::GetInstance()->LoadBlammopedia(ResourceManager::GetLoadDir() + std::string("blammopedia.dat"))) {
			quitGame = true;
			break;
		}

		model = new GameModel(initCfgOptions.GetDifficulty());
		display = new GameDisplay(model, initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight());

		// Initialize all controllers that we can...
		GameControllerManager::GetInstance()->InitAllControllers(model, display);

		LoadingScreen::GetInstance()->EndShowingLoadingScreen();
		debug_opengl_state();

		// This will run the game until quit or reinitialization
		GameRenderLoop();

		// Set whether the game has quit or not - if the game has not
		// quit then we must be reinitializing it
		quitGame = display->HasGameExited();

		// Kill everything graphics related
		KillGraphicsAndWindow(quitGame);
	}

	GameModelConstants::DeleteInstance();
	GameViewConstants::DeleteInstance();
	GameEventManager::DeleteInstance();
	Onomatoplex::Generator::DeleteInstance();
	Randomizer::DeleteInstance();

	// One-Time Deletion Stuff (only on exit) *****************************

	// Clean up all file and shader resources, ORDER MATTERS HERE!

	// Write whatever the current state of the configuration is back to the config (.ini) file
	initCfgOptions = ResourceManager::ReadConfigurationOptions(true);
	bool iniWriteResult = ResourceManager::WriteConfigurationOptionsToFile(initCfgOptions);
    UNUSED_VARIABLE(iniWriteResult);
	assert(iniWriteResult);

	ResourceManager::DeleteInstance();
	WindowManager::DeleteInstance();
    GameItemFactory::DeleteInstance();
	// ********************************************************************

	return 0;
}