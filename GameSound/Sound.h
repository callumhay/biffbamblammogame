#ifndef _SOUND_H_
#define _SOUND_H_


class Sound {

public:
	// Per-World Event and Mask Sounds
	enum WorldSound { TODO };
	
	// Main Menu Event and Mask Sounds
	enum MainMenuSound { 
		// Masks
		MainMenuBGMask,
		// Events
		MainMenuItemHighlightedEvent, 
		MainMenuItemEnteredEvent, 
		MainMenuItemSelectedEvent, 
		MainMenuItemScrolledEvent 
	};

	Sound();
	virtual ~Sound();
	
	//virtual void Play() = 0;

};

#endif