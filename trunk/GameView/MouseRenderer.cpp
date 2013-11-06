/**
 * MouseRenderer.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "MouseRenderer.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../BlammoEngine/Camera.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/BallBoostModel.h"
#include "../ResourceManager.h"

const double MouseRenderer::TIME_TO_SHOW_MOUSE_BEFORE_FADE = 2.0;
const double MouseRenderer::FADE_IN_TIME  = 0.1;
const double MouseRenderer::FADE_OUT_TIME = 0.5;

MouseRenderer::MouseRenderer() : currState(NotShowing), mouseX(0), mouseY(0), timeSinceLastShowMouse(0.0),
defaultMouseTex(NULL), boostMouseTex(NULL), windowHasFocus(false) {
    
    this->defaultMouseTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_DEFAULT_MOUSE, Texture::Trilinear));
    assert(this->defaultMouseTex != NULL);
    this->boostMouseTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BOOST_MOUSE, Texture::Trilinear));
    assert(this->boostMouseTex != NULL);

    this->SetState(NotShowing);
}

MouseRenderer::~MouseRenderer() {

    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->defaultMouseTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->boostMouseTex);
    assert(success);
    UNUSED_PARAMETER(success);
}

void MouseRenderer::Draw(double dT, const GameModel& gameModel) {
    switch (this->currState) {

        case NotShowing:
            // Don't draw or do anything!
            break;

        case FadingIn:
            this->Render(gameModel);
            if (this->mouseAlphaAnim.Tick(dT)) {
                this->SetState(Showing);
            }
            break;

        case Showing: {
            this->Render(gameModel);
            
            const BallBoostModel* boostModel = gameModel.GetBallBoostModel();
            bool isBallBoostActive = (boostModel != NULL && boostModel->GetBulletTimeState() != BallBoostModel::NotInBulletTime);
            
            if (this->timeSinceLastShowMouse >= MouseRenderer::TIME_TO_SHOW_MOUSE_BEFORE_FADE && !isBallBoostActive) {
                this->SetState(FadingOut);
            }
            else {
                this->timeSinceLastShowMouse += dT;
            }
            break;
        }

        case FadingOut:
            this->Render(gameModel);
            if (this->mouseAlphaAnim.Tick(dT)) {
                this->SetState(NotShowing);
            }
            break;

        default:
            assert(false);
            return;
    }
}

void MouseRenderer::SetState(MouseState newState) {
    switch (newState) {
        case NotShowing:
            this->mouseAlphaAnim.ClearLerp();
            this->mouseAlphaAnim.SetInterpolantValue(0.0f);
            break;

        case FadingIn: {
            // If we're already showing the mouse then we just ignore this
            if (this->currState == Showing || this->currState == FadingIn) {
                return;
            }
            
            // Do a cross fade if we were fading out...
            double fadeInTime = MouseRenderer::FADE_IN_TIME;
            if (this->currState == FadingOut) {
                fadeInTime = NumberFuncs::LerpOverFloat(0.0f, 1.0f, 0.0f, static_cast<float>(FADE_IN_TIME), this->mouseAlphaAnim.GetInterpolantValue());
            }

            this->mouseAlphaAnim.SetLerp(fadeInTime, 1.0f);
            this->mouseAlphaAnim.SetRepeat(false);
            break;
        }

        case Showing:
            this->mouseAlphaAnim.ClearLerp();
            this->mouseAlphaAnim.SetInterpolantValue(1.0f);
            break;

        case FadingOut: {
            // If we're already not showing the mouse then just ignore this
            if (this->currState == NotShowing || this->currState == FadingOut) {
                return;
            }

            // Do a cross fade if we were fading in...
            double fadeOutTime = MouseRenderer::FADE_OUT_TIME;
            if (this->currState == FadingIn) {
                fadeOutTime = NumberFuncs::LerpOverFloat(1.0f, 0.0f, 0.0f, static_cast<float>(FADE_OUT_TIME), this->mouseAlphaAnim.GetInterpolantValue());
            }

            this->mouseAlphaAnim.SetLerp(fadeOutTime, 0.0f);
            this->mouseAlphaAnim.SetRepeat(false);
            break;
        }

        default:
            assert(false);
            return;
    }

    this->currState = newState;
}

void MouseRenderer::Render(const GameModel& gameModel) {
    float alpha = this->mouseAlphaAnim.GetInterpolantValue();
    if (alpha <= 0.0f || !this->windowHasFocus) {
        return;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(this->mouseX, this->mouseY, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    Texture2D* mouseTexture = NULL;
    const BallBoostModel* boostModel = gameModel.GetBallBoostModel();
    if (boostModel != NULL && boostModel->GetBulletTimeState() != BallBoostModel::NotInBulletTime) {
        mouseTexture = this->boostMouseTex;
        // Rotate the cursor to be aligned with the center of the screen
        Vector2D centerToCursorVec = Point2D(this->mouseX, this->mouseY) - Point2D(Camera::GetWindowWidth()/2.0f, Camera::GetWindowHeight()/2.0f);
        float rotAngle = atan2f(centerToCursorVec[1], centerToCursorVec[0]) - M_PI_DIV2;
        glRotatef(Trig::radiansToDegrees(rotAngle), 0, 0, 1);
    }
    else {
        mouseTexture = this->defaultMouseTex;
    }

    assert(mouseTexture != NULL);

    mouseTexture->BindTexture();
    glScalef(mouseTexture->GetWidth(), mouseTexture->GetHeight(), 1);
    GeometryMaker::GetInstance()->DrawQuad();
    mouseTexture->UnbindTexture();

    glPopMatrix();
    Camera::PopWindowCoords();

    glPopAttrib();
}