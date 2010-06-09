#include "CrosshairLaserHUD.h"
#include "GameViewConstants.h"

#include "../GameModel/PlayerPaddle.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture.h"

#include "../ResourceManager.h"

CrosshairLaserHUD::CrosshairLaserHUD() : bulletCrosshairTex(NULL), beamCrosshairTex(NULL), beamCenterGlowTex(NULL) {
	this->bulletCrosshairTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BULLET_CROSSHAIR, Texture::Nearest, GL_TEXTURE_2D);
	this->beamCrosshairTex   = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BEAM_CROSSHAIR, Texture::Nearest, GL_TEXTURE_2D);
	this->beamCenterGlowTex  = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear, GL_TEXTURE_2D);

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

}

CrosshairLaserHUD::~CrosshairLaserHUD() {
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bulletCrosshairTex);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->beamCrosshairTex);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->beamCenterGlowTex);
}

/**
 * Draw the crosshair on the screen as a HUD element (ontop of everything,
 * in screen coordinates, directly in the middle).
 */
void CrosshairLaserHUD::Draw(const PlayerPaddle* paddle, int screenWidth, int screenHeight, float alpha) const {
	assert(paddle->GetIsPaddleCameraOn());

	// Don't draw anything if there's significant paddle effect active
	bool beamIsActive   = (paddle->GetPaddleType() & PlayerPaddle::LaserBeamPaddle) == PlayerPaddle::LaserBeamPaddle;
	bool bulletIsActive = (paddle->GetPaddleType() & PlayerPaddle::LaserBulletPaddle) == PlayerPaddle::LaserBulletPaddle;
	//bool rocketIsActive = (paddle->GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle;

	if (!beamIsActive && !bulletIsActive /*&& !rocketIsActive*/) {
			return;
	}

	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 	
	glDisable(GL_LIGHTING);
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

	if (bulletIsActive) {
		this->DrawBulletCrosshair(alpha);
	}

	if (beamIsActive) {
		this->DrawBeamCrosshair(alpha);
		if (paddle->GetIsLaserBeamFiring()) {
			this->DrawBeamBlast(alpha);

			// TODO: handle sticky paddle situation (3 beams)...
			if ((paddle->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
				const float quarterScreenWidth = static_cast<float>(screenWidth) / 4.0f;
				glPushMatrix();
				glTranslatef(quarterScreenWidth, 0.0f, 0.0f);
				this->DrawBeamBlast(alpha);
				glPopMatrix();

				glPushMatrix();
				glTranslatef(-quarterScreenWidth, 0.0f, 0.0f);
				this->DrawBeamBlast(alpha);
				glPopMatrix();
			}
		}
	}
	//if (rocketIsActive) {
	//}


	glPopMatrix();

	Camera::PopWindowCoords();
	glPopAttrib();
}

void CrosshairLaserHUD::Tick(double dT) {
	this->beamRadiusPulse.Tick(dT);
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

void CrosshairLaserHUD::DrawBeamBlast(float alpha) const {
	// Draw the beam
	Colour beamColour = GameViewConstants::GetInstance()->LASER_BEAM_COLOUR;
	glColor4f(beamColour.R(), beamColour.G(), beamColour.B(), 0.4f * alpha);
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