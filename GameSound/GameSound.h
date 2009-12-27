#ifndef _GAMESOUND_H_
#define _GAMESOUND_H_

class Sound;

#include <vector>
#include <string>

class GameSound {
	friend class MSFReader;

public:

	// Per-World Event and Mask Sounds
	enum WorldSound { TODO };
	
	// Main Menu Event and Mask Sounds
	enum MainMenuSound { 
		// Masks
		MainMenuBackgroundMask = 0,
		// Events
		MainMenuBackgroundBangSmallEvent,
		MainMenuBackgroundBangMediumEvent,
		MainMenuBackgroundBangBigEvent,
		MainMenuItemHighlightedEvent, 
		MainMenuItemEnteredEvent,
		MainMenuItemBackAndCancelEvent,
		MainMenuItemVerifyAndSelectEvent, 
		MainMenuItemScrolledEvent 
	};

	static bool IsSoundMask(int soundType);

	class SoundProbabilityPair {
		friend class GameSound;
	private:
		float probabilityIntervalMin;
		float probabilityIntervalMax;
		Sound* sound;

	public:
		Sound* GetSound() const { return this->sound; }
		float GetMinProb() const { return this->probabilityIntervalMin; }
		float GetMaxProb() const { return this->probabilityIntervalMax; }
		~SoundProbabilityPair();
	};

	static const int DEFAULT_DELAY = 0;
	static const int DEFAULT_LOOPS = 1;
	static const int DEFAULT_FADEIN = 0;
	static const int DEFAULT_FADEOUT = 0;

	~GameSound();

	void Play();
	void Stop();

	void Tick(double dT);

	void SetDelay(int delayInMS) { this->startDelay = delayInMS; }
	void SetLoops(int numLoops) { this->numLoops = numLoops; }
	void SetFadein(int fadeinMS) { this->fadein  = fadeinMS; }
	void SetFadeout(int fadeoutMS) { this->fadeout = fadeoutMS; }

	bool IsValid(std::string& invalidFilename) const;
	bool IsPlaying() const;

private:
	Sound* currentlyPlayingSound;
	std::vector<SoundProbabilityPair*> sounds;

	int startDelay;
	int numLoops;
	int fadein;
	int fadeout;

	GameSound(bool isMask, const std::string& name, const std::vector<int>& probabilities, const std::vector<std::string>& filepaths);
	
};

#endif // _SOUNDEVENT_H_