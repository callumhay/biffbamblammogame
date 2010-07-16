#ifndef __ESPONOMATAPARTICLE_H__
#define __ESPONOMATAPARTICLE_H__

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

class ESPOnomataParticle : public ESPParticle {
public: 
	ESPOnomataParticle(const TextureFontSet* font, const std::string& text);
	ESPOnomataParticle(const TextureFontSet* font);
	~ESPOnomataParticle();

	void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	void Tick(const double dT);
	void Draw(const Camera& camera, const ESP::ESPAlignment alignment);

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
	// Disallow copy and assign
	ESPOnomataParticle(const ESPOnomataParticle& p);
	ESPOnomataParticle& operator=(const ESPOnomataParticle& p);

};
#endif