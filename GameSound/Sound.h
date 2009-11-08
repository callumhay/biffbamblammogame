#ifndef _SOUND_H_
#define _SOUND_H_

#include "../BlammoEngine/BasicIncludes.h"

/**
 * Class to represent the abstract highlevel structure of a in-game sound.
 * This class encapsulates much of the functionality for loading sounds.
 */
class Sound {

public:
	// Per-World Event and Mask Sounds
	enum WorldSound { TODO };
	
	// Main Menu Event and Mask Sounds
	enum MainMenuSound { 
		// Masks
		MainMenuBackgroundMask = 0,
		// Events
		MainMenuItemHighlightedEvent, 
		MainMenuItemEnteredEvent, 
		MainMenuItemSelectedEvent, 
		MainMenuItemScrolledEvent 
	};

	Sound(const std::string& filepath);
	virtual ~Sound();
	
	static bool IsSoundMask(int soundType);

	virtual void Play()	{
		alSourcePlay(this->source);
	}
	virtual void Stop() {
		alSourceStop(this->source);
	}

	virtual void Tick(double dT)	= 0;

protected:
	static const ALfloat DEFAULT_SOURCE_POS[3];
	static const ALfloat DEFAULT_SOURCE_VEL[3];

	ALuint buffer;	// The ID for the OpenAL buffer that holds the sound data
	ALuint source;	// The ID for the OpenAL source, representing the point in space that emits the sound

	ALenum format;				// The format of the loaded sound 
	ALsizei size;					// The size in bytes of the loaded sound
	ALsizei frequency;		// The frequency of the loaded sound
	ALboolean isLooping;	// Whether the sound is set to loop indefinitely or not

};

#endif