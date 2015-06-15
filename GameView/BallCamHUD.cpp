/**
 * BallCamHUD.cpp
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

#include "BallCamHUD.h"
#include "GameAssets.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/FBObj.h"
#include "../GameModel/CannonBlock.h"
#include "../GameView/GameDisplay.h"

const float BallCamHUD::ROTATE_HINT_BOTTOM_FROM_SCREEN_BOTTOM = 150.0f;

const double BallCamHUD::LEVEL_DISPLAY_FADE_IN_ANIMATE_TIME  = 1.0;
const double BallCamHUD::LEVEL_DISPLAY_FADE_OUT_ANIMATE_TIME = 0.5;

const float BallCamHUD::BALL_CAM_FG_KEY_LIGHT_Z_POS  = 0.0f;   // This should always be zero!!! (otherwise there will be a mismatch between the ball light and key light)
const float BallCamHUD::BALL_CAM_FG_FILL_LIGHT_Z_POS = -10.0f;

BallCamHUD::BallCamHUD(GameAssets& assets) :
boostMalfunctionHUD(NULL), cannonObstructionHUD(NULL), cannon(NULL),
cannonRotateHint(assets.GetTutorialAssets(), "Rotate Cannon"),
cannonFireHint(assets.GetTutorialAssets(), "Fire Cannon"),
cannonCountdown(CannonBlock::BALL_CAMERA_ROTATION_TIME_IN_SECS), 
cannonHUDActive(false), canShootCannon(false) {

    this->boostMalfunctionHUD  = new MalfunctionTextHUD(GameViewConstants::BOOST_MALFUNCTION_TEXT, assets.GetSound());
    this->cannonObstructionHUD = new MalfunctionTextHUD(GameViewConstants::CANNON_OBSTRUCTION_MALFUNCTION_TEXT, assets.GetSound());

    this->levelOverlayHUDWidth  = static_cast<int>(GameDisplay::GetTextScalingFactor()*360);
    this->levelOverlayHUDHeight = static_cast<int>(GameDisplay::GetTextScalingFactor()*270);

    // Initialize the hints...
    std::list<GameViewConstants::XBoxButtonType> xboxButtonTypes;
    std::list<GameViewConstants::KeyboardButtonType> keyboardButtonTypes;
    std::list<std::string> buttonTexts;
    std::list<Colour> buttonColours;

    // Cannon rotation hint
    keyboardButtonTypes.clear();
    keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
    keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
    buttonTexts.clear();
    buttonTexts.push_back("A");
    buttonTexts.push_back("D");

    if (GameDisplay::IsArcadeModeEnabled()) {
        this->cannonRotateHint.SetArcadeButton(GameViewConstants::ArcadeJoystick, "", Colour(1,1,1));
    }
    else {
        this->cannonRotateHint.SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Left Analog", Colour(1,1,1));
        this->cannonRotateHint.SetKeyboardButtons(keyboardButtonTypes, buttonTexts);
    }

    // Cannon fire hint
    xboxButtonTypes.clear();
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    buttonTexts.clear();
    buttonTexts.push_back("RT");
    buttonTexts.push_back("LT");
    buttonColours.clear();
    buttonColours.push_back(Colour(1,1,1));
    buttonColours.push_back(Colour(1,1,1));

    if (GameDisplay::IsArcadeModeEnabled()) {
        this->cannonFireHint.SetArcadeButton(GameViewConstants::ArcadeFireButton, "Fire", 
            GameViewConstants::GetInstance()->ARCADE_FIRE_BUTTON_COLOUR);
    }
    else {
        this->cannonFireHint.SetXBoxButtons(xboxButtonTypes, buttonTexts, buttonColours);
        this->cannonFireHint.SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");
    }

    this->barrelOverlayTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BARREL_OVERLAY, Texture::Trilinear);
    assert(this->barrelOverlayTex != NULL);

    this->cannonOverlayFadeAnim.ClearLerp();
    this->cannonOverlayFadeAnim.SetInterpolantValue(0.0f);
    this->cannonOverlayFadeAnim.SetRepeat(false);

    std::vector<double> timeValues;
    timeValues.reserve(3);
    timeValues.push_back(0.0);
    timeValues.push_back(0.75);
    timeValues.push_back(1.5);
    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(3);
    colourValues.push_back(ColourRGBA(1.0f, 0.8f, 0.0f, 0.0f));
    colourValues.push_back(ColourRGBA(1.0f, 0.8f, 0.0f, 1.0f));
    colourValues.push_back(ColourRGBA(1.0f, 0.8f, 0.0f, 0.0f));

    this->arrowColourAnim.SetLerp(timeValues, colourValues);
    this->arrowColourAnim.SetRepeat(true);

    this->levelDisplayFadeAnim.ClearLerp();
    this->levelDisplayFadeAnim.SetInterpolantValue(0.0f);
    this->levelDisplayFadeAnim.SetRepeat(false);

    this->SetCanShootCannon(true);
}

BallCamHUD::~BallCamHUD() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->barrelOverlayTex);
    UNUSED_VARIABLE(success);
    assert(success);

    delete this->boostMalfunctionHUD;
    this->boostMalfunctionHUD = NULL;
    delete this->cannonObstructionHUD;
    this->cannonObstructionHUD = NULL;
}

void BallCamHUD::SetCanShootCannon(bool canShoot) {
    
    static const double TIME_FLASH_INTERVAL_IN_SECS = 1.0;

    if (canShoot && !this->canShootCannon) {
 
        std::vector<double> timeVals(3);
        timeVals[0] = 0.0; timeVals[1] = TIME_FLASH_INTERVAL_IN_SECS; timeVals[2] = 2*TIME_FLASH_INTERVAL_IN_SECS;

        std::vector<Colour> colourVals;
        colourVals.reserve(timeVals.size());
        colourVals.push_back(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR);
        colourVals.push_back(Colour(0.5f, 0.5f, 0.5f) + GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR);
        colourVals.push_back(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR);

        this->canShootCannonColourAnim.SetLerp(timeVals, colourVals);
        this->canShootCannonColourAnim.SetRepeat(true);

        this->canShootCannon = true;
    }
    else if (!canShoot && this->canShootCannon) {
        std::vector<double> timeVals(3);
        timeVals[0] = 0.0; timeVals[1] = TIME_FLASH_INTERVAL_IN_SECS; timeVals[2] = 2*TIME_FLASH_INTERVAL_IN_SECS;

        std::vector<Colour> colourVals;
        colourVals.reserve(timeVals.size());
        colourVals.push_back(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR);
        colourVals.push_back(Colour(0.5f, 0.5f, 0.5f) + GameViewConstants::GetInstance()->ITEM_BAD_COLOUR);
        colourVals.push_back(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR);

        this->canShootCannonColourAnim.SetLerp(timeVals, colourVals);
        this->canShootCannonColourAnim.SetRepeat(true);

        this->canShootCannon = false;
    }

}

void BallCamHUD::ToggleCannonHUD(bool activate, const CannonBlock* cannon) {
    if (activate) {
        
        if (this->cannonHUDActive) {
            return;
        }

        assert(cannon != NULL);
        this->cannon = cannon;
        this->cannonRotateHint.Show(0.5, 2.0);
        this->cannonFireHint.Show(0.5, 2.0);
        this->cannonOverlayFadeAnim.SetLerp(0.5, 0.9f);
        this->cannonHUDActive = true;
        this->cannonCountdown.Reset();

        this->boostMalfunctionHUD->Deactivate();
        this->cannonObstructionHUD->Deactivate();

        //this->levelDisplayFadeAnim.SetLerp(LEVEL_DISPLAY_FADE_OUT_ANIMATE_TIME, 0.0f);
    }
    else {

        if (!this->cannonHUDActive) {
            return;
        }

        this->cannon = NULL;
        this->cannonRotateHint.Unshow(0.0, 0.5);
        this->cannonFireHint.Unshow(0.0, 0.5);
        this->cannonOverlayFadeAnim.SetLerp(0.25, 0.0f);
        this->cannonHUDActive = false;
        this->cannonObstructionHUD->Deactivate();

        //this->levelDisplayFadeAnim.SetLerp(LEVEL_DISPLAY_FADE_IN_ANIMATE_TIME, 1.0f);
    }
}

void BallCamHUD::Activate() {
    this->levelDisplayFadeAnim.SetLerp(LEVEL_DISPLAY_FADE_IN_ANIMATE_TIME, 1.0f);
}

void BallCamHUD::Deactivate() {
    this->ToggleCannonHUD(false, NULL);
    this->boostMalfunctionHUD->Deactivate();
    this->cannonObstructionHUD->Deactivate();

    // Make sure this happens last
    this->levelDisplayFadeAnim.SetLerp(LEVEL_DISPLAY_FADE_OUT_ANIMATE_TIME, 0.0f);
}

void BallCamHUD::Reinitialize() {
    this->cannonRotateHint.Unshow(0.0, 0.0, true);
    this->cannonFireHint.Unshow(0.0, 0.0, true);
    this->cannonHUDActive = false;
    this->cannonCountdown.Reset();
    this->cannonOverlayFadeAnim.ClearLerp();
    this->cannonOverlayFadeAnim.SetInterpolantValue(0.0f);
    this->boostMalfunctionHUD->Reset();
    this->cannonObstructionHUD->Reset();
    
    this->levelDisplayFadeAnim.ClearLerp();
    this->levelDisplayFadeAnim.SetInterpolantValue(0.0f);
}

void BallCamHUD::DrawCannonHUD(double dT, const Camera& camera) {
    if (this->cannonRotateHint.GetAlpha() <= 0.0f && (!this->cannonHUDActive || this->cannon == NULL)) {
        return;
    }

    this->cannonOverlayFadeAnim.Tick(dT);
    this->cannonRotateHint.Tick(dT);
    this->cannonFireHint.Tick(dT);

    this->DrawCannonBarrelOverlay(dT, this->cannonOverlayFadeAnim.GetInterpolantValue());

    // Place the hint in the correct location on-screen
    float yPos = this->cannonRotateHint.GetHeight() + ROTATE_HINT_BOTTOM_FROM_SCREEN_BOTTOM;
    this->cannonRotateHint.SetTopLeftCorner((camera.GetWindowWidth() - this->cannonRotateHint.GetWidth()) / 2.0f, yPos);
    this->cannonRotateHint.Draw(camera);

    yPos -= (this->cannonRotateHint.GetHeight() + 50);
    this->cannonFireHint.SetTopLeftCorner((camera.GetWindowWidth() - this->cannonFireHint.GetWidth()) / 2.0f, yPos);
    this->cannonFireHint.Draw(camera);

    if (!this->cannonHUDActive || this->cannon == NULL) {
        return;
    }

    this->cannonCountdown.DrawUsingTimeLeft(camera, dT, 
        this->cannon->GetTotalRotationTime() - this->cannon->GetElapsedRotationTime());
}

void BallCamHUD::DrawCannonBarrelOverlay(double dT, float alpha) {
    static const float depth = 0.9f;
    int width  = Camera::GetWindowWidth();
    int height = Camera::GetWindowHeight();

    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float heightBufferSize = static_cast<float>(height - static_cast<int>(this->barrelOverlayTex->GetHeight()));
    float widthBufferSize  = static_cast<float>(width  - static_cast<int>(this->barrelOverlayTex->GetWidth()));
    float overlayWidth  = this->barrelOverlayTex->GetWidth();
    float overlayHeight = this->barrelOverlayTex->GetHeight();

    if (widthBufferSize >= 0 && heightBufferSize >= 0) {
        // Best case scenario, we can fit the full resolution overlay into the window
    }
    else {
        // A window dimension is constricting the overlay, figure out if it's the height or width or both
        bool widthBufferTooSmall  = widthBufferSize < 0;
        bool heightBufferTooSmall = heightBufferSize < 0;

        if (widthBufferTooSmall && heightBufferTooSmall) {
            // Both are too small, take the largest of the two and that will be the dimension
            if (width > height) {
                widthBufferSize = static_cast<float>(width - height);
                overlayWidth  = height;
                heightBufferSize = 0;
                overlayHeight = height;
            }
            else {
                heightBufferSize = static_cast<float>(height - width);
                overlayHeight = width;
                widthBufferSize = 0;
                overlayWidth  = width;
            }
        }
        else if (widthBufferTooSmall) {
            // Width is too small but the height isn't, we need to restrict the size based on width
            heightBufferSize = static_cast<float>(height - width);
            overlayHeight = width;
            widthBufferSize = 0;
            overlayWidth  = width;
        }
        else {
            // Height is too small but the width isn't, restrict based on height
            assert(heightBufferTooSmall);
            widthBufferSize = static_cast<float>(width - height);
            overlayWidth  = height;
            heightBufferSize = 0;
            overlayHeight = height;
        }
    }

    widthBufferSize  /= 2.0f;
    heightBufferSize /= 2.0f;

    float afterOverlayX = widthBufferSize + overlayWidth;
    float afterOverlayY = heightBufferSize + overlayHeight;

    glColor4f(0, 0, 0, alpha);
    glBegin(GL_QUADS);

    // Draw the two width buffers as black (if there are any)
    if (widthBufferSize > 0) {
        glVertex3f(0, 0, depth);
        glVertex3f(widthBufferSize, 0, depth);
        glVertex3f(widthBufferSize, height, depth);
        glVertex3f(0, height, depth);

        glVertex3f(afterOverlayX, 0, depth);
        glVertex3f(width, 0, depth);
        glVertex3f(width, height, depth);
        glVertex3f(afterOverlayX, height, depth);
    }

    // Draw the two height buffers as black (if there are any)
    if (heightBufferSize > 0) {
        glVertex3f(0, 0, depth);
        glVertex3f(width, 0, depth);
        glVertex3f(width, heightBufferSize, depth);
        glVertex3f(0, heightBufferSize, depth);

        glVertex3f(0, afterOverlayY, depth);
        glVertex3f(width, afterOverlayY, depth);
        glVertex3f(width, height, depth);
        glVertex3f(0, height, depth);
    }

    glEnd();

    // Now draw the overlay right in the center
    this->canShootCannonColourAnim.Tick(dT);
    const Colour& overlayColour = this->canShootCannonColourAnim.GetInterpolantValue();
    glColor4f(overlayColour.R(), overlayColour.G(), overlayColour.B(), alpha);

    this->barrelOverlayTex->BindTexture();

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex3f(widthBufferSize, heightBufferSize, depth);
    glTexCoord2i(1, 0);
    glVertex3f(afterOverlayX, heightBufferSize, depth);
    glTexCoord2i(1, 1);
    glVertex3f(afterOverlayX, afterOverlayY, depth);
    glTexCoord2i(0, 1);
    glVertex3f(widthBufferSize, afterOverlayY, depth);
    glEnd();

    this->barrelOverlayTex->UnbindTexture();

    // Draw arrows to indicate that the player can rotate the cannon...
    const float HORIZ_BORDER   = 10.0f;
    const float TRI_SIZE_HORIZ = std::min<float>(50.0f, std::max<float>(Camera::GetWindowWidth()*0.05 - 2*HORIZ_BORDER, 10.0f));
    const float TRI_SIZE_VERT  = 2.5f * TRI_SIZE_HORIZ;
    this->arrowColourAnim.Tick(dT);
    const ColourRGBA& arrowColour = this->arrowColourAnim.GetInterpolantValue();
    float lowerYPos = (Camera::GetWindowHeight() - TRI_SIZE_VERT) / 2.0f;

    glBegin(GL_TRIANGLES);
    glColor4f(arrowColour.R(), arrowColour.G(), arrowColour.B(), arrowColour.A()*alpha);

    glVertex2f(HORIZ_BORDER + TRI_SIZE_HORIZ, lowerYPos + TRI_SIZE_VERT);
    glVertex2f(HORIZ_BORDER, lowerYPos + TRI_SIZE_VERT / 2.0f);
    glVertex2f(HORIZ_BORDER + TRI_SIZE_HORIZ, lowerYPos);

    glVertex2f(Camera::GetWindowWidth() - (HORIZ_BORDER + TRI_SIZE_HORIZ), lowerYPos);
    glVertex2f(Camera::GetWindowWidth() - HORIZ_BORDER, lowerYPos + TRI_SIZE_VERT / 2.0f);
    glVertex2f(Camera::GetWindowWidth() - (HORIZ_BORDER + TRI_SIZE_HORIZ), lowerYPos + TRI_SIZE_VERT);

    glEnd();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
}


void BallCamHUD::DrawLevelDisplayHUD(double dT, const GameModel& gameModel) {

    this->levelDisplayFadeAnim.Tick(dT);
    float currAlpha = this->levelDisplayFadeAnim.GetInterpolantValue();
    if (currAlpha <= 0.0f) {
        return;
    }

    const GameLevel* currLevel = gameModel.GetCurrentLevel();

    static const float PADDING_PERCENT = 0.05f;

    const float WINDOW_WIDTH  = this->levelOverlayHUDWidth;
    const float WINDOW_HEIGHT = this->levelOverlayHUDHeight;

    const int HUD_PADDING_IN_PIXELS = static_cast<int>(this->levelOverlayHUDWidth*0.1);

    float levelWidth  = currLevel->GetLevelUnitWidth();
    float levelHeight = currLevel->GetLevelUnitHeight();

    float scale = 1.0f;
    float windowWidthWithPadding = WINDOW_WIDTH - 2.0f * WINDOW_WIDTH * PADDING_PERCENT;
    float windowHeightWithPadding = WINDOW_HEIGHT - 2.0f * WINDOW_HEIGHT * PADDING_PERCENT;
    scale = std::min<float>(windowWidthWithPadding / levelWidth, windowHeightWithPadding / levelHeight);

    

/*
    if (levelWidth > levelHeight) {
        float windowHeightWithPadding = WINDOW_HEIGHT - 2.0f * WINDOW_HEIGHT * PADDING_PERCENT;
        scale = windowHeightWithPadding / levelHeight;
    }
    else {
        float windowWidthWithPadding = WINDOW_WIDTH - 2.0f * WINDOW_WIDTH * PADDING_PERCENT;
        scale = windowWidthWithPadding / levelWidth;
    }
*/
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(Camera::GetWindowWidth() - WINDOW_WIDTH - HUD_PADDING_IN_PIXELS, HUD_PADDING_IN_PIXELS, 0.0f);

    glPushMatrix();
    glScalef(WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f);
    glTranslatef(0.5f, 0.5f, 0.0f);
    glColor4f(0, 0, 0, 0.5f*currAlpha);
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glTranslatef(WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f, 0.0f);
    glScalef(scale, scale, 1.0f);
    glTranslatef(-levelWidth/2.0f, -levelHeight/2.0f, 0.0f);

    glColor4f(1, 1, 1, 1*currAlpha);
    glLineWidth(1.0f);

    const std::vector<std::vector<LevelPiece*> >& pieces = gameModel.GetCurrentLevel()->GetCurrentLevelLayout();
    for (size_t i = 0; i < pieces.size(); i++) {
        const std::vector<LevelPiece*>& setOfPieces = pieces[i];
        for (size_t j = 0; j < setOfPieces.size(); j++) {
            const LevelPiece* currPiece = setOfPieces[j];
            currPiece->DrawWireframe();
        }
    }

    const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
    if (paddle->HasPaddleType(PlayerPaddle::InvisiPaddle)) {
        glColor4f(0, 0, 0, 0.33f*currAlpha);
    }
    else {
        glColor4f(0, 1, 0, 1.0f*currAlpha);
    }
    paddle->GetBounds().DrawSimpleBounds();

    const GameBall* ball = GameBall::GetBallCameraBall();
    if (ball == NULL) {
        ball = gameModel.GetBallChosenForBallCamera();
    }
    assert(ball != NULL);
    if (ball->HasBallType(GameBall::InvisiBall)) {
        glColor4f(0, 0, 0, 0.33f*currAlpha);
    }
    else {
        glColor4f(1, 1, 0, 1.0f*currAlpha);
    }
    ball->GetBounds().DrawSimpleBounds();

    glPopMatrix();

    Camera::PopWindowCoords();
    glPopAttrib();
}
