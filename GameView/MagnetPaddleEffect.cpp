/**
 * MagnetPaddleEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "MagnetPaddleEffect.h"
#include "GameViewConstants.h"

#include "../GameModel/PlayerPaddle.h"

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"

#include "../ResourceManager.h"

const double MagnetPaddleEffect::MAX_MAGNET_LINE_TIME           = 2.0;
const int MagnetPaddleEffect::NUM_MAGNET_LINES                  = 7;
const double MagnetPaddleEffect::TIME_DIFF_BETWEEN_MAGNET_LINES = MAX_MAGNET_LINE_TIME / static_cast<double>(NUM_MAGNET_LINES);
const float MagnetPaddleEffect::CIRCLE_WIDTH_TO_HEIGHT_RATIO    = 1.6f;

const float MagnetPaddleEffect::MIN_BASE_CIRCLE_HEIGHT = PlayerPaddle::PADDLE_HEIGHT_TOTAL;
const float MagnetPaddleEffect::MIN_BASE_CIRCLE_WIDTH  = MIN_BASE_CIRCLE_HEIGHT * CIRCLE_WIDTH_TO_HEIGHT_RATIO;
const float MagnetPaddleEffect::MAX_BASE_CIRCLE_HEIGHT = 7.2f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;
const float MagnetPaddleEffect::MAX_BASE_CIRCLE_WIDTH  = MAX_BASE_CIRCLE_HEIGHT * CIRCLE_WIDTH_TO_HEIGHT_RATIO;

MagnetPaddleEffect::MagnetPaddleEffect() : currState(StartingUp),
magnetLineTimeTrackers(NUM_MAGNET_LINES, 0.0), magnetCircleTex(NULL) {

    this->magnetCircleTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear, GL_TEXTURE_2D));
    this->Reset();
}

MagnetPaddleEffect::~MagnetPaddleEffect() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->magnetCircleTex);
    assert(success);
    UNUSED_VARIABLE(success);
}

void MagnetPaddleEffect::Reset() {
    double time = 0.0;
    for (int i = static_cast<int>(this->magnetLineTimeTrackers.size())-1; i >= 0 ; i--) {
        this->magnetLineTimeTrackers[i] = time;
        time -= TIME_DIFF_BETWEEN_MAGNET_LINES;
    }

    this->currState = StartingUp;
}

void MagnetPaddleEffect::Draw(double dT, const PlayerPaddle& p) {
    float scaleFactor = p.GetPaddleScaleFactor();
    float alphaMultiplier = 1.0f;

    if ((p.GetPaddleType() & PlayerPaddle::InvisiPaddle) == PlayerPaddle::InvisiPaddle) {
        alphaMultiplier = 0.1f;
    }

    switch (currState) {

        case StartingUp: {
            for (int i = 0; i < static_cast<int>(this->magnetLineTimeTrackers.size()); i++) {
                double& currTime = this->magnetLineTimeTrackers[i];
                currTime += dT;
            }

            if (this->magnetLineTimeTrackers.back() >= MAX_MAGNET_LINE_TIME) {
                this->magnetLineTimeTrackers.back() = MAX_MAGNET_LINE_TIME;
                this->currState = Running;
            }
            break;
        }

        case Running: {
            for (int i = 0; i < static_cast<int>(this->magnetLineTimeTrackers.size()); i++) {
                double& currTime = this->magnetLineTimeTrackers[i];
                currTime += dT;
                if (currTime > MAX_MAGNET_LINE_TIME) {
                    currTime = 0.0;
                }
            }

            break;
        }

        default:
            assert(false);
            break;
    }
    
    this->DrawLines(scaleFactor, alphaMultiplier);
}


void MagnetPaddleEffect::DrawLines(float scaleFactor, float alphaMultiplier) {

    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    this->magnetCircleTex->BindTexture();

    for (int i = static_cast<int>(this->magnetLineTimeTrackers.size())-1; i >= 0 ; i--) {

        double currTime  = this->magnetLineTimeTrackers[i];
        if (currTime < 0.0) {
            continue;
        }

        float currWidth  = this->GetCircleWidthFromTime(currTime, scaleFactor);
        float currHeight = this->GetCircleHeightFromTime(currTime, scaleFactor);
        
        float halfCurrWidth  = currWidth  / 2.0f;

        glColor4f(1, 1, 1, 0.6f * alphaMultiplier * this->GetAlphaFromTime(currTime));

        glPushMatrix();
        glTranslatef(halfCurrWidth, 0.0f, 0.0f);
        glScalef(currWidth, currHeight, 1.0f);
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-halfCurrWidth, 0.0f, 0.0f);
        glScalef(currWidth, currHeight, 1.0f);
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();
    }
    
    this->magnetCircleTex->UnbindTexture();

    glPopAttrib();
}
