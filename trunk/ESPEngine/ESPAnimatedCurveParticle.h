/**
 * ESPAnimatedCurveParticle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPANIMATEDCURVEPARTICLE_H__
#define __ESPANIMATEDCURVEPARTICLE_H__

#include "ESPParticle.h"
#include "../BlammoEngine/Bezier.h"

class ESPAnimatedCurveParticle : public ESPParticle {
public:
    ESPAnimatedCurveParticle(std::vector<Bezier*> bezierCurves, 
        const ESPInterval& lineThickness, const ESPInterval& timeToAnimateInterval);
	~ESPAnimatedCurveParticle();

	void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	void Tick(const double dT);
    void Draw(const Camera& camera, const ESP::ESPAlignment alignment);

private:
    const std::vector<Bezier*> possibleCurves;
    int currSelectedCurveIdx;
    double currTimeCounter;
    double timeToAnimate;
    ESPInterval timeToAnimateInterval;
    float currLineThickness;
    ESPInterval lineThickness;

    void SelectRandomCurve();
    double GetCurveT() const;

    DISALLOW_COPY_AND_ASSIGN(ESPAnimatedCurveParticle);
};

inline void ESPAnimatedCurveParticle::SelectRandomCurve() {
    this->currSelectedCurveIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->possibleCurves.size();
    this->currLineThickness = lineThickness.RandomValueInInterval();
    this->timeToAnimate = this->timeToAnimateInterval.RandomValueInInterval();
    this->currTimeCounter = this->timeToAnimate;
    assert(this->timeToAnimate > 0);
    assert(this->currLineThickness > 0);
}

inline double ESPAnimatedCurveParticle::GetCurveT() const {
    return 1.0 - (this->currTimeCounter / this->timeToAnimate);
}

#endif // __ESPANIMATEDCURVEPARTICLE_H__