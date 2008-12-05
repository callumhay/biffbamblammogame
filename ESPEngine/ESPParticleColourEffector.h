#ifndef __ESPPARTICLECOLOUREFFECTOR_H__
#define __ESPPARTICLECOLOUREFFECTOR_H__

#include "../BlammoEngine/Colour.h"

#include "ESPParticleEffector.h"

class ESPParticleColourEffector : public ESPParticleEffector {
private:
	float startAlpha, endAlpha;
public:
	ESPParticleColourEffector(float startAlpha, float endAlpha);
	~ESPParticleColourEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif