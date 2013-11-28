/**
 * MSFReader.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "MSFReader.h"
#include "GameSound.h"
#include "SingleSoundSource.h"
#include "SoundEffect.h"

#include "../ResourceManager.h"
#include "../GameView/GameViewConstants.h"
#include "../BlammoEngine/StringHelper.h"

const char* MSFReader::OPEN_SOUND_DEFINTION_BLOCK     = "{";
const char* MSFReader::CLOSE_SOUND_DEFINITION_BLOCK   = "}";

const char* MSFReader::IGNORE_KEYWORD = "ignore";
const char* MSFReader::IGNORE_LINE    = "//";
const char* MSFReader::FILE_KEYWORD   = "file";
const char* MSFReader::WORLD_KEYWORD  = "world";
const char* MSFReader::TYPE_KEYWORD   = "type";

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
    std::vector<std::string> effectTypes;
    SoundEffect::EffectParameterMap effectParamMap;

	double resetSequenceTime = -1;

	// Read through the music script file
	while (*inStream >> currReadStr && !error) {

		// Check for an ignored block definition...
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
		// Check for the opening of a sound definition block...
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
            effectTypes.clear();
            effectParamMap.clear();

		}
		// Check for the closing of a music definition block...
		else if (currReadStr == MSFReader::CLOSE_SOUND_DEFINITION_BLOCK) {

			// If there is was no open block to begin with then the file is poorly formatted...
            if (!soundDefBlockOpen) {
				error = true;
				errorStr = "Tried to close a non-opened sound definition block: extra '}' found.";
				continue;
			}
            if (lastBlockName == MSFReader::NO_BLOCK_NAME) {
                soundDefBlockOpen = false;
                std::cerr << "No block name found for sound/effect." << std::endl;
                continue;
            }

            if (lastEffectEnum != GameSound::NoEffect) {

                SoundEffect* newSoundEffect = gameSound.BuildSoundEffect(lastEffectEnum, lastBlockName, effectTypes, effectParamMap);
                if (newSoundEffect == NULL) {
                    std::cerr << "Could not build effect: " << lastBlockName << std::endl;
                    effectTypes.clear();
                    effectParamMap.clear();
                    soundDefBlockOpen = false;
                    continue;
                }

                assert(gameSound.globalEffects.find(lastEffectEnum) == gameSound.globalEffects.end());
                gameSound.globalEffects.insert(std::make_pair(lastEffectEnum, newSoundEffect));
            }
            else {
                // If it's not a effect then it must be a sound
			    
                // Make sure there is a sound with associated file name to load
                assert(lastSoundEnum != GameSound::NoSound);
                assert(!soundFilePaths.empty());

                AbstractSoundSource* newSoundSrc = gameSound.BuildSoundSource(lastSoundEnum, lastBlockName, soundFilePaths);
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

					std::string soundFileLine;
					std::getline(*inStream, soundFileLine);

					// Clean up extra whitespace
					soundFileLine = stringhelper::trim(soundFileLine);

                    // Check to see if there are any pipes to indicate multiple files...
                    if (soundFileLine.find("|") == std::string::npos) {

					    // If we're here then there is just a single file provided...
					    soundFileLine = std::string(GameViewConstants::GetInstance()->SOUND_DIR) + "/" + soundFileLine;
					    soundFilePaths.push_back(soundFileLine);
                    }
                    else {
                        // There's more than one file listed...
                        std::vector<std::string> tokens;
                        stringhelper::Tokenize(soundFileLine, tokens, std::string("|\n\r\t"));
                        
                        bool failed = false;
                        std::set<std::string> filesLoaded;
                        for (size_t i = 0; i < tokens.size(); i++) {

                            std::string tempFilePath = stringhelper::trim(tokens[i]);
                            if (tempFilePath.empty()) {
                                continue;
                            }
                            tempFilePath = std::string(GameViewConstants::GetInstance()->SOUND_DIR) + "/" + tempFilePath;

                            // Make sure there are no duplicate files
                            std::pair<std::set<std::string>::iterator, bool> found = filesLoaded.insert(tempFilePath);
                            if (!found.second) {
                                // The file was already part of the list...
                                errorStr = std::string("Duplicate file name found for game sound (") + lastBlockName + std::string(") : ") + tempFilePath;
                                failed = true;
                                break;
                            }

                            soundFilePaths.push_back(tempFilePath);
                        }

                        if (failed || soundFilePaths.empty()) {
                            resetSequenceTime = -1;
                            soundFilePaths.clear();
                            error = true;
                            continue;
                        }

                    }

				}
                else if (currReadStr == MSFReader::TYPE_KEYWORD) {
                    if (!FoundEqualsSyntax(error, errorStr, inStream)) {
						continue;
					}

                    // The type can be a comma separated list of types of effects...
                    std::string typeLine;
                    std::getline(*inStream, typeLine);
                    typeLine = stringhelper::trim(typeLine);
                    
                    // Tokenize the string by commas
                    stringhelper::Tokenize(typeLine, effectTypes, ",");
                    for (int i = 0; i < static_cast<int>(effectTypes.size()); i++) {
                        effectTypes[i] = stringhelper::trim(effectTypes[i]);
                    }
                
                    if (effectTypes.empty()) {
                        errorStr = "Invalid effect type declaration found, type was left empty.";
					    error = true;
					    continue;
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
                else if (!currReadStr.empty()) {
                    // Must be some sort of effect parameter...
                    if (!FoundEqualsSyntax(error, errorStr, inStream)) {
						continue;
					}
                    float paramValue;
                    if (!(*inStream >> paramValue)) {
                        errorStr = "Invalid parameter value found for " + currReadStr + ", in block " + lastBlockName + ". Value must be a float!";
					    error = true;
					    continue;
                    }
                    effectParamMap.insert(std::make_pair(stringhelper::trim(currReadStr), paramValue)); 
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
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MainMenuTitleBiffSlamEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MainMenuTitleBamSlamEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MainMenuTitleBlammoSlamEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemChangedSelectionEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemEnteredEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemCancelEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemVerifyAndSelectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuItemVerifyAndSelectStartGameEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuSelectionItemScrolledEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuScrollerItemScrolledEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MenuOpenSubMenuWindowEvent));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaBaseMenuItemChangedSelectionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaBaseMenuItemSelectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaListItemChangedSelectionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaListItemSelectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaListItemDeselectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaListDeselectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlammopediaListItemLockedEvent));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldMenuBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldMenuItemChangedSelectionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldMenuItemSelectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldMenuItemLockedEvent));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuItemChangedSelectionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuItemSelectEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelMenuItemLockedEvent));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, InGameMenuOpened));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, InGameMenuClosed));
    
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldBackgroundLoop));

	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleHitWallEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PlayerLostABallButIsStillAliveEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, LastBallExplodedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallBallCollisionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallBossCollisionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LastBallSpiralingToDeathLoop));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallPaddleCollisionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallStickyPaddleAttachEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallStickyPaddleBounceEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallShieldPaddleCollisionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallTeslaLightningCollisionEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallOrPaddleGrowEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallOrPaddleShrinkEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallOnPaddleTimerRunningOutLoop));
    
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallBlockBasicBounceEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallBlockCollisionColourChange));
	
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BombBlockDestroyedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, InkBlockDestroyedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, InkSplatterEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BasicBlockDestroyedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CannonBlockLoadedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CannonBlockFiredEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PortalTeleportEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CollateralBlockFlashingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CollateralBlockFallingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, CannonBlockRotatingLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, CannonBlockRotatingPart));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, TeslaLightningArcLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, SwitchBlockActivated));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockFrozenEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockOnFireLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, IceShatterEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, IceMeltedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, FireFrozeEvent));


    soundTypeMapping.insert(MAPPING_PAIR(GameSound, TurretRocketFiredEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, TurretRocketMovingLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossRocketFiredEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossRocketMovingLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleRocketLaunchEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleRocketMovingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, RocketExplodedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, RemoteControlRocketThrustEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, TurretMineFiredEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleMineFiredEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MineExplodedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MineLatchedOnEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LaserBulletShotEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LaserBeamFiringLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LaserDeflectedByShieldEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, RocketOrMineDeflectedByShieldEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleLaserBulletCollisionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleLaserBeamCollisionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleCollateralBlockCollisionEvent));


	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallSafetyNetCreatedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallSafetyNetDestroyedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelFlipEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelUnflipEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleShieldActivatedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, PaddleShieldDeactivatedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, MagnetPaddleLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, ItemSpawnedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerUpItemActivatedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerNeutralItemActivatedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, PowerDownItemActivatedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ItemTimerEndingLoop));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, ItemTimerEndedEvent));
	soundTypeMapping.insert(MAPPING_PAIR(GameSound, ItemMovingLoop));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossFadeInIntroEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossBackgroundLoopTransition));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossBackgroundLoopTransitionSingleHitEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossAngryBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossGlowEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossElectricitySpasmLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossAngryEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossHurtEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossBlowingUpLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossDeathFlashToFullscreenWhiteoutEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossCrosshairTargetingEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BossLaserBeamLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ClassicalBossSparkleEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ClassicalBossArmShakeLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ClassicalBossArmAttackEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ClassicalBossArmAttackHitEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ClassicalBossPowerChargeEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, GothicBossOrbAttackEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, GothicBossSummonItemChargeEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, GothicBossChargeShockwaveEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, GothicBossMassiveShockwaveEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossLightningRelayTurnOnEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossArmRotateLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossArmExtendEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossArmRetractEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossArmLevelCollisionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossArmPaddleCollisionEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossLevelRotatingLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossLightningBoltAttackEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, DecoBossElectricShockLoop));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, EnterBulletTimeEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, InBulletTimeLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ExitBulletTimeEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallBoostEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BallBoostGainedEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BoostMalfunctionPromptEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BoostAttemptWhileMalfunctioningEvent));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LifeUpAcquiredEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, StarAcquiredEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, FiveStarsAcquiredEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ScoreMultiplierIncreasedTo2Event));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ScoreMultiplierIncreasedTo3Event));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ScoreMultiplierIncreasedTo4Event));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, ScoreMultiplierLostEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc1));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc2));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc3));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc4));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc5));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc6));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc7));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc8));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc9));

    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelStartPaddleMoveUpEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelStartBallSpawnOnPaddleEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelEndFadeoutEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelSummaryBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelSummaryNewHighScoreEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelSummaryPointTallyLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelSummaryStarTallyEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelSummaryConfirmEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, LevelStarCostPaidUnlockEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldCompleteBackgroundLoop));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldCompleteVictoryMessageEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, WorldUnlockEvent));
    soundTypeMapping.insert(MAPPING_PAIR(GameSound, GameOverEvent));
}

void MSFReader::InitEffectTypeMapping() {
    if (!effectTypeMapping.empty()) {
        return;
    }

    effectTypeMapping.insert(MAPPING_PAIR(GameSound, BulletTimeEffect));
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