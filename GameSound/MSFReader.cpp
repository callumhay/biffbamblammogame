#include "MSFReader.h"
#include "SoundEvent.h"
#include "SoundMask.h"

#include "../ResourceManager.h"

#include "../GameView/GameViewConstants.h"

const char* MSFReader::OPEN_SOUND_DEFINTION_BLOCK			= "{";
const char* MSFReader::CLOSE_SOUND_DEFINITION_BLOCK		= "}";

const char* MSFReader::FILE_KEYWORD				= "file";
const char* MSFReader::DELAY_KEYWORD			= "delay";
const char* MSFReader::LOOPS_KEYWORD			= "loops";
const char* MSFReader::FADE_IN_KEYWORD		= "fadein";
const char* MSFReader::FADE_OUT_KEYWORD		= "fadeout";

const char* MSFReader::MAIN_MENU_BG_MASK									= "MainMenuBackgroundMask";
const char* MSFReader::MAIN_MENU_ITEM_HIGHLIGHTED_EVENT		= "MainMenuItemHighlightedEvent";
const char* MSFReader::MAIN_MENU_ITEM_ENTERED_EVENT				= "MainMenuItemEnteredEvent";
const char* MSFReader::MAIN_MENU_ITEM_SELECTED_EVENT			= "MainMenuItemSelectedEvent";
const char* MSFReader::MAIN_MENU_ITEM_SCROLLED_EVENT			= "MainMenuItemScrolledEvent";

bool MSFReader::ReadMainMenuMSF(std::map<Sound::MainMenuSound, Sound*>& mainMenuSounds) {

	// Grab a file in stream from the main manu music script file
	std::istringstream* inStream = ResourceManager::GetInstance()->FilepathToInStream(GameViewConstants::GetInstance()->MAIN_MENU_SOUND_SCRIPT);

	// Stuff for capturing errors
	bool error = false;
	std::string errorStr = "";

	// The current string being read from the file
	std::string currReadStr = "";
	
	// Data storage while we read the file - save the state of the file read so far
	bool soundDefBlockOpen = false;
	bool soundIsMask = false;
	std::string lastSoundType = "";

	int soundType = MSFReader::INVALID_SOUND_TYPE;
	std::string soundFilePath = "";
	int delay			= MSFReader::DEFAULT_DELAY;
	int loops			= MSFReader::DEFAULT_LOOPS;
	int fadein		= MSFReader::DEFAULT_FADEIN;
	int fadeout		= MSFReader::DEFAULT_FADEOUT;

	// Read through the music script file
	while (*inStream >> currReadStr && !error) {

		// Check for the opening of a music defintion block...
		if (currReadStr == MSFReader::OPEN_SOUND_DEFINTION_BLOCK) {
			if (soundDefBlockOpen) {
				error = true;
				errorStr = "Tried to open another sound definition block inside an already open block: extra '{' found.";
				continue;
			}
			// Reinitialize everything...
			soundDefBlockOpen = true;
			lastSoundType = "";
			soundFilePath = "";
			soundType = MSFReader::INVALID_SOUND_TYPE;
			delay			= MSFReader::DEFAULT_DELAY;
			loops			= MSFReader::DEFAULT_LOOPS;
			fadein		= MSFReader::DEFAULT_FADEIN;
			fadeout		= MSFReader::DEFAULT_FADEOUT;
		}
		// Check for the closing of a music defintion block...
		else if (currReadStr == MSFReader::CLOSE_SOUND_DEFINITION_BLOCK) {
			if (!soundDefBlockOpen) {
				error = true;
				errorStr = "Tried to close a non-opened sound definition block: extra '}' found.";
				continue;
			}
			assert(soundType != MSFReader::INVALID_SOUND_TYPE);
			
			soundDefBlockOpen = false;
			
			// Create the sound object...
			Sound* newSound = NULL;
			if (soundIsMask) {
				newSound = new SoundMask(soundFilePath);
			}
			else {
				newSound = new SoundEvent(soundFilePath, delay, loops, fadein, fadeout);
			}

			if (newSound == NULL) {
				error = true;
				errorStr = "Could not find sound file: " + soundFilePath;
				continue;
			}

			// Add the new sound to the mapping of sounds
			mainMenuSounds.insert(std::make_pair(static_cast<Sound::MainMenuSound>(soundType), newSound));
		}
		else {
			// Check to see if the sound definition block has been opened, if it has we will search for
			// the various parameters allowed for it, if not then we search for the keyword associated with it

			if (soundDefBlockOpen) {
				// Make sure that the sound type has already been defined, otherwise the script file
				// is invalid and we should get out of here
				if (soundType == MSFReader::INVALID_SOUND_TYPE) {
					error = true;
					errorStr = "Invalid sound type found: " + lastSoundType;
					continue;
				}

				// There are different parameters that are accepted based on the sound type...
				if (soundIsMask) {

					// For sound masks we just care about the file...
					if (currReadStr == MSFReader::FILE_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

						std::getline(*inStream, soundFilePath);
					}
				}
				else {
					// We are dealing with an event sound...
					if (currReadStr == MSFReader::FILE_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}
						
						// The rest of the line should contain the sound file path...
						std::getline(*inStream, soundFilePath);
					}
					else if (currReadStr == MSFReader::DELAY_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

					}
					else if (currReadStr == MSFReader::LOOPS_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

					}
					else if (currReadStr == MSFReader::FADE_IN_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

					}
					else if (currReadStr == MSFReader::FADE_OUT_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

					}
				}

			}
			else {
				// Sound definition block has not been opened yet, look for an identifier...
				soundType = MSFReader::ConvertMainMenuKeywordToSoundType(currReadStr);
				lastSoundType = currReadStr;
				if (soundType != MSFReader::INVALID_SOUND_TYPE) {
					soundIsMask = Sound::IsSoundMask(soundType);
				}
			}

		}
	}	// while

	// Clean up and return whether there was an error while reading
	delete inStream;
	inStream = NULL;

	// If there was an error then say so
	if (error) {
		std::cerr << errorStr << std::endl;
	}

	return !error;
}

/**
 * Converts the given music script file keyword sound name into an enumerated sound type
 * for Main Menu sounds.
 */
int MSFReader::ConvertMainMenuKeywordToSoundType(const std::string& soundName) {
	if (soundName == MSFReader::MAIN_MENU_BG_MASK) {
		return Sound::MainMenuBackgroundMask;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_HIGHLIGHTED_EVENT) {
		return Sound::MainMenuItemHighlightedEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_ENTERED_EVENT) {
		return Sound::MainMenuItemEnteredEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_SELECTED_EVENT) {
		return Sound::MainMenuItemSelectedEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_SCROLLED_EVENT) {
		return Sound::MainMenuItemScrolledEvent;
	}

	return MSFReader::INVALID_SOUND_TYPE;
}

/**
 * Private helper function for finding whether an equals sign was present in the file
 * at the current read position.
 */
bool MSFReader::FoundEqualsSyntax(bool& noEquals, std::string& errorStr, std::istringstream* inStream) {
	char equals = '\0';
	*inStream >> equals;
	if (equals != '=') {
		noEquals = true;
		errorStr = "Misplaced equals ('=') at file keyword in sound defintion block.";
		return false;
	}

	return true;
}