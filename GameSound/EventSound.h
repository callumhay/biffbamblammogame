/**
 * EventSound.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __EVENTSOUND_H__
#define __EVENTSOUND_H__

#include "Sound.h"
#include "SoundProbabilityPair.h"
#include "SoundTimedSequence.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"

/**
 * An instantaneous sound happening while the game is playing. Any sound linked 
 * to an event will not loop infinitely - it will only play once or some finite 
 * number of times.
 */
class EventSound : public Sound {

public:
	static const int DEFAULT_LOOPS;
	static const int DEFAULT_FADEIN;
	static const int DEFAULT_FADEOUT;

	~EventSound();

   static EventSound* BuildProbabilitySoundEvent(const std::string& name, int loops, int msFadein, int msFadeout,
                                                 const std::vector<int>& probabilities, const std::vector<std::string>& filepaths);
   static EventSound* BuildSequenceSoundEvent(const std::string& name, int loops, int msFadein, int msFadeout, 
                                              double resetSequenceTime, const std::vector<std::string>& filepaths);
	static EventSound* BuildSoundMask(const std::string& name, const std::string& filepath, int msFadein, int msFadeout);

	// Inherited from Sound
	Sound::SoundType GetType() const;
	bool IsValid() const;
	void Play(bool doFadeIn);
	void Pause();
	void UnPause();
	void Stop(bool doFadeout);
	bool IsPlaying() const;
	bool IsPaused() const;
	bool IsLooped() const;
	
	void SetVolume(int volume);

protected:
	EventSound(const std::string& name, int loops, int msFadein, int msFadeout);

	std::map<int, Mix_Chunk*> channels;			// Mapping of all channels that this sound is playing 
                                             // on to their respective mix chunks
	std::list<SoundProbabilityPair*> sounds;	// If there are multiple possible sounds that can be played by this
                                             // event then they will be in this list

   SoundTimedSequence* soundSequence;  // If there's a sound sequence then this will not be NULL and
                                       // will be used instead of the probability pairs

	int numLoops;		// The number of loops for this event/mask sound
	

	void DoChannelCleanUp();
};

inline Sound::SoundType EventSound::GetType() const {
	// A mask is just a singluar event sound that loops continuously until its told to stop
	if (this->IsLooped() && this->sounds.size() == 1) {
		return Sound::MaskSound;
	}

	return Sound::EventSound;
}

inline bool EventSound::IsValid() const {
	if (!Sound::IsValid()) {
		return false;
	}
	if (this->sounds.empty()) {
		return false;
	}
	if (this->numLoops != -1 && this->numLoops <= 0) {
		return false;
	}

	return true;
}

inline void EventSound::Play(bool doFadeIn) {
	assert(this->IsValid());

	// Go through all the sounds currently mapped and check to see which ones are still playing,
	// clean up the map if any aren't
	this->DoChannelCleanUp();

	// If the sound is looping and already playing then don't play it again,
	// just get out of here
	//if (this->GetType() == Sound::MaskSound && this->IsPlaying()) {
	//	return;
	//}

	// We determine which sound to play based on their probabilities and a randomly generated number
	double randomNum = Randomizer::GetInstance()->RandomNumZeroToOne();
	for (std::list<SoundProbabilityPair*>::iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {

		const SoundProbabilityPair* currSoundProbPair = *iter;
		if (currSoundProbPair->IsInInterval(randomNum)) {
			
			Mix_Chunk* chunkToPlay = currSoundProbPair->GetSoundChunk();
			int fadeInAmt = doFadeIn ? this->msFadein : 0;
			int loops = -1;
			if (this->numLoops > 0) {
				loops = this->numLoops - 1;
			}
			int result = -1;
			if (fadeInAmt == 0) {
				result = Mix_PlayChannel(-1, chunkToPlay, loops);
			}
			else {
				result = Mix_FadeInChannel(-1, chunkToPlay, loops, fadeInAmt);
			}
			if (result != -1) {
				this->channels.insert(std::make_pair(result, chunkToPlay));
				Mix_Volume(result, this->volume);
			}
			else {
				debug_output("Could not play sound (" << this->GetSoundName() << "): " << Mix_GetError());
			}
			break;
		}
	}
}

inline void EventSound::Pause() {
	if (this->IsPlaying()) {
		for (std::map<int, Mix_Chunk*>::iterator iter = this->channels.begin(); iter != this->channels.end(); ++iter) {
			Mix_Pause(iter->first);
		}
	}
}

inline void EventSound::UnPause() {
	if (this->IsPaused()) {
		for (std::map<int, Mix_Chunk*>::iterator iter = this->channels.begin(); iter != this->channels.end(); ++iter) {
			Mix_Resume(iter->first);
		}
	}
}

// Stop the sound from playing
inline void EventSound::Stop(bool doFadeout) {
	assert(this->IsValid());
	this->DoChannelCleanUp();
	
	// Check to see if the sound is still playing on the channel
	if (this->IsPlaying()) {
		assert(!this->channels.empty());
		int currChannel = this->channels.begin()->first;
		assert(currChannel != -1);

		int fadeoutAmt = doFadeout ? this->msFadeout : 0;
		if (fadeoutAmt == 0) {
			Mix_HaltChannel(currChannel);
		}
		else {
			Mix_FadeOutChannel(currChannel, fadeoutAmt);
		}
		
		this->channels.erase(this->channels.begin());
	}
}

inline bool EventSound::IsPlaying() const {
	// Go through all the channels and check to see if any are playing
	for (std::map<int, Mix_Chunk*>::const_iterator iter = this->channels.begin(); iter != this->channels.end(); ++iter) {
		if (Mix_Playing(iter->first) == 1) {
			return true;
		}
    }
	return false;
}

inline bool EventSound::IsPaused() const {
	for (std::map<int, Mix_Chunk*>::const_iterator iter = this->channels.begin(); iter != this->channels.end(); ++iter) {
		if (Mix_Paused(iter->first)) {
			return true;
		}
	}
	return false;
}

inline bool EventSound::IsLooped() const {
	return (this->numLoops == -1);
}

inline void EventSound::SetVolume(int volume) {
	Sound::SetVolume(volume);
	// Go through all sound chunks in this and set the volume
	for (std::list<SoundProbabilityPair*>::iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {
		SoundProbabilityPair* currPair = *iter;
		Mix_Chunk* soundChunk = currPair->GetSoundChunk();
		assert(soundChunk != NULL);
		Mix_VolumeChunk(soundChunk, volume);
	}
}

#endif // __EVENTSOUND_H__