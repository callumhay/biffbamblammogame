#include "LoadingScreen.h"
#include "GameFontAssetsManager.h"
#include "CgFxBloom.h"

#include "../ESPEngine/ESPUtil.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/FBObj.h"

LoadingScreen* LoadingScreen::instance = NULL;

const std::string LoadingScreen::LOADING_TEXT = "Loading ...";
const float LoadingScreen::GAP_PIXELS	= 20.0f;

const int LoadingScreen::LOADING_BAR_WIDTH	= 550;
const int LoadingScreen::LOADING_BAR_HEIGHT = 50;
const std::string LoadingScreen::ABSURD_LOADING_DESCRIPTION = "ABSURD";

LoadingScreen::LoadingScreen() : loadingScreenOn(false), width(0), height(0), 
numExpectedUpdates(0), numCallsToUpdate(0), loadingScreenFBO(NULL), bloomEffect(NULL) {
	// At the very least we need fonts to display info on the loading screen...
	GameFontAssetsManager::GetInstance()->LoadMinimalFonts();
	
	// Create our various labels...
	const TextureFontSet* loadingFont = GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small);
	this->loadingLabel = TextLabel2D(loadingFont, LoadingScreen::LOADING_TEXT);
	this->loadingLabel.SetColour(Colour(0.457, 0.695, 0.863));
	this->loadingLabel.SetDropShadow(Colour(0,0,0), 0.10);

	const TextureFontSet* itemLoadingFont = GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium);
	this->itemLoadingLabel = TextLabel2D(itemLoadingFont, "");
	this->itemLoadingLabel.SetColour(Colour(0, 0, 0));
	this->itemLoadingLabel.SetDropShadow(Colour(0,0,0), 0.05);

	// Load the absurd descriptions...
	this->absurdLoadingDescriptions.reserve(6);
	this->absurdLoadingDescriptions.push_back("Generating mind-numbing onamotapoeia ...");
	this->absurdLoadingDescriptions.push_back("Initializing blocky stuffs ...");
	this->absurdLoadingDescriptions.push_back("Loading balls of doom ...");
	this->absurdLoadingDescriptions.push_back("Loading extraneous punctuation ...");
	this->absurdLoadingDescriptions.push_back("Pulling Levers ...");
	this->absurdLoadingDescriptions.push_back("Pushing buttons ...");
	this->absurdLoadingDescriptions.push_back("Generating distracting colours ...");
	this->absurdLoadingDescriptions.push_back("Baking cake ...");
	this->absurdLoadingDescriptions.push_back("Eating cake ... Om nom nom");
	this->absurdLoadingDescriptions.push_back("Compiling sounds to words ...");
	this->absurdLoadingDescriptions.push_back("Contacting Skynet ...");
	this->absurdLoadingDescriptions.push_back("Communicating with the MCP ...");
	this->absurdLoadingDescriptions.push_back("Nomming your cpu ...");
	this->absurdLoadingDescriptions.push_back("Affixing suffixes and prefixes...");

	this->lastRandomAbsurdity = Randomizer::GetInstance()->RandomUnsignedInt() % this->absurdLoadingDescriptions.size();

}

LoadingScreen::~LoadingScreen() {
	if (this->loadingScreenFBO != NULL) {
		delete this->loadingScreenFBO;
		this->loadingScreenFBO = NULL;
	}
	if (this->bloomEffect != NULL) {
		delete this->bloomEffect;
		this->bloomEffect = NULL;
	}
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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	debug_opengl_state();
}

void LoadingScreen::SetupFullscreenEffect(int width, int height) {
	// Setup the FBO for the loading screen
	if (this->loadingScreenFBO != NULL) {
		delete this->loadingScreenFBO;
	}
	this->loadingScreenFBO = new FBObj(width, height, Texture::Nearest, FBObj::NoAttachment);

	// Setup the bloom effect and its parameters
	if (this->bloomEffect != NULL) {
		delete this->bloomEffect;
	}
	this->bloomEffect = new CgFxBloom(this->loadingScreenFBO);
	this->bloomEffect->SetHighlightThreshold(0.4f);
	this->bloomEffect->SetSceneIntensity(0.70f);
	this->bloomEffect->SetGlowIntensity(0.3f);
	this->bloomEffect->SetHighlightIntensity(0.1f);
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

	// Setup the FBO and bloom CgFx Effect
	this->SetupFullscreenEffect(width, height);

	this->numCallsToUpdate = 0;
	this->numExpectedUpdates = numExpectedUpdates;
	this->width = width;
	this->height = height;
	this->loadingLabel.SetTopLeftCorner(Point2D(0 + GAP_PIXELS, this->loadingLabel.GetHeight() + GAP_PIXELS));
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	// Initialize basic OpenGL state
	LoadingScreen::InitOpenGLForLoadingScreen();

	this->loadingScreenFBO->BindFBObj();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	this->loadingLabel.Draw();
	this->DrawLoadingBar();

	this->loadingScreenFBO->UnbindFBObj();
	this->bloomEffect->Draw(width, height, 0.0f);
	this->loadingScreenFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();

	SDL_GL_SwapBuffers();

	glPopAttrib();

	this->loadingScreenOn = true;
	
#ifndef _DEBUG
	// Just to get things up and running initially, give time for user to see loading screen
	SDL_Delay(500);
#endif
}

/**
 * Called in the middle of showing the loading screen - a call MUST be made to
 * StartShowLoadingScreen before calling this function. This will update the loading
 * screen to a certain percentage based on the numExpectedUpdates and the number of
 * calls to this function so far.
 */
void LoadingScreen::UpdateLoadingScreen(std::string loadingStr) {
	if (!this->loadingScreenOn) { 
		return; 
	}

	// If the loading string asks for absurdity then make it something absurd...
	if (loadingStr == LoadingScreen::ABSURD_LOADING_DESCRIPTION) {
		assert(this->lastRandomAbsurdity >= 0 && this->lastRandomAbsurdity < this->absurdLoadingDescriptions.size());
		loadingStr = this->absurdLoadingDescriptions[this->lastRandomAbsurdity];
		this->lastRandomAbsurdity = (this->lastRandomAbsurdity + 1) % this->absurdLoadingDescriptions.size();
	}

	// Increment the counter of updates and make sure we are under what is expected
	this->numCallsToUpdate++;
	assert(this->numCallsToUpdate <= this->numExpectedUpdates);

	ESPInterval randColourR(0.25f, 0.8f);
	ESPInterval randColourG(0.25f, 0.8f);
	ESPInterval randColourB(0.25f, 0.8f);
	this->itemLoadingLabel.SetColour(Colour(randColourR.RandomValueInInterval(), randColourG.RandomValueInInterval(), randColourB.RandomValueInInterval()));
	this->itemLoadingLabel.SetText(loadingStr);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	// Initialize basic OpenGL state
	LoadingScreen::InitOpenGLForLoadingScreen();
	this->itemLoadingLabel.SetTopLeftCorner(Point2D((this->width - this->itemLoadingLabel.GetLastRasterWidth()) / 2.0f, (this->height + LOADING_BAR_HEIGHT) / 2 + this->itemLoadingLabel.GetHeight() + GAP_PIXELS/2));

	// Bind the FBO so we draw the loading screen into it
	this->loadingScreenFBO->BindFBObj();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Draw the loading/progress bar
	this->DrawLoadingBar();

	// Draw labels for loading screen
	this->loadingLabel.Draw();
	this->itemLoadingLabel.Draw();

	// Unbind the FBO, add bloom and draw it as a texture on a full screen quad
	this->loadingScreenFBO->UnbindFBObj();
	this->bloomEffect->Draw(width, height, 0.0f);
	this->loadingScreenFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();

	SDL_GL_SwapBuffers();

	glPopAttrib();
	debug_opengl_state();
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


	this->loadingScreenOn = false;
	this->numCallsToUpdate = 0;
	this->numExpectedUpdates = 0;

}

/**
 * Private helper function for drawing the loading bar for the loading screen.
 */
void LoadingScreen::DrawLoadingBar() {
	// Figure out what estimated percentage of loading is complete and fill the bar based on that percentage
	float percentageDone			= NumberFuncs::MinF(1.0f, static_cast<float>(this->numCallsToUpdate) / static_cast<float>(this->numExpectedUpdates));
	float lengthOfLoadingBar	= percentageDone * LOADING_BAR_WIDTH;

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
	glColor4f(0, 0, 0, 1);
	
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