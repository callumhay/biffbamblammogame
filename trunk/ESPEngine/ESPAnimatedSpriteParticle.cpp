/**
 * ESPAnimatedSpriteParticle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ESPAnimatedSpriteParticle.h"

#include "../BlammoEngine/Texture2D.h"

ESPAnimatedSpriteParticle::ESPAnimatedSpriteParticle(const Texture2D* spriteTex, int perSpriteXSize, int perSpriteYSize, double fps) :
ESPParticle(), spriteTex(spriteTex), spriteXSize(perSpriteXSize), spriteYSize(perSpriteYSize),
timePerFrame(1.0 / fps), currFrameTimeCounter(0.0), numSprites(0), currSpriteIdx(0), numSpriteRows(0), numSpriteCols(0) {
    
    assert(perSpriteXSize > 0 && perSpriteXSize <= static_cast<int>(spriteTex->GetWidth()));
    assert(perSpriteYSize > 0 && perSpriteYSize <= static_cast<int>(spriteTex->GetHeight()));

    this->numSpriteRows = static_cast<int>(spriteTex->GetHeight() / perSpriteYSize);
    this->numSpriteCols = static_cast<int>(spriteTex->GetWidth() / perSpriteXSize);
    this->numSprites    = numSpriteRows * numSpriteCols;
}

ESPAnimatedSpriteParticle::~ESPAnimatedSpriteParticle() {
}

void ESPAnimatedSpriteParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
    ESPParticle::Revive(pos, vel, size, rot, totalLifespan);

    // Reset the animation
    this->currSpriteIdx = 0;
    this->currFrameTimeCounter = 0.0;
}

void ESPAnimatedSpriteParticle::Tick(const double dT) {
	ESPParticle::Tick(dT);

    if (this->currSpriteIdx >= this->numSprites) {
        return;
    }

    // Update the current frame of the sprite's animation...
    if (this->currFrameTimeCounter >= this->timePerFrame) {
        this->currSpriteIdx = (this->currSpriteIdx + 1);
        this->currFrameTimeCounter = 0.0;
    }
    else {
        this->currFrameTimeCounter += dT;
    }
}

void ESPAnimatedSpriteParticle::Draw(const Camera& camera, const ESP::ESPAlignment alignment) {
	// Don't draw if dead...
	if (this->IsDead() || this->currSpriteIdx >= this->numSprites) {
		return;
	}

	// Transform and draw the particle
	glPushMatrix();

	// Do any personal alignment transforms...
	Matrix4x4 personalAlignXF = this->GetPersonalAlignmentTransform(camera, alignment);
	
	glTranslatef(this->position[0], this->position[1], this->position[2]);
	glMultMatrixf(personalAlignXF.begin());
	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	
    // Figure out the texture coordinates based on the current frame of the animation...
    float leftTexCoord, rightTexCoord, topTexCoord, bottomTexCoord;
    this->GetCurrAnimationTextureCoords(leftTexCoord, rightTexCoord, topTexCoord, bottomTexCoord);

	glBegin(GL_QUADS);
	glNormal3i(0, 0, 1);
	glTexCoord2f(leftTexCoord,  bottomTexCoord); glVertex2f(-0.5f, -0.5f);
	glTexCoord2f(rightTexCoord, bottomTexCoord); glVertex2f( 0.5f, -0.5f);
	glTexCoord2f(rightTexCoord, topTexCoord);    glVertex2f( 0.5f,  0.5f);
	glTexCoord2f(leftTexCoord,  topTexCoord);    glVertex2f(-0.5f,  0.5f);
	glEnd();
    
	glPopMatrix();
}

void ESPAnimatedSpriteParticle::GetCurrAnimationTextureCoords(float& leftTexCoord, float& rightTexCoord,
                                                              float& topTexCoord, float& bottomTexCoord) const {

    int colIdx = this->currSpriteIdx % this->numSpriteCols;
    int rowIdx = static_cast<int>(this->currSpriteIdx / this->numSpriteCols);

    leftTexCoord   = static_cast<float>(colIdx * this->spriteXSize) / static_cast<float>(this->spriteTex->GetWidth());
    rightTexCoord  = static_cast<float>(colIdx * this->spriteXSize + this->spriteXSize) / static_cast<float>(this->spriteTex->GetWidth());
    bottomTexCoord = static_cast<float>((this->numSpriteRows - (rowIdx + 1)) * this->spriteYSize) / static_cast<float>(this->spriteTex->GetHeight());
    topTexCoord    = static_cast<float>((this->numSpriteRows - (rowIdx + 1)) * this->spriteYSize + this->spriteYSize) / static_cast<float>(this->spriteTex->GetHeight());


}