/**
 * Reverb3DSoundEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __REVERB3DSOUNDEFFECT_H__
#define __REVERB3DSOUNDEFFECT_H__

#include "SoundEffect.h"

class Reverb3DSoundEffect : public SoundEffect {
public:
    Reverb3DSoundEffect(const EffectParameterMap& parameterMap);
    ~Reverb3DSoundEffect();

private:
    int roomAtten;
    int roomHighFreqAtten;
    float roomRolloff;
    float decayTime;
    float decayHighFreqRatio;
    int reflectionAtten;
    float reflectionDelay;
    int reverbAtten;
    float reverbDelay;
    float diffusion;
    float density;
    float highFreqReference;

    bool EnableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;
    bool DisableEffect(irrklang::ISoundEffectControl& soundEffectCtrl) const;

    DISALLOW_COPY_AND_ASSIGN(Reverb3DSoundEffect);
};

#endif // __REVERB3DSOUNDEFFECT_H__