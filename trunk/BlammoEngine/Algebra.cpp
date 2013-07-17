/**
 * Algebra.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "Algebra.h"

Randomizer* Randomizer::instance = NULL;

Randomizer::Randomizer() : 
randomIntGen(BlammoTime::GetSystemTimeInMillisecs()), 
randomDoubleGen(BlammoTime::GetSystemTimeInMillisecs()) {
}
