/**
 * CrosshairLaserHUD.cpp
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

#include "CrosshairLaserHUD.h"
#include "GameViewConstants.h"

#include "../GameModel/PlayerPaddle.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture.h"

#include "../ResourceManager.h"

CrosshairLaserHUD::CrosshairLaserHUD() : bulletCrosshairTex(NULL), 
beamCrosshairTex(NULL), beamCenterGlowTex(NULL) {

	this->bulletCrosshairTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BULLET_CROSSHAIR, Texture::Nearest, GL_TEXTURE_2D);
    assert(this->bulletCrosshairTex != NULL);
	this->beamCrosshairTex   = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BEAM_CROSSHAIR, Texture::Nearest, GL_TEXTURE_2D);
    assert(this->beamCrosshairTex != NULL);
	this->beamCenterGlowTex  = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->beamCenterGlowTex != NULL);

	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0);
	times.push_back(0.3f);
	times.push_back(0.6f);

	std::vector<float> values;
	values.reserve(3);
	values.push_back(1.0f);
	values.push_back(0.9f);
	values.push_back(1.0f);

	this->beamRadiusPulse.SetLerp(times, values);
	this->beamRadiusPulse.SetRepeat(true);

	times.clear();
	times.reserve(5);
	times.push_back(0.0);
	times.push_back(1.0);
	times.push_back(2.0);
	times.push_back(3.0);
	times.push_back(3.25);

	std::vector<Vector3D> arrowFadeVals;
	arrowFadeVals.reserve(5);
	arrowFadeVals.push_back(Vector3D(1.0, 0.0, 0.0));
	arrowFadeVals.push_back(Vector3D(0.0, 1.0, 0.0));
	arrowFadeVals.push_back(Vector3D(0.0, 0.0, 1.0));
	arrowFadeVals.push_back(Vector3D(0.0, 0.0, 0.0));
	arrowFadeVals.push_back(Vector3D(1.0, 0.0, 0.0));

	this->arrowFadeLerp.SetLerp(times, arrowFadeVals);
	this->arrowFadeLerp.SetRepeat(true);
}

CrosshairLaserHUD::~CrosshairLaserHUD() {
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bulletCrosshairTex);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->beamCrosshairTex);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->beamCenterGlowTex);
}

/**
 * Draw the crosshair on the screen as a HUD element (on top of everything,
 * in screen coordinates, directly in the middle).
 */
void CrosshairLaserHUD::Draw(const PlayerPaddle* paddle, int screenWidth, int screenHeight, float alpha) const {
	assert(paddle->GetIsPaddleCameraOn());

	// Don't draw anything if there's significant paddle effect active
	bool beamIsActive   = paddle->HasPaddleType(PlayerPaddle::LaserBeamPaddle);
    bool bulletIsActive = paddle->HasPaddleType(PlayerPaddle::LaserBulletPaddle | PlayerPaddle::FlameBlasterPaddle | PlayerPaddle::IceBlasterPaddle);
    bool rocketIsActive = paddle->HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle);
    bool mineIsActive   = paddle->HasPaddleType(PlayerPaddle::MineLauncherPaddle);
	if (!beamIsActive && !bulletIsActive && !rocketIsActive && !mineIsActive) {
	    return;
	}

	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT); 	
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	// Draw directly onto the screen a HUD of crosshairs
	Camera::PushWindowCoords();
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Draw the center crosshairs...
	glTranslatef(static_cast<float>(screenWidth) / 2.0f, static_cast<float>(screenHeight) / 2.0f, 0.0f);

	if (bulletIsActive || mineIsActive) {
		this->DrawBulletCrosshair(alpha);
	}

	if (beamIsActive) {
		this->DrawBeamCrosshair(alpha);
		if (paddle->GetIsLaserBeamFiring()) {
			this->DrawBeamBlast(alpha);

			// Handle sticky paddle situation (3 beams)...
			if (paddle->HasPaddleType(PlayerPaddle::StickyPaddle)) {
				const float quarterScreenWidth = static_cast<float>(screenWidth) / 4.0f;
				glPushMatrix();
				glTranslatef(quarterScreenWidth, 0.0f, 0.0f);
				this->DrawBeamBlast(alpha);
				glTranslatef(-2.0f*quarterScreenWidth, 0.0f, 0.0f);
				this->DrawBeamBlast(alpha);
				glPopMatrix();
			}
		}
	}

	if (rocketIsActive) {
		this->DrawRocketHUD(alpha);
	}
	glPopMatrix();

	Camera::PopWindowCoords();
	glPopAttrib();
}

void CrosshairLaserHUD::Tick(double dT) {
	this->beamRadiusPulse.Tick(dT);
	this->arrowFadeLerp.Tick(dT);
}

/**
 * Draw the crosshair for the laser bullet HUD. This will be centered around the origin
 * so it will require a transform before this function call to move it anywhere.
 */
void CrosshairLaserHUD::DrawBulletCrosshair(float alpha) const {
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	this->bulletCrosshairTex->BindTexture();
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(-CrosshairLaserHUD::LASER_BULLET_CROSSHAIR_HALF_WIDTH, -CrosshairLaserHUD::LASER_BULLET_CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(1, 0); glVertex2i(CrosshairLaserHUD::LASER_BULLET_CROSSHAIR_HALF_WIDTH, -CrosshairLaserHUD::LASER_BULLET_CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(1, 1); glVertex2i(CrosshairLaserHUD::LASER_BULLET_CROSSHAIR_HALF_WIDTH, CrosshairLaserHUD::LASER_BULLET_CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(0, 1); glVertex2i(-CrosshairLaserHUD::LASER_BULLET_CROSSHAIR_HALF_WIDTH, CrosshairLaserHUD::LASER_BULLET_CROSSHAIR_HALF_HEIGHT);
	glEnd();
	this->bulletCrosshairTex->UnbindTexture();
}

/**
 * Draw the crosshair for the laser beam HUD. This will be centered around the origin
 * so it will require a transform before this function call to move it anywhere.
 */
void CrosshairLaserHUD::DrawBeamCrosshair(float alpha) const {
	// Draw crosshair
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	this->beamCrosshairTex->BindTexture();
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(-CrosshairLaserHUD::LASER_BEAM_CROSSHAIR_HALF_WIDTH, -CrosshairLaserHUD::LASER_BEAM_CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(1, 0); glVertex2i(CrosshairLaserHUD::LASER_BEAM_CROSSHAIR_HALF_WIDTH, -CrosshairLaserHUD::LASER_BEAM_CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(1, 1); glVertex2i(CrosshairLaserHUD::LASER_BEAM_CROSSHAIR_HALF_WIDTH, CrosshairLaserHUD::LASER_BEAM_CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(0, 1); glVertex2i(-CrosshairLaserHUD::LASER_BEAM_CROSSHAIR_HALF_WIDTH, CrosshairLaserHUD::LASER_BEAM_CROSSHAIR_HALF_HEIGHT);
	glEnd();
	this->beamCrosshairTex->UnbindTexture();
}

/**
 * Draw the HUD for when a rocket is sitting on top of the paddle.
 */
void CrosshairLaserHUD::DrawRocketHUD(float alpha) const {
	static const int ARROW_BASE_TO_APEX   = 32;
	static const int ARROW_BASE_SIZE      = 64;
	static const int ARROW_HALF_BASE_SIZE = ARROW_BASE_SIZE / 2;
	static const int HALF_WIDTH_TO_TRI    = 280;
	static const int ARROW_GAP						= ARROW_BASE_TO_APEX;
	static const int HALF_WIDTH_PLUS_HEIGHT = HALF_WIDTH_TO_TRI + ARROW_BASE_TO_APEX;

	glDisable(GL_TEXTURE_2D);

	static const Colour ARROW_COLOUR(0.8, 0.8, 0.8);
	const Vector3D& currAlphas = this->arrowFadeLerp.GetInterpolantValue();
	
	glBegin(GL_TRIANGLES);

	glColor4f(ARROW_COLOUR.R(), ARROW_COLOUR.G(), ARROW_COLOUR.B(), alpha * currAlphas[0]);
	glVertex2f(HALF_WIDTH_TO_TRI, 0);
	glVertex2f(HALF_WIDTH_PLUS_HEIGHT, -ARROW_HALF_BASE_SIZE);
	glVertex2f(HALF_WIDTH_PLUS_HEIGHT, ARROW_HALF_BASE_SIZE);

	glVertex2f(-HALF_WIDTH_TO_TRI, 0);
	glVertex2f(-HALF_WIDTH_PLUS_HEIGHT, ARROW_HALF_BASE_SIZE);
	glVertex2f(-HALF_WIDTH_PLUS_HEIGHT, -ARROW_HALF_BASE_SIZE);

	glVertex2f(0, HALF_WIDTH_TO_TRI);
	glVertex2f(ARROW_HALF_BASE_SIZE, HALF_WIDTH_PLUS_HEIGHT);
	glVertex2f(-ARROW_HALF_BASE_SIZE, HALF_WIDTH_PLUS_HEIGHT);

	glVertex2f(0, -HALF_WIDTH_TO_TRI);
	glVertex2f(-ARROW_HALF_BASE_SIZE, -HALF_WIDTH_PLUS_HEIGHT);
	glVertex2f(ARROW_HALF_BASE_SIZE, -HALF_WIDTH_PLUS_HEIGHT);

	glColor4f(ARROW_COLOUR.R(), ARROW_COLOUR.G(), ARROW_COLOUR.B(), alpha * currAlphas[1]);
	glVertex2f(HALF_WIDTH_TO_TRI-ARROW_GAP, 0);
	glVertex2f(HALF_WIDTH_PLUS_HEIGHT-ARROW_GAP, -ARROW_HALF_BASE_SIZE);
	glVertex2f(HALF_WIDTH_PLUS_HEIGHT-ARROW_GAP, ARROW_HALF_BASE_SIZE);

	glVertex2f(-HALF_WIDTH_TO_TRI+ARROW_GAP, 0);
	glVertex2f(-HALF_WIDTH_PLUS_HEIGHT+ARROW_GAP, ARROW_HALF_BASE_SIZE);
	glVertex2f(-HALF_WIDTH_PLUS_HEIGHT+ARROW_GAP, -ARROW_HALF_BASE_SIZE);

	glVertex2f(0, HALF_WIDTH_TO_TRI-ARROW_GAP);
	glVertex2f(ARROW_HALF_BASE_SIZE, HALF_WIDTH_PLUS_HEIGHT-ARROW_GAP);
	glVertex2f(-ARROW_HALF_BASE_SIZE, HALF_WIDTH_PLUS_HEIGHT-ARROW_GAP);

	glVertex2f(0, -HALF_WIDTH_TO_TRI+ARROW_GAP);
	glVertex2f(-ARROW_HALF_BASE_SIZE, -HALF_WIDTH_PLUS_HEIGHT+ARROW_GAP);
	glVertex2f(ARROW_HALF_BASE_SIZE, -HALF_WIDTH_PLUS_HEIGHT+ARROW_GAP);

	glColor4f(ARROW_COLOUR.R(), ARROW_COLOUR.G(), ARROW_COLOUR.B(), alpha * currAlphas[2]);
	glVertex2f(HALF_WIDTH_TO_TRI-ARROW_GAP-ARROW_GAP, 0);
	glVertex2f(HALF_WIDTH_PLUS_HEIGHT-ARROW_GAP-ARROW_GAP, -ARROW_HALF_BASE_SIZE);
	glVertex2f(HALF_WIDTH_PLUS_HEIGHT-ARROW_GAP-ARROW_GAP, ARROW_HALF_BASE_SIZE);

	glVertex2f(-HALF_WIDTH_TO_TRI+ARROW_GAP+ARROW_GAP, 0);
	glVertex2f(-HALF_WIDTH_PLUS_HEIGHT+ARROW_GAP+ARROW_GAP, ARROW_HALF_BASE_SIZE);
	glVertex2f(-HALF_WIDTH_PLUS_HEIGHT+ARROW_GAP+ARROW_GAP, -ARROW_HALF_BASE_SIZE);

	glVertex2f(0, HALF_WIDTH_TO_TRI-ARROW_GAP-ARROW_GAP);
	glVertex2f(ARROW_HALF_BASE_SIZE, HALF_WIDTH_PLUS_HEIGHT-ARROW_GAP-ARROW_GAP);
	glVertex2f(-ARROW_HALF_BASE_SIZE, HALF_WIDTH_PLUS_HEIGHT-ARROW_GAP-ARROW_GAP);

	glVertex2f(0, -HALF_WIDTH_TO_TRI+ARROW_GAP+ARROW_GAP);
	glVertex2f(-ARROW_HALF_BASE_SIZE, -HALF_WIDTH_PLUS_HEIGHT+ARROW_GAP+ARROW_GAP);
	glVertex2f(ARROW_HALF_BASE_SIZE, -HALF_WIDTH_PLUS_HEIGHT+ARROW_GAP+ARROW_GAP);

	glEnd();

	//glLineWidth(2.0f);
	//glColor4f(1.0f, 1.0f, 1.0f, alpha);
}

void CrosshairLaserHUD::DrawBeamBlast(float alpha) const {
	// Draw the beam center glowy thingy
	glColor4f(0.75f, 1.0f, 1.0f, 0.4f * alpha);
	this->beamCenterGlowTex->BindTexture();

	glPushMatrix();
	glScalef(this->beamRadiusPulse.GetInterpolantValue(), this->beamRadiusPulse.GetInterpolantValue(), 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(-CrosshairLaserHUD::LASER_BEAM_GLOW_HALF_WIDTH, -CrosshairLaserHUD::LASER_BEAM_GLOW_HALF_HEIGHT);
		glTexCoord2i(1, 0); glVertex2i(CrosshairLaserHUD::LASER_BEAM_GLOW_HALF_WIDTH, -CrosshairLaserHUD::LASER_BEAM_GLOW_HALF_HEIGHT);
		glTexCoord2i(1, 1); glVertex2i(CrosshairLaserHUD::LASER_BEAM_GLOW_HALF_WIDTH, CrosshairLaserHUD::LASER_BEAM_GLOW_HALF_HEIGHT);
		glTexCoord2i(0, 1); glVertex2i(-CrosshairLaserHUD::LASER_BEAM_GLOW_HALF_WIDTH, CrosshairLaserHUD::LASER_BEAM_GLOW_HALF_HEIGHT);
	glEnd();
	glPopMatrix();
	this->beamCenterGlowTex->UnbindTexture();
}