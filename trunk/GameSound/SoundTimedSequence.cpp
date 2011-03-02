/**
 * SoundTimedSequence.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "SoundTimedSequence.h"

SoundTimedSequence::SoundTimedSequence(const std::vector<Mix_Chunk*>& soundSequence, double timeBeforeReset) :
soundSequence(soundSequence), lastPlayTime(0), currSequenceIndex(0) {
   assert(!soundSequence.empty());

   this->timeBeforeResetInMs = static_cast<long>(timeBeforeReset / 0.001);
}

SoundTimedSequence::~SoundTimedSequence() {
}
   
