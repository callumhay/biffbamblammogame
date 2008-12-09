#include "InGameDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameViewConstants.h"

// model stuffs
#include "../GameModel/GameModel.h"

#include "../BlammoEngine/BlammoEngine.h"



const std::string InGameDisplayState::LIVES_LABEL_TEXT = "Lives: ";
const unsigned int InGameDisplayState::HUD_X_INDENT = 10;	
const unsigned int InGameDisplayState::HUD_Y_INDENT = 10;

InGameDisplayState::InGameDisplayState(GameDisplay* display) : DisplayState(display), 
mvEffector(Vector3D(0, -2.5, 0)),
colourEffector(1, 0) {

	// Render to texture setup
	this->renderToTexBeforeBall = Texture2D::CreateEmptyTexture2D(Texture::Nearest, display->GetDisplayWidth(), display->GetDisplayHeight());
	assert(this->renderToTexBeforeBall != NULL);

	// Set HUD display elements
	float dropShadowAmt = 0.05f;
	Colour shadowColourHUD(0, 0, 0);
	Colour textColourHUD(1, 1, 1);
	// Score display
	this->scoreLabel = TextLabel2D(this->display->GetAssets()->GetFont(GameAssets::AllPurpose, GameAssets::Small), "0");
	this->scoreLabel.SetColour(textColourHUD);
	this->scoreLabel.SetDropShadow(shadowColourHUD, dropShadowAmt);
	// Lives left display
	this->livesLabel = TextLabel2D(this->display->GetAssets()->GetFont(GameAssets::AllPurpose, GameAssets::Small), LIVES_LABEL_TEXT);
	this->livesLabel.SetColour(textColourHUD);
	this->livesLabel.SetDropShadow(shadowColourHUD, dropShadowAmt);

	// TODO: GET RID OF THIS
	//bool result = ptEmitTest.SetParticles(1, GameViewConstants::GetInstance()->RESOURCE_DIR + "/" + GameViewConstants::GetInstance()->TEXTURE_DIR + "/test_textures/TestSprite.png");//Smoke_Puff1_64x64.png");
	//assert(result);

	DropShadow dpTemp;
	dpTemp.amountPercentage = 0.20f;
	for (int i = 0; i < 50; i++) {
		ESPOnomataParticle* newParticle = new ESPOnomataParticle(this->display->GetAssets()->GetFont(GameAssets::AllPurpose, GameAssets::Small));

		newParticle->SetDropShadow(dpTemp);
		this->bunchOParticles.push_back(newParticle);
		ptEmitTest.AddParticle(newParticle);
	}

	ptEmitTest.SetSpawnDelta(ESPInterval(1, 2));
	ptEmitTest.SetInitialSpd(ESPInterval(1, 3));
	ptEmitTest.SetParticleLife(ESPInterval(10, 10));
	ptEmitTest.SetParticleSize(ESPInterval(1, 1));
	ptEmitTest.SetEmitAngleInDegrees(40);
	ptEmitTest.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	ptEmitTest.SetParticleAlignment(ESP::ViewPointAligned);
	//ptEmitTest.AddEffector(&colourEffector);
	//ptEmitTest.AddEffector(&mvEffector);
}

InGameDisplayState::~InGameDisplayState() {
	delete this->renderToTexBeforeBall;

	// GET RID OF THIS
	for (size_t i = 0; i < this->bunchOParticles.size(); i++) {
		delete this->bunchOParticles[i];
	}
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
	this->display->GetCamera().ApplyCameraTransform();

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

	glPushMatrix();
	
	// Draw the background scenery
	Vector2D levelDim = this->display->GetModel()->GetLevelUnitDimensions();
	glTranslatef(0.0f, -levelDim[1]/2.0f, 0.0f);
	this->display->GetAssets()->DrawBackground(dT, this->display->GetCamera());
	
	FBOManager::GetInstance()->UnbindFBO();												// Unbind the FBO so that we can use the render to texture for special ball effects
	this->renderToTexBeforeBall->RenderTextureToFullscreenQuad(); // Render the texture to a fullscreen quad

	// Enable multisampling for the foreground rendering
	glEnable(GL_MULTISAMPLE);

	// Draw the foreground stuff (paddle, items, ball)
	glTranslatef(-levelDim[0]/2.0f, 0, 0.0f);	

	// Items...
	std::list<GameItem*>& gameItems = this->display->GetModel()->GetLiveItems();
	for (std::list<GameItem*>::iterator iter = gameItems.begin(); iter != gameItems.end(); iter++) {
		this->display->GetAssets()->DrawItem((**iter), this->display->GetCamera());
	}

	// Paddle...
	this->display->GetAssets()->DrawPaddle(*this->display->GetModel()->GetPlayerPaddle(), this->display->GetCamera());
	
	// Ball...
	this->display->GetAssets()->DrawGameBall(*this->display->GetModel()->GetGameBall(), this->display->GetCamera(), this->renderToTexBeforeBall);
	glPopMatrix();
	
	this->display->GetAssets()->DrawLevelPieces(this->display->GetCamera());
	
	glDisable(GL_MULTISAMPLE);

	// TODO: get rid of this
	//glPushMatrix();
	//glLoadIdentity();
	ptEmitTest.Tick(dT);
	ptEmitTest.Draw(this->display->GetCamera());
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