/**
 * ESPParticleScaleEffector.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

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

    ScaleEffect(float pulseRate, float pulseGrowthScale) : isInit(false), pulseRate(pulseRate), pulseGrowthScale(pulseGrowthScale) {}
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

    void ResetScaleEffect(const ScaleEffect& effect) {
        assert(this->effect.isInit);
        this->effect = effect;
        this->effect.isInit = true;
    }

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif