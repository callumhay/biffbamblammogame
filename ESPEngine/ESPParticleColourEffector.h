#ifndef __ESPPARTICLECOLOUREFFECTOR_H__
#define __ESPPARTICLECOLOUREFFECTOR_H__

#include "../BlammoEngine/Colour.h"

#include "ESPParticleEffector.h"

class ESPParticleColourEffector : public ESPParticleEffector {
private:
	bool useStartColour;
	float startAlpha, endAlpha;
	Colour startColour, endColour;

public:
	ESPParticleColourEffector();
	ESPParticleColourEffector(float startAlpha, float endAlpha);
	ESPParticleColourEffector(const Colour& colour, float startAlpha, float endAlpha);
	ESPParticleColourEffector(const Colour& colour, float alpha);
	ESPParticleColourEffector(const ColourRGBA& start, const ColourRGBA& end);
	virtual ~ESPParticleColourEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);

	float GetStartAlpha() const;
	float GetEndAlpha() const;

	void SetStartAlpha(float alpha);
	void SetEndAlpha(float alpha);

};

inline float ESPParticleColourEffector::GetStartAlpha() const {
	return this->startAlpha;
}

inline float ESPParticleColourEffector::GetEndAlpha() const {
	return this->endAlpha;
}

inline void ESPParticleColourEffector::SetStartAlpha(float alpha) {
	this->startAlpha = alpha;
}

inline void ESPParticleColourEffector::SetEndAlpha(float alpha) {
	this->endAlpha = alpha;
}


#endif