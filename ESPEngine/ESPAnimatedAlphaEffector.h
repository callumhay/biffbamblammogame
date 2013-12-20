/**
 * ESPAnimatedAlphaEffector.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPANIMATEDALPHAEFFECTOR_H__
#define __ESPANIMATEDALPHAEFFECTOR_H__

#include "../BlammoEngine/Animation.h"

#include "ESPParticleEffector.h"
#include "ESPParticle.h"

class ESPAnimatedAlphaEffector : public ESPParticleEffector {
public:
    ESPAnimatedAlphaEffector() : ESPParticleEffector() {
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


#endif // __ESPANIMATEDALPHAEFFECTOR_H__