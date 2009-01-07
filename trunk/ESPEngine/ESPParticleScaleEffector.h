#ifndef __ESPPARTICLESCALEEFFECTOR_H__
#define __ESPPARTICLESCALEEFFECTOR_H__

#include "../BlammoEngine/Colour.h"

#include "ESPParticleEffector.h"

class ESPParticleScaleEffector : public ESPParticleEffector {
private:
	float startScale, endScale;
public:
	ESPParticleScaleEffector(float startScale, float endScale);
	~ESPParticleScaleEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif