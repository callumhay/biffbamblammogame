#include "Algebra.h"

Randomizer* Randomizer::instance = NULL;

Randomizer::Randomizer() : 
randomIntGen(BlammoTime::GetSystemTimeInMillisecs()), 
randomDoubleGen(BlammoTime::GetSystemTimeInMillisecs()) {
}

/**
 * Get a random unsigned integer.
 * Returns: Random uint.
 */
unsigned int Randomizer::RandomUnsignedInt() {
	return this->randomIntGen();
}

/**
 * Get a random negative or postivive unity integer.
 * Returns: +1 or -1, randomly.
 */
int Randomizer::RandomNegativeOrPositive() {
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
double Randomizer::RandomNumZeroToOne() {
	return this->randomDoubleGen();
}

/**
 * Get a random number in the interval [-1, 1]
 * Returns: double in range [-1, 1]
 */
double Randomizer::RandomNumNegOneToOne() {
	double randNum = this->randomDoubleGen();
	double randomSign = this->RandomNegativeOrPositive();
	return randNum * randomSign;
}