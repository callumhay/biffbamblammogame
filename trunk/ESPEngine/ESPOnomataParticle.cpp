#include "ESPOnomataParticle.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/TextureFontSet.h"

ESPOnomataParticle::ESPOnomataParticle(const TextureFontSet* font, const std::string& text) : ESPParticle(), font(font), currStr(text), useGivenString(true) {
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

void ESPOnomataParticle::GenerateNewString() {
	assert(!this->useGivenString);
	this->currStr = Onomatoplex::Generator::GetInstance()->Generate(this->soundType, this->extremeness);
	this->currHalfStrWidth = TextureFontSet::TEXT_3D_SCALE * this->font->GetWidth(this->currStr) * 0.5f;
}
void ESPOnomataParticle::SetRandomSoundType() {
	assert(!this->useGivenString);
	unsigned int soundTypeRnd		= Randomizer::GetInstance()->RandomUnsignedInt() % Onomatoplex::NumSoundTypes;
	this->soundType		= static_cast<Onomatoplex::SoundType>(soundTypeRnd);
}
void ESPOnomataParticle::SetRandomExtremeness() {
	assert(!this->useGivenString);
	unsigned int extremenessRnd	= Randomizer::GetInstance()->RandomUnsignedInt() % Onomatoplex::NumExtremenessTypes;
	this->extremeness	= static_cast<Onomatoplex::Extremeness>(extremenessRnd);
}

/**
 * Revive this particle with the given lifespan length in seconds.
 */
void ESPOnomataParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
	// Set the members to reflect a 'new life'
	ESPParticle::Revive(pos, vel, size, rot, totalLifespan);
	
	if (!this->useGivenString) {
		this->GenerateNewString();
	}
}

/**
 * Called each frame with the delta time for that frame, this will
 * provide a slice of the lifetime of the particle.
 */
void ESPOnomataParticle::Tick(const double dT) {
	ESPParticle::Tick(dT);
}

/**
 * Draw this particle as it is currently.
 */
void ESPOnomataParticle::Draw(const Camera& camera, const ESP::ESPAlignment alignment) {
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
		glTranslatef(-this->currHalfStrWidth, -this->currHalfStrHeight, 0.0f);
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