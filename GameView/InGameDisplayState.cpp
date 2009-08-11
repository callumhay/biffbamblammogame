#include "InGameDisplayState.h"
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
	camera.SetPerspectiveWithFOV(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), 
															 this->display->GetModel()->GetTransformInfo()->GetCameraFOVAngle());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// TODO: since we now reset the camera and reapply its transform each frame, we need to tell the MODEL about debug transforms!!
	camera.SetTransform(this->display->GetModel()->GetTransformInfo()->GetCameraTransform());
	camera.ApplyCameraTransform(dT);
	

	debug_opengl_state();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Draw the game scene
	this->DrawGameScene(dT);
	
	// Draw the HUD
	this->DrawGameHUD(dT);
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

	//glPushMatrix();
	//glScalef(5,5,5);
	//GeometryMaker::GetInstance()->DrawSphere();
	//glPopMatrix();

	// Unbind the background FBO
	backgroundFBO->UnbindFBObj();

	debug_opengl_state();
}

void InGameDisplayState::RenderForegroundWithBackgroundToFBO(double dT) {
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

	// Level pieces
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->DrawLevelPieces(dT, currLevel, this->display->GetCamera());

	// Balls...
	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	this->display->GetAssets()->DrawGameBalls(dT, *this->display->GetModel(), this->display->GetCamera(), negHalfLevelDim);
	glPopMatrix();

	// Safety net (if active)
	this->display->GetAssets()->DrawSafetyNetIfActive(dT, currLevel, this->display->GetCamera());

	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Draw the dropping items if not in the last pass
	if (!fboAssets->DrawItemsInLastPass()) {
		std::list<GameItem*>& gameItems = this->display->GetModel()->GetLiveItems();
		for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); iter++) {
			this->display->GetAssets()->DrawItem(dT, this->display->GetCamera(), (**iter));
		}				
	}

	// Paddle...
	this->display->GetAssets()->DrawPaddle(dT, *this->display->GetModel()->GetPlayerPaddle(), this->display->GetCamera());

	fullSceneFBO->UnbindFBObj();
	
	// Draw Post-Fullscene effects
	postFullSceneFBO->BindFBObj();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	fullSceneFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1);
	// Render any post-processing effects for various items/objects in the game
	this->display->GetAssets()->DrawPaddlePostEffects(dT, *this->display->GetModel()->GetPlayerPaddle(), this->display->GetCamera());
	
	postFullSceneFBO->UnbindFBObj();

	glPopMatrix();

	// Do a gaussian blur for a softer feeling
	this->display->GetAssets()->GetFBOAssets()->RenderFullSceneBlur(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), dT);

	debug_opengl_state();
}

void InGameDisplayState::RenderFinalGather(double dT) {
	GameFBOAssets* fboAssets = this->display->GetAssets()->GetFBOAssets();

	// Render fullscreen effects 
	FBObj* initialFBO = fboAssets->RenderInitialFullscreenEffects(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), dT);
	assert(initialFBO != NULL);

	FBObj* finalFBO = fboAssets->GetFinalFullScreenFBO();
	assert(finalFBO != NULL);

	// Final non-fullscreen draw pass - draw the falling items and particles
	finalFBO->BindFBObj();
	initialFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);

	// Render all effects that do not go through all the post-processing filters...
	Vector2D negHalfLevelDim = -0.5 * this->display->GetModel()->GetLevelUnitDimensions();
	glPushMatrix();
	Matrix4x4 gameTransform = this->display->GetModel()->GetTransformInfo()->GetGameTransform();
	glMultMatrixf(gameTransform.begin());
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	
	// Draw the dropping items if in the last pass
	if (fboAssets->DrawItemsInLastPass()) {
		std::list<GameItem*>& gameItems = this->display->GetModel()->GetLiveItems();
		for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); iter++) {
			this->display->GetAssets()->DrawItem(dT, this->display->GetCamera(), (**iter));
		}			
	}

	// Typical Particle effects...
	this->display->GetAssets()->GetESPAssets()->DrawParticleEffects(dT, this->display->GetCamera());

	// Absolute final effects call for various object effects
	this->display->GetAssets()->DrawGameBallsFinalEffects(dT, *this->display->GetModel(), this->display->GetCamera());

	finalFBO->UnbindFBObj();

	// Render the final fullscreen effects
	fboAssets->RenderFinalFullscreenEffects(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), dT);

	glPopMatrix();
	debug_opengl_state();
}

/**
 * Helper function for drawing the Heads-Up-Display (HUD) for the game, 
 * including points, lives left, etc.
 */
void InGameDisplayState::DrawGameHUD(double dT) {
	// Draw the points in the top-right corner of the display
	std::stringstream ptStrStream;
	ptStrStream << this->display->GetModel()->GetScore();
	this->scoreLabel.SetText(ptStrStream.str());
	this->scoreLabel.SetTopLeftCorner(Point2D(this->display->GetDisplayWidth() - HUD_X_INDENT - this->scoreLabel.GetLastRasterWidth(), 
																			      this->display->GetDisplayHeight() - HUD_Y_INDENT));
	this->scoreLabel.Draw();

	// Draw the number of lives left in the top-left corner of the display
	this->display->GetAssets()->GetLifeHUD()->Draw(dT, this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	
	// Draw the timers that are currently in existance
	const std::list<GameItemTimer*>& activeTimers = this->display->GetModel()->GetActiveTimers();
	this->display->GetAssets()->DrawTimers(activeTimers, this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
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

	// Debug draw boundry of paddle...
	this->display->GetModel()->GetPlayerPaddle()->DebugDraw();
	
	// Debug draw of boundries of each block...
	std::vector<std::vector<LevelPiece*>> pieces = this->display->GetModel()->GetCurrentLevel()->GetCurrentLevelLayout();
	for (size_t i = 0; i < pieces.size(); i++) {
		std::vector<LevelPiece*> setOfPieces = pieces[i];
		for (size_t j = 0; j < setOfPieces.size(); j++) {
			setOfPieces[j]->DebugDraw();
		}
	}
	glPopMatrix();
	debug_opengl_state();
}
#endif