
#include "LevelStartDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFontAssetsManager.h"
#include "LevelMesh.h"

#include "../GameModel/GameModel.h"

const double LevelStartDisplayState::FADE_IN_TIME										= 1.5;
const double LevelStartDisplayState::WIPE_TIME											= 1.0;
const double LevelStartDisplayState::LEVEL_TEXT_FADE_OUT_TIME				= 3.0;
const double LevelStartDisplayState::LEVEL_BLOCK_FADE_IN_TIME				= 2.0;

const float LevelStartDisplayState::LEVEL_NAME_WIPE_FADE_QUAD_SIZE	= 100.0f;
const float LevelStartDisplayState::LEVEL_TEXT_X_PADDING						= 50;			// Padding from the right-hand side of the screen to the level name text
const float LevelStartDisplayState::LEVEL_TEXT_Y_PADDING						= 80;			// Padding from the bottom of the screen to the bottom of the level name text

LevelStartDisplayState::LevelStartDisplayState(GameDisplay* display) : 
DisplayState(display), renderPipeline(display),
levelNameLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big), "") {

	Camera& camera = this->display->GetCamera();

	// Clear up any stuff to an initial state in cases where things might still 
	// be set unchanged from a previously loaded game
	camera.ClearCameraShake();

	// Setup the level name text label
	const GameLevel* level = this->display->GetModel()->GetCurrentLevel();
	levelNameLabel.SetText(level->GetName());
	levelNameLabel.SetColour(Colour(1.0f, 0.78f, 0.0f));
	float startDropShadowAmt = 0.05f;
	levelNameLabel.SetDropShadow(Colour(0.0f, 0.0f, 0.0f), startDropShadowAmt);
	levelNameLabel.SetScale(1.5f * this->display->GetTextScalingFactor());

	const Point2D levelTextTopLeftCorner(camera.GetWindowWidth() - levelNameLabel.GetLastRasterWidth() - LevelStartDisplayState::LEVEL_TEXT_X_PADDING,
		levelNameLabel.GetHeight() + LevelStartDisplayState::LEVEL_TEXT_Y_PADDING);
	levelNameLabel.SetTopLeftCorner(levelTextTopLeftCorner);

	// Setup any animations when starting up a level...
	this->fadeInAnimation.SetLerp(LevelStartDisplayState::WIPE_TIME + 1.0, LevelStartDisplayState::WIPE_TIME + 1.0 + LevelStartDisplayState::FADE_IN_TIME, 1.0f, 0.0f);
	this->fadeInAnimation.SetRepeat(false);
	this->fadeInAnimation.SetInterpolantValue(1.0f);

	this->showLevelNameWipeAnimation.SetLerp(0.0, LevelStartDisplayState::WIPE_TIME, 
		0.0f, this->levelNameLabel.GetLastRasterWidth() + LevelStartDisplayState::LEVEL_TEXT_X_PADDING + 2 * LevelStartDisplayState::LEVEL_NAME_WIPE_FADE_QUAD_SIZE);
	this->showLevelNameWipeAnimation.SetRepeat(false);
	this->showLevelNameWipeAnimation.SetInterpolantValue(0.0f);

	double startOfNameFadeOut = LevelStartDisplayState::WIPE_TIME + 0.5;
	double endOfNameFadeOut   = startOfNameFadeOut + LevelStartDisplayState::LEVEL_TEXT_FADE_OUT_TIME;
	this->levelNameFadeOutAnimation.SetLerp(startOfNameFadeOut, endOfNameFadeOut, 1.0f, 0.0f);
	this->levelNameFadeOutAnimation.SetRepeat(false);
	this->levelNameFadeOutAnimation.SetInterpolantValue(1.0f);

	this->dropShadowAnimation.SetLerp(startOfNameFadeOut, endOfNameFadeOut, startDropShadowAmt, 0.5f);
	this->dropShadowAnimation.SetRepeat(false);
	this->dropShadowAnimation.SetInterpolantValue(startDropShadowAmt);

	double startTimeOfBlockFadeIn = LevelStartDisplayState::WIPE_TIME + 1.0 + LevelStartDisplayState::FADE_IN_TIME;
	double endOfBlockFadeIn       = startTimeOfBlockFadeIn + LevelStartDisplayState::LEVEL_BLOCK_FADE_IN_TIME;
	this->blockFadeInAnimation.SetLerp(startTimeOfBlockFadeIn, endOfBlockFadeIn, 0.0f, 1.0f);
	this->blockFadeInAnimation.SetRepeat(false);
	this->blockFadeInAnimation.SetInterpolantValue(0.0f);
}

LevelStartDisplayState::~LevelStartDisplayState() {
}

void LevelStartDisplayState::RenderFrame(double dT) {
	const Camera& camera = this->display->GetCamera();

	bool fadeInDone           = this->fadeInAnimation.Tick(dT);
	bool wipeInDone           = this->showLevelNameWipeAnimation.Tick(dT);
	bool levelTextFadeOutDone = this->levelNameFadeOutAnimation.Tick(dT);
	bool dropShadowAnimDone   = this->dropShadowAnimation.Tick(dT);
	bool levelPieceFadeInDone = this->blockFadeInAnimation.Tick(dT);

	// Fade in the level pieces...
	if (!levelPieceFadeInDone) {
		this->display->GetAssets()->GetCurrentLevelMesh()->SetLevelAlpha(this->blockFadeInAnimation.GetInterpolantValue());

	}

	renderPipeline.RenderFrameWithoutHUD(dT);

	// Render the fade-in if we haven't already
	if (!fadeInDone) {
		// Draw the fade quad overlay
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 1.0f, 
																										 ColourRGBA(1, 1, 1, this->fadeInAnimation.GetInterpolantValue()));
		glPopAttrib();
	}

	if (!levelTextFadeOutDone) {
		// If we're done the fade in then we need to do the level text display...
		this->levelNameLabel.SetAlpha(this->levelNameFadeOutAnimation.GetInterpolantValue());
		this->levelNameLabel.SetDropShadowAmount(this->dropShadowAnimation.GetInterpolantValue());
		this->levelNameLabel.Draw();

		if (!wipeInDone) {
			glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_POLYGON_BIT);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			const float hPadding              = 20;
			const float quadHeight						= this->levelNameLabel.GetHeight() + 2 * LevelStartDisplayState::LEVEL_TEXT_Y_PADDING;
			const float currOpaqueQuadWidth	  = hPadding + this->levelNameLabel.GetLastRasterWidth() + LevelStartDisplayState::LEVEL_TEXT_X_PADDING + LevelStartDisplayState::LEVEL_NAME_WIPE_FADE_QUAD_SIZE - 
																					this->showLevelNameWipeAnimation.GetInterpolantValue();
			Camera::PushWindowCoords();

			//const float originUTexCoord = (this->levelNameLabel.GetTopLeftCorner()[0]) / static_cast<float>(camera.GetWindowWidth());
			//const float originVTexCoord = quadHeight / static_cast<float>(camera.GetWindowHeight());
			//const float opaqueQuadUTexCoord = originUTexCoord + (currOpaqueQuadWidth / static_cast<float>(camera.GetWindowWidth())); 
			//const float opaqueQuadPlusWipeQuadUTexCoord = opaqueQuadUTexCoord + (LEVEL_NAME_WIPE_FADE_QUAD_SIZE / static_cast<float>(camera.GetWindowWidth())); 

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glTranslatef(this->levelNameLabel.GetTopLeftCorner()[0] - hPadding, quadHeight, 0.0f);

			// Draw the wipe-in quads - 2 quads, one is totally opaque and starts covering the entire
			// header, the other quad is a gradiant from white to transparent and it moves along the header
			// over time, revealing it as it goes (wipe fade in)
			
			//finalFBOTex->BindTexture();
			glBegin(GL_QUADS);

			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			if (currOpaqueQuadWidth > 0.0f) {
				//glTexCoord2f(originUTexCoord, originVTexCoord);     
				glVertex2f(0.0f, 0.0f);
				//glTexCoord2f(originUTexCoord, 0.0f);                
				glVertex2f(0.0f, -quadHeight);
				//glTexCoord2f(opaqueQuadUTexCoord, 0.0f);            
				glVertex2f(currOpaqueQuadWidth, -quadHeight);
				//glTexCoord2f(opaqueQuadUTexCoord, originVTexCoord); 
				glVertex2f(currOpaqueQuadWidth, 0.0f);
			}
			glEnd();

			glBegin(GL_QUADS);
			// Gradient quad used to do the wipe
			//glTexCoord2f(opaqueQuadUTexCoord, originVTexCoord); 
			glVertex2f(currOpaqueQuadWidth, 0.0f);
			//glTexCoord2f(opaqueQuadUTexCoord, 0.0f);            
			glVertex2f(currOpaqueQuadWidth, -quadHeight);
			glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
			//glTexCoord2f(opaqueQuadPlusWipeQuadUTexCoord, 0.0f);            
			glVertex2f(currOpaqueQuadWidth + LEVEL_NAME_WIPE_FADE_QUAD_SIZE, -quadHeight);
			//glTexCoord2f(opaqueQuadPlusWipeQuadUTexCoord, originVTexCoord); 
			glVertex2f(currOpaqueQuadWidth + LEVEL_NAME_WIPE_FADE_QUAD_SIZE, 0.0f);

			glEnd();

			glPopMatrix();
			Camera::PopWindowCoords();
			glPopAttrib();
		}
	}
	
	// All done animating the start of the level - go to the official in-game state
	if (fadeInDone && wipeInDone && levelTextFadeOutDone && dropShadowAnimDone && levelPieceFadeInDone) {
		this->display->SetCurrentStateAsNextQueuedState();
		return;
	}

	debug_opengl_state();
}

/**
 * Called when the display size changes - we need to recreate our framebuffer objects 
 * to account for the new screen size.
 */
void LevelStartDisplayState::DisplaySizeChanged(int width, int height) {
	this->display->GetAssets()->GetFBOAssets()->ResizeFBOAssets(width, height);
}
