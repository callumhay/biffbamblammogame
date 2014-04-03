/**
 * ESPMultiAlphaEffector.h
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

#ifndef __ESPMULTIALPHAEFFECTOR_H__
#define __ESPMULTIALPHAEFFECTOR_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"

#include "ESPEffector.h"

class ESPMultiAlphaEffector : public ESPEffector {
public:
    ESPMultiAlphaEffector();
    ~ESPMultiAlphaEffector() {}

    void AffectParticleOnTick(double dT, ESPParticle* particle);
    void AffectBeamOnTick(double dT, ESPBeam* beam);

    void SetAlphas(const std::vector<float>& alphas);
    void SetAlphasWithPercentage(const std::vector<std::pair<float, double> >& alphas);

    ESPEffector* Clone() const;

private:
	std::vector<double> percentages;
	std::vector<float> alphas;

    float CalculateAlpha(double lifeElapsed, double totalLifespan) const;

    DISALLOW_COPY_AND_ASSIGN(ESPMultiAlphaEffector);
};

inline ESPEffector* ESPMultiAlphaEffector::Clone() const {
    ESPMultiAlphaEffector* clone = new ESPMultiAlphaEffector();
    clone->percentages = this->percentages;
    clone->alphas = this->alphas;
    return clone;
}

#endif // __ESPMULTIALPHAEFFECTOR_H__