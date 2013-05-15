/**
 * FullscreenFlashEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __FULLSCREENFLASHEFFECTINFO_H__
#define __FULLSCREENFLASHEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class FullscreenFlashEffectInfo : public BossEffectEventInfo {
public:
    FullscreenFlashEffectInfo(double timeInSecs, float shakeMultiplier = 1.0f) : 
      BossEffectEventInfo(), timeInSecs(timeInSecs), shakeMultiplier(shakeMultiplier) {}
    ~FullscreenFlashEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::FullscreenFlashInfo; }

    double GetTime() const { return this->timeInSecs; }
    float GetShakeMultiplier() const { return this->shakeMultiplier; }

private:
    const double timeInSecs;
    const float shakeMultiplier;

    DISALLOW_COPY_AND_ASSIGN(FullscreenFlashEffectInfo);
};

#endif // __FULLSCREENFLASHEFFECTINFO_H__