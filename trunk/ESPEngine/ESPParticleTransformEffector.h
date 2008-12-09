#ifndef __ESPPARTICLETRANSFORMEFFECTOR_H__
#define __ESPPARTICLETRANSFORMEFFECTOR_H__

#include "ESPParticleEffector.h"

class ESPParticleTransformEffector : public ESPParticleEffector {

public:
	ESPParticleTransformEffector();
	~ESPParticleTransformEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);

};
#endif