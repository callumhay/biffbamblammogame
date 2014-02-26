/**
 * ESPParticleColourEffector.h
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