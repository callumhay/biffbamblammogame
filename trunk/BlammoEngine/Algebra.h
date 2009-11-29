/**
 * Algebra.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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

#ifndef M_PI_MULT2
#define M_PI_MULT2 M_PI * 2.0f
#endif

#define SQRT_2 1.4142135623730950488

// Epsilon value for doing equivalence operations and the like
#define EPSILON 0.000001f

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

class Randomizer {
private:
	static Randomizer* instance;

	MTRand_int32  randomIntGen;			// Generates random 32-bit integers
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
};
#endif
