/**
 * GargleSoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GARGLESOUNDEFFECT_H__
#define __GARGLESOUNDEFFECT_H__

#include "SoundEffect.h"

class GargleSoundEffect : public SoundEffect {
public:
    GargleSoundEffect(const EffectParameterMap& parameterMap);
    ~GargleSoundEffect();

private:
    int rate;
    bool isSinWave;

    bool EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;
    bool DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;

    DISALLOW_COPY_AND_ASSIGN(GargleSoundEffect);
};

#endif // __GARGLESOUNDEFFECT_H__