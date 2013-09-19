/**
 * ElectrifiedEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ELECTRIFIEDEFFECTINFO_H__
#define __ELECTRIFIEDEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class ElectrifiedEffectInfo : public BossEffectEventInfo {
public:
    ElectrifiedEffectInfo(const Point3D& position, float size, double timeInSecs, const Colour& brightColour) : 
      BossEffectEventInfo(), position(position), size(size), timeInSecs(timeInSecs), colour(brightColour) {}
      ~ElectrifiedEffectInfo() {}

      BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::ElectrifiedInfo; }

      const Point3D& GetPosition() const { return this->position; }
      float GetSize() const { return this->size; }
      double GetTimeInSecs() const { return this->timeInSecs; }
      const Colour& GetColour() const { return this->colour; }

private:
    const Point3D position;
    const float size;
    const double timeInSecs;
    const Colour colour;

    DISALLOW_COPY_AND_ASSIGN(ElectrifiedEffectInfo);
};

#endif // __ELECTRIFIEDEFFECTINFO_H__