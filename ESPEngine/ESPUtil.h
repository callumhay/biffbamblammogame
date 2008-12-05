#ifndef __ESPUTIL_H__
#define __ESPUTIL_H__

#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Matrix.h"

class Camera;

namespace ESP {
	// Possible alignment configurations for sprites and particles w.r.t. the viewer
	enum ESPAlignment { ScreenAligned, ViewPlaneAligned, ViewPointAligned, AxisAligned };
};

// An interval pairing of values
struct ESPInterval {
	float minValue, maxValue;

	ESPInterval() : minValue(0.0f), maxValue(0.0f) {}
	ESPInterval(float min, float max) : minValue(min), maxValue(max) {
		if (min > max) {
			this->minValue = 0;
			this->maxValue = 0;
		}
	}

	float RandomValueInInterval() {
		return static_cast<float>(minValue + Randomizer::GetInstance()->RandomNumZeroToOne()*(maxValue - minValue));
	}
};


#endif