/**
 * OmniLaserBallEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "OmniLaserBallEffect.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/GameModelConstants.h"
#include "../ResourceManager.h"

OmniLaserBallEffect::OmniLaserBallEffect() : sentryTexture(NULL) {
    this->sentryTexture = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, 
        Texture::Trilinear, GL_TEXTURE_2D));
    assert(this->sentryTexture != NULL);
    
    //this->flareTexture = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_LENSFLARE, 
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
    assert(success);
    //success = ResourceManager::GetInstance()->ReleaseTextureResource(this->flareTexture);
    //assert(success);
}

// Draw three glowing sentries orbiting around the ball
void OmniLaserBallEffect::Draw(double dT, const Camera& camera, const GameBall& ball) {

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

    glColor4f(omniLaserColour.R(), omniLaserColour.G(), omniLaserColour.B(), 1);
    this->sentryTexture->BindTexture();
    this->DrawSentries(ball, sentrySize);

    glPopMatrix();
    glPopAttrib();

    this->rotAngleAnim.Tick(dT);
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