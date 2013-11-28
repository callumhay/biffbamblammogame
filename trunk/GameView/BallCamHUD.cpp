/**
 * BallCamHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BallCamHUD.h"
#include "GameAssets.h"

#include "../GameModel/CannonBlock.h"

const float BallCamHUD::ROTATE_HINT_BOTTOM_FROM_SCREEN_BOTTOM = 150.0f;

BallCamHUD::BallCamHUD(GameAssets& assets, BoostMalfunctionHUD* boostMalfunctionHUD) :
boostMalfunctionHUD(boostMalfunctionHUD), 
cannonRotateHint(assets.GetTutorialAssets(), "Rotate Cannon"),
cannonFireHint(assets.GetTutorialAssets(), "Fire Cannon"),
cannonCountdown(CannonBlock::BALL_CAMERA_ROTATION_TIME_IN_SECS), cannonHUDActive(false), cannon(NULL) {

    assert(boostMalfunctionHUD != NULL);

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

    this->cannonRotateHint.SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Left Analog", Colour(1,1,1));
    this->cannonRotateHint.SetKeyboardButtons(keyboardButtonTypes, buttonTexts);

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

    this->cannonFireHint.SetXBoxButtons(xboxButtonTypes, buttonTexts, buttonColours);
    this->cannonFireHint.SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");

    this->barrelOverlayTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BARREL_OVERLAY, Texture::Trilinear);
    assert(this->barrelOverlayTex != NULL);

    this->overlayFadeAnim.ClearLerp();
    this->overlayFadeAnim.SetInterpolantValue(0.0f);
    this->overlayFadeAnim.SetRepeat(false);

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
}

BallCamHUD::~BallCamHUD() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->barrelOverlayTex);
    UNUSED_VARIABLE(success);
    assert(success);
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
        this->overlayFadeAnim.SetLerp(0.5, 0.9f);
        this->cannonHUDActive = true;
        this->cannonCountdown.Reset();

        this->boostMalfunctionHUD->Deactivate();
    }
    else {

        if (!this->cannonHUDActive) {
            return;
        }

        this->cannon = NULL;
        this->cannonRotateHint.Unshow(0.0, 0.5);
        this->cannonFireHint.Unshow(0.0, 0.5);
        this->overlayFadeAnim.SetLerp(0.25, 0.0f);
        this->cannonHUDActive = false;
    }
}

void BallCamHUD::Reinitialize() {
    this->cannonRotateHint.Unshow(0.0, 0.0, true);
    this->cannonFireHint.Unshow(0.0, 0.0, true);
    this->cannonHUDActive = false;
    this->cannonCountdown.Reset();
    this->overlayFadeAnim.ClearLerp();
    this->overlayFadeAnim.SetInterpolantValue(0.0f);
    this->boostMalfunctionHUD->Reset();
}

void BallCamHUD::DrawCannonHUD(double dT, const Camera& camera) {
    if (this->cannonRotateHint.GetAlpha() <= 0.0f && (!this->cannonHUDActive || this->cannon == NULL)) {
        return;
    }

    this->overlayFadeAnim.Tick(dT);
    this->cannonRotateHint.Tick(dT);
    this->cannonFireHint.Tick(dT);

    this->DrawCannonBarrelOverlay(dT, this->overlayFadeAnim.GetInterpolantValue());

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

    // Now draw the overlay right in the center
    glColor4f(1, 1, 1, alpha);
    glEnd();

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

    debug_opengl_state();
}
