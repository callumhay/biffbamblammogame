/**
 * CompositeSoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __COMPOSITESOUNDEFFECT_H__
#define __COMPOSITESOUNDEFFECT_H__

#include "SoundEffect.h"

class CompositeSoundEffect : public SoundEffect {
public:
    static CompositeSoundEffect* Build(const EffectParameterMap& parameterMap, std::vector<SoundEffect*> soundEffects);
    ~CompositeSoundEffect();

private:
    std::vector<SoundEffect*> soundEffects;

    CompositeSoundEffect(const EffectParameterMap& parameterMap);

    bool EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;
    bool DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;

    DISALLOW_COPY_AND_ASSIGN(CompositeSoundEffect);
};

#endif // __COMPOSITESOUNDEFFECT_H__