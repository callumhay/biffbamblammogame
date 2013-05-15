/**
 * ShockwaveEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SHOCKWAVEEFFECTINFO_H__
#define __SHOCKWAVEEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class ShockwaveEffectInfo : public BossEffectEventInfo {
public:
    ShockwaveEffectInfo(const Point2D& pos, float size, double timeInSecs) : 
      BossEffectEventInfo(), pos(pos), size(size), timeInSecs(timeInSecs) {}
    ~ShockwaveEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::ShockwaveInfo; }

    const Point2D& GetPosition() const { return this->pos; }
    float GetSize() const { return this->size; }
    double GetTime() const { return this->timeInSecs; }

private:
    const Point2D pos;
    const float size;
    const double timeInSecs;

    DISALLOW_COPY_AND_ASSIGN(ShockwaveEffectInfo);
};

#endif // __SHOCKWAVEEFFECTINFO_H__