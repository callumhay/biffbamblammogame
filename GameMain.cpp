/**
 * GameMain.cpp
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

#ifdef _DEBUG
// Visual Leak Detector Includes
//#include <vld.h>
#endif

#include "BlammoEngine/FBObj.h"
#include "BlammoEngine/Noise.h"
#include "BlammoEngine/GeometryMaker.h"

#include "GameView/GameDisplay.h"
#include "GameView/GameViewConstants.h"
#include "GameView/GameFontAssetsManager.h"
#include "GameView/LoadingScreen.h"
#include "GameView/PersistentTextureManager.h"
#include "GameView/GameViewEventManager.h"

#include "GameSound/GameSound.h"

#include "GameModel/GameModel.h"
#include "GameModel/GameEventManager.h"
#include "GameModel/GameModelConstants.h"
#include "GameModel/Onomatoplex.h"
#include "GameModel/ArcadeLeaderboard.h"

#include "GameControl/GameControllerManager.h"

#include "ResourceManager.h"
#include "WindowManager.h"
#include "ConfigOptions.h"

static GameSound* sound     = NULL;
static GameModel* model     = NULL;
static GameDisplay* display = NULL;

static bool arcadeMode = false;

/**
 * Clean up the ModelViewController classes that run the game.
 */
static void CleanUpMVC() {
    // Order matters here!!!

	if (display != NULL) {
		delete display;
		display = NULL;
	}

    GameControllerManager::DeleteInstance();

    if (arcadeMode) {
        ResourceManager::WriteLeaderboard(ResourceManager::ReadLeaderboard(true, *model));
    }

	if (model != NULL) {
		delete model;
		model = NULL;
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
	static const double MAX_DELTA = 1.0 / 20.0;
	bool quitGame = false;

	// Main render loop...
	while (!display->HasGameExited() && !display->ShouldGameReinitialize()) {
		Uint32 startOfFrameTime = SDL_GetTicks();

		// Synchronize the controller state with the current game loop
		GameControllerManager::GetInstance()->SyncControllers(frameTimeDelta);

		// Don't let the game run at less than 20 fps (i.e., more than 1/20 of a second per frame)
        frameTimeDelta = std::min<double>(MAX_DELTA, frameTimeDelta);

		// Render what's currently being displayed by the game
		display->Render(frameTimeDelta);
		SDL_GL_SwapBuffers();

		SDL_Delay(GameDisplay::FRAME_SLEEP_MS);

		// Process controller events
		quitGame = GameControllerManager::GetInstance()->ProcessControllers(frameTimeDelta);
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

	// Clear up misc. singletons
	LoadingScreen::DeleteInstance();
	Noise::DeleteInstance();
	GeometryMaker::DeleteInstance();
	LoadingScreen::DeleteInstance();
	GameFontAssetsManager::DeleteInstance();
    PersistentTextureManager::DeleteInstance();

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

    std::string serialPort = "";
    if (argc > 1) {
        if (std::string(argv[1]) == std::string("-a")) {
            arcadeMode = true;
            if (argc > 2) {
                serialPort = std::string(argv[2]);
            }
        }
    }

	// Set the default config options - these will be read from and written to
	// the .ini file as we need them
	ConfigOptions initCfgOptions(arcadeMode);
	// ********************************************************************

	// We continue to run the game until a definitive quit has been issued
	// in this case we will break from the loop
	bool quitGame = false;
	while (!quitGame) {

		// Read the .ini file options (used to initialize various settings in the game)
		initCfgOptions = ResourceManager::ReadConfigurationOptions(true, arcadeMode);

		// Setup the window - this initializes OpenGL so that OGL calls may be made after this
		if (!WindowManager::GetInstance()->Init(initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight(), 
                                                initCfgOptions.GetIsFullscreenOn())) {
			
            // Try one more time but without full screen on (full screen forces the resolution, if the res isn't supported
            // it's typically the culprit of the failure)
            if (!WindowManager::GetInstance()->Init(initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight(), false)) {
                quitGame = true;
			    break;
            }
		}

        // Reset the config options based on what the window manager decided to be a non-erroneous configuration
        initCfgOptions.SetWindowWidth(WindowManager::GetInstance()->GetWidth());
        initCfgOptions.SetWindowHeight(WindowManager::GetInstance()->GetHeight());
        initCfgOptions.SetIsFullscreenOn(WindowManager::GetInstance()->GetIsFullscreen());
        ResourceManager::WriteConfigurationOptionsToFile(initCfgOptions);

		// Establish the resource manager
        ResourceManager::InitResourceManager(ResourceManager::GetLoadDir() + std::string(ResourceManager::RESOURCE_ZIP), argv[0]);

		// Load extensions
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cout << "Error loading extensions: " << glewGetErrorString(err) << std::endl;
			quitGame = true;
			break;
		}

		// TODO: VSync: option for this?
		BlammoTime::SetVSync(initCfgOptions.GetIsVSyncOn());

        // MAKE SURE WE SET THE CAMERA DIMENSIONS!!!!
        Camera::SetWindowDimensions(initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight());

		// Create the MVC while showing the loading screen...
		LoadingScreen::GetInstance()->StartShowLoadingScreen(initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight(), 7);
		
        // Set the controller sensitivity from the initial configuration options
        //GameControllerManager::GetInstance()->SetControllerSensitivity(initCfgOptions.GetControllerSensitivity());

		// Load the blammopedia...
		LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
        if (!ResourceManager::GetInstance()->LoadBlammopedia(ResourceManager::GetLoadDir() + 
            std::string(ResourceManager::BLAMMOPEDIA_FILE))) {

			quitGame = true;
			break;
		}

        // Load basic default in-memory sounds
        LoadingScreen::GetInstance()->UpdateLoadingScreenWithRandomLoadStr();
        sound = new GameSound();
        if (sound->Init(arcadeMode)) {
            sound->LoadGlobalSounds();
            sound->SetSFXVolume(static_cast<float>(initCfgOptions.GetSFXVolume()) / 100.0f);
            sound->SetMusicVolume(static_cast<float>(initCfgOptions.GetMusicVolume()) / 100.0f);
            std::cout << "Sound initialized successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to load game sound." << std::endl;
        }

        model = new GameModel(sound, initCfgOptions.GetDifficulty(), initCfgOptions.GetInvertBallBoost(), initCfgOptions.GetBallBoostMode());
        if (arcadeMode) {
            ResourceManager::ReadLeaderboard(true, *model);
        }
		display = new GameDisplay(model, sound, initCfgOptions.GetWindowWidth(), initCfgOptions.GetWindowHeight(), arcadeMode);

		// Initialize all controllers that we can...
		GameControllerManager::GetInstance()->InitAllControllers(model, display, arcadeMode);
        GameControllerManager::GetInstance()->SetArcadeSerialPort(serialPort);

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
    GameViewEventManager::DeleteInstance();
	Onomatoplex::Generator::DeleteInstance();
	Randomizer::DeleteInstance();

	// One-Time Deletion Stuff (only on exit) *****************************

	// Clean up all file and shader resources, ORDER MATTERS HERE!

	// Write whatever the current state of the configuration is back to the config (.ini) file
	initCfgOptions = ResourceManager::ReadConfigurationOptions(true, arcadeMode);
	bool iniWriteResult = ResourceManager::WriteConfigurationOptionsToFile(initCfgOptions);
    UNUSED_VARIABLE(iniWriteResult);
	assert(iniWriteResult);

	ResourceManager::DeleteInstance();
	WindowManager::DeleteInstance();
    GameItemFactory::DeleteInstance();
	// ********************************************************************

	return 0;
}