/**
 * BossEffectEventInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOSSEFFECTEVENTINFO_H__
#define __BOSSEFFECTEVENTINFO_H__

class BossEffectEventInfo {
public:
    BossEffectEventInfo() {};
    virtual ~BossEffectEventInfo() {};

    enum Type { 
        PowerChargeInfo, ExpandingHaloInfo, SparkBurstInfo, ElectricitySpasmInfo, PuffOfSmokeInfo,
        ShockwaveInfo, FullscreenFlashInfo, DebrisInfo
    };

    virtual BossEffectEventInfo::Type GetType() const = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(BossEffectEventInfo);
};

#endif // __EFFECTEVENTINFO_H__