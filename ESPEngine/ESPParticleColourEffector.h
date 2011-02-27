#ifndef __ESPPARTICLECOLOUREFFECTOR_H__
#define __ESPPARTICLECOLOUREFFECTOR_H__

#include "../BlammoEngine/Colour.h"

#include "ESPParticleEffector.h"

class ESPParticleColourEffector : public ESPParticleEffector {
public:
	ESPParticleColourEffector();
	ESPParticleColourEffector(float startAlpha, float endAlpha);
	ESPParticleColourEffector(const Colour& colour, float startAlpha, float endAlpha);
	ESPParticleColourEffector(const Colour& colour, float alpha);
	ESPParticleColourEffector(const ColourRGBA& start, const ColourRGBA& end);
	~ESPParticleColourEffector();

	void AffectParticleOnTick(double dT, ESPParticle* particle);

	float GetStartAlpha() const;
	float GetEndAlpha() const;

	void SetStartAlpha(float alpha);
	void SetEndAlpha(float alpha);
    void SetEndColour(const Colour& c) {
        this->endColour = c;
    }

private:
	bool useStartColour;
	float startAlpha, endAlpha;
	Colour startColour, endColour;

    DISALLOW_COPY_AND_ASSIGN(ESPParticleColourEffector);
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