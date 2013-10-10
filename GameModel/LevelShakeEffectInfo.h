/**
 * GeneralEffectEventInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __LEVELSHAKEEVENTINFO_H__
#define __LEVELSHAKEEVENTINFO_H__

#include "GeneralEffectEventInfo.h"

class LevelShakeEffectInfo : public GeneralEffectEventInfo {
public:
    LevelShakeEffectInfo(double timeInSecs, const Vector3D& shakeVec, float shakeSpd) :
      timeInSecs(timeInSecs), shakeVec(shakeVec), shakeSpd(shakeSpd) {}

    GeneralEffectEventInfo::Type GetType() const { return GeneralEffectEventInfo::LevelShake; }

    double GetTimeInSeconds() const { return this->timeInSecs; }
    const Vector3D& GetShakeVector() const { return this->shakeVec; }
    float GetShakeSpeed() const { return this->shakeSpd; }

private:
    const double timeInSecs;
    const Vector3D shakeVec;
    const float shakeSpd;

    DISALLOW_COPY_AND_ASSIGN(LevelShakeEffectInfo);
};

#endif // __LEVELSHAKEEVENTINFO_H__