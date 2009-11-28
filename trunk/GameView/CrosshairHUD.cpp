#include "CrosshairHUD.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture.h"

#include "../ResourceManager.h"

CrosshairHUD::CrosshairHUD() : crosshairTex(NULL) {
	this->crosshairTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CROSSHAIR, Texture::Nearest, GL_TEXTURE_2D);
}

CrosshairHUD::~CrosshairHUD() {
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->crosshairTex);
	assert(success);
}

/**
 * Draw the crosshair on the screen as a HUD element (ontop of everything,
 * in screen coordinates, directly in the middle).
 */
void CrosshairHUD::Draw(int screenWidth, int screenHeight, float alpha) const {
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

	glTranslatef(static_cast<float>(screenWidth) / 2.0f, static_cast<float>(screenHeight) / 2.0f, 0.0f);
	
	this->crosshairTex->BindTexture();
	glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, alpha);
		glTexCoord2i(0, 0); glVertex2i(-CrosshairHUD::CROSSHAIR_HALF_WIDTH, -CrosshairHUD::CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(1, 0); glVertex2i(CrosshairHUD::CROSSHAIR_HALF_WIDTH, -CrosshairHUD::CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(1, 1); glVertex2i(CrosshairHUD::CROSSHAIR_HALF_WIDTH, CrosshairHUD::CROSSHAIR_HALF_HEIGHT);
		glTexCoord2i(0, 1); glVertex2i(-CrosshairHUD::CROSSHAIR_HALF_WIDTH, CrosshairHUD::CROSSHAIR_HALF_HEIGHT);
	glEnd();
	this->crosshairTex->UnbindTexture();

	glPopMatrix();

	Camera::PopWindowCoords();
	glPopAttrib();
}