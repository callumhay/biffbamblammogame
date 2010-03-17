#include "InGameDisplayState.h"
#include "InGameMenuState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"
#include "LivesLeftHUD.h"
#include "CgFxGaussianBlur.h"
#include "CgFxAfterImage.h"
#include "CgFxBloom.h"

// Game Model stuff
#include "../GameModel/GameModel.h"
#include "../GameModel/Beam.h"

const std::string InGameDisplayState::LIVES_LABEL_TEXT = "Lives: ";
const unsigned int InGameDisplayState::HUD_X_INDENT = 10;	
const unsigned int InGameDisplayState::HUD_Y_INDENT = 10;

InGameDisplayState::InGameDisplayState(GameDisplay* display) : DisplayState(display) {

	// Set HUD display elements
	float dropShadowAmt = 0.05f;
	Colour shadowColourHUD(0, 0, 0);
	Colour textColourHUD(1, 1, 1);
	// Score display
	this->scoreLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), "0");
	this->scoreLabel.SetColour(textColourHUD);
	this->scoreLabel.SetDropShadow(shadowColourHUD, dropShadowAmt);

	// Clear up any stuff to an initial state in cases where things might still 
	// be set unchanged from a previously loaded game
	this->display->GetCamera().ClearCameraShake();
	// ...

	debug_opengl_state();
}

InGameDisplayState::~InGameDisplayState() {
}

/**
 * This will render a frame of the actual game while it is in-play.
 */
void InGameDisplayState::RenderFrame(double dT) {
	// Camera Stuff 
	Camera& camera = this->display->GetCamera();
	camera.SetPerspectiveWithFOV(this->display->GetModel()->GetTransformInfo()->GetCameraFOVAngle());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// TODO: since we now reset the camera and reapply its transform each frame, we need to tell the MODEL about debug transforms!!
#ifdef _DEBUG
	if (!GameDisplay::IsCameraDetached()) {
		camera.SetTransform(this->display->GetModel()->GetTransformInfo()->GetCameraTransform());
	}
#else
	camera.SetTransform(this->display->GetModel()->GetTransformInfo()->GetCameraTransform());
#endif	
	camera.ApplyCameraTransform(dT);
	
	debug_opengl_state();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Draw the game scene
	this->DrawGameScene(dT);
	
	// Draw the HUD
	this->DrawGameHUD(dT);
}

void InGameDisplayState::KeyPressed(SDLKey key, SDLMod modifier) {
	// We only interpret one key press - when the user wants to access the in-game menu...
	if (key == SDLK_ESCAPE) {
		this->display->SetCurrentState(new InGameMenuState(this->display));
	}
}

void InGameDisplayState::KeyReleased(SDLKey key, SDLMod modifier) {
}

// Private helper functions ************************************************************

/**
 * Helper function for drawing the game scene - this includes background and foreground objects
 * related to the game itself.
 */
void InGameDisplayState::DrawGameScene(double dT) {

	// Tick the assets (update them for amount of elapsed time dT).
	this->display->GetAssets()->Tick(dT);

	this->RenderBackgroundToFBO(dT);
	this->RenderForegroundWithBackgroundToFBO(dT);
	this->RenderFinalGather(dT);

	debug_opengl_state();

#ifdef _DEBUG
	this->DebugDrawBounds();
	this->display->GetAssets()->DebugDrawLights();
#endif
}

/**
 * Render the background of the current scene into the background FBO.
 */
void InGameDisplayState::RenderBackgroundToFBO(double dT) { 
	Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();
	
	// Attach the background FBO
	FBObj* backgroundFBO = this->display->GetAssets()->GetFBOAssets()->GetBackgroundFBO();
	assert(backgroundFBO != NULL);

	backgroundFBO->BindFBObj();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Draw the background of the current scene
	glPushMatrix();
	glTranslatef(0.0f, negHalfLevelDim[1], 0.0f);

	this->display->GetAssets()->DrawSkybox(this->display->GetCamera());
	this->display->GetAssets()->DrawBackgroundModel(this->display->GetCamera());
	this->display->GetAssets()->DrawBackgroundEffects(this->display->GetCamera());

	glPopMatrix();

	// Unbind the background FBO
	backgroundFBO->UnbindFBObj();

	debug_opengl_state();
}

void InGameDisplayState::RenderForegroundWithBackgroundToFBO(double dT) {
	const Camera& camera = this->display->GetCamera();
	
	Vector2D negHalfLevelDim = -0.5 * this->display->GetModel()->GetLevelUnitDimensions();

	GameFBOAssets* fboAssets = this->display->GetAssets()->GetFBOAssets();
	assert(fboAssets != NULL);

	FBObj* postFullSceneFBO = fboAssets->GetPostFullSceneFBO();
	FBObj* fullSceneFBO = fboAssets->GetFullSceneFBO();
	FBObj* backgroundFBO = fboAssets->GetBackgroundFBO();
	
	assert(postFullSceneFBO != NULL);
	assert(fullSceneFBO != NULL);
	assert(backgroundFBO != NULL);

	fullSceneFBO->BindFBObj();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	backgroundFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);

	glPushMatrix();
	Matrix4x4 gameTransform = this->display->GetModel()->GetTransformInfo()->GetGameTransform();
	glMultMatrixf(gameTransform.begin());

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Paddle...
	this->display->GetAssets()->DrawPaddle(dT, *this->display->GetModel()->GetPlayerPaddle(), camera);

	glPopMatrix();

	// Level pieces
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->DrawLevelPieces(dT, currLevel, camera);

	// Balls...
	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	this->display->GetAssets()->DrawGameBalls(dT, *this->display->GetModel(), camera, negHalfLevelDim);
	glPopMatrix();

	// Safety net (if active)
	this->display->GetAssets()->DrawSafetyNetIfActive(dT, currLevel, camera);

	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Draw the dropping items if not in the last pass
	if (!fboAssets->DrawItemsInLastPass()) {
		std::list<GameItem*>& gameItems = this->display->GetModel()->GetLiveItems();
		for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); ++iter) {
			this->display->GetAssets()->DrawItem(dT, camera, (**iter));
		}				
	}

	// Render the beam effects
	this->display->GetAssets()->DrawBeams(dT, *this->display->GetModel(), camera);

	fullSceneFBO->UnbindFBObj();
	
	// Draw Post-Fullscene effects
	postFullSceneFBO->BindFBObj();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	fullSceneFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1);
	
	// Render any post-processing effects for various items/objects in the game
	this->display->GetAssets()->DrawPaddlePostEffects(dT, *this->display->GetModel(), camera);

	postFullSceneFBO->UnbindFBObj();

	glPopMatrix();

	// Do a gaussian blur for a softer feeling
	this->display->GetAssets()->GetFBOAssets()->RenderFullSceneBlur(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);

	debug_opengl_state();
}

void InGameDisplayState::RenderFinalGather(double dT) {
	GameFBOAssets* fboAssets = this->display->GetAssets()->GetFBOAssets();
	GameModel* gameModel = this->display->GetModel();
	const Camera& camera = this->display->GetCamera();

	// Render fullscreen effects 
	FBObj* initialFBO = fboAssets->RenderInitialFullscreenEffects(camera.GetWindowWidth(), camera.GetWindowHeight(), dT);
	assert(initialFBO != NULL);

	FBObj* finalFBO = fboAssets->GetFinalFullScreenFBO();
	assert(finalFBO != NULL);

	// Final non-fullscreen draw pass - draw the falling items and particles
	finalFBO->BindFBObj();
	initialFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);

	// Render all effects that do not go through all the post-processing filters...
	Vector2D negHalfLevelDim = -0.5 * gameModel->GetLevelUnitDimensions();
	glPushMatrix();
	Matrix4x4 gameTransform = gameModel->GetTransformInfo()->GetGameTransform();
	glMultMatrixf(gameTransform.begin());
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	
	// Draw the dropping items if in the last pass
	if (fboAssets->DrawItemsInLastPass()) {
		std::list<GameItem*>& gameItems = gameModel->GetLiveItems();
		for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); ++iter) {
			this->display->GetAssets()->DrawItem(dT, camera, (**iter));
		}			
	}

	// Typical Particle effects...
	this->display->GetAssets()->GetESPAssets()->DrawParticleEffects(dT, camera, Vector3D(negHalfLevelDim));

	// Absolute post effects call for various object effects
	this->display->GetAssets()->DrawGameBallsPostEffects(dT, *gameModel, camera);

	finalFBO->UnbindFBObj();

	// Render the final fullscreen effects
	fboAssets->RenderFinalFullscreenEffects(camera.GetWindowWidth(), camera.GetWindowHeight(), dT, *gameModel);

	glPopMatrix();
	debug_opengl_state();
}

/**
 * Helper function for drawing the Heads-Up-Display (HUD) for the game, 
 * including points, lives left, etc.
 */
void InGameDisplayState::DrawGameHUD(double dT) {
	
	GameModel* gameModel = this->display->GetModel();
	const Camera& camera = this->display->GetCamera();

	const int DISPLAY_WIDTH  = camera.GetWindowWidth();
	const int DISPLAY_HEIGHT = camera.GetWindowHeight();

	// Draw the points in the top-right corner of the display
	std::stringstream ptStrStream;
	ptStrStream << gameModel->GetScore();
	this->scoreLabel.SetText(ptStrStream.str());
	this->scoreLabel.SetTopLeftCorner(Point2D(DISPLAY_WIDTH - HUD_X_INDENT - this->scoreLabel.GetLastRasterWidth(), DISPLAY_HEIGHT - HUD_Y_INDENT));
	this->scoreLabel.Draw();

	// Draw the number of lives left in the top-left corner of the display
	this->display->GetAssets()->GetLifeHUD()->Draw(dT, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	
	// Draw the timers that are currently in existance
	this->display->GetAssets()->DrawTimers(dT, camera);

	// Draw any HUD special elements based on currently active items, etc.
	this->display->GetAssets()->DrawActiveItemHUDElements(dT, *gameModel, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	debug_opengl_state();
}

/**
 * Called when the display size changes - we need to recreate our framebuffer objects 
 * to account for the new screen size.
 */
void InGameDisplayState::DisplaySizeChanged(int width, int height) {
	this->display->GetAssets()->GetFBOAssets()->ResizeFBOAssets(width, height);
}

#ifdef _DEBUG
/**
 * Debugging function that draws the collision boundries of level pieces.
 */
void InGameDisplayState::DebugDrawBounds() {
	if (!GameDisplay::IsDrawDebugBoundsOn()) { return; }
	Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Draw boundry of paddle...
	this->display->GetModel()->GetPlayerPaddle()->DebugDraw();
	
	// Draw of boundries of each block...
	std::vector<std::vector<LevelPiece*>> pieces = this->display->GetModel()->GetCurrentLevel()->GetCurrentLevelLayout();
	for (size_t i = 0; i < pieces.size(); i++) {
		std::vector<LevelPiece*> setOfPieces = pieces[i];
		for (size_t j = 0; j < setOfPieces.size(); j++) {
			setOfPieces[j]->DebugDraw();
		}
	}

	// Draw any beam rays...
	std::list<Beam*>& beams = this->display->GetModel()->GetActiveBeams();
	for (std::list<Beam*>::const_iterator iter = beams.begin(); iter != beams.end(); ++iter) {
		(*iter)->DebugDraw();
	}

	glPopMatrix();
	debug_opengl_state();
}
#endif