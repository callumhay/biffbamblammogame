#include "BlammoEngine/BlammoEngine.h"

#include "GameView/GameDisplay.h"
#include "GameView/GameViewConstants.h"
#include "GameView/GameFontAssetsManager.h"

#include "GameModel/GameModel.h"
#include "GameModel/GameEventManager.h"
#include "GameModel/GameModelConstants.h"

#include "GameController.h"

// Initialization Constants for the application
static const char* WINDOW_TITLE		= "Biff! Bam!! Blammo!?!";
static const char* ICON_FILEPATH  = "resources/icons/BiffBamBlammoIcon.bmp"; 
static const int INIT_WIDTH = 1024;
static const int INIT_HEIGHT = 768;

static GameModel *model = NULL;
static GameController *controller = NULL;
static GameDisplay *display = NULL;

static const unsigned int DEFAULT_VIDEO_FLAGS = SDL_OPENGL | SDL_SWSURFACE | SDL_RESIZABLE;
static int BitsPerPixel = 16;
static SDL_Surface* VideoSurface = NULL;

void ResizeWindow(int w, int h) {
	display->ChangeDisplaySize(w, h);
}

static void KeyDownEventHandler(SDL_keysym* keysym) {
	switch (keysym->sym) {
	case SDLK_ESCAPE:
		SDL_Quit();
		exit(0);
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
				SDL_Quit();
				exit(0);
				break;
		}
	}
}

/**
 * Initialize SDL.
 * Returns: true on success, false otherwise.
 */
bool InitSDLWindow() {
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
  BitsPerPixel = info->vfmt->BitsPerPixel;

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
	VideoSurface = SDL_SetVideoMode(INIT_WIDTH, INIT_HEIGHT, BitsPerPixel, DEFAULT_VIDEO_FLAGS);
	if (VideoSurface == NULL) {
		std::cerr << "Unable to set video mode: " << SDL_GetError();
    return false;
  }
	
	SDL_Surface* icon = SDL_LoadBMP(ICON_FILEPATH);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 255, 0, 255));
	SDL_WM_SetIcon(icon, NULL);

	return true;
}


// Driver function for the game.
int main(int argc, char *argv[]) {
	// Setup the SDL window
	if (!InitSDLWindow()) {
		SDL_Quit();
		return -1;
	}
	
	// Load extensions
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cout << "Error loading extensions: " << glewGetErrorString(err) << std::endl;
		return -1;
	}

	// Create the MVC...
	model = new GameModel();
	display = new GameDisplay(model, INIT_WIDTH, INIT_HEIGHT);
	controller = new GameController(model, display);

	// TODO: VSync: option for this?
	BlammoTime::SetVSync(0);

	double frameTimeDelta = 0.0;
	// Try to keep it at 60fps...
	const double maxDelta = 1.0 / 60.0;

	// Main render loop...
	while(true) {
		Uint32 startOfFrameTime = SDL_GetTicks();
		
		// Don't let the game run at less than 60 fps
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

	// Clear up MVC
	delete model;
	delete display;
	delete controller;

	// Clear up singletons
	GameEventManager::DeleteInstance();
	CgShaderManager::DeleteInstance();
	FBOManager::DeleteInstance();
	GameModelConstants::DeleteInstance();
	GameViewConstants::DeleteInstance();
	GameFontAssetsManager::DeleteInstance();
	Randomizer::DeleteInstance();

	return 0;
}