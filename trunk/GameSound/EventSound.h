#ifndef __EVENTSOUND_H__
#define __EVENTSOUND_H__

#include "Sound.h"
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

	static EventSound* BuildSoundEvent(const std::string& name, int loops, int msFadein, int msFadeout,
																		 const std::vector<int>& probabilities, const std::vector<std::string>& filepaths);
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
	
protected:
	EventSound(const std::string& name, int loops, int msFadein, int msFadeout);

	// Inner class used for pairing sound with a probability interval - used when an event has multiple
	// possible sounds that it can play
	class SoundProbabilityPair {
	private:
		float probabilityIntervalMin;
		float probabilityIntervalMax;
		Mix_Chunk* soundChunk;

	public:
		SoundProbabilityPair(float intervalMin, float intervalMax, Mix_Chunk* sound) :
				probabilityIntervalMin(intervalMin), probabilityIntervalMax(intervalMax), soundChunk(sound) {}

		Mix_Chunk* GetSoundChunk() const { return this->soundChunk; }
		float GetMinProb() const { return this->probabilityIntervalMin; }
		float GetMaxProb() const { return this->probabilityIntervalMax; }

		void SetMaxProb(float amt) { this->probabilityIntervalMax = amt; }
		
		bool IsInInterval(float val) const { return val >= this->GetMinProb() && val < this->GetMaxProb(); }
		
		~SoundProbabilityPair() {};
	};


	Mix_Chunk* playingSoundChunk;	// Pointer to the sound chunk that's currently playing, NULL if nothing is playing
	int channel;									// SDL Mixer channel that the sound plays on

	std::list<SoundProbabilityPair> sounds;	// If there are multiple possible sounds that can be played by this
																					// event then they will be in this list

	int numLoops;		// The number of loops for this event/mask sound
	
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

	// Stop playing any previous sound if the sound is meant to be looped
	if (this->IsLooped()) {
		this->Stop(false);
	}

	// We determine which sound to play based on their probabilities and a randomly generated number
	double randomNum = Randomizer::GetInstance()->RandomNumZeroToOne();
	for (std::list<SoundProbabilityPair>::iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {

		const SoundProbabilityPair& currSoundProbPair = *iter;
		if (currSoundProbPair.IsInInterval(randomNum)) {
			
			this->playingSoundChunk = currSoundProbPair.GetSoundChunk();
			int fadeInAmt = doFadeIn ? this->msFadein : 0;
			this->channel = Mix_FadeInChannel(-1, this->playingSoundChunk, this->numLoops, fadeInAmt);
			break;
		}

	}

}

inline void EventSound::Pause() {
	if (this->IsPlaying()) {
		Mix_Pause(this->channel);
	}
}

inline void EventSound::UnPause() {
	if (this->IsPaused()) {
		Mix_Resume(this->channel);
	}
}

// Stop the sound from playing
inline void EventSound::Stop(bool doFadeout) {
	assert(this->IsValid());
	
	// Check to see if the sound is still playing on the channel
	if (this->IsPlaying()) {
		int fadeoutAmt = doFadeout ? this->msFadeout : 0;
		Mix_FadeOutChannel(this->channel, fadeoutAmt);
	}

	// Sound event has finished playing
	this->channel = Sound::INVALID_SDL_CHANNEL;
	this->playingSoundChunk = NULL;
}

inline bool EventSound::IsPlaying() const {
	Mix_Chunk* playingSDLChunk = Mix_GetChunk(this->channel);
	return (this->playingSoundChunk != NULL && Mix_Playing(this->channel) && playingSDLChunk == this->playingSoundChunk);
}

inline bool EventSound::IsPaused() const {
	return (this->playingSoundChunk != NULL && Mix_Paused(this->channel));
}

inline bool EventSound::IsLooped() const {
	return (this->numLoops == -1);
}

#endif // __EVENTSOUND_H__