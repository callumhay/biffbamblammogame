/**
 * ESPAnimatedAlphaEffector.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __ESPANIMATEDALPHAEFFECTOR_H__
#define __ESPANIMATEDALPHAEFFECTOR_H__

#include "../BlammoEngine/Animation.h"

#include "ESPEffector.h"
#include "ESPParticle.h"
#include "ESPBeam.h"

class ESPAnimatedAlphaEffector : public ESPEffector {
public:
    ESPAnimatedAlphaEffector() : ESPEffector() {
        this->alphaAnimation0.SetInterpolantValue(1.0f);
        this->alphaAnimation1.SetInterpolantValue(1.0f);
    }
    ~ESPAnimatedAlphaEffector() {}

    void SetAnimation0(const AnimationMultiLerp<float>& animation) {
        this->alphaAnimation0 = animation;
    }
    void SetAnimation1(const AnimationMultiLerp<float>& animation) {
        this->alphaAnimation1 = animation;
    }

    void AffectParticleOnTick(double dT, ESPParticle* particle);
    void AffectBeamOnTick(double dT, ESPBeam* beam);

    ESPEffector* Clone() const;
    
private:
    AnimationMultiLerp<float> alphaAnimation0;
    AnimationMultiLerp<float> alphaAnimation1;

    DISALLOW_COPY_AND_ASSIGN(ESPAnimatedAlphaEffector);
};

inline void ESPAnimatedAlphaEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
    this->alphaAnimation0.Tick(dT);
    this->alphaAnimation1.Tick(dT);
    particle->SetAlpha(this->alphaAnimation0.GetInterpolantValue() * this->alphaAnimation1.GetInterpolantValue());
}

inline void ESPAnimatedAlphaEffector::AffectBeamOnTick(double, ESPBeam*) {
    assert(false);
    // NOT IMPLEMENTED YET
}

inline ESPEffector* ESPAnimatedAlphaEffector::Clone() const {
    ESPAnimatedAlphaEffector* clone = new ESPAnimatedAlphaEffector();
    clone->alphaAnimation0 = this->alphaAnimation0;
    clone->alphaAnimation1 = this->alphaAnimation1;
    return clone;
}


#endif // __ESPANIMATEDALPHAEFFECTOR_H__