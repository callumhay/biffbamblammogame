#ifndef __ESPPARTICLESCALEEFFECTOR_H__
#define __ESPPARTICLESCALEEFFECTOR_H__

#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Vector.h"

#include "ESPParticleEffector.h"

class ESPParticleScaleEffector;

class ScaleEffect {
private:
	bool isInit;

public:
	// Variables for pulse effects
	float pulseRate;				// (Hz) Pulse rate in number of pulses per second
	float pulseGrowthScale;	// Amount of growth of a single pulse from smallest scale to largest

	ScaleEffect() : isInit(false), pulseRate(0), pulseGrowthScale(1) {}

	friend class ESPParticleScaleEffector;
};

class ESPParticleScaleEffector : public ESPParticleEffector {

private:
	ScaleEffect effect;
	Vector2D startScale, endScale;

public:

	ESPParticleScaleEffector(const Vector2D& startScale, const Vector2D& endScale);
	ESPParticleScaleEffector(float startScale, float endScale);
	ESPParticleScaleEffector(const ScaleEffect& effect);
	~ESPParticleScaleEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif