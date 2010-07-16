#ifndef __ESPUTIL_H__
#define __ESPUTIL_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Matrix.h"

class Camera;

namespace ESP {
	// Possible alignment configurations for sprites and particles w.r.t. the viewer
	enum ESPAlignment { ScreenAligned, ScreenAlignedFollowVelocity, ViewPlaneAligned, ViewPointAligned, AxisAligned };
};

// An interval pairing of values
struct ESPInterval {
	float minValue, maxValue;

	ESPInterval() : minValue(0.0f), maxValue(0.0f) {}
	ESPInterval(float singleVal) : minValue(singleVal), maxValue(singleVal) {}
	ESPInterval(float min, float max) : minValue(min), maxValue(max) {
		if (min > max) {
			assert(false);
			this->minValue = 0;
			this->maxValue = 0;
		}
	}

	void CopyFromInterval(const ESPInterval& other) {
		this->minValue = other.minValue;
		this->maxValue = other.maxValue;
	}

	float RandomValueInInterval() const {
		return static_cast<float>(minValue + Randomizer::GetInstance()->RandomNumZeroToOne()*(maxValue - minValue));
	}

	float MeanValueInInterval() const {
		return (minValue + maxValue) / 2.0f;
	}	
};

// A function with a single variable
class ESPFunc {
public:
	virtual float Func(float t);
};

// The sine function for a modifiable sine wave
class ESPSineFunc : public ESPFunc {
private:
	float amplitude, phaseShift, freq, verticalShift;
public:
	ESPSineFunc(float amplitude, float phaseShift, float freq, float verticalShift) :
		amplitude(amplitude), phaseShift(phaseShift), freq(freq), verticalShift(verticalShift) {}

	virtual float Func(float t) {
		return amplitude * sinf(freq*t + phaseShift) + verticalShift;
	}
};


#endif