#include "InGameDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"
#include "LivesLeftHUD.h"

// Game Model stuff
#include "../GameModel/GameModel.h"

// Engine stuff
#include "../BlammoEngine/BlammoEngine.h"


const std::string InGameDisplayState::LIVES_LABEL_TEXT = "Lives: ";
const unsigned int InGameDisplayState::HUD_X_INDENT = 10;	
const unsigned int InGameDisplayState::HUD_Y_INDENT = 10;

InGameDisplayState::InGameDisplayState(GameDisplay* display) : DisplayState(display) {

	// Render to texture setup
	this->renderToTexBackground = Texture2D::CreateEmptyTextureRectangle(display->GetDisplayWidth(), display->GetDisplayHeight());
	assert(this->renderToTexBackground != NULL);
	this->renderToTexEverything = Texture2D::CreateEmptyTextureRectangle(display->GetDisplayWidth(), display->GetDisplayHeight());
	assert(this->renderToTexEverything != NULL);

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
	delete this->renderToTexEverything;
	this->renderToTexEverything = NULL;
	delete this->renderToTexBackground;
	this->renderToTexBackground = NULL;
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

	// Render the full scene into an FBO
	//this->RenderFullSceneToFBO(dT);
	this->RenderBackgroundToFBO();

	// Redraw a multisampled background model, pieces, items, ball and paddle
	this->DrawScene(dT);

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Post-processing effects...
	this->display->GetAssets()->GetESPAssets()->DrawPostProcessingESPEffects(dT, this->display->GetCamera(), this->renderToTexEverything);
	glPopMatrix();
	
	debug_opengl_state();

#ifdef _DEBUG
	this->DebugDrawBounds();
	this->display->GetAssets()->DebugDrawLights();
#endif
}

/**
 * Render the background of the current scene into the background FBO.
 */
void InGameDisplayState::RenderBackgroundToFBO() { 
	Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();
	
	// Attach the background FBO
	bool success = FBOManager::GetInstance()->SetupFBO(*this->renderToTexBackground);
	assert(success);
	FBOManager::GetInstance()->BindFBO();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Draw the background of the current scene
	glPushMatrix();
	glTranslatef(0.0f, negHalfLevelDim[1], 0.0f);
	this->display->GetAssets()->DrawBackgroundModel(this->display->GetCamera());
	this->display->GetAssets()->DrawSkybox(this->display->GetCamera());
	this->display->GetAssets()->DrawBackgroundEffects(this->display->GetCamera());
	
	glPopMatrix();

	// Unbind the background FBO
	FBOManager::GetInstance()->UnbindFBO(*this->renderToTexBackground);
	debug_opengl_state();
}

/**
 * Render the entire scene (except for post processing effects / particles) into
 * the 'everything' FBO.
 */
void InGameDisplayState::RenderFullSceneToFBO(double dT) {
	// Attach the full scene FBO
	bool success = FBOManager::GetInstance()->SetupFBO(*this->renderToTexEverything);
	assert(success);
	FBOManager::GetInstance()->BindFBO();

	this->DrawScene(dT);

	// Unbind the full scene FBO
	FBOManager::GetInstance()->UnbindFBO(*this->renderToTexEverything);
	debug_opengl_state();
}

/**
 * Draw the entire game scene - this does not render to FBO and allows for multisampling/antialiasing.
 */
void InGameDisplayState::DrawScene(double dT) {
	Vector2D negHalfLevelDim = -0.5 * this->display->GetModel()->GetLevelUnitDimensions();

	// Start by rendering a full screen quad of the scene background rendered using FBO and texture
	this->renderToTexBackground->RenderTextureToFullscreenQuad();

	// Now draw everything again, this time as multisampled
	glEnable(GL_MULTISAMPLE);

	// Background Model and effects...
	glPushMatrix();
	glTranslatef(0.0f, negHalfLevelDim[1], 0.0f);
	this->display->GetAssets()->DrawBackgroundModel(this->display->GetCamera());
	this->display->GetAssets()->DrawBackgroundEffects(this->display->GetCamera());
	glPopMatrix();

	glPushMatrix();
	Matrix4x4 gameTransform = this->display->GetModel()->GetTransformInfo().GetGameTransform();
	glMultMatrixf(gameTransform.begin());

	// Level pieces...
	const GameLevel* currLevel = this->display->GetModel()->GetCurrentLevel();
	this->display->GetAssets()->DrawLevelPieces(dT, currLevel, this->display->GetCamera());

	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	
	// Items...
	std::list<GameItem*>& gameItems = this->display->GetModel()->GetLiveItems();
	for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); iter++) {
		this->display->GetAssets()->DrawItem(dT, this->display->GetCamera(), (**iter));
	}	

	// Paddle...
	this->display->GetAssets()->DrawPaddle(dT, *this->display->GetModel()->GetPlayerPaddle(), this->display->GetCamera());

	// Balls...	
	this->display->GetAssets()->DrawGameBalls(dT, *this->display->GetModel(), this->display->GetCamera(), this->renderToTexBackground, negHalfLevelDim);

	glDisable(GL_MULTISAMPLE);

	// Typical Particle effects...
	this->display->GetAssets()->GetESPAssets()->DrawParticleEffects(dT, this->display->GetCamera());
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

void InGameDisplayState::DisplaySizeChanged(int width, int height) {
	delete this->renderToTexEverything;
	this->renderToTexEverything = Texture2D::CreateEmptyTextureRectangle(width, height);
	delete this->renderToTexBackground;
	this->renderToTexBackground = Texture2D::CreateEmptyTextureRectangle(width, height);
	debug_opengl_state();
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