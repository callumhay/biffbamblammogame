/**
 * EventSound.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "EventSound.h"
#include "../ResourceManager.h"

const int EventSound::DEFAULT_LOOPS = 1;
const int EventSound::DEFAULT_FADEIN = 0;
const int EventSound::DEFAULT_FADEOUT = 0;

EventSound::EventSound(const std::string& name, int loops, int msFadein, int msFadeout) :
Sound(name, msFadein, msFadeout), numLoops(loops), soundSequence(NULL) {
}

EventSound::~EventSound() {
	if (this->IsValid()) {
		// Make sure the sound is stopped first
		size_t numChannels = this->channels.size();
		for (size_t i = 0; i < numChannels; i++) {
			this->Stop(false);
		}
	}
	assert(this->channels.empty());

	// Delete all the probability-sound pairings
	for (std::list<SoundProbabilityPair*>::iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {
		SoundProbabilityPair* currPair = *iter;
		Mix_Chunk* soundChunk = currPair->GetSoundChunk();

		bool success = ResourceManager::GetInstance()->ReleaseEventSoundResource(soundChunk);
		if (!success) {
			std::cerr << "Error releasing event sound " << this->GetSoundName() << std::endl;
		}
      
      delete currPair;
      currPair = NULL;

	}
	this->sounds.clear();

   if (this->soundSequence != NULL) {
      delete this->soundSequence;
      this->soundSequence = NULL;
   }
}

// Static factory method for building a sound event (finite looping sound)
EventSound* EventSound::BuildProbabilitySoundEvent(const std::string& name, int loops, int msFadein, int msFadeout,
                                                   const std::vector<int>& probabilities, const std::vector<std::string>& filepaths) {
	
	// Build the new sound event...
	EventSound* newSoundEvent = new EventSound(name, loops, msFadein, msFadeout);

	assert(probabilities.size() == filepaths.size());
	const size_t NUM_SOUNDS = probabilities.size();

	// Start by calculating the total probability number
	float totalProbability = 0.0f;
	for (size_t i = 0; i < NUM_SOUNDS; i++) {
		totalProbability += probabilities[i];
	}

	// Create the set of sounds with their corresponding probability intervals
	float currIntervalVal = 0.0f;

	for (size_t i = 0; i < NUM_SOUNDS; i++) {
		const std::string& currFilepath = filepaths[i];
		const float currProbability	  = probabilities[i];
		Mix_Chunk* soundChunk			  = NULL;

		// Set the interval [x1, x2) where the sound will either execute or not
		float probabilityIntervalMin = currIntervalVal;
		currIntervalVal += (currProbability / totalProbability);
		float probabilityIntervalMax = currIntervalVal;

		soundChunk = ResourceManager::GetInstance()->GetEventSoundResource(currFilepath);
		if (soundChunk == NULL) {
			debug_output("Failed to load sound: " << currFilepath);
			assert(false);
			delete newSoundEvent;
			newSoundEvent = NULL;
			return NULL;
		}

		SoundProbabilityPair* newSoundProb = new SoundProbabilityPair(probabilityIntervalMin, probabilityIntervalMax, soundChunk);
		newSoundEvent->sounds.push_back(newSoundProb);
	}
	assert(fabs(currIntervalVal - 1.0f) < EPSILON);

	// Since the interval uses a strict less than, we add a bit to the final interval to be correct
	if (!newSoundEvent->sounds.empty()) {
		newSoundEvent->sounds.back()->SetMaxProb(1.1);
	}

	if (!newSoundEvent->IsValid()) {
		debug_output("Invalid event sound created for sound " << name);
		delete newSoundEvent;
		newSoundEvent = NULL;
	}

	return newSoundEvent;
}

// Static factory method from building a sequence sound event - one where a sequence of sounds plays if
// played within a certain amount of time
EventSound* EventSound::BuildSequenceSoundEvent(const std::string& name, int loops, int msFadein, int msFadeout, 
                                                double resetSequenceTime, const std::vector<std::string>& filepaths) {
   assert(!filepaths.empty());

   // Build the new sound event...
	EventSound* newSoundEvent = new EventSound(name, loops, msFadein, msFadeout);

   // Build the vector of SDL sounds in the sequence
   bool failed = false;
   std::vector<Mix_Chunk*> sequence;
   for (std::vector<std::string>::const_iterator iter = filepaths.begin(); iter != filepaths.end(); ++iter) {
      const std::string& currFilepath = *iter;
		Mix_Chunk* soundChunk = ResourceManager::GetInstance()->GetEventSoundResource(currFilepath);
		if (soundChunk == NULL) {
			debug_output("Failed to load sound: " << currFilepath);
			failed = true;
		}
      sequence.push_back(soundChunk);
   }

   // Check to see if we failed to load any particular sound chunk into memory
   if (failed) {
      for (std::vector<Mix_Chunk*>::iterator iter = sequence.begin(); iter != sequence.end(); ++iter) {
         Mix_Chunk* chunk = *iter;
         ResourceManager::GetInstance()->ReleaseEventSoundResource(chunk);
      }

      assert(false);
		delete newSoundEvent;
		newSoundEvent = NULL;
      return NULL;
   }


   SoundTimedSequence* soundSeq = new SoundTimedSequence(sequence, resetSequenceTime);
   newSoundEvent->soundSequence = soundSeq;

   return newSoundEvent;
}

// Static factory method for building a sound event mask (infinite looping sound)
EventSound* EventSound::BuildSoundMask(const std::string& name, const std::string& filepath, int msFadein, int msFadeout) {
	// Create the SDL objects required to play the sound
	Mix_Chunk* soundChunk = ResourceManager::GetInstance()->GetEventSoundResource(filepath);
	if (soundChunk == NULL) {
		debug_output("Failed to load sound: " << filepath);
		assert(false);
	}

	// Build the new sound
	EventSound* eventMaskSound = new EventSound(name, -1, msFadein, msFadeout);
	SoundProbabilityPair* probSoundPair = new SoundProbabilityPair(0.0, 1.1, soundChunk);
	eventMaskSound->sounds.push_back(probSoundPair);

	if (!eventMaskSound->IsValid()) {
		debug_output("Invalid mask sound created for sound " << name << " from file " << filepath);
		delete eventMaskSound;
		eventMaskSound = NULL;
	}

	return eventMaskSound;
}


void EventSound::DoChannelCleanUp() {
	std::list<std::map<int, Mix_Chunk*>::iterator> toRemove;
	for (std::map<int, Mix_Chunk*>::iterator iter = this->channels.begin(); iter != this->channels.end(); ++iter) {
		if (Mix_Playing(iter->first) != 1) {
			toRemove.push_back(iter);
		}
	}

	for (std::list<std::map<int, Mix_Chunk*>::iterator>::iterator iter = toRemove.begin(); iter != toRemove.end(); ++iter) {
		this->channels.erase(*iter);
	}
}