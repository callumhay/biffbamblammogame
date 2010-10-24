#include "MSFReader.h"
#include "GameSoundAssets.h"
#include "Sound.h"
#include "EventSound.h"
#include "MusicSound.h"

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

const char* MSFReader::IGNORE_KEYWORD			= "ignore";
const char* MSFReader::IGNORE_LINE				= "//";

const char* MSFReader::FILE_KEYWORD				= "file";
const char* MSFReader::LOOPS_KEYWORD			= "loops";
const char* MSFReader::FADE_IN_KEYWORD		= "fadein";
const char* MSFReader::FADE_OUT_KEYWORD		= "fadeout";

std::map<std::string, int> MSFReader::soundTypeMapping;

const char* MSFReader::MAIN_MENU_BG_MUSIC                   = "MainMenuBackgroundMusic";
const char* MSFReader::MAIN_MENU_ITEM_CHANGED_EVENT         = "MainMenuItemChangedEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_SMALL_EVENT        = "MainMenuBackgroundBangSmallEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_MEDIUM_EVENT       = "MainMenuBackgroundBangMediumEvent";
const char* MSFReader::MAIN_MENU_BG_BANG_BIG_EVENT          = "MainMenuBackgroundBangBigEvent";
const char* MSFReader::MAIN_MENU_ITEM_ENTERED_EVENT         = "MainMenuItemEnteredEvent";
const char* MSFReader::MAIN_MENU_ITEM_BACK_AND_CANCEL_EVENT = "MainMenuItemBackAndCancelEvent";
const char* MSFReader::MAIN_MENU_ITEM_VERIFY_AND_SEL_EVENT  = "MainMenuItemVerifyAndSelectEvent";
const char* MSFReader::MAIN_MENU_ITEM_SCROLLED_EVENT        = "MainMenuItemScrolledEvent";

const char* MSFReader::WORLD_BG_MUSIC														= "WorldBackgroundMusic";
const char* MSFReader::WORLD_PADDLE_HIT_WALL_EVENT							= "WorldSoundPaddleHitWallEvent";
const char* MSFReader::WORLD_BALL_LOST_EVENT										= "WorldSoundPlayerLostABallEvent";
const char* MSFReader::WORLD_LAST_BALL_EXPLODED_EVENT						= "WorldSoundLastBallExplodedEvent";
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
const char* MSFReader::WORLD_CANNON_BLOCK_FIRED_EVENT						= "WorldSoundCannonBlockFiredEvent";
const char* MSFReader::WORLD_PORTAL_TELEPORT_EVENT							= "WorldSoundPortalTeleportEvent";
const char* MSFReader::WORLD_ROCKET_EXPLODED_EVENT							= "WorldSoundRocketExplodedEvent";
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
const char* MSFReader::WORLD_LASER_BEAM_FIRING_MASK               = "WorldSoundLaserBeamFiringMask";
const char* MSFReader::WORLD_COLLATERAL_BLOCK_FLASHING_MASK			= "WorldSoundCollateralBlockFlashingMask";
const char* MSFReader::WORLD_COLLATERAL_BLOCK_FALLING_MASK			= "WorldSoundCollateralBlockFallingMask";
const char* MSFReader::WORLD_CANNON_BLOCK_ROTATING_MASK	         = "WorldSoundCannonBlockRotatingMask";
const char* MSFReader::WORLD_LAST_BALL_SPIRAL_TO_DEATH_MASK			= "WorldSoundLastBallSpiralingToDeathMask";
const char* MSFReader::WORLD_ITEM_MOVING_MASK                     = "WorldSoundItemMovingMask";


bool MSFReader::ReadMSF(const std::string& filepath, std::map<int, Sound*>& sounds) {
	MSFReader::InitSoundTypeMapping();
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
	Sound::SoundType soundType = Sound::EventSound;
	std::string lastSoundType = "";

	int soundAsset = MSFReader::INVALID_SOUND_TYPE;
	std::vector<std::string> soundFilePaths;
	std::vector<int> probabilities;
	int loops		= EventSound::DEFAULT_LOOPS;
	int fadein		= EventSound::DEFAULT_FADEIN;
	int fadeout		= EventSound::DEFAULT_FADEOUT;
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
				case Sound::EventSound: {
					if (resetSequenceTime == -1) {
						newGameSound = EventSound::BuildProbabilitySoundEvent(lastSoundType, loops, fadein, fadeout, probabilities, soundFilePaths);
					}
					else {
						newGameSound = EventSound::BuildSequenceSoundEvent(lastSoundType, loops, fadein, fadeout, resetSequenceTime, soundFilePaths);
					}
																}
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
			resetSequenceTime = -1;
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

					// The rest of the line should contain a set of probabilities and filepaths or just a single filepath
					//... or it should contain a sequence definition...
					std::string soundFileLine;
					std::getline(*inStream, soundFileLine);
					// Clean up extra whitespace
					soundFileLine = stringhelper::trim(soundFileLine);

					if (soundFileLine.find("(") == std::string::npos && soundFileLine.find("[") == std::string::npos) {
						// If we're here then there is just a single file provided...
						soundFileLine = std::string(GameViewConstants::GetInstance()->SOUND_DIR) + "/" + soundFileLine;

						// Check to see if the file exists...
						std::ifstream test(soundFileLine.c_str(), std::ios::in | std::ios::binary);
						if (!test.good()) {
							errorStr = "Could not open file: " + soundFileLine;
							error = true;
							continue;
						}
						test.close();

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
							if (probability <= 0 || probability > 5) {
								failed = true;
								errorStr = "Invalid probability value(s) provided on line: " + soundFileLine + ". Probabilities must be in the range [1, 5].";
								break;
							}

							std::string tempFilePath = stringhelper::trim(tokens[i+1]);
							tempFilePath = std::string(GameViewConstants::GetInstance()->SOUND_DIR) + "/" + tempFilePath;

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
	delete inStream;
	inStream = NULL;

	// If there was an error then say so
	if (error) {
		std::cerr << errorStr << std::endl;
	}

	return !error;
}

void MSFReader::InitSoundTypeMapping() {
	// Check to see if the mapping has already been initialized
	if (!soundTypeMapping.empty()) {
		return;
	}

	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_BG_MUSIC, GameSoundAssets::MainMenuBackgroundMusic));
	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_BG_BANG_SMALL_EVENT, GameSoundAssets::MainMenuBackgroundBangSmallEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_BG_BANG_MEDIUM_EVENT, GameSoundAssets::MainMenuBackgroundBangMediumEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_BG_BANG_BIG_EVENT, GameSoundAssets::MainMenuBackgroundBangBigEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_ITEM_CHANGED_EVENT, GameSoundAssets::MainMenuItemChangedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_ITEM_ENTERED_EVENT, GameSoundAssets::MainMenuItemEnteredEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_ITEM_BACK_AND_CANCEL_EVENT, GameSoundAssets::MainMenuItemBackAndCancelEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_ITEM_VERIFY_AND_SEL_EVENT, GameSoundAssets::MainMenuItemVerifyAndSelectEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::MAIN_MENU_ITEM_SCROLLED_EVENT, GameSoundAssets::MainMenuItemScrolledEvent));

	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BG_MUSIC, GameSoundAssets::WorldBackgroundMusic));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_PADDLE_HIT_WALL_EVENT, GameSoundAssets::WorldSoundPaddleHitWallEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BALL_LOST_EVENT, GameSoundAssets::WorldSoundPlayerLostABallEvent));

	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_LAST_BALL_EXPLODED_EVENT, GameSoundAssets::WorldSoundLastBallExplodedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BALL_SPAWN_EVENT, GameSoundAssets::WorldSoundBallSpawnEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BALL_BLOCK_COLLISION_EVENT, GameSoundAssets::WorldSoundBallBlockCollisionEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BALL_PADDLE_COLLISION_EVENT, GameSoundAssets::WorldSoundBallPaddleCollisionEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_STICKY_BALL_PADDLE_COLLISION_EVENT, GameSoundAssets::WorldSoundStickyBallPaddleCollisionEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BALL_BALL_COLLISION_EVENT, GameSoundAssets::WorldSoundBallBallCollisionEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BOMB_BLOCK_DESTROYED_EVENT, GameSoundAssets::WorldSoundBombBlockDestroyedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_INK_BLOCK_DESTROYED_EVENT, GameSoundAssets::WorldSoundInkBlockDestroyedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BASIC_BLOCK_DESTROYED_EVENT, GameSoundAssets::WorldSoundBasicBlockDestroyedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_COLLATERAL_BLOCK_DESTROYED_EVENT, GameSoundAssets::WorldSoundCollateralBlockDestroyedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_CANNON_BLOCK_LOADED_EVENT, GameSoundAssets::WorldSoundCannonBlockLoadedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_CANNON_BLOCK_FIRED_EVENT, GameSoundAssets::WorldSoundCannonBlockFiredEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_PORTAL_TELEPORT_EVENT, GameSoundAssets::WorldSoundPortalTeleportEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_ROCKET_EXPLODED_EVENT, GameSoundAssets::WorldSoundRocketExplodedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_SAFETY_NET_CREATED_EVENT, GameSoundAssets::WorldSoundBallSafetyNetCreatedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_SAFETY_NET_DESTROYED_EVENT, GameSoundAssets::WorldSoundBallSafetyNetDestroyedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_ITEM_SPAWNED_EVENT, GameSoundAssets::WorldSoundItemSpawnedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_POWER_UP_ACTIVATED_EVENT, GameSoundAssets::WorldSoundPowerUpItemActivatedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_POWER_NEUTRAL_ACTIVATED_EVENT, GameSoundAssets::WorldSoundPowerNeutralItemActivatedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_POWER_DOWN_ACTIVATED_EVENT, GameSoundAssets::WorldSoundPowerDownItemActivatedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_POWER_UP_TIMER_END_EVENT, GameSoundAssets::WorldSoundPowerUpItemTimerEndsEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_POWER_NEUTRAL_TIMER_END_EVENT, GameSoundAssets::WorldSoundPowerNeutralItemTimerEndsEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_POWER_DOWN_TIMER_END_EVENT, GameSoundAssets::WorldSoundPowerDownItemTimerEndsEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BALL_OR_PADDLE_GROW_EVENT, GameSoundAssets::WorldSoundBallOrPaddleGrowEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_BALL_OR_PADDLE_SHRINK_EVENT, GameSoundAssets::WorldSoundBallOrPaddleShrinkEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_LASER_BULLET_SHOT_EVENT, GameSoundAssets::WorldSoundLaserBulletShotEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_LEVEL_COMPLETED_EVENT, GameSoundAssets::WorldSoundLevelCompletedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_WORLD_COMPLETED_EVENT, GameSoundAssets::WorldSoundWorldCompletedEvent));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_INK_SPLATTER_MASK, GameSoundAssets::WorldSoundInkSplatterMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_POISON_SICK_MASK, GameSoundAssets::WorldSoundPoisonSicknessMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_LASER_BULLET_MOVING_MASK, GameSoundAssets::WorldSoundLaserBulletMovingMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_ROCKET_MOVING_MASK, GameSoundAssets::WorldSoundRocketMovingMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_LASER_BEAM_FIRING_MASK, GameSoundAssets::WorldSoundLaserBeamFiringMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_COLLATERAL_BLOCK_FLASHING_MASK, GameSoundAssets::WorldSoundCollateralBlockFlashingMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_COLLATERAL_BLOCK_FALLING_MASK, GameSoundAssets::WorldSoundCollateralBlockFallingMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_CANNON_BLOCK_ROTATING_MASK, GameSoundAssets::WorldSoundCannonBlockRotatingMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_LAST_BALL_SPIRAL_TO_DEATH_MASK, GameSoundAssets::WorldSoundLastBallSpiralingToDeathMask));
	soundTypeMapping.insert(std::make_pair(MSFReader::WORLD_ITEM_MOVING_MASK, GameSoundAssets::WorldSoundItemMovingMask));
}

/**
* Converts the given music script file keyword sound name into an enumerated sound type
* for the various game sounds.
*/
int MSFReader::ConvertKeywordToSoundType(const std::string& soundName) {
	std::map<std::string, int>::iterator findIter = soundTypeMapping.find(soundName);
	if (findIter == soundTypeMapping.end()) {
		return MSFReader::INVALID_SOUND_TYPE;
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