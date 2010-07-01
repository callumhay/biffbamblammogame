#include "EventSound.h"
#include "../ResourceManager.h"

const int EventSound::DEFAULT_LOOPS = 1;
const int EventSound::DEFAULT_FADEIN = 0;
const int EventSound::DEFAULT_FADEOUT = 0;

EventSound::EventSound(const std::string& name, int loops, int msFadein, int msFadeout) :
Sound(name, msFadein, msFadeout), numLoops(loops),
playingSoundChunk(NULL), channel(Sound::INVALID_SDL_CHANNEL) {
}

EventSound::~EventSound() {
	if (this->IsValid()) {
		// Make sure the sound is stopped first
		this->Stop(false);
	}

	// Delete all the probability-sound pairings
	for (std::list<SoundProbabilityPair>::iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {
		SoundProbabilityPair& currPair = *iter;
		Mix_Chunk* soundChunk = currPair.GetSoundChunk();
#ifdef _DEBUG
		Mix_FreeChunk(soundChunk);
#else
		bool success = ResourceManager::GetInstance()->ReleaseEventSoundResource(soundChunk);
		if (!success) {
			std::cerr << "Error releasing event sound " << this->GetSoundName() << std::endl;
		}
#endif
	}
	this->sounds.clear();
	this->playingSoundChunk = NULL;
}

// Static factory method for building a sound event (finite looping sound)
EventSound* EventSound::BuildSoundEvent(const std::string& name, int loops, int msFadein, int msFadeout,
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
		const float currProbability		  = probabilities[i];
		Mix_Chunk* soundChunk						= NULL;

		// Set the interval [x1, x2) where the sound will either execute or not
		float probabilityIntervalMin = currIntervalVal;
		currIntervalVal += (currProbability / totalProbability);
		float probabilityIntervalMax = currIntervalVal;

#ifdef _DEBUG
		soundChunk = Mix_LoadWAV(currFilepath.c_str());
#else
		soundChunk = ResourceManager::GetInstance()->GetEventSoundResource(currFilepath);
#endif
		if (soundChunk == NULL) {
			debug_output("Failed to load sound: " << currFilepath);
			assert(false);
			delete newSoundEvent;
			newSoundEvent = NULL;
			return NULL;
		}

		SoundProbabilityPair newSoundProb(probabilityIntervalMin, probabilityIntervalMax, soundChunk);
		newSoundEvent->sounds.push_back(newSoundProb);
	}
	assert(fabs(currIntervalVal - 1.0f) < EPSILON);

	// Since the interval uses a strict less than, we add a bit to the final interval to be correct
	if (!newSoundEvent->sounds.empty()) {
		newSoundEvent->sounds.back().SetMaxProb(1.1);
	}

	if (!newSoundEvent->IsValid()) {
		debug_output("Invalid event sound created for sound " << name);
		delete newSoundEvent;
		newSoundEvent = NULL;
	}

	return newSoundEvent;
}

// Static factory method for building a sound event mask (infinite looping sound)
EventSound* EventSound::BuildSoundMask(const std::string& name, const std::string& filepath, int msFadein, int msFadeout) {
	// Create the SDL objects required to play the sound
	Mix_Chunk* soundChunk = NULL;
#ifdef _DEBUG
	soundChunk = Mix_LoadWAV(filepath.c_str());
#else
	soundChunk = ResourceManager::GetInstance()->GetEventSoundResource(filepath);
#endif

	if (soundChunk == NULL) {
		debug_output("Failed to load sound: " << filepath);
		assert(false);
	}

	// Build the new sound
	EventSound* eventMaskSound = new EventSound(name, -1, msFadein, msFadeout);
	SoundProbabilityPair probSoundPair(0.0, 1.1, soundChunk);
	eventMaskSound->sounds.push_back(probSoundPair);

	if (!eventMaskSound->IsValid()) {
		debug_output("Invalid mask sound created for sound " << name << " from file " << filepath);
		delete eventMaskSound;
		eventMaskSound = NULL;
	}

	return eventMaskSound;
}


