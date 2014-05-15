/**
 * MagnetPaddleEffect.cpp
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
    
    float zRotInDegs  = p.GetZRotation();
    float scaleFactor = p.GetPaddleScaleFactor();
    float alphaMultiplier = p.GetAlpha();
    
    if (p.HasPaddleType(PlayerPaddle::InvisiPaddle)) {
        alphaMultiplier *= 0.2f;
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
    
    this->DrawLines(zRotInDegs, scaleFactor, alphaMultiplier);
}


void MagnetPaddleEffect::DrawLines(float zRotInDegs, float scaleFactor, float alphaMultiplier) {

    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    this->magnetCircleTex->BindTexture();

    glPushMatrix();
    glRotatef(zRotInDegs, 0, 0, 1);

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
    
    glPopMatrix();

    this->magnetCircleTex->UnbindTexture();

    glPopAttrib();
}
