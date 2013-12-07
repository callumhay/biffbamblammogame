/**
 * ESPOnomataParticle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "ESPOnomataParticle.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/TextureFontSet.h"

ESPOnomataParticle::ESPOnomataParticle(const TextureFontSet* font, const std::string& text) : 
ESPParticle(), font(font), currStr(text), useGivenString(true) {
	assert(font != NULL);
	this->currHalfStrHeight = TextureFontSet::TEXT_3D_SCALE * font->GetHeight() * 0.5f;
	this->currHalfStrWidth = TextureFontSet::TEXT_3D_SCALE * this->font->GetWidth(this->currStr) * 0.5f;
}

ESPOnomataParticle::ESPOnomataParticle(const TextureFontSet* font) : ESPParticle(), font(font), useGivenString(false) {
	assert(font != NULL);
	this->currHalfStrHeight = TextureFontSet::TEXT_3D_SCALE * font->GetHeight() * 0.5f;

	// Random sound type and extremeness
	this->SetRandomSoundType();
	this->SetRandomExtremeness();
	this->GenerateNewString();
}

ESPOnomataParticle::~ESPOnomataParticle() {
}

/**
 * Draw this particle as it is currently.
 */
void ESPOnomataParticle::Draw(const Camera& camera, const ESP::ESPAlignment& alignment) {

	// Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	// Transform and draw the particle...
	Matrix4x4 personalAlignXF = this->GetPersonalAlignmentTransform(camera, alignment);

	// If set, draw the shadow
	if (this->dropShadow.isSet) {
		float dropAmt = 2.0f * this->currHalfStrHeight * this->dropShadow.amountPercentage;
		Vector2D dsScale = this->dropShadow.scale * this->size;
		glPushMatrix();
		glTranslatef(this->position[0] + dropAmt, this->position[1] - dropAmt, this->position[2]); 
		glMultMatrixf(personalAlignXF.begin());
		glScalef(dsScale[0], dsScale[1], 1.0f);
		glRotatef(this->rotation, 0, 0, -1);
		glTranslatef(-this->currHalfStrWidth, -this->currHalfStrHeight, 0.00f);
		glColor4f(this->dropShadow.colour.R(), this->dropShadow.colour.G(), this->dropShadow.colour.B(), this->alpha);
		this->font->Print(this->currStr);
		glPopMatrix();
	}

	// Draw the font itself
	glPushMatrix();
	glTranslatef(this->position[0], this->position[1], this->position[2]);
	glMultMatrixf(personalAlignXF.begin());
	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glTranslatef(-this->currHalfStrWidth, -this->currHalfStrHeight, 0.0f);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	this->font->Print(this->currStr);
	glPopMatrix();
}