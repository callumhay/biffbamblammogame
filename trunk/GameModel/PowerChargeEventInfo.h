/**
 * PowerChargeEventInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __POWERCHARGEEVENTINFO_H__
#define __POWERCHARGEEVENTINFO_H__

#include "BossEffectEventInfo.h"

class BossBodyPart;

/**
 * Information carried from the GameModel to represent a a boss effect where the boss is charging up
 * their weapon.
 */
class PowerChargeEventInfo : public BossEffectEventInfo {
public:
    PowerChargeEventInfo(const BossBodyPart* chargingPart, double timeInSecs, const Colour& colour) : 
      BossEffectEventInfo(), chargingPart(chargingPart), chargeTimeInSecs(timeInSecs), colour(colour) {}
    ~PowerChargeEventInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::PowerChargeInfo; }

    const BossBodyPart* GetChargingPart() const { return this->chargingPart; }
    double GetChargeTimeInSecs() const { return this->chargeTimeInSecs; }
    const Colour& GetColour() const { return this->colour; }

private:
    const BossBodyPart* chargingPart;
    const double chargeTimeInSecs;
    const Colour colour;

    DISALLOW_COPY_AND_ASSIGN(PowerChargeEventInfo);
};

#endif // __POWERCHARGEEVENTINFO_H__