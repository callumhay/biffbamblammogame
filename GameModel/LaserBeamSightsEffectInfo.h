/**
 * LaserBeamSightsEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __LASERBEAMSIGHTSEFFECTINFO_H__
#define __LASERBEAMSIGHTSEFFECTINFO_H__

#include "BossEffectEventInfo.h"

class LaserBeamSightsEffectInfo : public BossEffectEventInfo {
public:
    LaserBeamSightsEffectInfo(const Point2D& firingPt, const Point2D& targetPt, double durationInSecs) : 
      BossEffectEventInfo(), firingPt(firingPt), targetPt(targetPt), durationInSecs(durationInSecs) {}
    ~LaserBeamSightsEffectInfo() {}

    BossEffectEventInfo::Type GetType() const { return BossEffectEventInfo::LaserBeamSightsInfo; }

    const Point2D& GetFiringPoint() const { return this->firingPt; }
    const Point2D& GetTargetPoint() const { return this->targetPt; }
    double GetDurationInSecs() const { return this->durationInSecs; }

private:
    Point2D firingPt;
    Point2D targetPt;
    double durationInSecs;

    DISALLOW_COPY_AND_ASSIGN(LaserBeamSightsEffectInfo);
};

#endif // __LASERBEAMSIGHTSEFFECTINFO_H__