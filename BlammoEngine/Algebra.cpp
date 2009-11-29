/**
 * Algebra.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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