/**
 * ExpandingHaloEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __EXPANDINGHALOEFFECTINFO_H__
#define __EXPANDINGHALOEFFECTINFO_H__

class ExpandingHaloEffectInfo : public BossEffectEventInfo {
public:
    ExpandingHaloEffectInfo(const BossBodyPart* part, double timeInSecs, const Colour& colour) : 
      BossEffectEventInfo(), part(part), timeInSecs(timeInSecs), colour(colour) {}
    ~ExpandingHaloEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::ExpandingHaloInfo; }

    const BossBodyPart* GetPart() const { return this->part; }
    double GetTimeInSecs() const { return this->timeInSecs; }
    const Colour& GetColour() const { return this->colour; }

private:
    const BossBodyPart* part;
    const double timeInSecs;
    const Colour colour;

    DISALLOW_COPY_AND_ASSIGN(ExpandingHaloEffectInfo);
};

#endif // __EXPANDINGHALOEFFECTINFO_H__