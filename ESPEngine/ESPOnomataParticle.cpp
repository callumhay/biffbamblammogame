/**
 * ESPOnomataParticle.cpp
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
void ESPOnomataParticle::Draw(const Matrix4x4& modelMat, const Matrix4x4& modelInvTMat, 
                              const Camera& camera, const ESP::ESPAlignment& alignment) {

	// Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	// Transform and draw the particle...
    Matrix4x4 alignmentMat;
	this->GetPersonalAlignmentTransform(modelMat, modelInvTMat, camera, alignment, this->position, alignmentMat);

	// If set, draw the shadow
	if (this->dropShadow.isSet) {
		float dropAmt = 2.0f * this->currHalfStrHeight * this->dropShadow.amountPercentage;
		Vector2D dsScale = this->dropShadow.scale * this->size;
		glPushMatrix();

        alignmentMat[12]  += dropAmt; 
        alignmentMat[13]  -= dropAmt; 
		glMultMatrixf(alignmentMat.begin());
        alignmentMat[12]  -= dropAmt; 
        alignmentMat[13]  += dropAmt; 

		glScalef(dsScale[0], dsScale[1], 1.0f);
		glRotatef(this->rotation, 0, 0, -1);
		glTranslatef(-this->currHalfStrWidth, -this->currHalfStrHeight, 0.00f);
		glColor4f(this->dropShadow.colour.R(), this->dropShadow.colour.G(), this->dropShadow.colour.B(), this->alpha);
		this->font->Print(this->currStr);
		glPopMatrix();
	}

	// Draw the font itself
	glPushMatrix();
	glMultMatrixf(alignmentMat.begin());
	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glTranslatef(-this->currHalfStrWidth, -this->currHalfStrHeight, 0.0f);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	this->font->Print(this->currStr);
	glPopMatrix();
}