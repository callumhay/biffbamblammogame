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

#ifndef __POWERCHARGEEFFECTINFO_H__
#define __POWERCHARGEEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class BossBodyPart;

/**
 * Information carried from the GameModel to represent a a boss effect where the boss is charging up
 * their weapon.
 */
class PowerChargeEffectInfo : public BossEffectEventInfo {
public:
    PowerChargeEffectInfo(const BossBodyPart* chargingPart, double timeInSecs, const Colour& colour, 
        float sizeMultiplier = 1.0f, const Vector2D& posOffset = Vector2D(0,0)) : 
      BossEffectEventInfo(), chargingPart(chargingPart), chargeTimeInSecs(timeInSecs), 
          colour(colour), sizeMultiplier(sizeMultiplier), posOffset(posOffset) {}
    ~PowerChargeEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::PowerChargeInfo; }

    const BossBodyPart* GetChargingPart() const { return this->chargingPart; }
    double GetChargeTimeInSecs() const { return this->chargeTimeInSecs; }
    const Colour& GetColour() const { return this->colour; }
    float GetSizeMultiplier() const { return this->sizeMultiplier; }
    const Vector2D& GetPositionOffset() const { return this->posOffset; }

private:
    const BossBodyPart* chargingPart;
    const double chargeTimeInSecs;
    const Colour colour;
    const float sizeMultiplier;
    const Vector2D& posOffset;

    DISALLOW_COPY_AND_ASSIGN(PowerChargeEffectInfo);
};

#endif // __POWERCHARGEEFFECTINFO_H__