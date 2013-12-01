/**
 * LoadingScreen.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "LoadingScreen.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../ESPEngine/ESPUtil.h"
#include "../ResourceManager.h"

LoadingScreen* LoadingScreen::instance = NULL;

const float LoadingScreen::GAP_PIXELS	= 20.0f;

const int LoadingScreen::LOADING_BAR_WIDTH	= 550;
const int LoadingScreen::LOADING_BAR_HEIGHT = 50;
const char* LoadingScreen::ABSURD_LOADING_DESCRIPTION = "ABSURD";

LoadingScreen::LoadingScreen() : loadingScreenOn(false), width(0), height(0), 
numExpectedUpdates(0), numCallsToUpdate(0), starryBG(NULL) {
	// At the very least we need fonts to display info on the loading screen...
	GameFontAssetsManager::GetInstance()->LoadMinimalFonts();
	
	const TextureFontSet* itemLoadingFont = GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
        GameFontAssetsManager::Medium);
	assert(itemLoadingFont != NULL);
    this->itemLoadingLabel = TextLabel2D(itemLoadingFont, "");
	this->itemLoadingLabel.SetColour(Colour(0, 0, 0));
	this->itemLoadingLabel.SetDropShadow(Colour(0,0,0), 0.05);

    this->starryBG = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_STARFIELD, Texture::Trilinear));
    assert(this->starryBG != NULL);

	// Load the absurd descriptions...
	this->absurdLoadingDescriptions.reserve(21);
	this->absurdLoadingDescriptions.push_back("Synthesizing mind-numbing onomatopoeia ...");
	this->absurdLoadingDescriptions.push_back("Initializing blocky stuffs ...");
	this->absurdLoadingDescriptions.push_back("Loading balls of doom ...");
	this->absurdLoadingDescriptions.push_back("Loading extraneous punctuation ...");
	this->absurdLoadingDescriptions.push_back("Pulling Levers ...");
	this->absurdLoadingDescriptions.push_back("Pushing buttons ...");
	this->absurdLoadingDescriptions.push_back("Twisting knobs ...");
	this->absurdLoadingDescriptions.push_back("Making breakfast ...");
	this->absurdLoadingDescriptions.push_back("Composing Music ...");
	this->absurdLoadingDescriptions.push_back("Generating distracting colours ...");
	this->absurdLoadingDescriptions.push_back("Baking cake ...");
	this->absurdLoadingDescriptions.push_back("Eating cake ...");
	this->absurdLoadingDescriptions.push_back("Compiling sounds to words ...");
	this->absurdLoadingDescriptions.push_back("Doing ... stuff ...");
	this->absurdLoadingDescriptions.push_back("Reconciling Quantum Mechanics and Relativity ...");
	this->absurdLoadingDescriptions.push_back("Deploying Nanotubes ...");
	this->absurdLoadingDescriptions.push_back("Loading moderately relevant things ...");
	this->absurdLoadingDescriptions.push_back("Nomming your CPU ...");
	this->absurdLoadingDescriptions.push_back("Affixing suffixes and prefixes ...");
    this->absurdLoadingDescriptions.push_back("Adhering to reasonably consistent art style ...");
    this->absurdLoadingDescriptions.push_back("Transfixing art movements ...");

	this->lastRandomAbsurdity = Randomizer::GetInstance()->RandomUnsignedInt() % this->absurdLoadingDescriptions.size();

}

LoadingScreen::~LoadingScreen() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starryBG);
    assert(success);
    UNUSED_VARIABLE(success);
}

/**
 * Static private helper function for initializing a very basic opengl state
 * for the loading screen.
 */
void LoadingScreen::InitOpenGLForLoadingScreen() {

	// Initialize a very basic OpenGL context...
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_3D);
    glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	debug_opengl_state();
}

void LoadingScreen::RenderLoadingScreen() {
	
    glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	// Initialize basic OpenGL state
	LoadingScreen::InitOpenGLForLoadingScreen();
	this->itemLoadingLabel.SetTopLeftCorner((this->width - this->itemLoadingLabel.GetLastRasterWidth()) / 2.0f, 
                                            (this->height + LOADING_BAR_HEIGHT) / 2 + this->itemLoadingLabel.GetHeight() + GAP_PIXELS/2);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // Draw the starry background...
    this->starryBG->BindTexture();
    GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(this->width, this->height, 
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(this->width) / static_cast<float>(this->starryBG->GetWidth()),
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(this->height) / static_cast<float>(this->starryBG->GetHeight()));
    this->starryBG->UnbindTexture();

	// Draw the loading/progress bar
	this->DrawLoadingBar();

	// Draw labels for loading screen
	this->itemLoadingLabel.Draw();

	SDL_GL_SwapBuffers();

	glPopAttrib();
	debug_opengl_state();
}

/**
 * Function used to initialize and start the loading screen, this MUST be called before any other
 * loading screen functions and should end with a call to EndShowingLoadingScreen.
 */
void LoadingScreen::StartShowLoadingScreen(int width, int height, unsigned int numExpectedUpdates) {
	assert(!this->loadingScreenOn);
	if (this->loadingScreenOn) {
		return;
	}

	debug_output("Loading Screen start...");

	this->numCallsToUpdate = 0;
	this->numExpectedUpdates = numExpectedUpdates + 1;
	this->width = width;
	this->height = height;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	// Initialize basic OpenGL state
	LoadingScreen::InitOpenGLForLoadingScreen();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	debug_opengl_state();
	
	this->DrawLoadingBar();

	debug_opengl_state();

	SDL_GL_SwapBuffers();

	glPopAttrib();

	this->loadingScreenOn = true;
	
    this->UpdateLoadingScreenWithRandomLoadStr();
}

/**
 * Called in the middle of showing the loading screen - a call MUST be made to
 * StartShowLoadingScreen before calling this function. This will update the loading
 * screen to a certain percentage based on the numExpectedUpdates and the number of
 * calls to this function so far.
 */
void LoadingScreen::UpdateLoadingScreen(const std::string& loadingStr) {
	if (!this->loadingScreenOn) { 
		return; 
	}
    std::string resultLoadStr(loadingStr);

	// If the loading string asks for absurdity then make it something absurd...
    if (loadingStr == LoadingScreen::ABSURD_LOADING_DESCRIPTION) {
		assert(this->lastRandomAbsurdity >= 0 && this->lastRandomAbsurdity < this->absurdLoadingDescriptions.size());
		resultLoadStr = this->absurdLoadingDescriptions[this->lastRandomAbsurdity];
		this->lastRandomAbsurdity = (this->lastRandomAbsurdity + 1) % this->absurdLoadingDescriptions.size();
	}

	// Set the loading text label
	ESPInterval randColourR(0.4f, 1.0f);
	ESPInterval randColourG(0.4f, 1.0f);
	ESPInterval randColourB(0.4f, 1.0f);
	this->itemLoadingLabel.SetColour(Colour(randColourR.RandomValueInInterval(), randColourG.RandomValueInInterval(), randColourB.RandomValueInInterval()));
	this->itemLoadingLabel.SetText(resultLoadStr);

	// Increment the counter of updates and make sure we are under what is expected
	this->numCallsToUpdate++;
	assert(this->numCallsToUpdate <= this->numExpectedUpdates);

	this->RenderLoadingScreen();
}

/**
 * Called at the very end, when we want to retire the loading screen. This MUST be called
 * at the very end and calls to StartShowLoadingScreen and UpdateLoadingScreen MUST be
 * made before it.
 */
void LoadingScreen::EndShowingLoadingScreen() {
	assert(this->loadingScreenOn);
	if (!this->loadingScreenOn) { 
		return; 
	}

	// Re-render the last update...
	this->RenderLoadingScreen();

	this->loadingScreenOn = false;
	this->numCallsToUpdate = 0;
	this->numExpectedUpdates = 0;

	//GameDisplay::SetInitialRenderOptions();
}

/**
 * Private helper function for drawing the loading bar for the loading screen.
 */
void LoadingScreen::DrawLoadingBar() {
	if (!this->loadingScreenOn) { 
		return; 
	}
	// Figure out what estimated percentage of loading is complete and fill the bar based on that percentage
	float percentageDone     = std::min<float>(1.0f, static_cast<float>(this->numCallsToUpdate) / static_cast<float>(this->numExpectedUpdates));
	float lengthOfLoadingBar = percentageDone * LOADING_BAR_WIDTH;

	Point2D loadingBarUpperLeft = Point2D((this->width - LOADING_BAR_WIDTH) / 2.0f, (this->height + LOADING_BAR_HEIGHT) / 2.0f);

	// Draw the loading bar:
	Camera::PushWindowCoords();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Loading bar fill
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1, 0, 0, 1);
	glBegin(GL_QUADS);
	glVertex2f(loadingBarUpperLeft[0], loadingBarUpperLeft[1]);
	glVertex2f(loadingBarUpperLeft[0], loadingBarUpperLeft[1] - LOADING_BAR_HEIGHT);
	glVertex2f(loadingBarUpperLeft[0] + lengthOfLoadingBar, loadingBarUpperLeft[1] - LOADING_BAR_HEIGHT);
	glVertex2f(loadingBarUpperLeft[0] + lengthOfLoadingBar, loadingBarUpperLeft[1]);
	glEnd();

	// Outline of loading bar
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(4.0f);
	glPointSize(3.0f);
	glColor4f(1, 1, 1, 1);
	
	glBegin(GL_LINE_LOOP);
	glVertex2f(loadingBarUpperLeft[0], loadingBarUpperLeft[1]);
	glVertex2f(loadingBarUpperLeft[0], loadingBarUpperLeft[1] - LOADING_BAR_HEIGHT);
	glVertex2f(loadingBarUpperLeft[0] + LOADING_BAR_WIDTH, loadingBarUpperLeft[1] - LOADING_BAR_HEIGHT);
	glVertex2f(loadingBarUpperLeft[0] + LOADING_BAR_WIDTH, loadingBarUpperLeft[1]);
	glEnd();
	
	glBegin(GL_POINTS);
	glVertex2f(loadingBarUpperLeft[0], loadingBarUpperLeft[1]);
	glVertex2f(loadingBarUpperLeft[0], loadingBarUpperLeft[1] - LOADING_BAR_HEIGHT);
	glVertex2f(loadingBarUpperLeft[0] + LOADING_BAR_WIDTH, loadingBarUpperLeft[1] - LOADING_BAR_HEIGHT);
	glVertex2f(loadingBarUpperLeft[0] + LOADING_BAR_WIDTH, loadingBarUpperLeft[1]);
	glEnd();

	glPolygonMode(GL_FRONT, GL_FILL);
	glPopMatrix();
	Camera::PopWindowCoords();

	debug_opengl_state();
}