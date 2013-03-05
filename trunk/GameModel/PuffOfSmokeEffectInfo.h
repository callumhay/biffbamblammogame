/**
 * PuffOfSmokeEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PUFFOFSMOKEEFFECTINFO_H__
#define __PUFFOFSMOKEEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class PuffOfSmokeEffectInfo : public BossEffectEventInfo {
public:
    PuffOfSmokeEffectInfo(const Point2D& pos, float size, const Colour& colour) : 
      BossEffectEventInfo(), pos(pos), size(size), colour(colour) {}
    ~PuffOfSmokeEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::PuffOfSmokeInfo; }

    const Point2D& GetPosition() const { return this->pos; }
    float GetSize() const { return this->size; }
    const Colour& GetColour() const { return this->colour; }

private:
    const Point2D pos;
    const float size;
    const Colour colour;

    DISALLOW_COPY_AND_ASSIGN(PuffOfSmokeEffectInfo);
};

#endif // __PUFFOFSMOKEEFFECTINFO_H__