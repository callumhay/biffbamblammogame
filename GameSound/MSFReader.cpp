#include "MSFReader.h"
#include "Sound.h"
#include "GameSound.h"

#include "../ResourceManager.h"
#include "../GameView/GameViewConstants.h"
#include "../BlammoEngine/StringHelper.h"

const char* MSFReader::OPEN_SOUND_DEFINTION_BLOCK			= "{";
const char* MSFReader::CLOSE_SOUND_DEFINITION_BLOCK		= "}";
const char* MSFReader::OPEN_ENCLOSING_PROB_FILE_PAIR	= "(";
const char* MSFReader::CLOSE_ENCLOSING_PROB_FILE_PAIR	= ")";
const char* MSFReader::PROB_DEFINITION_SYNTAX					= ":";

const char* MSFReader::IGNORE_KEYWORD			= "ignore";

const char* MSFReader::FILE_KEYWORD				= "file";
const char* MSFReader::DELAY_KEYWORD			= "delay";
const char* MSFReader::LOOPS_KEYWORD			= "loops";
const char* MSFReader::FADE_IN_KEYWORD		= "fadein";
const char* MSFReader::FADE_OUT_KEYWORD		= "fadeout";

const char* MSFReader::MAIN_MENU_BG_MASK										= "MainMenuBackgroundMask";

const char* MSFReader::MAIN_MENU_ITEM_HIGHLIGHTED_EVENT			= "MainMenuItemHighlightedEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_SMALL_EVENT				= "MainMenuBackgroundBangSmallEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_MEDIUM_EVENT				= "MainMenuBackgroundBangMediumEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_BIG_EVENT					= "MainMenuBackgroundBangBigEvent";
const char* MSFReader::MAIN_MENU_ITEM_ENTERED_EVENT					= "MainMenuItemEnteredEvent";
const char* MSFReader::MAIN_MENU_ITEM_BACK_AND_CANCEL_EVENT	= "MainMenuItemBackAndCancelEvent";
const char* MSFReader::MAIN_MENU_ITEM_VERIFY_AND_SEL_EVENT	= "MainMenuItemVerifyAndSelectEvent";
const char* MSFReader::MAIN_MENU_ITEM_SCROLLED_EVENT				= "MainMenuItemScrolledEvent";

bool MSFReader::ReadMSF(const std::string& filepath, std::map<int, GameSound*>& sounds) {

	// Grab a file in stream from the main manu music script file:
	// in debug mode we load right off disk, in release we load it from the zip file system
#ifdef _DEBUG
	std::ifstream* inStream = new std::ifstream(filepath.c_str());
#else 
	std::istringstream* inStream = ResourceManager::GetInstance()->FilepathToInStream(filepath);
#endif

	if (inStream == NULL) {
		return false;
	}

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
	std::vector<std::string> soundFilePaths;
	std::vector<int> probabilities;
	int delay			= GameSound::DEFAULT_DELAY;
	int loops			= GameSound::DEFAULT_LOOPS;
	int fadein		= GameSound::DEFAULT_FADEIN;
	int fadeout		= GameSound::DEFAULT_FADEOUT;

	// Read through the music script file
	while (*inStream >> currReadStr && !error) {

		// Check for an ignored block defintion...
		if (currReadStr == MSFReader::IGNORE_KEYWORD) {
			// Read the whole block and just discard it
			bool goodSyntax = false;
			while (*inStream >> currReadStr) {
				if (currReadStr == MSFReader::CLOSE_SOUND_DEFINITION_BLOCK) {
					goodSyntax = true;
					break;
				}
			}

			// Make sure that the closing sound definition block bracket was found
			if (!goodSyntax) {
				error = true;
				errorStr = "Ignored sound definition block was never properly closed with a completing bracket: missing '}'.";
				continue;
			}
		}
		// Check for the opening of a music defintion block...
		else if (currReadStr == MSFReader::OPEN_SOUND_DEFINTION_BLOCK) {
			if (soundDefBlockOpen) {
				error = true;
				errorStr = "Tried to open another sound definition block inside an already open block: extra '{' found.";
				continue;
			}
			// Reinitialize everything...
			soundDefBlockOpen = true;
			soundFilePaths.clear();
			probabilities.clear();

			delay			= GameSound::DEFAULT_DELAY;
			loops			= GameSound::DEFAULT_LOOPS;
			fadein		= GameSound::DEFAULT_FADEIN;
			fadeout		= GameSound::DEFAULT_FADEOUT;
		}
		// Check for the closing of a music defintion block...
		else if (currReadStr == MSFReader::CLOSE_SOUND_DEFINITION_BLOCK) {
			if (!soundDefBlockOpen) {
				error = true;
				errorStr = "Tried to close a non-opened sound definition block: extra '}' found.";
				continue;
			}
			assert(soundType != MSFReader::INVALID_SOUND_TYPE);
			assert(lastSoundType != "");
			assert(soundFilePaths.size() > 0);
			
			GameSound* newGameSound = new GameSound(soundIsMask, lastSoundType, probabilities, soundFilePaths);
			
			// Check for validity of the sounds created for the event...
			std::string invalidFile;
			if (!newGameSound->IsValid(invalidFile)) {
				delete newGameSound;
				newGameSound = NULL;
				error = true;
				errorStr = "Invalid sound event created for file: " + invalidFile + ", file path is likely incorrect or file is missing.";
				continue;
			}

			if (!soundIsMask) {
				newGameSound->SetDelay(delay);
				newGameSound->SetLoops(loops);
				newGameSound->SetFadein(fadein);
				newGameSound->SetFadeout(fadeout);
			}
			
			sounds.insert(std::make_pair(soundType, newGameSound));

			// Reset relevant variables for the next music definition block
			soundType = MSFReader::INVALID_SOUND_TYPE;
			soundDefBlockOpen = false;
			lastSoundType = "";
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

				if (currReadStr == MSFReader::FILE_KEYWORD) {
					if (!FoundEqualsSyntax(error, errorStr, inStream)) {
						continue;
					}
					
					// The rest of the line should contain a set of probabilities and filepaths...
					std::string soundFileLine;
					std::getline(*inStream, soundFileLine);
					// Clean up extra whitespace
					soundFileLine = stringhelper::trim(soundFileLine);

					// Read in the (probability : filepath) pairings:
					// Split the line up into the probabilities and files
					std::vector<std::string> tokens;
					stringhelper::Tokenize(soundFileLine, tokens, std::string(MSFReader::OPEN_ENCLOSING_PROB_FILE_PAIR) +
						std::string(MSFReader::CLOSE_ENCLOSING_PROB_FILE_PAIR) + std::string(MSFReader::PROB_DEFINITION_SYNTAX) + std::string("\n\r\t "));
					// Should be pairs of probabilities and file paths...
					if (tokens.size() % 2 != 0) {
						error = true;
						errorStr = "Invalid format of probabilities to filepaths provided on line: " + soundFileLine + ".";
						continue;
					}
					
					bool failed = false;
					std::set<std::string> filesLoaded;
					for (size_t i = 0; i < tokens.size(); i += 2) {
						std::stringstream currProb(tokens[i]);
						
						int probability = 0;
						currProb >> probability;

						// Make sure the probabilities add up to something sensible
						if (probability <= 0 || probability > 5) {
							failed = true;
							errorStr = "Invalid probability value(s) provided on line: " + soundFileLine + ". Probabilities must be in the range [1, 5].";
							break;
						}

						std::string tempFilePath = stringhelper::trim(tokens[i+1]);
						tempFilePath = GameViewConstants::GetInstance()->SOUND_DIR + "/" + tempFilePath;
						
						// Check to see if the file exists...
						std::ifstream test(tempFilePath.c_str(), std::ios::in | std::ios::binary);
						if (!test.good()) {
							errorStr = "Could not open file: " + tempFilePath;
							failed = true;
							break;
						}
						test.close();

						// Make sure there are no duplicate files
						std::pair<std::set<std::string>::iterator, bool> found = filesLoaded.insert(tempFilePath);
						if (!found.second) {
							// The file was already part of the list...
							errorStr = std::string("Duplicate file name found for game sound (") + lastSoundType + std::string(") : ") + tempFilePath;
							failed = true;
							break;
						}

						probabilities.push_back(probability);
						soundFilePaths.push_back(tempFilePath);
					}

					if (failed) {
						probabilities.clear();
						soundFilePaths.clear();
						error = true;
						continue;
					}
				}

				// There are different parameters that are accepted based on the sound type...
				if (!soundIsMask) {

					// We are dealing with an event sound...
					if (currReadStr == MSFReader::DELAY_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

						if(*inStream >> delay) {
							if (delay < 0 || delay > 60000) {
								error = true;
								errorStr = "Invalid delay value found. Delays must be in the range [0, 60000].";
								continue;
							}
						}
						else {
							error = true;
							errorStr = "Could not parse delay value.";
							continue;
						}

					}
					else if (currReadStr == MSFReader::LOOPS_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

						if (*inStream >> loops) {
							if (loops < 1 || loops > 99) {
								error = true;
								errorStr = "Invalid loops value found. The number of loops must be in the range [1, 99].";
								continue;
							}
						}
						else {
							error = true;
							errorStr = "Could not parse loops value.";
							continue;
						}

					}
					else if (currReadStr == MSFReader::FADE_IN_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

						if (*inStream >> fadein) {
							if (fadein < 0) {
								error = true;
								errorStr = "Invalid fadein value found. Fadein must be a value greater than or equal to zero.";
								continue;
							}
						}
						else {
							error = true;
							errorStr = "Could not parse fadein value.";
							continue;
						}

					}
					else if (currReadStr == MSFReader::FADE_OUT_KEYWORD) {
						if (!FoundEqualsSyntax(error, errorStr, inStream)) {
							continue;
						}

						if (*inStream >> fadeout) {
							if (fadeout < 0) {
								error = true;
								errorStr = "Invalid fadeout value found. Fadeout must be a value greater than or equal to zero.";
								continue;
							}
						}
						else {
							error = true;
							errorStr = "Could not parse fadeout value.";
							continue;
						}
					}
				}
			}
			else {
				// Sound definition block has not been opened yet, look for an identifier...
				soundType = MSFReader::ConvertKeywordToSoundType(currReadStr);
				lastSoundType = currReadStr;
				if (soundType != MSFReader::INVALID_SOUND_TYPE) {
					soundIsMask = GameSound::IsSoundMask(soundType);
				}
			}

		}
	}	// while

	// Clean up and return whether there was an error while reading
#ifdef _DEBUG
	inStream->close();
#endif
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
 * for the various game sounds.
 */
int MSFReader::ConvertKeywordToSoundType(const std::string& soundName) {
	if (soundName == MSFReader::MAIN_MENU_BG_MASK) {
		return GameSound::MainMenuBackgroundMask;
	}
	else if (soundName == MSFReader::MAIN_MENU_BG_BANG_SMALL_EVENT) {
		return GameSound::MainMenuBackgroundBangSmallEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_BG_BANG_MEDIUM_EVENT) {
		return GameSound::MainMenuBackgroundBangMediumEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_BG_BANG_BIG_EVENT) {
		return GameSound::MainMenuBackgroundBangBigEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_HIGHLIGHTED_EVENT) {
		return GameSound::MainMenuItemHighlightedEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_ENTERED_EVENT) {
		return GameSound::MainMenuItemEnteredEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_BACK_AND_CANCEL_EVENT) {
		return GameSound::MainMenuItemBackAndCancelEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_VERIFY_AND_SEL_EVENT) {
		return GameSound::MainMenuItemVerifyAndSelectEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_SCROLLED_EVENT) {
		return GameSound::MainMenuItemScrolledEvent;
	}

	return MSFReader::INVALID_SOUND_TYPE;
}

/**
 * Private helper function for finding whether an equals sign was present in the file
 * at the current read position.
 */
bool MSFReader::FoundEqualsSyntax(bool& noEquals, std::string& errorStr, std::istream* inStream) {
	char equals = '\0';
	*inStream >> equals;
	if (equals != '=') {
		noEquals = true;
		errorStr = "Misplaced equals ('=') at file keyword in sound defintion block.";
		return false;
	}

	return true;
}