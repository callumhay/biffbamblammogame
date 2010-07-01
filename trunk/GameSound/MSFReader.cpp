#include "MSFReader.h"
#include "GameSoundAssets.h"
#include "Sound.h"
#include "EventSound.h"
#include "MusicSound.h"

#include "../ResourceManager.h"
#include "../GameView/GameViewConstants.h"
#include "../BlammoEngine/StringHelper.h"

const char* MSFReader::OPEN_SOUND_DEFINTION_BLOCK			= "{";
const char* MSFReader::CLOSE_SOUND_DEFINITION_BLOCK		= "}";
const char* MSFReader::OPEN_ENCLOSING_PROB_FILE_PAIR	= "(";
const char* MSFReader::CLOSE_ENCLOSING_PROB_FILE_PAIR	= ")";
const char* MSFReader::PROB_DEFINITION_SYNTAX					= ":";

const char* MSFReader::IGNORE_KEYWORD			= "ignore";
const char* MSFReader::IGNORE_LINE				= "//";

const char* MSFReader::FILE_KEYWORD				= "file";
const char* MSFReader::LOOPS_KEYWORD			= "loops";
const char* MSFReader::FADE_IN_KEYWORD		= "fadein";
const char* MSFReader::FADE_OUT_KEYWORD		= "fadeout";

const char* MSFReader::MAIN_MENU_BG_MUSIC										= "MainMenuBackgroundMusic";
const char* MSFReader::MAIN_MENU_ITEM_HIGHLIGHTED_EVENT			= "MainMenuItemHighlightedEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_SMALL_EVENT				= "MainMenuBackgroundBangSmallEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_MEDIUM_EVENT				= "MainMenuBackgroundBangMediumEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_BIG_EVENT					= "MainMenuBackgroundBangBigEvent";
const char* MSFReader::MAIN_MENU_ITEM_ENTERED_EVENT					= "MainMenuItemEnteredEvent";
const char* MSFReader::MAIN_MENU_ITEM_BACK_AND_CANCEL_EVENT	= "MainMenuItemBackAndCancelEvent";
const char* MSFReader::MAIN_MENU_ITEM_VERIFY_AND_SEL_EVENT	= "MainMenuItemVerifyAndSelectEvent";
const char* MSFReader::MAIN_MENU_ITEM_SCROLLED_EVENT				= "MainMenuItemScrolledEvent";

const char* MSFReader::WORLD_BG_MUSIC														= "WorldBackgroundMusic";
const char* MSFReader::WORLD_PADDLE_HIT_WALL_EVENT							= "WorldSoundPaddleHitWallEvent";
const char* MSFReader::WORLD_BALL_LOST_EVENT										= "WorldSoundPlayerLostABallEvent";
const char* MSFReader::WORLD_LAST_BALL_EXPLODED_EVENT						= "WorldSoundPlayerLostABallEvent";
const char* MSFReader::WORLD_BALL_SPAWN_EVENT										= "WorldSoundBallSpawnEvent";
const char* MSFReader::WORLD_BALL_BLOCK_COLLISION_EVENT					= "WorldSoundBallBlockCollisionEvent";
const char* MSFReader::WORLD_BALL_PADDLE_COLLISION_EVENT				= "WorldSoundBallPaddleCollisionEvent";
const char* MSFReader::WORLD_STICKY_BALL_PADDLE_COLLISION_EVENT	= "WorldSoundStickyBallPaddleCollisionEvent";
const char* MSFReader::WORLD_BALL_BALL_COLLISION_EVENT					= "WorldSoundBallBallCollisionEvent";
const char* MSFReader::WORLD_BOMB_BLOCK_DESTROYED_EVENT					= "WorldSoundBombBlockDestroyedEvent";
const char* MSFReader::WORLD_INK_BLOCK_DESTROYED_EVENT					= "WorldSoundInkBlockDestroyedEvent";
const char* MSFReader::WORLD_BASIC_BLOCK_DESTROYED_EVENT				= "WorldSoundBasicBlockDestroyedEvent";
const char* MSFReader::WORLD_COLLATERAL_BLOCK_DESTROYED_EVENT		= "WorldSoundCollateralBlockDestroyedEvent";
const char* MSFReader::WORLD_CANNON_BLOCK_LOADED_EVENT					= "WorldSoundCannonBlockLoadedEvent";
const char* MSFReader::WORLD_PORTAL_TELEPORT_EVENT							= "WorldSoundPortalTeleportEvent";
const char* MSFReader::WORLD_SAFETY_NET_CREATED_EVENT						= "WorldSoundBallSafetyNetCreatedEvent";
const char* MSFReader::WORLD_SAFETY_NET_DESTROYED_EVENT					= "WorldSoundBallSafetyNetDestroyedEvent";
const char* MSFReader::WORLD_ITEM_SPAWNED_EVENT									= "WorldSoundItemSpawnedEvent";
const char* MSFReader::WORLD_POWER_UP_ACTIVATED_EVENT						= "WorldSoundPowerUpItemActivatedEvent";
const char* MSFReader::WORLD_POWER_NEUTRAL_ACTIVATED_EVENT			= "WorldSoundPowerNeutralItemActivatedEvent";
const char* MSFReader::WORLD_POWER_DOWN_ACTIVATED_EVENT					= "WorldSoundPowerDownItemActivatedEvent";
const char* MSFReader::WORLD_POWER_UP_TIMER_END_EVENT						= "WorldSoundPowerUpItemTimerEndsEvent";
const char* MSFReader::WORLD_POWER_NEUTRAL_TIMER_END_EVENT			= "WorldSoundPowerNeutralItemTimerEndsEvent";
const char* MSFReader::WORLD_POWER_DOWN_TIMER_END_EVENT					= "WorldSoundPowerDownItemTimerEndsEvent";
const char* MSFReader::WORLD_BALL_OR_PADDLE_GROW_EVENT					= "WorldSoundBallOrPaddleGrowEvent";
const char* MSFReader::WORLD_BALL_OR_PADDLE_SHRINK_EVENT				= "WorldSoundBallOrPaddleShrinkEvent";
const char* MSFReader::WORLD_LASER_BULLET_SHOT_EVENT						= "WorldSoundLaserBulletShotEvent";
const char* MSFReader::WORLD_LEVEL_COMPLETED_EVENT							= "WorldSoundLevelCompletedEvent";
const char* MSFReader::WORLD_WORLD_COMPLETED_EVENT							= "WorldSoundWorldCompletedEvent";
const char* MSFReader::WORLD_INK_SPLATTER_MASK									= "WorldSoundInkSplatterMask";
const char* MSFReader::WORLD_POISON_SICK_MASK										= "WorldSoundPoisonSicknessMask";
const char* MSFReader::WORLD_LASER_BULLET_MOVING_MASK						= "WorldSoundLaserBulletMovingMask";
const char* MSFReader::WORLD_ROCKET_MOVING_MASK									= "WorldSoundRocketMovingMask";
const char* MSFReader::WORLD_LASER_BEAM_FIRING_MASK							= "WorldSoundLaserBeamFiringMask";
const char* MSFReader::WORLD_COLLATERAL_BLOCK_FLASHING_MASK			= "WorldSoundCollateralBlockFlashingMask";
const char* MSFReader::WORLD_COLLATERAL_BLOCK_FALLING_MASK			= "WorldSoundCollateralBlockFallingMask";
const char* MSFReader::WORLD_CANNON_BLOCK_ROTATING_MASK					= "WorldSoundCannonBlockRotatingMask";

bool MSFReader::ReadMSF(const std::string& filepath, std::map<int, Sound*>& sounds) {

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
	Sound::SoundType soundType = Sound::EventSound;
	std::string lastSoundType = "";

	int soundAsset = MSFReader::INVALID_SOUND_TYPE;
	std::vector<std::string> soundFilePaths;
	std::vector<int> probabilities;
	int loops			= EventSound::DEFAULT_LOOPS;
	int fadein		= EventSound::DEFAULT_FADEIN;
	int fadeout		= EventSound::DEFAULT_FADEOUT;

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

			loops			= EventSound::DEFAULT_LOOPS;
			fadein		= EventSound::DEFAULT_FADEIN;
			fadeout		= EventSound::DEFAULT_FADEOUT;
		}
		// Check for the closing of a music defintion block...
		else if (currReadStr == MSFReader::CLOSE_SOUND_DEFINITION_BLOCK) {
			if (!soundDefBlockOpen) {
				error = true;
				errorStr = "Tried to close a non-opened sound definition block: extra '}' found.";
				continue;
			}
			assert(soundAsset != MSFReader::INVALID_SOUND_TYPE);
			assert(lastSoundType != "");
			assert(soundFilePaths.size() > 0);
			
			// TODO 
			// GameSound* newGameSound = new GameSound(soundIsMask, lastSoundType, probabilities, soundFilePaths);
			Sound* newGameSound = NULL;
			switch (soundType) {
				case Sound::MaskSound:
					newGameSound = EventSound::BuildSoundMask(lastSoundType, soundFilePaths.front(), fadein, fadeout);
					break;
				case Sound::EventSound:
					newGameSound = EventSound::BuildSoundEvent(lastSoundType, loops, fadein, fadeout, probabilities, soundFilePaths);
					break;
				case Sound::MusicSound:
					newGameSound = MusicSound::BuildMusicSound(lastSoundType, soundFilePaths.front(), fadein, fadeout);
					break;
				default:
					assert(false);
					break;
			}

			if (newGameSound == NULL) {
				error = true;
				errorStr = "Error loading sound " + lastSoundType + ".";
				continue;				
			}
			assert(newGameSound->IsValid());

			sounds.insert(std::make_pair(soundAsset, newGameSound));

			// Reset relevant variables for the next music definition block
			soundAsset = MSFReader::INVALID_SOUND_TYPE;
			soundDefBlockOpen = false;
			lastSoundType = "";
		}
		else {
			// Check to see if the sound definition block has been opened, if it has we will search for
			// the various parameters allowed for it, if not then we search for the keyword associated with it

			if (soundDefBlockOpen) {
				// Make sure that the sound type has already been defined, otherwise the script file
				// is invalid and we should get out of here
				if (soundAsset == MSFReader::INVALID_SOUND_TYPE) {
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
				
				// There are different parameters that are accepted based on the sound type...
				if (soundType == Sound::EventSound) {
					// We are dealing with an event sound...
					if (currReadStr == MSFReader::LOOPS_KEYWORD) {
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
				}
			}
			else {
				// Sound definition block has not been opened yet, look for an identifier...
				soundAsset = MSFReader::ConvertKeywordToSoundType(currReadStr);
				lastSoundType = currReadStr;
				if (soundAsset == MSFReader::INVALID_SOUND_TYPE){
					error = true;
					errorStr = "Invalid sound type: " + currReadStr;
					continue;
				}
				else if (GameSoundAssets::IsMaskSound(soundAsset)) {
					soundType = Sound::MaskSound;
				}
				else if (GameSoundAssets::IsMusicSound(soundAsset)) {
					soundType = Sound::MusicSound;
				}
				else {
					soundType = Sound::EventSound;
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
	if (soundName == MSFReader::MAIN_MENU_BG_MUSIC) {
		return GameSoundAssets::MainMenuBackgroundMusic;
	}
	else if (soundName == MSFReader::MAIN_MENU_BG_BANG_SMALL_EVENT) {
		return GameSoundAssets::MainMenuBackgroundBangSmallEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_BG_BANG_MEDIUM_EVENT) {
		return GameSoundAssets::MainMenuBackgroundBangMediumEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_BG_BANG_BIG_EVENT) {
		return GameSoundAssets::MainMenuBackgroundBangBigEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_HIGHLIGHTED_EVENT) {
		return GameSoundAssets::MainMenuItemHighlightedEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_ENTERED_EVENT) {
		return GameSoundAssets::MainMenuItemEnteredEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_BACK_AND_CANCEL_EVENT) {
		return GameSoundAssets::MainMenuItemBackAndCancelEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_VERIFY_AND_SEL_EVENT) {
		return GameSoundAssets::MainMenuItemVerifyAndSelectEvent;
	}
	else if (soundName == MSFReader::MAIN_MENU_ITEM_SCROLLED_EVENT) {
		return GameSoundAssets::MainMenuItemScrolledEvent;
	}
	else if (soundName == MSFReader::WORLD_BG_MUSIC) {
		return GameSoundAssets::WorldBackgroundMusic;
	}
	else if (soundName == MSFReader::WORLD_PADDLE_HIT_WALL_EVENT) {
		return GameSoundAssets::WorldSoundPaddleHitWallEvent;
	}
	else if (soundName == MSFReader::WORLD_BALL_LOST_EVENT) {
		return GameSoundAssets::WorldSoundPlayerLostABallEvent;
	}
	else if (soundName == MSFReader::WORLD_LAST_BALL_EXPLODED_EVENT) {
		return GameSoundAssets::WorldSoundLastBallExplodedEvent;
	}
	else if (soundName == MSFReader::WORLD_BALL_SPAWN_EVENT) {
		return GameSoundAssets::WorldSoundBallSpawnEvent;
	}
	else if (soundName == MSFReader::WORLD_BALL_BLOCK_COLLISION_EVENT) {
		return GameSoundAssets::WorldSoundBallBlockCollisionEvent;
	}
	else if (soundName == MSFReader::WORLD_BALL_PADDLE_COLLISION_EVENT) {
		return GameSoundAssets::WorldSoundBallPaddleCollisionEvent;
	}
	else if (soundName == MSFReader::WORLD_STICKY_BALL_PADDLE_COLLISION_EVENT) {
		return GameSoundAssets::WorldSoundStickyBallPaddleCollisionEvent;
	}
	else if (soundName == MSFReader::WORLD_BALL_BALL_COLLISION_EVENT) {
		return GameSoundAssets::WorldSoundBallBallCollisionEvent;
	}
	else if (soundName == MSFReader::WORLD_BOMB_BLOCK_DESTROYED_EVENT) {
		return GameSoundAssets::WorldSoundBombBlockDestroyedEvent;
	}
	else if (soundName == MSFReader::WORLD_INK_BLOCK_DESTROYED_EVENT) {
		return GameSoundAssets::WorldSoundInkBlockDestroyedEvent;
	}
	else if (soundName == MSFReader::WORLD_BASIC_BLOCK_DESTROYED_EVENT) {
		return GameSoundAssets::WorldSoundBasicBlockDestroyedEvent;
	}
	else if (soundName == MSFReader::WORLD_COLLATERAL_BLOCK_DESTROYED_EVENT) {
		return GameSoundAssets::WorldSoundCollateralBlockDestroyedEvent;
	}
	else if (soundName == MSFReader::WORLD_CANNON_BLOCK_LOADED_EVENT) {
		return GameSoundAssets::WorldSoundCannonBlockLoadedEvent;
	}
	else if (soundName == MSFReader::WORLD_PORTAL_TELEPORT_EVENT) {
		return GameSoundAssets::WorldSoundPortalTeleportEvent;
	}
	else if (soundName == MSFReader::WORLD_SAFETY_NET_CREATED_EVENT) {
		return GameSoundAssets::WorldSoundBallSafetyNetCreatedEvent;
	}
	else if (soundName == MSFReader::WORLD_SAFETY_NET_DESTROYED_EVENT) {
		return GameSoundAssets::WorldSoundBallSafetyNetDestroyedEvent;
	}
	else if (soundName == MSFReader::WORLD_ITEM_SPAWNED_EVENT) {
		return GameSoundAssets::WorldSoundItemSpawnedEvent;
	}
	else if (soundName == MSFReader::WORLD_POWER_UP_ACTIVATED_EVENT) {
		return GameSoundAssets::WorldSoundPowerUpItemActivatedEvent;
	}
	else if (soundName == MSFReader::WORLD_POWER_NEUTRAL_ACTIVATED_EVENT) {
		return GameSoundAssets::WorldSoundPowerNeutralItemActivatedEvent;
	}
	else if (soundName == MSFReader::WORLD_POWER_DOWN_ACTIVATED_EVENT) {
		return GameSoundAssets::WorldSoundPowerDownItemActivatedEvent;
	}
	else if (soundName == MSFReader::WORLD_POWER_UP_TIMER_END_EVENT) {
		return GameSoundAssets::WorldSoundPowerUpItemTimerEndsEvent;
	}
	else if (soundName == MSFReader::WORLD_POWER_NEUTRAL_TIMER_END_EVENT) {
		return GameSoundAssets::WorldSoundPowerNeutralItemTimerEndsEvent;
	}
	else if (soundName == MSFReader::WORLD_POWER_DOWN_TIMER_END_EVENT) {
		return GameSoundAssets::WorldSoundPowerDownItemTimerEndsEvent;
	}
	else if (soundName ==MSFReader::WORLD_BALL_OR_PADDLE_GROW_EVENT) {
		return GameSoundAssets::WorldSoundBallOrPaddleGrowEvent;
	}
	else if (soundName ==MSFReader::WORLD_BALL_OR_PADDLE_SHRINK_EVENT) {
		return GameSoundAssets::WorldSoundBallOrPaddleShrinkEvent;
	}
	else if (soundName ==MSFReader::WORLD_LASER_BULLET_SHOT_EVENT) {
		return GameSoundAssets::WorldSoundLaserBulletShotEvent;
	}
	else if (soundName ==MSFReader::WORLD_LEVEL_COMPLETED_EVENT) {
		return GameSoundAssets::WorldSoundLevelCompletedEvent;
	}
	else if (soundName ==MSFReader::WORLD_WORLD_COMPLETED_EVENT) {
		return GameSoundAssets::WorldSoundWorldCompletedEvent;
	}
	else if (soundName ==MSFReader::WORLD_INK_SPLATTER_MASK) {
		return GameSoundAssets::WorldSoundInkSplatterMask;
	}			
	else if (soundName ==MSFReader::WORLD_POISON_SICK_MASK) {
		return GameSoundAssets::WorldSoundPoisonSicknessMask;
	}	
	else if (soundName ==MSFReader::WORLD_LASER_BULLET_MOVING_MASK) {
		return GameSoundAssets::WorldSoundLaserBulletMovingMask;
	}
	else if (soundName ==MSFReader::WORLD_ROCKET_MOVING_MASK) {
		return GameSoundAssets::WorldSoundRocketMovingMask;
	}
	else if (soundName ==MSFReader::WORLD_LASER_BEAM_FIRING_MASK) {
		return GameSoundAssets::WorldSoundLaserBeamFiringMask;
	}
	else if (soundName ==MSFReader::WORLD_COLLATERAL_BLOCK_FLASHING_MASK) {
		return GameSoundAssets::WorldSoundCollateralBlockFlashingMask;
	}
	else if (soundName ==MSFReader::WORLD_COLLATERAL_BLOCK_FALLING_MASK) {
		return GameSoundAssets::WorldSoundCollateralBlockFallingMask;
	}
	else if (soundName ==MSFReader::WORLD_CANNON_BLOCK_ROTATING_MASK) {
		return GameSoundAssets::WorldSoundCannonBlockRotatingMask;
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