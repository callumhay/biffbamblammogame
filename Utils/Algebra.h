#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <assert.h>
#include <stdlib.h>
#include <time.h>

// Define essential math constants
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#ifndef M_PI_DIV2
#define M_PI_DIV2 M_PI / 2.0f
#endif

#ifndef M_PI_MULT2
#define M_PI_MULT2 M_PI * 2.0f
#endif


// Epsilon value for doing equivalence operations and the like
#define EPSILON 0.0000001f

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

namespace Randomizer {
	void InitializeRandomizer();
	void InitializeRandomizer(unsigned int seed);
	int Random();
	double RandomNumZeroToOne();
	double RandomNumNegOneToOne();
};

inline void Randomizer::InitializeRandomizer() {
	srand(static_cast<unsigned int>(time(NULL)));
};

inline void Randomizer::InitializeRandomizer(unsigned int seed) {
	srand(seed);
};

// A random value in [0, RAND_MAX]
inline int Randomizer::Random() {
	return rand();
}

// Return a random number in [0, 1]
inline double Randomizer::RandomNumZeroToOne() {
	return static_cast<double>(rand())/(static_cast<double>(RAND_MAX));
};

// Return a random number in [-1, 1]
inline double Randomizer::RandomNumNegOneToOne() {
	double randNum = rand()/(static_cast<double>(RAND_MAX));
	double randomSign = -1.0;
	if (rand() % 2 == 0) {
		randomSign = 1.0;
	}
	return randNum * randomSign;
};
#endif
