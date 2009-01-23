#include "InGameDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"

// Game Model stuff
#include "../GameModel/GameModel.h"

// Engine stuff
#include "../BlammoEngine/BlammoEngine.h"


const std::string InGameDisplayState::LIVES_LABEL_TEXT = "Lives: ";
const unsigned int InGameDisplayState::HUD_X_INDENT = 10;	
const unsigned int InGameDisplayState::HUD_Y_INDENT = 10;

InGameDisplayState::InGameDisplayState(GameDisplay* display) : DisplayState(display) {

	// Render to texture setup
	this->renderToTexBeforeBall = Texture2D::CreateEmptyTexture2D(Texture::Nearest, display->GetDisplayWidth(), display->GetDisplayHeight());
	assert(this->renderToTexBeforeBall != NULL);

	// Set HUD display elements
	float dropShadowAmt = 0.05f;
	Colour shadowColourHUD(0, 0, 0);
	Colour textColourHUD(1, 1, 1);
	// Score display
	this->scoreLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), "0");
	this->scoreLabel.SetColour(textColourHUD);
	this->scoreLabel.SetDropShadow(shadowColourHUD, dropShadowAmt);
	// Lives left display
	this->livesLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), LIVES_LABEL_TEXT);
	this->livesLabel.SetColour(textColourHUD);
	this->livesLabel.SetDropShadow(shadowColourHUD, dropShadowAmt);
}

InGameDisplayState::~InGameDisplayState() {
	delete this->renderToTexBeforeBall;
}

/**
 * This will render a frame of the actual game while it is in-play.
 */
void InGameDisplayState::RenderFrame(double dT) {

	// Attach the FBO
	bool success = FBOManager::GetInstance()->SetupFBO(*this->renderToTexBeforeBall);
	assert(success);
	FBOManager::GetInstance()->BindFBO();

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Camera Stuff 
	this->display->GetCamera().SetPerspective(this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	this->display->GetCamera().ApplyCameraTransform(dT);
	
	// Draw the game scene
	this->DrawGameScene(dT);
	
	// Draw the HUD
	this->DrawGameHUD();
}

// Private helper functions ************************************************************

/**
 * Helper function for drawing the game scene - this includes background and foreground objects
 * related to the game itself.
 */
void InGameDisplayState::DrawGameScene(double dT) {

	Vector2D negHalfLevelDim = -0.5 * this->display->GetModel()->GetLevelUnitDimensions();
	glPushMatrix();
	
	// Draw the background scenery
	glTranslatef(0.0f, negHalfLevelDim[1], 0.0f);
	this->display->GetAssets()->DrawSkybox(dT, this->display->GetCamera());
	this->display->GetAssets()->DrawBackgroundModel(dT, this->display->GetCamera());
	this->display->GetAssets()->DrawBackgroundEffects(dT, this->display->GetCamera());
	
	FBOManager::GetInstance()->UnbindFBO();												// Unbind the FBO so that we can use the render to texture for special ball effects
	this->renderToTexBeforeBall->RenderTextureToFullscreenQuad(); // Render the texture to a fullscreen quad

	// Enable multisampling for the foreground rendering
	glEnable(GL_MULTISAMPLE);

	// Redraw the background model using multisampling... too expensive...
	//this->display->GetAssets()->DrawBackgroundModel(dT, this->display->GetCamera());

	// Draw the foreground stuff (paddle, items, ball)
	glTranslatef(negHalfLevelDim[0], 0, 0.0f);	

	// Items...
	std::list<GameItem*>& gameItems = this->display->GetModel()->GetLiveItems();
	for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); iter++) {
		this->display->GetAssets()->DrawItem(dT, this->display->GetCamera(), (**iter));
	}

	// Paddle...
	this->display->GetAssets()->DrawPaddle(*this->display->GetModel()->GetPlayerPaddle(), this->display->GetCamera());
	
	// Ball...
	this->display->GetAssets()->DrawGameBall(dT, *this->display->GetModel()->GetGameBall(), this->display->GetCamera(), this->renderToTexBeforeBall);
	glPopMatrix();
	
	// Level pieces (blocks and stuff)...
	this->display->GetAssets()->DrawLevelPieces(this->display->GetCamera());
	glDisable(GL_MULTISAMPLE);

	// Draw particles / ESP effects
	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	this->display->GetAssets()->DrawParticleEffects(dT, this->display->GetCamera());
	glPopMatrix();

	// Debug draw of boundries of each block...
	//glPushMatrix();
	//glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);
	//std::vector<std::vector<LevelPiece*>> pieces = this->display->GetModel()->GetCurrentLevel()->GetCurrentLevelLayout();
	//for (size_t i = 0; i < pieces.size(); i++) {
		//std::vector<LevelPiece*> setOfPieces = pieces[i];
		//for (size_t j = 0; j < setOfPieces.size(); j++) {
			//setOfPieces[j]->DebugDraw();
		//}
	//}
	//glPopMatrix();
}

/**
 * Helper function for drawing the Heads-Up-Display (HUD) for the game, 
 * including points, lives left, etc.
 */
void InGameDisplayState::DrawGameHUD() {
	// Draw the points in the top-right corner of the display
	std::stringstream ptStrStream;
	ptStrStream << this->display->GetModel()->GetScore();
	this->scoreLabel.SetText(ptStrStream.str());
	this->scoreLabel.SetTopLeftCorner(Point2D(this->display->GetDisplayWidth() - HUD_X_INDENT - this->scoreLabel.GetLastRasterWidth(), 
																			      this->display->GetDisplayHeight() - HUD_Y_INDENT));
	this->scoreLabel.Draw();

	// Draw the number of lives left in the top-left corner of the display
	// TODO: figure out number of lives... also, perhaps sprites or models instead?
	std::stringstream livesStrStream;
	livesStrStream << LIVES_LABEL_TEXT << this->display->GetModel()->GetLivesLeft();
	this->livesLabel.SetText(livesStrStream.str());
	this->livesLabel.SetTopLeftCorner(Point2D(HUD_X_INDENT, this->display->GetDisplayHeight() - HUD_Y_INDENT));
	this->livesLabel.Draw();

	// Draw the timers that are currently in existance
	const std::list<GameItemTimer*>& activeTimers = this->display->GetModel()->GetActiveTimers();
	this->display->GetAssets()->DrawTimers(activeTimers, this->display->GetDisplayWidth(), this->display->GetDisplayHeight());

}

void InGameDisplayState::DisplaySizeChanged(int width, int height) {
	delete this->renderToTexBeforeBall;
	this->renderToTexBeforeBall = Texture2D::CreateEmptyTexture2D(Texture::Nearest, width, height);
}