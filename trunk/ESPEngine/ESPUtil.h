/**
 * ESPUtil.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ESPUTIL_H__
#define __ESPUTIL_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Matrix.h"

class Camera;

namespace ESP {
	// Possible alignment configurations for sprites and particles w.r.t. the viewer
	enum ESPAlignment { NoAlignment, ScreenAligned, ScreenAlignedFollowVelocity, 
        ScreenAlignedGlobalUpVec, AxisAligned, ScreenPlaneAligned, GlobalAxisAlignedX };
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

    float LerpInterval(double x0, double x1, double x) const {
        return NumberFuncs::LerpOverFloat<float>(x0, x1, this->minValue, this->maxValue, x);
    }

	bool operator==(const ESPInterval& other) const {
		return (this->minValue == other.minValue) && (this->maxValue == other.maxValue);
	}
	bool operator!=(const ESPInterval& other) const {
		return !(*this == other);
	}
};

inline ESPInterval operator*(const ESPInterval& i, float f) {
    return ESPInterval(f * i.minValue, f* i.maxValue);
}
inline ESPInterval operator*(float f, const ESPInterval& i) {
    return ESPInterval(f * i.minValue, f* i.maxValue);
}

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