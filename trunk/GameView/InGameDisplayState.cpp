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
	this->display->GetCamera().SetPerspective(this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	this->display->GetCamera().ApplyCameraTransform(dT);

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
	Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();		
	
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
	Vector2D negHalfLevelDim = -0.5 * this->display->GetModel()->GetLevelUnitDimensions();

	FBObj* fullSceneFBO = this->display->GetAssets()->GetFBOAssets()->GetFullSceneFBO();
	FBObj* backgroundFBO = this->display->GetAssets()->GetFBOAssets()->GetBackgroundFBO();
	assert(fullSceneFBO != NULL);
	assert(backgroundFBO != NULL);

	fullSceneFBO->BindFBObj();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	backgroundFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);

	glPushMatrix();
	Matrix4x4 gameTransform = this->display->GetModel()->GetTransformInfo().GetGameTransform();
	glMultMatrixf(gameTransform.begin());

	// Level pieces...
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->DrawLevelPieces(dT, currLevel, this->display->GetCamera());

	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Paddle...
	this->display->GetAssets()->DrawPaddle(dT, *this->display->GetModel()->GetPlayerPaddle(), this->display->GetCamera());

	// Balls...	
	this->display->GetAssets()->DrawGameBalls(dT, *this->display->GetModel(), this->display->GetCamera(), backgroundFBO->GetFBOTexture(), negHalfLevelDim);

	glPopMatrix();

	fullSceneFBO->UnbindFBObj();

	// Do a gaussian blur on the foreground for a softer feeling
	this->display->GetAssets()->GetFBOAssets()->RenderFullSceneBlur(this->display->GetDisplayWidth(), this->display->GetDisplayHeight());

	debug_opengl_state();
}

void InGameDisplayState::RenderFinalGather(double dT) {
	GameFBOAssets* fboAssets = this->display->GetAssets()->GetFBOAssets();

	// Do some purdy bloom - this adds a nice contrasty highlighted touch to the entire scene
	fboAssets->RenderBloom(this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	// Add motion blur / afterimage effect 
	fboAssets->RenderAfterImage(this->display->GetDisplayWidth(), this->display->GetDisplayHeight());

	// Render all effects that do not go through all the post-processing filters...
	Vector2D negHalfLevelDim = -0.5 * this->display->GetModel()->GetLevelUnitDimensions();
	glPushMatrix();
	Matrix4x4 gameTransform = this->display->GetModel()->GetTransformInfo().GetGameTransform();
	glMultMatrixf(gameTransform.begin());
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	
	// Items...
	std::list<GameItem*>& gameItems = this->display->GetModel()->GetLiveItems();
	for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); iter++) {
		this->display->GetAssets()->DrawItem(dT, this->display->GetCamera(), (**iter));
	}	

	// Typical Particle effects...
	this->display->GetAssets()->GetESPAssets()->DrawParticleEffects(dT, this->display->GetCamera());
	// Post-processing effects...
	this->display->GetAssets()->GetESPAssets()->DrawPostProcessingESPEffects(dT, this->display->GetCamera(), fboAssets->GetFullSceneFBO()->GetFBOTexture());

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