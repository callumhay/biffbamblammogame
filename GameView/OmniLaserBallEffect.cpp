/**
 * OmniLaserBallEffect.cpp
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

#include "OmniLaserBallEffect.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/GameModelConstants.h"
#include "../ResourceManager.h"

OmniLaserBallEffect::OmniLaserBallEffect() : sentryTexture(NULL) {
    this->sentryTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, 
        Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->sentryTexture != NULL);
    
    //this->flareTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_LENSFLARE, 
    //    Texture::Trilinear, GL_TEXTURE_2D));
    //assert(this->flareTexture != NULL);

    std::vector<double> timeValues;
    timeValues.push_back(0.0);
    timeValues.push_back(0.25);
    timeValues.push_back(0.5);
    std::vector<float> rotationValues;
    rotationValues.push_back(0.0f);
    rotationValues.push_back(180.0f);
    rotationValues.push_back(360.0f);

    this->rotAngleAnim.SetLerp(timeValues, rotationValues);
    this->rotAngleAnim.SetRepeat(true);
    this->rotAngleAnim.SetInterpolantValue(0.0f);
}

OmniLaserBallEffect::~OmniLaserBallEffect() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sentryTexture);
    UNUSED_VARIABLE(success);
    assert(success);
    //success = ResourceManager::GetInstance()->ReleaseTextureResource(this->flareTexture);
    //assert(success);
}

// Draw three glowing sentries orbiting around the ball
void OmniLaserBallEffect::Draw(double dT, bool doTick, const Camera& camera, const GameBall& ball) {

    float sentrySize = 1.25f * ball.GetBounds().Radius();
    const Point2D& ballPosition = ball.GetCenterPosition2D();
    Matrix4x4 screenAlignMatrix = camera.GenerateScreenAlignMatrix();
    const Colour& omniLaserColour = GameModelConstants::GetInstance()->OMNI_LASER_BALL_COLOUR;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	
    glEnable(GL_BLEND);    
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(ballPosition[0], ballPosition[1], 0.0f);

    glMultMatrixf(screenAlignMatrix.begin());
    glRotatef(this->rotAngleAnim.GetInterpolantValue(), 0, 0, 1);

    glColor4f(omniLaserColour.R(), omniLaserColour.G(), omniLaserColour.B(), ball.GetAlpha());
    this->sentryTexture->BindTexture();
    this->DrawSentries(ball, sentrySize);

    glPopMatrix();
    glPopAttrib();

    if (doTick) {
        this->rotAngleAnim.Tick(dT);
    }
}

void OmniLaserBallEffect::DrawSentries(const GameBall& ball, float size) {
    glPushMatrix();
    glTranslatef(0, ball.GetBounds().Radius() + (0.55f * size), 0);
    glScalef(size, size, 1);
	GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPushMatrix();
    glRotatef(120, 0, 0, 1);
    glTranslatef(0, ball.GetBounds().Radius() + (0.55f * size), 0);
    glScalef(size, size, 1);
	GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPushMatrix();
    glRotatef(240, 0, 0, 1);
    glTranslatef(0, ball.GetBounds().Radius() + (0.55f * size), 0);
    glScalef(size, size, 1);
	GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();
}