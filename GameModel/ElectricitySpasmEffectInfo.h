/**
 * ElectricitySpasmEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ELECTRICITYSPASMEFFECTINFO_H__
#define __ELECTRICITYSPASMEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class ElectricitySpasmEffectInfo : public BossEffectEventInfo {
public:
    ElectricitySpasmEffectInfo(const BossBodyPart* part, double timeInSecs, const Colour& colour) : 
      BossEffectEventInfo(), part(part), timeInSecs(timeInSecs), colour(colour) {}
    ~ElectricitySpasmEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::ElectricitySpasmInfo; }

    const BossBodyPart* GetPart() const { return this->part; }
    double GetTimeInSecs() const { return this->timeInSecs; }
    const Colour& GetColour() const { return this->colour; }

private:
    const BossBodyPart* part;
    const double timeInSecs;
    const Colour colour;

    DISALLOW_COPY_AND_ASSIGN(ElectricitySpasmEffectInfo);
};

#endif // __ELECTRICITYSPASMEFFECTINFO_H__