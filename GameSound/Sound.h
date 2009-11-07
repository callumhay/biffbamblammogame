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

	virtual void Play()						= 0;
	virtual void Tick(double dT)	= 0;

};

#endif