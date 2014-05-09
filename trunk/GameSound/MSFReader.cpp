/**
 * MSFReader.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

	// Initialize the mapping from strings to enums for all the possible game sounds and effects
    MSFReader::InitSoundTypeMapping();
    MSFReader::InitEffectTypeMapping();

	// Grab a file in stream from the main menu music script file:
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
	if (!MSFReader::soundTypeMapping.empty()) {
		return;
	}

    GameSound::musicSoundTypeMap.clear();

#define INSERT_MAPPING_PAIR(Class, EnumVal, isMusic) \
    MSFReader::soundTypeMapping.insert(MAPPING_PAIR(Class, EnumVal)); \
    GameSound::musicSoundTypeMap[Class::EnumVal] = isMusic

	INSERT_MAPPING_PAIR(GameSound, MainMenuBackgroundLoop, true);
    INSERT_MAPPING_PAIR(GameSound, MainMenuTitleBiffSlamEvent, false);
    INSERT_MAPPING_PAIR(GameSound, MainMenuTitleBamSlamEvent, false);
    INSERT_MAPPING_PAIR(GameSound, MainMenuTitleBlammoSlamEvent, false);
	INSERT_MAPPING_PAIR(GameSound, MenuItemChangedSelectionEvent, false);
	INSERT_MAPPING_PAIR(GameSound, MenuItemEnteredEvent, false);
	INSERT_MAPPING_PAIR(GameSound, MenuItemCancelEvent, false);
	INSERT_MAPPING_PAIR(GameSound, MenuItemVerifyAndSelectEvent, false);
    INSERT_MAPPING_PAIR(GameSound, MenuItemVerifyAndSelectStartGameEvent, false);
	INSERT_MAPPING_PAIR(GameSound, MenuSelectionItemScrolledEvent, false);
    INSERT_MAPPING_PAIR(GameSound, MenuScrollerItemScrolledEvent, false);
    INSERT_MAPPING_PAIR(GameSound, MenuOpenSubMenuWindowEvent, false);

    INSERT_MAPPING_PAIR(GameSound, BlammopediaBackgroundLoop, true);
    INSERT_MAPPING_PAIR(GameSound, BlammopediaBaseMenuItemChangedSelectionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlammopediaBaseMenuItemSelectEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlammopediaListItemChangedSelectionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlammopediaListItemSelectEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlammopediaListItemDeselectEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlammopediaListDeselectEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlammopediaListItemLockedEvent, false);

    INSERT_MAPPING_PAIR(GameSound, WorldMenuBackgroundLoop, true);
    INSERT_MAPPING_PAIR(GameSound, WorldMenuItemChangedSelectionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, WorldMenuItemSelectEvent, false);
    INSERT_MAPPING_PAIR(GameSound, WorldMenuItemLockedEvent, false);

    INSERT_MAPPING_PAIR(GameSound, LevelMenuBackgroundLoop, true);
    INSERT_MAPPING_PAIR(GameSound, LevelMenuItemChangedSelectionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelMenuItemSelectEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelMenuItemLockedEvent, false);

    INSERT_MAPPING_PAIR(GameSound, InGameMenuOpened, false);
    INSERT_MAPPING_PAIR(GameSound, InGameMenuClosed, false);
    
	INSERT_MAPPING_PAIR(GameSound, WorldBackgroundLoop, true);

	INSERT_MAPPING_PAIR(GameSound, PaddleHitWallEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleFrozenEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleOnFireEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleIceShakeEvent, false);

    INSERT_MAPPING_PAIR(GameSound, PaddlePortalTeleportEvent, false);

	INSERT_MAPPING_PAIR(GameSound, PlayerLostABallButIsStillAliveEvent, false);
	INSERT_MAPPING_PAIR(GameSound, LastBallExplodedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallBallCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallBossCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LastBallSpiralingToDeathLoop, false);

    INSERT_MAPPING_PAIR(GameSound, BallPaddleCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallStickyPaddleAttachEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallStickyPaddleBounceEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallShieldPaddleCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallTeslaLightningCollisionEvent, false);
	INSERT_MAPPING_PAIR(GameSound, BallOrPaddleGrowEvent, false);
	INSERT_MAPPING_PAIR(GameSound, BallOrPaddleShrinkEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallOnPaddleTimerRunningOutLoop, false);
    
	INSERT_MAPPING_PAIR(GameSound, BallBlockBasicBounceEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallBlockCollisionColourChange, false);
    INSERT_MAPPING_PAIR(GameSound, BallPortalTeleportEvent, false);
	
	INSERT_MAPPING_PAIR(GameSound, BombBlockDestroyedEvent, false);
	INSERT_MAPPING_PAIR(GameSound, InkBlockDestroyedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, InkSplatterEvent, false);
	INSERT_MAPPING_PAIR(GameSound, BasicBlockDestroyedEvent, false);
	INSERT_MAPPING_PAIR(GameSound, CannonBlockLoadedEvent, false);
	INSERT_MAPPING_PAIR(GameSound, CannonBlockFiredEvent, false);
    INSERT_MAPPING_PAIR(GameSound, CannonObstructionMalfunctionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FragileCannonDestroyedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, EnergyProjectilePortalTeleportEvent, false);
    INSERT_MAPPING_PAIR(GameSound, NonBallObjectPortalTeleportEvent, false);

    INSERT_MAPPING_PAIR(GameSound, CollateralBlockHitEvent, false);
	INSERT_MAPPING_PAIR(GameSound, CollateralBlockFlashingLoop, false);
	INSERT_MAPPING_PAIR(GameSound, CollateralBlockFallingLoop, false);
	INSERT_MAPPING_PAIR(GameSound, CannonBlockRotatingLoop, false);
    INSERT_MAPPING_PAIR(GameSound, CannonBlockRotatingPart, false);
    INSERT_MAPPING_PAIR(GameSound, TeslaLightningArcLoop, false);
    INSERT_MAPPING_PAIR(GameSound, SwitchBlockActivated, false);
    INSERT_MAPPING_PAIR(GameSound, ItemDropBlockLockedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlockFrozenEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlockOnFireLoop, false);
    INSERT_MAPPING_PAIR(GameSound, IceShatterEvent, false);
    INSERT_MAPPING_PAIR(GameSound, IceMeltedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FireFrozeEvent, false);


    INSERT_MAPPING_PAIR(GameSound, TurretRocketFiredEvent, false);
    INSERT_MAPPING_PAIR(GameSound, TurretRocketMovingLoop, false);
    INSERT_MAPPING_PAIR(GameSound, BossRocketFiredEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossRocketMovingLoop, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleRocketLaunchEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleRocketMovingLoop, false);
	INSERT_MAPPING_PAIR(GameSound, RocketExplodedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, RemoteControlRocketThrustEvent, false);
    INSERT_MAPPING_PAIR(GameSound, TurretMineFiredEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleMineFiredEvent, false);
    INSERT_MAPPING_PAIR(GameSound, MineExplodedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, MineLatchedOnEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FlameBlasterShotEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FlameBlasterHitEvent, false);
    INSERT_MAPPING_PAIR(GameSound, IceBlasterShotEvent, false);
    INSERT_MAPPING_PAIR(GameSound, IceBlasterHitEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LaserBulletShotEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LaserBeamFiringLoop, false);
    INSERT_MAPPING_PAIR(GameSound, LaserDeflectedByShieldEvent, false);
    INSERT_MAPPING_PAIR(GameSound, RocketOrMineDeflectedByShieldEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlastDeflectedByShieldEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FireGlobShieldCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FireGlobBlockCollisionEvent, false);

    INSERT_MAPPING_PAIR(GameSound, PaddleFireGlobCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleLaserBulletCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleLaserBeamCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleCollateralBlockCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PortalProjectileOpenedEvent, false);


	INSERT_MAPPING_PAIR(GameSound, BallSafetyNetCreatedEvent, false);
	INSERT_MAPPING_PAIR(GameSound, BallSafetyNetDestroyedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelFlipEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelUnflipEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleShieldActivatedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, PaddleShieldDeactivatedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, MagnetPaddleLoop, false);
	INSERT_MAPPING_PAIR(GameSound, ItemSpawnedEvent, false);
	INSERT_MAPPING_PAIR(GameSound, PowerUpItemActivatedEvent, false);
	INSERT_MAPPING_PAIR(GameSound, PowerNeutralItemActivatedEvent, false);
	INSERT_MAPPING_PAIR(GameSound, PowerDownItemActivatedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, ItemTimerEndingLoop, false);
	INSERT_MAPPING_PAIR(GameSound, ItemTimerEndedEvent, false);
	INSERT_MAPPING_PAIR(GameSound, ItemMovingLoop, false);

    INSERT_MAPPING_PAIR(GameSound, BossFadeInIntroEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossBackgroundLoop, true);
    INSERT_MAPPING_PAIR(GameSound, BossBackgroundLoopTransition, true);
    INSERT_MAPPING_PAIR(GameSound, BossBackgroundLoopTransitionSingleHitEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossAngryBackgroundLoop, true);
    INSERT_MAPPING_PAIR(GameSound, BossGlowEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossElectricitySpasmLoop, false);
    INSERT_MAPPING_PAIR(GameSound, BossAngryEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossHurtEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossBlowingUpLoop, false);
    INSERT_MAPPING_PAIR(GameSound, BossDeathFlashToFullscreenWhiteoutEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossCrosshairTargetingEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossLaserBeamLoop, false);
    INSERT_MAPPING_PAIR(GameSound, BossFrozenEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossIceShatterEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BossIceShakeEvent, false);
    INSERT_MAPPING_PAIR(GameSound, ClassicalBossSparkleEvent, false);
    INSERT_MAPPING_PAIR(GameSound, ClassicalBossArmShakeLoop, false);
    INSERT_MAPPING_PAIR(GameSound, ClassicalBossArmAttackEvent, false);
    INSERT_MAPPING_PAIR(GameSound, ClassicalBossArmAttackHitEvent, false);
    INSERT_MAPPING_PAIR(GameSound, ClassicalBossPowerChargeEvent, false);
    INSERT_MAPPING_PAIR(GameSound, GothicBossOrbAttackEvent, false);
    INSERT_MAPPING_PAIR(GameSound, GothicBossSummonItemChargeEvent, false);
    INSERT_MAPPING_PAIR(GameSound, GothicBossChargeShockwaveEvent, false);
    INSERT_MAPPING_PAIR(GameSound, GothicBossMassiveShockwaveEvent, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossLightningRelayTurnOnEvent, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossArmRotateLoop, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossArmExtendEvent, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossArmRetractEvent, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossArmLevelCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossArmPaddleCollisionEvent, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossLevelRotatingLoop, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossLightningBoltAttackEvent, false);
    INSERT_MAPPING_PAIR(GameSound, DecoBossElectricShockLoop, false);

    INSERT_MAPPING_PAIR(GameSound, FuturismBossTeleportationChargingEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FuturismBossTeleportationEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FuturismBossPortalSummonChargeEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FuturismBossPortalFireEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FuturismBossTwitchBeamWarningEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FuturismBossArcBeamWarningEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FuturismBossAttractorBeamEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FuturismBossChargingEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FuturismBossSpinCoolDownEvent, false);

    INSERT_MAPPING_PAIR(GameSound, EnterBulletTimeEvent, false);
    INSERT_MAPPING_PAIR(GameSound, InBulletTimeLoop, false);
    INSERT_MAPPING_PAIR(GameSound, ExitBulletTimeEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallBoostEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BallBoostGainedEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BoostMalfunctionPromptEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BoostAttemptWhileMalfunctioningEvent, false);

    INSERT_MAPPING_PAIR(GameSound, LifeUpAcquiredEvent, false);
    INSERT_MAPPING_PAIR(GameSound, StarAcquiredEvent, false);
    INSERT_MAPPING_PAIR(GameSound, FiveStarsAcquiredEvent, false);
    INSERT_MAPPING_PAIR(GameSound, ScoreMultiplierIncreasedTo2Event, false);
    INSERT_MAPPING_PAIR(GameSound, ScoreMultiplierIncreasedTo3Event, false);
    INSERT_MAPPING_PAIR(GameSound, ScoreMultiplierIncreasedTo4Event, false);
    INSERT_MAPPING_PAIR(GameSound, ScoreMultiplierLostEvent, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc1, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc2, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc3, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc4, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc5, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc6, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc7, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc8, false);
    INSERT_MAPPING_PAIR(GameSound, BlockBrokenMultiplierCounterInc9, false);

    INSERT_MAPPING_PAIR(GameSound, LevelStartPaddleMoveUpEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelStartBallSpawnOnPaddleEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelEndFadeoutEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelSummaryBackgroundLoop, true);
    INSERT_MAPPING_PAIR(GameSound, LevelSummaryNewHighScoreEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelSummaryAllStarsEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelSummaryPointTallyLoop, false);
    INSERT_MAPPING_PAIR(GameSound, LevelSummaryPointTallySkipEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelSummaryStarTallyEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelSummaryConfirmEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelBasicUnlockEvent, false);
    INSERT_MAPPING_PAIR(GameSound, LevelStarCostPaidUnlockEvent, false);
    INSERT_MAPPING_PAIR(GameSound, WorldCompleteBackgroundLoop, true);
    INSERT_MAPPING_PAIR(GameSound, WorldCompleteVictoryMessageEvent, false);
    INSERT_MAPPING_PAIR(GameSound, WorldSummaryConfirmEvent, false);
    INSERT_MAPPING_PAIR(GameSound, WorldUnlockEvent, false);
    INSERT_MAPPING_PAIR(GameSound, GameOverEvent, true);

#undef INSERT_MAPPING_PAIR
}

void MSFReader::InitEffectTypeMapping() {
    if (!MSFReader::effectTypeMapping.empty()) {
        return;
    }

    MSFReader::effectTypeMapping.insert(MAPPING_PAIR(GameSound, BulletTimeEffect));
	MSFReader::effectTypeMapping.insert(MAPPING_PAIR(GameSound, InkSplatterEffect));
	MSFReader::effectTypeMapping.insert(MAPPING_PAIR(GameSound, PoisonEffect));
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