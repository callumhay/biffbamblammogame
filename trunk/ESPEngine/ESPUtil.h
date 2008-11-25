#ifndef __ESPUTIL_H__
#define __ESPUTIL_H__

#include "../BlammoEngine/Algebra.h"

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
		return static_cast<float>(minValue + Randomizer::RandomNumZeroToOne()*(maxValue - minValue));
	}
};


#endif