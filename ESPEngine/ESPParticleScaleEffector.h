/**
 * ESPParticleScaleEffector.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ESPPARTICLESCALEEFFECTOR_H__
#define __ESPPARTICLESCALEEFFECTOR_H__

#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Vector.h"

#include "ESPEffector.h"

class ESPParticleScaleEffector;

class ScaleEffect {
    friend class ESPParticleScaleEffector;
public:
	// Variables for pulse effects
	float pulseRate;				// (Hz) Pulse rate in number of pulses per second
	float pulseGrowthScale;	// Amount of growth of a single pulse from smallest scale to largest

    ScaleEffect() : isInit(false), pulseRate(0), pulseGrowthScale(1) {}
    ScaleEffect(float pulseRate, float pulseGrowthScale) : isInit(false), pulseRate(pulseRate), pulseGrowthScale(pulseGrowthScale) {}
    ScaleEffect(const ScaleEffect& copy) : isInit(copy.isInit), pulseRate(copy.pulseRate), pulseGrowthScale(copy.pulseGrowthScale) {}

    ScaleEffect& operator=(const ScaleEffect& copy) {
        this->isInit = copy.isInit;
        this->pulseRate = copy.pulseRate;
        this->pulseGrowthScale = copy.pulseGrowthScale;
        return *this;
    }	
private:
    bool isInit;
};

class ESPParticleScaleEffector : public ESPEffector {
public:
    ESPParticleScaleEffector(const Vector2D& startScale, const Vector2D& endScale);
    ESPParticleScaleEffector(float startScale, float endScale);
    ESPParticleScaleEffector(const ScaleEffect& effect);
    ESPParticleScaleEffector(const ESPParticleScaleEffector& copy);
    ~ESPParticleScaleEffector();

    const Vector2D& GetEndScale() const {
        return this->endScale;
    }

    void ResetScaleEffect(const ScaleEffect& effect) {
        assert(this->effect.isInit);
        this->effect = effect;
        this->effect.isInit = true;
    }

    void AffectParticleOnTick(double dT, ESPParticle* particle);
    void AffectBeamOnTick(double dT, ESPBeam* beam);

    ESPEffector* Clone() const;

    ESPParticleScaleEffector& operator=(const ESPParticleScaleEffector& copy);

private:
    ESPParticleScaleEffector() {}

	ScaleEffect effect;
	Vector2D startScale, endScale;
};

inline ESPParticleScaleEffector::ESPParticleScaleEffector(const ESPParticleScaleEffector& copy) {
    *this = copy;
}

inline ESPEffector* ESPParticleScaleEffector::Clone() const {
    return new ESPParticleScaleEffector(*this);
}

inline ESPParticleScaleEffector& ESPParticleScaleEffector::operator=(const ESPParticleScaleEffector& copy) {
    this->effect = copy.effect;
    this->startScale = copy.startScale;
    this->endScale = copy.endScale;
    return *this;
}

#endif