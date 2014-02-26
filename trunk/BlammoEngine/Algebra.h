/**
 * Algebra.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __ALGEBRA_H__
#define __ALGEBRA_H__

#include "BasicIncludes.h"
#include "BlammoTime.h"

// Define essential math constants
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_DIV2
#define M_PI_DIV2 M_PI / 2.0f
#endif

#ifndef M_PI_DIV4
#define M_PI_DIV4 M_PI / 4.0f
#endif

#ifndef M_PI_MULT2
#define M_PI_MULT2 M_PI * 2.0f
#endif

#define SQRT_2 1.4142135623730950488

// Epsilon value for doing equivalence operations and the like
#define EPSILON 0.00001f

// Trigonometry namespace for functions that are nice to have
// for basic conversions and trigonometric math stuffs
namespace Trig {
  float degreesToRadians(float degrees);
  float radiansToDegrees(float rads);
};

inline float Trig::degreesToRadians(float degrees){
  return M_PI / 180.0f * degrees;
};

inline float Trig::radiansToDegrees(float rads){
  return 180.0f / M_PI * rads;
};

namespace NumberFuncs {

	int SignOf(int a);
	int SignOf(float a);
	int NextPowerOfTwo(int a);
	float MinF(float a, float b);
	float MaxF(float a, float b);
	float Clamp(float n, float min, float max);

    /**
     * Returns the expected linear interpolation value of 'y' with the given x range [x0, x1] and y range [y0, y1] with
     * the given value of x.
     */
    template <typename T> T Lerp(const T& x0, const T& x1, const T& y0, const T& y1, const T& x) {
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }
    template <typename T> T LerpOverTime(double x0, double x1, const T& y0, const T& y1, double x) {
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }
    template <typename T> T LerpOverFloat(float x0, float x1, const T& y0, const T& y1, float x) {
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }
};

inline int NumberFuncs::SignOf(int a) {
	return (a == 0) ? 0 : (a < 0 ? -1 : 1); 
};

inline int NumberFuncs::SignOf(float a) {
	return (a == 0) ? 0 : (a < 0 ? -1 : 1); 
};

// Returns the first power of 2 greater than or equal
// to the given integer.
inline int NumberFuncs::NextPowerOfTwo(int a) {
	int temp = 1;
	while (temp < a) {
		temp <<= 1;
	}
	return temp;
};

inline float NumberFuncs::MinF(float a, float b) {
	return a > b ? b : a;
}

inline float NumberFuncs::MaxF(float a, float b) {
	return a > b ? a : b;
}

inline float NumberFuncs::Clamp(float n, float min, float max) {
	if (n < min) { return min; }
	if (n > max) { return max; }
	return n;
}

class Randomizer {
private:
	static Randomizer* instance;

	MTRand_int32  randomIntGen;     // Generates random 32-bit integers
	MTRand_closed randomDoubleGen;	// Generates random double precision floating point numbers in [0, 1]

	Randomizer();
	~Randomizer(){};

public:
	
	static Randomizer* GetInstance() {
		if (Randomizer::instance == NULL) {
			Randomizer::instance = new Randomizer();
		}
		return Randomizer::instance;
	}

	static void DeleteInstance() {
		if (Randomizer::instance != NULL) {
			delete Randomizer::instance;
			Randomizer::instance = NULL;
		}
	}
	
	unsigned int RandomUnsignedInt();
	int		 RandomNegativeOrPositive();
	double RandomNumZeroToOne();
	double RandomNumNegOneToOne();
    bool RandomTrueOrFalse();
};

/**
 * Get a random unsigned integer.
 * Returns: Random uint.
 */
inline unsigned int Randomizer::RandomUnsignedInt() {
	return this->randomIntGen();
}

/**
 * Get a random negative or postivive unity integer.
 * Returns: +1 or -1, randomly.
 */
inline int Randomizer::RandomNegativeOrPositive() {
	int randomSign = -1;
	if (this->randomIntGen() % 2 == 0) {
		randomSign = 1;
	}
	return randomSign;
}

/**
 * Get a random number in the interval [0,1]
 * Returns: double in range [0,1].
 */
inline double Randomizer::RandomNumZeroToOne() {
	return this->randomDoubleGen();
}

/**
 * Get a random number in the interval [-1, 1]
 * Returns: double in range [-1, 1]
 */
inline double Randomizer::RandomNumNegOneToOne() {
	double randNum = this->randomDoubleGen();
	double randomSign = this->RandomNegativeOrPositive();
	return randNum * randomSign;
}

inline bool Randomizer::RandomTrueOrFalse() {
    return (RandomUnsignedInt() % 2 == 0);
}

#endif
