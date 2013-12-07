/**
 * ESPOrthoOnomataParticle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPORTHOONOMATAPARTICLE_H__
#define __ESPORTHOONOMATAPARTICLE_H__

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

#include "../GameModel/Onomatoplex.h"

#include "ESPUtil.h"
#include "ESPParticle.h"

class TextureFontSet;

class ESPOrthoOnomataParticle : public ESPParticle {
public: 
	ESPOrthoOnomataParticle(const TextureFontSet* font, const std::string& text);
	ESPOrthoOnomataParticle(const TextureFontSet* font);
	~ESPOrthoOnomataParticle();

	void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	void Tick(const double dT);
	void Draw(const Camera& camera, const ESP::ESPAlignment& alignment);

	void SetDropShadow(const DropShadow& ds) {
		this->dropShadow = ds;
		this->dropShadow.isSet = true;
	}
	void SetOnomatoplexSound(Onomatoplex::SoundType st, Onomatoplex::Extremeness e) {
		this->soundType = st;
		this->extremeness = e;
		this->GenerateNewString();
	}

	Onomatoplex::SoundType GetSoundType() const { return this->soundType; }
	Onomatoplex::Extremeness GetSoundExtremeness() const { return this->extremeness; }

protected:
	bool useGivenString;

	const TextureFontSet*		 font;
	Onomatoplex::SoundType	 soundType;
	Onomatoplex::Extremeness extremeness;
	DropShadow dropShadow;

	std::string currStr;
	float currHalfStrWidth;
	float currHalfStrHeight;

	void SetRandomSoundType();
	void SetRandomExtremeness();
	void GenerateNewString();

private:
	DISALLOW_COPY_AND_ASSIGN(ESPOrthoOnomataParticle);

};

inline void ESPOrthoOnomataParticle::GenerateNewString() {
	assert(!this->useGivenString);
	this->currStr = Onomatoplex::Generator::GetInstance()->Generate(this->soundType, this->extremeness);
	this->currHalfStrWidth = this->font->GetWidth(this->currStr) * 0.5f;
}
inline void ESPOrthoOnomataParticle::SetRandomSoundType() {
	assert(!this->useGivenString);
	unsigned int soundTypeRnd = Randomizer::GetInstance()->RandomUnsignedInt() % Onomatoplex::NumSoundTypes;
	this->soundType		= static_cast<Onomatoplex::SoundType>(soundTypeRnd);
}
inline void ESPOrthoOnomataParticle::SetRandomExtremeness() {
	assert(!this->useGivenString);
	unsigned int extremenessRnd	= Randomizer::GetInstance()->RandomUnsignedInt() % Onomatoplex::NumExtremenessTypes;
	this->extremeness	= static_cast<Onomatoplex::Extremeness>(extremenessRnd);
}

/**
 * Revive this particle with the given lifespan length in seconds.
 */
inline void ESPOrthoOnomataParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
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
inline void ESPOrthoOnomataParticle::Tick(const double dT) {
	ESPParticle::Tick(dT);
}


#endif // __ESPORTHOONOMATAPARTICLE_H__