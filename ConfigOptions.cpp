/**
 * ConfigOptions.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#include "ConfigOptions.h"

const char* ConfigOptions::INI_FILEPATH           = "BiffBamBlammo.ini";

const char* ConfigOptions::WINDOW_HEIGHT_VAR      = "window_height";
const char* ConfigOptions::WINDOW_WIDTH_VAR       = "window_width";
const char* ConfigOptions::WINDOW_FULLSCREEN_VAR  = "fullscreen";
const char* ConfigOptions::WINDOW_VSYNC_VAR       = "vsync";
const char* ConfigOptions::MUSIC_VOLUME_VAR       = "music_vol";
const char* ConfigOptions::SFX_VOLUME_VAR         = "sfx_vol";
const char* ConfigOptions::DIFFICULTY_VAR         = "difficulty";
const char* ConfigOptions::BALL_BOOST_MODE_VAR    = "ball_boost_mode";
const char* ConfigOptions::INVERT_BALL_BOOST_VAR  = "invert_ball_boost";

const int ConfigOptions::MIN_WINDOW_SIZE	= 480;
const int ConfigOptions::MAX_WINDOW_SIZE	= 2048;
const int ConfigOptions::MIN_VOLUME         = 0;
const int ConfigOptions::MAX_VOLUME         = 100;

const int  ConfigOptions::DEFAULT_WINDOW_WIDTH                  = 1024;
const int  ConfigOptions::DEFAULT_WINDOW_HEIGHT                 = 768;
const bool ConfigOptions::DEFAULT_FULLSCREEN_TOGGLE		        = false;
const bool ConfigOptions::DEFAULT_VSYNC_TOGGLE                  = false;
const int  ConfigOptions::DEFAULT_MUSIC_VOLUME                  = ConfigOptions::MAX_VOLUME;
const int  ConfigOptions::DEFAULT_SFX_VOLUME                    = ConfigOptions::MAX_VOLUME;
const bool ConfigOptions::DEFAULT_INVERT_BALL_BOOST_TOGGLE      = true;
const BallBoostModel::BallBoostMode ConfigOptions::DEFAULT_BALL_BOOST_MODE = BallBoostModel::Slingshot;
const GameModel::Difficulty ConfigOptions::DEFAULT_DIFFICULTY              = GameModel::MediumDifficulty;

const char* ConfigOptions::SLINGSHOT_STR        = "slingshot";
const char* ConfigOptions::PRESS_TO_RELEASE_STR = "press_to_release";

const char* ConfigOptions::EASY_DIFFICULTY_STR      = "easy";
const char* ConfigOptions::MEDIUM_DIFFICULTY_STR    = "medium";
const char* ConfigOptions::HARD_DIFFICULTY_STR      = "hard";

ConfigOptions::ConfigOptions() : 
windowWidth(DEFAULT_WINDOW_WIDTH), windowHeight(DEFAULT_WINDOW_HEIGHT),
fullscreenIsOn(DEFAULT_FULLSCREEN_TOGGLE), vSyncIsOn(DEFAULT_VSYNC_TOGGLE), 
musicVolume(ConfigOptions::DEFAULT_MUSIC_VOLUME), sfxVolume(ConfigOptions::DEFAULT_SFX_VOLUME),
invertBallBoost(DEFAULT_INVERT_BALL_BOOST_TOGGLE), ballBoostMode(DEFAULT_BALL_BOOST_MODE), difficulty(DEFAULT_DIFFICULTY) {
}

/**
 * Set the resolution configuration via a string with the following format:
 * "<width> x <height>".
 */
void ConfigOptions::SetResolutionByString(const std::string& resStr) {
	std::stringstream resStrStream(resStr);
	int winWidth = this->windowWidth;
	resStrStream >> winWidth;

	// Read in the 'x' that seperates width from height
	char tempX;
	resStrStream >> tempX;
	assert(tempX == 'x' || tempX == 'X');

	int winHeight = this->windowHeight;
	resStrStream >> winHeight;

	this->SetWindowWidth(winWidth);
	this->SetWindowHeight(winHeight);
}

std::vector<std::string> ConfigOptions::GetDifficultyItems() {
    std::vector<std::string> difficultyItems;
    difficultyItems.reserve(3);
    difficultyItems.push_back("Easy");
    difficultyItems.push_back("Normal");
    difficultyItems.push_back("Hard");
    return difficultyItems;
}

std::vector<std::string> ConfigOptions::GetBallBoostModeItems() {
    std::vector<std::string> boostModeItems;
    boostModeItems.reserve(2);
    boostModeItems.push_back("Auto");
    boostModeItems.push_back("Manual");
    return boostModeItems;
}

std::vector<std::string> ConfigOptions::GetOnOffItems() {
    std::vector<std::string> onOffItems;
    onOffItems.reserve(2);
    onOffItems.push_back("Off");
    onOffItems.push_back("On");
    return onOffItems;
}

/**
 * Reads the configuration options from the .ini file on disk.
 * Returns: A heap object pointer to a set of configurations created from reading
 * the .ini file on disk, NULL otherwise.
 */
ConfigOptions* ConfigOptions::ReadConfigOptionsFromFile() {

	// Open the .ini file off disk
	std::ifstream inFile;
    inFile.open((ResourceManager::GetLoadDir() + std::string(ConfigOptions::INI_FILEPATH)).c_str());
	if (!inFile.is_open()) {
		// Couldn't find or open the file
		inFile.close();
		return NULL;
	}

	// This will load the object with default values - in cases of errors
	// while we read from the .ini, we can always just use the defaults
	ConfigOptions* cfgOptions = new ConfigOptions();

	// Read in all the configuration values
	std::string currStr;	// stores the currently read string from file
	char skipEquals;			// temporary variable for throwing away the '='

#define READ_IN_FILE_FAIL(inFile, valueToRead) if (!(inFile >> valueToRead)) { inFile.close(); return cfgOptions; }

	while (inFile >> currStr) {
		
		// Window width config
		if (currStr == ConfigOptions::WINDOW_WIDTH_VAR) {
			READ_IN_FILE_FAIL(inFile, skipEquals);

			// Read the window width
			int tempWinWidth = ConfigOptions::DEFAULT_WINDOW_WIDTH;
			READ_IN_FILE_FAIL(inFile, tempWinWidth);

			if (tempWinWidth <= ConfigOptions::MAX_WINDOW_SIZE && tempWinWidth >= ConfigOptions::MIN_WINDOW_SIZE) {
				cfgOptions->windowWidth = tempWinWidth;
			}
		}
		// Window height config
		else if (currStr == ConfigOptions::WINDOW_HEIGHT_VAR) {
			READ_IN_FILE_FAIL(inFile, skipEquals);

			// Read the window width
			int tempWinHeight = ConfigOptions::DEFAULT_WINDOW_HEIGHT;
			READ_IN_FILE_FAIL(inFile, tempWinHeight);

			if (tempWinHeight <= ConfigOptions::MAX_WINDOW_SIZE && tempWinHeight >= ConfigOptions::MIN_WINDOW_SIZE) {
				cfgOptions->windowHeight = tempWinHeight;
			}
		}
		// Fullscreen config
		else if (currStr == ConfigOptions::WINDOW_FULLSCREEN_VAR) {
			READ_IN_FILE_FAIL(inFile, skipEquals);

			// Read whether the window is fullscreen or not
			int isFullscreen = ConfigOptions::DEFAULT_FULLSCREEN_TOGGLE ? 1 : 0;
			READ_IN_FILE_FAIL(inFile, isFullscreen);

			if (isFullscreen == 0) {
				cfgOptions->fullscreenIsOn = false;
			}
			else {
				cfgOptions->fullscreenIsOn = true;
			}
		}
		// Vertical Sync config
		else if (currStr == ConfigOptions::WINDOW_VSYNC_VAR) {
			READ_IN_FILE_FAIL(inFile, skipEquals);

			// Read in whether the frames are synced with the monitor
			int isVSynced = ConfigOptions::DEFAULT_VSYNC_TOGGLE ? 1 : 0;
			inFile >> isVSynced;

			if (isVSynced == 0) {
				cfgOptions->vSyncIsOn = false;
			}
			else {
				cfgOptions->vSyncIsOn = true;
			}
		}
        // Music volume config
		else if (currStr == ConfigOptions::MUSIC_VOLUME_VAR) {
			READ_IN_FILE_FAIL(inFile, skipEquals);

			// Read in the sound/music volume for the game
			int musicVolume = ConfigOptions::DEFAULT_MUSIC_VOLUME;
			READ_IN_FILE_FAIL(inFile, musicVolume);
			cfgOptions->musicVolume = std::max<int>(ConfigOptions::MIN_VOLUME, std::min<int>(ConfigOptions::MAX_VOLUME, musicVolume));
		}
        // SFX volume config
        else if (currStr == ConfigOptions::SFX_VOLUME_VAR) {
            READ_IN_FILE_FAIL(inFile, skipEquals);

            // Read in the sound/music volume for the game
            int sfxVolume = ConfigOptions::DEFAULT_SFX_VOLUME;
            READ_IN_FILE_FAIL(inFile, sfxVolume);
            cfgOptions->sfxVolume = std::max<int>(ConfigOptions::MIN_VOLUME, std::min<int>(ConfigOptions::MAX_VOLUME, sfxVolume));
        }
        // Inverted ball boosting config
        else if (currStr == ConfigOptions::INVERT_BALL_BOOST_VAR) {
			READ_IN_FILE_FAIL(inFile, skipEquals);

			// Read in whether the ball boost controls are inverted or not
			int isInverted = ConfigOptions::DEFAULT_INVERT_BALL_BOOST_TOGGLE ? 1 : 0;
			inFile >> isInverted;

			if (isInverted == 0) {
                cfgOptions->invertBallBoost = false;
			}
			else {
				cfgOptions->invertBallBoost = true;
			}
        }
        // Ball boosting mode config
        else if (currStr == ConfigOptions::BALL_BOOST_MODE_VAR) {
            READ_IN_FILE_FAIL(inFile, skipEquals);

            std::string boostModeStr;
            bool success = ConfigOptions::BallBoostModeToString(ConfigOptions::DEFAULT_BALL_BOOST_MODE, boostModeStr);
            assert(success);

            READ_IN_FILE_FAIL(inFile, boostModeStr);

            success = ConfigOptions::StringToBallBoostMode(boostModeStr, cfgOptions->ballBoostMode);
            if (!success) { 
                cfgOptions->ballBoostMode = ConfigOptions::DEFAULT_BALL_BOOST_MODE;
            }
        }
        // Game difficulty config
        else if (currStr == ConfigOptions::DIFFICULTY_VAR) {
            READ_IN_FILE_FAIL(inFile, skipEquals);
            
            std::string difficultyStr;
            bool success = ConfigOptions::DifficultyToString(ConfigOptions::DEFAULT_DIFFICULTY, difficultyStr);
            assert(success);

            READ_IN_FILE_FAIL(inFile, difficultyStr);

            success = ConfigOptions::StringToDifficulty(difficultyStr, cfgOptions->difficulty);
            if (!success) { 
                cfgOptions->difficulty = ConfigOptions::DEFAULT_DIFFICULTY;
            }
        }
	}
	
	inFile.close();
	return cfgOptions;

#undef READ_IN_FILE_FAIL
}

/**
 * Writes the currently stored configuration values from memory to file.
 * Returns: true on success, false otherwise.
 */
bool ConfigOptions::WriteConfigOptionsToFile() const {
	// Open or create the .ini file for writing
	std::ofstream outFile;
	outFile.open((ResourceManager::GetLoadDir() + std::string(ConfigOptions::INI_FILEPATH)).c_str());
	if (!outFile.is_open()) {
		// Couldn't open for writing...
		outFile.close();
		return false;
	}

    bool success = true;

	// Write all the configuration values (and comments along with them):
	// Write a description of what this file is for...
	outFile << "// This is the Biff! Bam!! Blammo!?! game configuration file, it is used to initialize"	<< std::endl;
	outFile << "// various game options (video, audio, etc.) at startup. To change the options you may"	<< std::endl; 
	outFile << "// configure them manually in-game via the \"Options\" menu or manually, by following"	<< std::endl;
	outFile << "// the guidelines below." << std::endl;
	outFile << "// Remember to always leave spaces between '=' characters!" << std::endl;
	outFile << std::endl;
	
	// Write the window dimensions
	outFile << "// Window pixel dimensions" << std::endl;
	outFile << ConfigOptions::WINDOW_WIDTH_VAR  << " = " << this->windowWidth  << std::endl;
	outFile << ConfigOptions::WINDOW_HEIGHT_VAR << " = " << this->windowHeight << std::endl;
	outFile << std::endl;

	// Fullscreen option
	outFile << "// Fullscreen mode (0 - off, 1 - on)" << std::endl;
	outFile << ConfigOptions::WINDOW_FULLSCREEN_VAR << " = " << (this->fullscreenIsOn ? "1" : "0") << std::endl;
	outFile << std::endl;

	// Vertical Sync option
	outFile << "// Vertical sync (0 - off, 1 - on)" << std::endl;
	outFile << ConfigOptions::WINDOW_VSYNC_VAR << " = " << (this->vSyncIsOn ? "1" : "0") << std::endl;
	outFile << std::endl;

	// Music Volume option
	outFile << "// Music Volume (0 - mute, 100 - loudest)" << std::endl;
	outFile << ConfigOptions::MUSIC_VOLUME_VAR << " = " << (this->musicVolume) << std::endl;
	outFile << std::endl;
	
    // SFX Volume option
    outFile << "// SFX Volume (0 - mute, 100 - loudest)" << std::endl;
    outFile << ConfigOptions::SFX_VOLUME_VAR << " = " << (this->sfxVolume) << std::endl;
    outFile << std::endl;

    // Inverted ball boosting option
    outFile << "// Inverted ball boosting (0 - off, 1 - on)" << std::endl;
    outFile << ConfigOptions::INVERT_BALL_BOOST_VAR << " = " << (this->invertBallBoost ? "1" : "0") << std::endl;
    outFile << std::endl;

    // Ball boost mode option
    std::string ballBoostModeString;
    success &= this->BallBoostModeToString(this->ballBoostMode, ballBoostModeString);
    assert(success);

    outFile << "// Ball boost mode {slingshot, press_to_release}" << std::endl;
    outFile << ConfigOptions::BALL_BOOST_MODE_VAR << " = " << ballBoostModeString << std::endl;
    outFile << std::endl;

    // Difficulty option
    std::string difficultyString;
    success &= this->DifficultyToString(this->difficulty, difficultyString);
    assert(success);

    outFile << "// Difficulty of the game {easy, medium, hard}" << std::endl;
    outFile << ConfigOptions::DIFFICULTY_VAR << " = " << difficultyString << std::endl;
    //outFile << std::endl;

	outFile.close();
	return success;
}

bool ConfigOptions::BallBoostModeToString(const BallBoostModel::BallBoostMode& mode, std::string& modeStr) {
    switch (mode) {
        case BallBoostModel::Slingshot:
            modeStr = ConfigOptions::SLINGSHOT_STR;
            break;
        case BallBoostModel::PressToRelease:
            modeStr = ConfigOptions::PRESS_TO_RELEASE_STR;
            break;
        default:
            assert(false);
            return false;
    }

    return true;
}

bool ConfigOptions::StringToBallBoostMode(const std::string& modeStr, BallBoostModel::BallBoostMode& mode) {
    if (modeStr.compare(ConfigOptions::SLINGSHOT_STR) == 0) {
        mode = BallBoostModel::Slingshot;
    }
    else if (modeStr.compare(ConfigOptions::PRESS_TO_RELEASE_STR) == 0) {
        mode = BallBoostModel::PressToRelease;
    }
    else {
        return false;
    }

    return true;
}

bool ConfigOptions::DifficultyToString(const GameModel::Difficulty& difficulty, std::string& difficultyStr) {
    switch (difficulty) {
        case GameModel::EasyDifficulty:
            difficultyStr = ConfigOptions::EASY_DIFFICULTY_STR;
            break;
        case GameModel::MediumDifficulty:
            difficultyStr = ConfigOptions::MEDIUM_DIFFICULTY_STR;
            break;
        case GameModel::HardDifficulty:
            difficultyStr = ConfigOptions::HARD_DIFFICULTY_STR;
            break;
        default:
            assert(false);
            return false;
    }

    return true;
}

bool ConfigOptions::StringToDifficulty(const std::string& difficultyStr, GameModel::Difficulty& difficulty) {
    if (difficultyStr.compare(ConfigOptions::EASY_DIFFICULTY_STR) == 0) {
        difficulty = GameModel::EasyDifficulty;
    }
    else if (difficultyStr.compare(ConfigOptions::MEDIUM_DIFFICULTY_STR) == 0) {
        difficulty = GameModel::MediumDifficulty;
    }
    else if (difficultyStr.compare(ConfigOptions::HARD_DIFFICULTY_STR) == 0) {
        difficulty = GameModel::HardDifficulty;
    }
    else {
        return false;
    }

    return true;
}