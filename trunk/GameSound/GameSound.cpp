#include "GameSound.h"
#include "Sound.h"

#include "../BlammoEngine/Algebra.h"

GameSound::SoundProbabilityPair::~SoundProbabilityPair() {
	delete this->sound;
	this->sound = NULL;
}

GameSound::GameSound(bool isMask, const std::string& name, const std::vector<int>& probabilities, const std::vector<std::string>& filepaths) :
startDelay(DEFAULT_DELAY), numLoops(DEFAULT_LOOPS), fadein(DEFAULT_FADEIN), currentlyPlayingSound(NULL),
fadeout(DEFAULT_FADEOUT) {

	assert(probabilities.size() == filepaths.size());
	const size_t NUM_SOUNDS = probabilities.size();

	// Start by calculating the total probability number
	float totalProbability = 0.0f;
	for (size_t i = 0; i < NUM_SOUNDS; i++) {
		totalProbability += probabilities[i];
	}

	// Create the set of sounds with their corresponding probability intervals
	this->sounds.reserve(NUM_SOUNDS);
	float currIntervalVal = 0.0f;

	for (size_t i = 0; i < NUM_SOUNDS; i++) {
		const std::string& currFilepath = filepaths[i];
		const float currProbability		  = probabilities[i];

		SoundProbabilityPair* newSoundProb = new SoundProbabilityPair();
		newSoundProb->sound = new Sound(name + "_" + currFilepath, currFilepath);
		newSoundProb->sound->SetLooping(isMask);

		// Set the interval [x1, x2) where the sound will either execute or not
		newSoundProb->probabilityIntervalMin = currIntervalVal;
		currIntervalVal += (currProbability / totalProbability);
		newSoundProb->probabilityIntervalMax = currIntervalVal;

		this->sounds.push_back(newSoundProb);
	}
	assert(fabs(currIntervalVal - 1.0f) < EPSILON);

	// Since the interval uses a strict less than, we add a bit to the final interval to be correct
	this->sounds[NUM_SOUNDS-1]->probabilityIntervalMax = 1.1;

}

GameSound::~GameSound() {
	// Delete all the probability-sound pairings
	for (std::vector<SoundProbabilityPair*>::iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {
		SoundProbabilityPair* currPair = *iter;
		delete currPair;
		currPair= NULL;
	}

	this->currentlyPlayingSound = NULL;
}

/**
 * Gets whether the given sound type is a sound mask or not.
 */
bool GameSound::IsSoundMask(int soundType) {
	return soundType == MainMenuBackgroundMask;
}

/**
 * Play a sound from this game sound object.
 */
void GameSound::Play() {
	// First of all stop any previous current sound from playing
	this->Stop();

	// We determine which sound to play based on their probabilities and a randomly generated number
	double randomNum = Randomizer::GetInstance()->RandomNumZeroToOne();
	for (std::vector<SoundProbabilityPair*>::iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {
		SoundProbabilityPair* currSoundProbPair = *iter;
		if (randomNum >= currSoundProbPair->probabilityIntervalMin && randomNum < currSoundProbPair->probabilityIntervalMax) {
			this->currentlyPlayingSound = currSoundProbPair->sound;
			this->currentlyPlayingSound->Play();
			break;
		}
	}
}

/**
 * Stop any currently playing sound.
 */
void GameSound::Stop() {
	// Make sure there's a currently playing sound and stop it
	if (this->currentlyPlayingSound != NULL) {
		this->currentlyPlayingSound->Stop();
		this->currentlyPlayingSound = NULL;
	}
}

void GameSound::Tick(double dT) {
	// DOESNT DO ANYTHING YET...
	/*
	for (std::vector<SoundProbabilityPair*>::iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {
		iter->Tick(dT);
	}
	*/
}

/**
 * Check each sound in this sound event to see if it loaded properly. If any one sound didn't then
 * this will return false. If this funciton returns false it will also set the given string variable to the
 * filename corresponding to the invalid sound.
 */
bool GameSound::IsValid(std::string& invalidFilename) const {
	for (std::vector<SoundProbabilityPair*>::const_iterator iter = this->sounds.begin(); iter != this->sounds.end(); ++iter) {
		const SoundProbabilityPair* soundProbPair = *iter;
		if (!soundProbPair->GetSound()->IsValid()) {
			invalidFilename = soundProbPair->GetSound()->GetSoundFilename();
			return false;
		}
	}
	return true;
}

/**
 * Check whether this game sound is playing or not.
 * Returns: true if currently playing, false otherwise.
 */
bool GameSound::IsPlaying() const {
	if (this->currentlyPlayingSound != NULL) {
		return this->currentlyPlayingSound->IsPlaying();
	}
	else {
		return false;
	}
}