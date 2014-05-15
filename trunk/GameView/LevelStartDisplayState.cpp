/**
 * LevelStartDisplayState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "LevelStartDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameESPAssets.h"
#include "GameFBOAssets.h"
#include "GameFontAssetsManager.h"
#include "LevelMesh.h"
#include "LivesLeftHUD.h"
#include "PointsHUD.h"
#include "BallBoostHUD.h"
#include "BallCamHUD.h"
#include "PaddleCamHUD.h"
#include "MenuBackgroundRenderer.h"

#include "../ESPEngine/ESPPointEmitter.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/GameTransformMgr.h"
#include "../WindowManager.h"

const double LevelStartDisplayState::FADE_IN_TIME               = 1.25;
const double LevelStartDisplayState::WIPE_TIME                  = 0.8f;
const double LevelStartDisplayState::LEVEL_TEXT_FADE_OUT_TIME   = 2.5;
const double LevelStartDisplayState::LEVEL_BLOCK_FADE_IN_TIME   = 1.25;

const float LevelStartDisplayState::LEVEL_NAME_WIPE_FADE_QUAD_SIZE	= 100.0f;
const float LevelStartDisplayState::LEVEL_TEXT_X_PADDING            = 50;			// Padding from the right-hand side of the screen to the level name text
const float LevelStartDisplayState::LEVEL_TEXT_Y_PADDING            = 80;			// Padding from the bottom of the screen to the bottom of the level name text

LevelStartDisplayState::LevelStartDisplayState(GameDisplay* display) : 
DisplayState(display), renderPipeline(display), shockwaveEmitter(NULL),
levelNameLabel(GameFontAssetsManager::GetInstance()->GetFont(
               GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big), ""),
paddleMoveUpSoundID(INVALID_SOUND_ID), ballSpawnSoundID(INVALID_SOUND_ID) {

    GameModel* gameModel = this->display->GetModel();
    
    // Reset the GameModel's transform manager...
    gameModel->GetTransformInfo()->Reset();

    // Make sure the ball is in the center of the level, on top of the paddle
	GameBall* ball = this->display->GetModel()->GetGameBalls().front();
	assert(ball != NULL);
    const PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();
    assert(paddle != NULL);

    Vector2D disp = Vector2D(0.0f, ball->GetBounds().Radius() + paddle->GetHalfHeight() + 0.1f);
	Point2D ballLoc = paddle->GetCenterPosition() + disp;
    ball->SetCenterPosition(ballLoc);

    // Make sure the game is in the "BallOnPaddleState" before going through with the 
    // initialization for the visuals
    this->display->GetModel()->UnsetPause(GameModel::PauseGame);
    this->display->GetModel()->UnsetPause(GameModel::PauseState);
    while (gameModel->GetCurrentStateType() != GameState::BallOnPaddleStateType) {
        this->display->UpdateModel(EPSILON);
    }

	// Pause all game play elements in the game model
	gameModel->SetPauseState(GameModel::PausePaddle | GameModel::PauseBall | GameModel::PauseState);

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

	double startOfNameFadeOut = LevelStartDisplayState::WIPE_TIME + 0.3;
	double endOfNameFadeOut   = startOfNameFadeOut + LevelStartDisplayState::LEVEL_TEXT_FADE_OUT_TIME;
	this->levelNameFadeOutAnimation.SetLerp(startOfNameFadeOut, endOfNameFadeOut, 1.0f, 0.0f);
	this->levelNameFadeOutAnimation.SetRepeat(false);
	this->levelNameFadeOutAnimation.SetInterpolantValue(1.0f);

	//this->dropShadowAnimation.SetLerp(startOfNameFadeOut, endOfNameFadeOut, startDropShadowAmt, 0.5f);
	//this->dropShadowAnimation.SetRepeat(false);
	//this->dropShadowAnimation.SetInterpolantValue(startDropShadowAmt);

	double startTimeOfBlockFadeIn = LevelStartDisplayState::WIPE_TIME + 0.5 + LevelStartDisplayState::FADE_IN_TIME;
	double endOfBlockFadeIn       = startTimeOfBlockFadeIn + LevelStartDisplayState::LEVEL_BLOCK_FADE_IN_TIME;
	this->blockFadeInAnimation.SetLerp(startTimeOfBlockFadeIn, endOfBlockFadeIn, 0.0f, 1.0f);
	this->blockFadeInAnimation.SetRepeat(false);
	this->blockFadeInAnimation.SetInterpolantValue(0.0f);

	float paddleAnimMoveDist   = 0.33f * std::max<float>(level->GetLevelUnitWidth(), level->GetLevelUnitHeight());
	float paddleStartPos       = paddle->GetCenterPosition()[1] - paddleAnimMoveDist;
	double endOfPaddleMove     = endOfBlockFadeIn + 0.15;

	this->paddleMoveUpAnimation.SetLerp(startTimeOfBlockFadeIn, endOfPaddleMove, paddleStartPos, paddle->GetCenterPosition()[1]);
	this->paddleMoveUpAnimation.SetRepeat(false);
	this->paddleMoveUpAnimation.SetInterpolantValue(paddleStartPos);

	this->ballFadeInAnimation.SetLerp(endOfPaddleMove, endOfPaddleMove + 0.25, 0.0f, 1.0f);
	this->ballFadeInAnimation.SetRepeat(false);
	this->ballFadeInAnimation.SetInterpolantValue(0.0f);

	Vector2D negHalfLevelDim = -0.5 * this->display->GetModel()->GetLevelUnitDimensions();
	Point3D emitCenter(ball->GetCenterPosition2D() + negHalfLevelDim, 0.0f);

	this->shockwaveEmitter = this->display->GetAssets()->GetESPAssets()->CreateShockwaveEffect(emitCenter, 4 * ball->GetBounds().Radius(), 1.3f);
	this->starEmitter = this->display->GetAssets()->GetESPAssets()->CreateBlockBreakSmashyBits(emitCenter, ESPInterval(0.8f, 1.0f), 
		ESPInterval(0.5f, 1.0f), ESPInterval(0.0f), false, 20);

	// Reset any HUD animations - this makes sure the animations are always displayed (e.g., the life balls fly in)
	// at the beginning of each level
    GameAssets* assets = this->display->GetAssets();
	assets->GetLifeHUD()->Reinitialize();
	assets->GetLifeHUD()->LivesGained(this->display->GetModel()->GetLivesLeft());
    assets->GetBoostHUD()->Reinitialize();
    assets->GetPointsHUD()->Reinitialize();
    assets->GetBallCamHUD()->Reinitialize();
    assets->GetPaddleCamHUD()->Reinitialize();
    assets->ReinitializeSkipLabel();

    // Special case: If the level is a boss level then all the lights start off and no music plays...
    if (level->GetHasBoss()) {
        assets->ToggleLights(false, 0.01);
    }
    else {
        GameSound* sound = this->display->GetSound();
        assert(sound != NULL);
        sound->PlaySound(GameSound::WorldBackgroundLoop, true, true, 1.0f);
    }

    WindowManager::GetInstance()->ShowCursor(false);
}

LevelStartDisplayState::~LevelStartDisplayState() {
	delete this->shockwaveEmitter;
	this->shockwaveEmitter = NULL;
	delete this->starEmitter;
	this->starEmitter = NULL;
}

void LevelStartDisplayState::RenderFrame(double dT) {
	const Camera& camera   = this->display->GetCamera();
    GameAssets* gameAssets = this->display->GetAssets();
    MenuBackgroundRenderer* bgRenderer = this->display->GetMenuBGRenderer();

	bool fadeInDone           = this->fadeInAnimation.Tick(dT);
	bool wipeInDone           = this->showLevelNameWipeAnimation.Tick(dT);
	bool levelTextFadeOutDone = this->levelNameFadeOutAnimation.Tick(dT);
	bool levelPieceFadeInDone = this->blockFadeInAnimation.Tick(dT);
	bool paddleMoveDone       = this->paddleMoveUpAnimation.Tick(dT);
	bool ballFadeInDone       = this->ballFadeInAnimation.Tick(dT);

	// Fade in the level pieces...
	gameAssets->GetCurrentLevelMesh()->SetLevelAlpha(this->blockFadeInAnimation.GetInterpolantValue());
	// Fade in Tesla lightning...
	gameAssets->GetESPAssets()->SetTeslaLightiningAlpha(this->blockFadeInAnimation.GetInterpolantValue());

	PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();
	GameBall* ball       = this->display->GetModel()->GetGameBalls().front();
	assert(paddle != NULL);
	assert(ball != NULL);

	// Animate the ball and paddle...
	const Point2D& paddleCenter = paddle->GetCenterPosition();
	paddle->SetCenterPosition(Point2D(paddleCenter[0], this->paddleMoveUpAnimation.GetInterpolantValue()));

	float ballAlpha = this->ballFadeInAnimation.GetInterpolantValue();
	ball->SetAlpha(ballAlpha);

	renderPipeline.RenderFrameWithoutHUD(dT);
    // Render the HUD with alpha...
    renderPipeline.RenderHUDWithAlpha(0.0, this->blockFadeInAnimation.GetInterpolantValue());
    
	// If the ball is appearing, draw the emitters around the ball for a cool, flashy entrance
	if (ballAlpha > 0.0f) {
		this->shockwaveEmitter->Tick(dT);
		this->starEmitter->Tick(dT);
		this->shockwaveEmitter->Draw(camera);
		this->starEmitter->Draw(camera);
	}

    this->display->GetMouse()->Draw(dT, *this->display->GetModel());

	// Render the fade-in if we haven't already
	if (!fadeInDone) {
		// Draw the background...
        bgRenderer->DrawNonAnimatedFadeBG(this->fadeInAnimation.GetInterpolantValue());
	}

	if (!levelTextFadeOutDone) {

		// If we're done the fade in then we need to do the level text display...
		this->levelNameLabel.SetAlpha(this->levelNameFadeOutAnimation.GetInterpolantValue());
		this->levelNameLabel.Draw();

		if (!wipeInDone) {
            glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_POLYGON_BIT);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            Camera::PushWindowCoords();

            Texture* bgTexture = bgRenderer->GetMenuBGTexture();
            bgTexture->BindTexture();
            this->DrawWipeIn();
            bgTexture->UnbindTexture();

            Camera::PopWindowCoords();

            glPopAttrib();
		}
	}
	
	// All done animating the start of the level - go to the official in-game state
	if (fadeInDone && wipeInDone && levelTextFadeOutDone && levelPieceFadeInDone) {
        
        GameSound* sound = this->display->GetSound();
        assert(sound != NULL);

        if (this->paddleMoveUpSoundID == INVALID_SOUND_ID) {
            this->paddleMoveUpSoundID = sound->PlaySound(GameSound::LevelStartPaddleMoveUpEvent, false, false);
        }
        if (paddleMoveDone) {
            if (this->ballSpawnSoundID == INVALID_SOUND_ID) {
                this->ballSpawnSoundID = sound->PlaySound(GameSound::LevelStartBallSpawnOnPaddleEvent, false, false);
            }

            if (ballFadeInDone && this->shockwaveEmitter->IsDead() && this->starEmitter->IsDead()) {
                
                // Make sure the ball is visible!!
                ball->SetAlpha(1.0f);

                this->display->SetCurrentStateAsNextQueuedState();
                return;
            }
        }
	}

	debug_opengl_state();
}

void LevelStartDisplayState::DrawWipeIn() {

    MenuBackgroundRenderer* bgRenderer = this->display->GetMenuBGRenderer();

    const float hPadding              = 20;
    const float quadHeight            = this->levelNameLabel.GetHeight() + 2 * LevelStartDisplayState::LEVEL_TEXT_Y_PADDING;
    const float currOpaqueQuadWidth	  = hPadding + this->levelNameLabel.GetLastRasterWidth() + 
        LevelStartDisplayState::LEVEL_TEXT_X_PADDING + 
        LevelStartDisplayState::LEVEL_NAME_WIPE_FADE_QUAD_SIZE - 
        this->showLevelNameWipeAnimation.GetInterpolantValue();
    
    Texture* bgTexture = bgRenderer->GetMenuBGTexture();

    float screenWidth = static_cast<float>(Camera::GetWindowWidth());
    float screenHeight = static_cast<float>(Camera::GetWindowHeight());

    float totalTexU = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenWidth / static_cast<float>(bgTexture->GetWidth()));
    float totalTexV = (GameViewConstants::STARRY_BG_TILE_MULTIPLIER * screenHeight / static_cast<float>(bgTexture->GetHeight()));

    float startTexU = (this->levelNameLabel.GetTopLeftCorner()[0] - hPadding) * totalTexU / screenWidth;
    float startTexV = quadHeight * totalTexV / screenHeight;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(this->levelNameLabel.GetTopLeftCorner()[0] - hPadding, quadHeight, 0.0f);

    glBegin(GL_QUADS);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    if (currOpaqueQuadWidth > 0.0f) {
        glTexCoord2f(startTexU, startTexV);     
        glVertex2f(0.0f, 0.0f);
        glTexCoord2f(startTexU, 0.0f);                
        glVertex2f(0.0f, -quadHeight);
        glTexCoord2f(startTexU + currOpaqueQuadWidth * totalTexU / screenWidth, 0.0f);            
        glVertex2f(currOpaqueQuadWidth, -quadHeight);
        glTexCoord2f(startTexU + currOpaqueQuadWidth * totalTexU / screenWidth, startTexV);
        glVertex2f(currOpaqueQuadWidth, 0.0f);
    }
    glEnd();

    glBegin(GL_QUADS);

    // Gradient quad used to do the wipe
    glTexCoord2f(startTexU + currOpaqueQuadWidth * totalTexU / screenWidth, startTexV);
    glVertex2f(currOpaqueQuadWidth, 0.0f);
    glTexCoord2f(startTexU + currOpaqueQuadWidth * totalTexU / screenWidth, 0.0f);           
    glVertex2f(currOpaqueQuadWidth, -quadHeight);
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    glTexCoord2f(startTexU + (currOpaqueQuadWidth + LEVEL_NAME_WIPE_FADE_QUAD_SIZE) * totalTexU / screenWidth, 0.0f);           
    glVertex2f(currOpaqueQuadWidth + LEVEL_NAME_WIPE_FADE_QUAD_SIZE, -quadHeight);
    glTexCoord2f(startTexU + (currOpaqueQuadWidth + LEVEL_NAME_WIPE_FADE_QUAD_SIZE) * totalTexU / screenWidth, startTexV);
    glVertex2f(currOpaqueQuadWidth + LEVEL_NAME_WIPE_FADE_QUAD_SIZE, 0.0f);

    glEnd();

    glPopMatrix();
}

/**
 * Called when the display size changes - we need to recreate our framebuffer objects 
 * to account for the new screen size.
 */
void LevelStartDisplayState::DisplaySizeChanged(int width, int height) {
	this->display->GetAssets()->GetFBOAssets()->ResizeFBOAssets(width, height);
}
