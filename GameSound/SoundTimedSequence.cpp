#include "SoundTimedSequence.h"

SoundTimedSequence::SoundTimedSequence(const std::vector<Mix_Chunk*>& soundSequence, double timeBeforeReset) :
soundSequence(soundSequence), lastPlayTime(0), currSequenceIndex(0) {
   assert(!soundSequence.empty());

   this->timeBeforeResetInMs = static_cast<long>(timeBeforeReset / 0.001);
}

SoundTimedSequence::~SoundTimedSequence() {
}
   
