#ifndef __SOUNDTIMEDSEQUENCE_H__
#define __SOUNDTIMEDSEQUENCE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/BlammoTime.h"

// Inner class used for coupling a sequence of sounds together which will play
// in sequence only if the even occurs within a certain time interval of the previously
// played sound in the sequence. Other options are availble (e.g., what happens when you reach
// the end of a sequence etc.)
class SoundTimedSequence {
public:
   SoundTimedSequence(const std::vector<Mix_Chunk*>& soundSequence, double timeBeforeReset);
   ~SoundTimedSequence();
   
   Mix_Chunk* GetNextSoundToPlay();

private:
   long timeBeforeResetInMs;              // The time allowance between playing sounds in this sequence (in milliseconds)
                                          // before the sequence resets to the first sound
   int currSequenceIndex;                 // Index of the current sound in the soundSequence being played
   std::vector<Mix_Chunk*> soundSequence; // Sequence of sounds to play, starting at index 0
   long lastPlayTime; // The last system time the sequence was called on to play a sound

   DISALLOW_COPY_AND_ASSIGN(SoundTimedSequence);
};

/**
 * Get the next sound to play in the sequence based on the current time and the
 * last time that this function was called.
 */
inline Mix_Chunk* SoundTimedSequence::GetNextSoundToPlay() {
   long currentTime = BlammoTime::GetSystemTimeInMillisecs();
   if ((currentTime - this->lastPlayTime) < this->timeBeforeResetInMs) {
      // Play the next sound in the sequence (unless we're at the end of
      // the sequence, in which case just keep playing the last sound).
      this->currSequenceIndex = std::min<int>(static_cast<int>(this->soundSequence.size()-1), this->currSequenceIndex+1);
   }
   else {
      // Time expired, reset the sound to play to the first in the sequence
      this->currSequenceIndex = 0;
   }
   
   this->lastPlayTime = currentTime;
   return this->soundSequence[this->currSequenceIndex];
}

#endif // __SOUNDTIMEDSEQUENCE_H__