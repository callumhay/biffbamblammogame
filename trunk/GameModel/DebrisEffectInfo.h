/**
 * DebrisEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __DEBRISEFFECTINFO_H__
#define __DEBRISEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class DebrisEffectInfo : public BossEffectEventInfo {
public:
    DebrisEffectInfo(const BossBodyPart* part, const Point2D& explosionCenter, const Colour& colour, 
        double minLifeOfDebrisInSecs, double maxLifeOfDebrisInSecs, int numDebrisBits) : 
      BossEffectEventInfo(), part(part), explosionCenter(explosionCenter), colour(colour), 
          minLifeOfDebrisInSecs(minLifeOfDebrisInSecs), maxLifeOfDebrisInSecs(maxLifeOfDebrisInSecs), 
          numDebrisBits(numDebrisBits) {
        assert(part != NULL);
        assert(minLifeOfDebrisInSecs > 0.0 && maxLifeOfDebrisInSecs >= minLifeOfDebrisInSecs);
        assert(numDebrisBits > 0);
      }
    ~DebrisEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::DebrisInfo; }

    const BossBodyPart* GetPart() const { return this->part; }
    const Point2D& GetExplosionCenter() const { return this->explosionCenter; }
    const Colour& GetColour() const { return this->colour; }
    double GetMinLifeOfDebrisInSecs() const { return this->minLifeOfDebrisInSecs; }
    double GetMaxLifeOfDebrisInSecs() const { return this->maxLifeOfDebrisInSecs; }
    int GetNumDebrisBits() const { return this->numDebrisBits; }
    
private:
    const BossBodyPart* part;
    const Point2D explosionCenter;
    const Colour& colour;
    double minLifeOfDebrisInSecs;
    double maxLifeOfDebrisInSecs;
    const int numDebrisBits;

    DISALLOW_COPY_AND_ASSIGN(DebrisEffectInfo);
};

#endif // __DEBRISEFFECTINFO_H__