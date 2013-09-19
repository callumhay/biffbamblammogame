/**
 * PowerChargeEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SPARKBURSTEFFECTINFO_H__
#define __SPARKBURSTEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class SparkBurstEffectInfo : public BossEffectEventInfo {
public:
    SparkBurstEffectInfo(const BossBodyPart* part, double timeInSecs, const Colour& colour, const Vector3D& offset = Vector3D(0,0,0)) : 
      BossEffectEventInfo(), part(part), timeInSecs(timeInSecs), colour(colour), offset(offset) {}
    ~SparkBurstEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::SparkBurstInfo; }

    const BossBodyPart* GetPart() const { return this->part; }
    double GetTimeInSecs() const { return this->timeInSecs; }
    const Colour& GetColour() const { return this->colour; }
    const Vector3D& GetOffset() const { return this->offset; }

private:
    const BossBodyPart* part;
    const double timeInSecs;
    const Colour colour;
    const Vector3D offset;

    DISALLOW_COPY_AND_ASSIGN(SparkBurstEffectInfo);
};

#endif // __SPARKBURSTEFFECTINFO_H__