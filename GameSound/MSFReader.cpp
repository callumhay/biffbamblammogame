/**
 * MSFReader.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MSFReader.h"
#include "GameSound.h"
#include "SoundSource.h"

#include "../ResourceManager.h"
#include "../GameView/GameViewConstants.h"
#include "../BlammoEngine/StringHelper.h"

const char* MSFReader::OPEN_SOUND_DEFINTION_BLOCK     = "{";
const char* MSFReader::CLOSE_SOUND_DEFINITION_BLOCK   = "}";
const char* MSFReader::OPEN_ENCLOSING_PROB_FILE_PAIR	= "(";
const char* MSFReader::CLOSE_ENCLOSING_PROB_FILE_PAIR	= ")";
const char* MSFReader::PROB_DEFINITION_SYNTAX         = ":";
const char* MSFReader::OPEN_TIME_SEQUENCE_DEFINITION  = "[";
const char* MSFReader::CLOSE_TIME_SEQUENCE_DEFINITION = "]";

const char* MSFReader::IGNORE_KEYWORD = "ignore";
const char* MSFReader::IGNORE_LINE    = "//";
const char* MSFReader::FILE_KEYWORD   = "file";
const char* MSFReader::WORLD_KEYWORD  = "world";

const char* MSFReader::NO_BLOCK_NAME = "";

std::map<std::string, GameSound::SoundType>  MSFReader::soundTypeMapping;
std::map<std::string, GameSound::EffectType> MSFReader::effectTypeMapping;

bool MSFReader::ReadMSF(GameSound& gameSound, const std::string& filepath) {

	// Intialize the mapping from strings to enums for all the possible game sounds and effects
    MSFReader::InitSoundTypeMapping();
    MSFReader::InitEffectTypeMapping();

	// Grab a file in stream from the main manu music script file:
	// in debug mode we load right off disk, in release we load it from the zip file system
	std::istringstream* inStream = ResourceManager::GetInstance()->FilepathToInStream(filepath);
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
	std::string lastBlockName = MSFReader::NO_BLOCK_NAME;
    GameWorld::WorldStyle worldSoundStyle = GameWorld::None;
    GameSound::EffectType lastEffectEnum  = GameSound::NoEffect;
    GameSound::SoundType lastSoundEnum    = GameSound::NoSound;
	std::vector<std::string> soundFilePaths;
	std::vector<int> probabilities;
	double resetSequenceTime = -1;

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
		else if (currReadStr == MSFReader::IGNORE_LINE) {
			std::getline(*inStream, currReadStr);
			continue;
		}	
		// Check for the opening of a sound defintion block...
		else if (currReadStr == MSFReader::OPEN_SOUND_DEFINTION_BLOCK) {
			if (soundDefBlockOpen) {
				error = true;
				errorStr = "Tried to open another sound definition block inside an already open block: extra '{' found.";
				continue;
			}
            soundDefBlockOpen = true;

			// Reinitialize all data that can be contained in a sound/effect definition block...
            worldSoundStyle = GameWorld::None;
			soundFilePaths.clear();
			probabilities.clear();

		}
		// Check for the closing of a music defintion block...
		else if (currReadStr == MSFReader::CLOSE_SOUND_DEFINITION_BLOCK) {

			// If there is was no open block to begin with then the file is poorly formatted...
            if (!soundDefBlockOpen) {
				error = true;
				errorStr = "Tried to close a non-opened sound definition block: extra '}' found.";
				continue;
			}
            if (lastBlockName == MSFReader::NO_BLOCK_NAME) {
                error = true;
                errorStr = "No block name found for sound/effect.";
                continue;
            }

            if (lastEffectEnum != GameSound::NoEffect) {
                //SoundEffect* newSoundEffect = new SoundEffect(lastBlockName, ...);
                assert(false);
                //gameSound.globalEffects.insert(std::make_pair(lastEffectEnum, newSoundEffect));
            }
            else {
                // If it's not a sound effect then it must be a sound
			    
                // Make sure there is a sound with associated file name to load
                assert(lastSoundEnum != GameSound::NoSound);
                assert(!soundFilePaths.empty());

                SoundSource* newSoundSrc = gameSound.BuildSoundSource(lastSoundEnum, lastBlockName, soundFilePaths.front());//probabilities, soundFilePaths);
			    assert(newSoundSrc != NULL);
            
                if (worldSoundStyle != GameWorld::None) {
                    // The sound is specific to a world style, add it to that mapping
                    gameSound.worldSounds[worldSoundStyle].insert(std::make_pair(lastSoundEnum, newSoundSrc));
                }
                else {
                    // The sound is a global sound, add it to that mapping
                    assert(gameSound.globalSounds.find(lastSoundEnum) == gameSound.globalSounds.end());
                    gameSound.globalSounds.insert(std::make_pair(lastSoundEnum, newSoundSrc));
                }
            }

            // We're no longer in a sound definition block...
            soundDefBlockOpen = false;

			// Reset relevant variables for the next sound/effect definition block
            worldSoundStyle = GameWorld::None;
            lastEffectEnum  = GameSound::NoEffect;
            lastSoundEnum   = GameSound::NoSound;
            lastBlockName   = MSFReader::NO_BLOCK_NAME;
			resetSequenceTime = -1;
		}
		else {
			// Check to see if the sound definition block has been opened, if it has we will search for
			// the various parameters allowed for it, if not then we search for the keyword associated with it

            if (soundDefBlockOpen) {
                if (currReadStr == MSFReader::FILE_KEYWORD) {
					if (!FoundEqualsSyntax(error, errorStr, inStream)) {
						continue;
					}

					// The rest of the line should contain a set of probabilities and filepaths or just a single filepath
					//... or it should contain a sequence definition...
					std::string soundFileLine;
					std::getline(*inStream, soundFileLine);

					// Clean up extra whitespace
					soundFileLine = stringhelper::trim(soundFileLine);
					if (soundFileLine.find("(") == std::string::npos && soundFileLine.find("[") == std::string::npos) {
						// If we're here then there is just a single file provided...
						soundFileLine = std::string(GameViewConstants::GetInstance()->SOUND_DIR) + "/" + soundFileLine;
						probabilities.push_back(1);
						soundFilePaths.push_back(soundFileLine);
					}
					else if (soundFileLine.find("[") != std::string::npos) {
						// If we're here then a time sequence was provided, it takes the form of:
						// [<time_before_sequence_reset> <relative_filepath_1>  <relative_filepath_2> ...  <relative_filepath_n>]

						std::vector<std::string> tokens;
						stringhelper::Tokenize(soundFileLine, tokens, std::string(MSFReader::OPEN_TIME_SEQUENCE_DEFINITION) +
							std::string(MSFReader::CLOSE_TIME_SEQUENCE_DEFINITION) + std::string("\n\r\t "));


						bool failed = false;
						for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter) {
							std::stringstream currTokenStream(*iter);
							if (!(currTokenStream >> resetSequenceTime)) {
								failed = true;
								errorStr = std::string("Invalid format of time sequence, first parameter must be a time value in seconds. ") +
									std::string("Error found on line: ") + soundFileLine + std::string(".");
								break;
							}

							std::string currFilepath;
							while (currTokenStream >> currFilepath) {
								soundFilePaths.push_back(currFilepath);
							}
						}

						if (failed) {
							resetSequenceTime = -1;
							soundFilePaths.clear();
							error = true;
							continue;
					 }
					}
					else {
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
							if (probability <= 0 || probability > 100) {
								failed = true;
								errorStr = "Invalid probability value(s) provided on line: " + soundFileLine + ". Probabilities must be in the range [1, 100].";
								break;
							}

							std::string tempFilePath = stringhelper::trim(tokens[i+1]);
							tempFilePath = std::string(GameViewConstants::GetInstance()->SOUND_DIR) + "/" + tempFilePath;

							// Make sure there are no duplicate files
							std::pair<std::set<std::string>::iterator, bool> found = filesLoaded.insert(tempFilePath);
							if (!found.second) {
								// The file was already part of the list...
								errorStr = std::string("Duplicate file name found for game sound (") + lastBlockName + std::string(") : ") + tempFilePath;
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
				}
                else if (currReadStr == MSFReader::WORLD_KEYWORD) {
                    if (!FoundEqualsSyntax(error, errorStr, inStream)) {
						continue;
					}

					// The rest of the line should contain the name of the world, clean up whitespace
					std::string worldName;
					std::getline(*inStream, worldName);
					worldName = stringhelper::trim(worldName);

                    // Convert to the world style
                    worldSoundStyle = GameWorld::GetWorldStyleFromString(worldName);
                    if (worldSoundStyle == GameWorld::None) {
                        errorStr = "Invalid world style (" + worldName + ") specified for sound " + lastBlockName;
					    error = true;
					    continue;
                    }
                }
			}
			else {

				// Sound definition block has not been opened yet, look for an identifier for either a sound or effect...
				lastSoundEnum = MSFReader::ConvertKeywordToSoundType(currReadStr);
				lastBlockName = currReadStr;

                if (lastSoundEnum == GameSound::NoSound) {
                    lastEffectEnum = MSFReader::ConvertKeywordToEffectType(currReadStr);
                    if (lastEffectEnum == GameSound::NoEffect) {
                        error = true;
					    errorStr = "Invalid sound/effect type: " + currReadStr;
					    continue;
                    }
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

#define MAPPING_PAIR(Class, EnumVal) std::make_pair(STRINGIFY(EnumVal), Class::EnumVal)

void MSFReader::InitSoundTypeMapping() {
	// Check to see if the mapping has already been initialized
	if (!soundTypeMapping.empty()) {
		return;
	}


	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MainMenuBackgroundLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemChangedSelectionEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemEnteredEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemCancelEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemVerifyAndSelectEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuSelectionItemScrolledEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuScrollerItemScrolledEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuOpenSubMenuWindowEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuCloseSubMenuWindowEvent));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaMenuBackgroundLoop));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldMenuBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldMenuItemChangedSelectionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldMenuItemSelectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldMenuItemLockedEvent));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuItemChangedSelectionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuPageChangedSelectionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuItemSelectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuItemLockedEvent));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, InGameMenuOpened));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, InGameMenuClosed));
    

	soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldBackgroundLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleHitWallEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PlayerLostABallButIsStillAliveEvent));

	soundTypeMapping.insert(MAPPING_PAIR(GameSound, LastBallExplodedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallSpawnEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallBlockCollisionEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallPaddleCollisionEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, StickyBallPaddleCollisionEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallBallCollisionEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BombBlockDestroyedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, InkBlockDestroyedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BasicBlockDestroyedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CannonBlockLoadedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CannonBlockFiredEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PortalTeleportEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, RocketExplodedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallSafetyNetCreatedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallSafetyNetDestroyedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, ItemSpawnedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerUpItemActivatedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerNeutralItemActivatedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerDownItemActivatedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerUpItemTimerEndsEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerNeutralItemTimerEndsEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerDownItemTimerEndsEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallOrPaddleGrowEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallOrPaddleShrinkEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, LaserBulletShotEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelCompletedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldCompletedEvent));

	soundTypeMapping.insert(MAPPING_PAIR(GameSound, LaserBulletMovingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, RocketMovingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, LaserBeamFiringLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CollateralBlockFlashingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CollateralBlockFallingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CannonBlockRotatingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, LastBallSpiralingToDeathLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, ItemMovingLoop));
}

void MSFReader::InitEffectTypeMapping() {
    if (!effectTypeMapping.empty()) {
        return;
    }

	effectTypeMapping.insert(MAPPING_PAIR(GameSound, InkSplatterEffect));
	effectTypeMapping.insert(MAPPING_PAIR(GameSound, PoisonEffect));
}

/**
* Converts the given music script file keyword sound name into an enumerated sound type
* for the various game sounds.
*/
GameSound::SoundType MSFReader::ConvertKeywordToSoundType(const std::string& soundName) {
	std::map<std::string, GameSound::SoundType>::iterator findIter = soundTypeMapping.find(soundName);
	if (findIter == soundTypeMapping.end()) {
        return GameSound::NoSound;
	}

	return findIter->second;
}
GameSound::EffectType MSFReader::ConvertKeywordToEffectType(const std::string& effectName) {
    std::map<std::string, GameSound::EffectType>::iterator findIter = effectTypeMapping.find(effectName);
    if (findIter == effectTypeMapping.end()) {
        return GameSound::NoEffect;
    }

    return findIter->second;
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