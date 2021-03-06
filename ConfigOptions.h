/**
 * ConfigOptions.h
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

#ifndef __CONFIGOPTIONS_H__
#define __CONFIGOPTIONS_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/BallBoostModel.h"

#include "ResourceManager.h"

/**
 * Stores data for initializing video, audio, etc. options,
 * created by the resource manager on loading the game's .ini file.
 */
class ConfigOptions {
	// Befriend the reading and writing functions in the resource manager - we want the
	// resource manager to be able to use this class easily
	friend ConfigOptions ResourceManager::ReadConfigurationOptions(bool forceReadFromFile, bool arcadeMode);
	friend bool ResourceManager::WriteConfigurationOptionsToFile(const ConfigOptions& configOptions);

public:
	// Boundry values for each variable
	static const int MIN_WINDOW_SIZE;
	static const int MAX_WINDOW_SIZE;
	static const int MIN_VOLUME;
	static const int MAX_VOLUME;

	ConfigOptions(bool arcadeMode);
	~ConfigOptions() {}
	
	// Accessor functions for all of the configuration options
	inline int GetWindowWidth() const { return this->windowWidth; }
	inline int GetWindowHeight() const { return this->windowHeight; }
	inline std::string GetResolutionString() const { 
		std::stringstream resolutionStr;
		resolutionStr << this->windowWidth << " x " << this->windowHeight;
		return resolutionStr.str();
	}

	inline bool GetIsFullscreenOn() const { return this->fullscreenIsOn; }
	inline bool GetIsVSyncOn() const { return this->vSyncIsOn; }
	inline int GetMusicVolume() const { return this->musicVolume; }
    inline int GetSFXVolume() const { return this->sfxVolume; }
    inline bool GetInvertBallBoost() const { return this->invertBallBoost; }
    inline BallBoostModel::BallBoostMode GetBallBoostMode() const { return this->ballBoostMode; }
    inline GameModel::Difficulty GetDifficulty() const { return this->difficulty; }

	// Setter functions for all the configuration options
	inline void SetWindowWidth(int width) { 
		assert(width >= MIN_WINDOW_SIZE && width <= MAX_WINDOW_SIZE);
		this->windowWidth = std::max<int>(MIN_WINDOW_SIZE, std::min<int>(MAX_WINDOW_SIZE, width));
	}
	inline void SetWindowHeight(int height) { 
		assert(height >= MIN_WINDOW_SIZE && height <= MAX_WINDOW_SIZE);
		this->windowHeight = std::max<int>(MIN_WINDOW_SIZE, std::min<int>(MAX_WINDOW_SIZE, height));
	}
	void SetResolutionByString(const std::string& resStr);
	inline void SetIsFullscreenOn(bool isOn) { this->fullscreenIsOn = isOn; }
	inline void SetIsVSyncOn(bool isOn) { this->vSyncIsOn = isOn; }
	
    inline void SetMusicVolume(int volume) {
		assert(volume >= MIN_VOLUME && volume <= MAX_VOLUME);
		this->musicVolume = std::max<int>(MIN_VOLUME, std::min<int>(MAX_VOLUME, volume));
	}
    inline void SetSFXVolume(int volume) {
        assert(volume >= MIN_VOLUME && volume <= MAX_VOLUME);
        this->sfxVolume = std::max<int>(MIN_VOLUME, std::min<int>(MAX_VOLUME, volume));
    }

    inline void SetInvertBallBoost(bool isInverted) { this->invertBallBoost = isInverted; }
    inline void SetBallBoostMode(BallBoostModel::BallBoostMode mode) { this->ballBoostMode = mode; }
    inline void SetDifficulty(GameModel::Difficulty difficulty) { this->difficulty = difficulty; }

    static std::vector<std::string> GetDifficultyItems();
    static std::vector<std::string> GetBallBoostModeItems();
    static std::vector<std::string> GetOnOffItems();

    static bool IsOnItemSelected(int idx) { return idx == 1; }

private:
	// Filepath to the game's .ini file
	static const char* INI_FILEPATH;

	// The variable (initialization string) names for the various values in the .ini file
	static const char* WINDOW_HEIGHT_VAR;
	static const char* WINDOW_WIDTH_VAR;
	static const char* WINDOW_FULLSCREEN_VAR;
	static const char* WINDOW_VSYNC_VAR;
	static const char* MUSIC_VOLUME_VAR;
    static const char* SFX_VOLUME_VAR;
    static const char* INVERT_BALL_BOOST_VAR;
    static const char* BALL_BOOST_MODE_VAR;
    static const char* DIFFICULTY_VAR;

	// Default values for each initialization variable
	static const int  DEFAULT_WINDOW_WIDTH;
	static const int  DEFAULT_WINDOW_HEIGHT;
	static const bool DEFAULT_FULLSCREEN_TOGGLE;
	static const bool DEFAULT_VSYNC_TOGGLE;
	static const int  DEFAULT_MUSIC_VOLUME;
    static const int  DEFAULT_SFX_VOLUME;
    static const bool DEFAULT_INVERT_BALL_BOOST_TOGGLE;
    static const BallBoostModel::BallBoostMode DEFAULT_BALL_BOOST_MODE;
    static const GameModel::Difficulty DEFAULT_DIFFICULTY;

	// Variables for storing the configuration values in memory
	int windowWidth, windowHeight;
	bool fullscreenIsOn;
	bool vSyncIsOn;
	int musicVolume;
    int sfxVolume;
    bool invertBallBoost;
    BallBoostModel::BallBoostMode ballBoostMode;
    GameModel::Difficulty difficulty;

    static bool BallBoostModeToString(const BallBoostModel::BallBoostMode& mode, std::string& modeStr);
    static bool StringToBallBoostMode(const std::string& modeStr, BallBoostModel::BallBoostMode& mode);

    static bool DifficultyToString(const GameModel::Difficulty& difficulty, std::string& difficultyStr);
    static bool StringToDifficulty(const std::string& difficultyStr, GameModel::Difficulty& difficulty);

	static ConfigOptions* ReadConfigOptionsFromFile(bool arcadeMode);
	bool WriteConfigOptionsToFile() const;

    static const char* SLINGSHOT_STR;
    static const char* PRESS_TO_RELEASE_STR;
        
    static const char* EASY_DIFFICULTY_STR;
    static const char* MEDIUM_DIFFICULTY_STR;
    static const char* HARD_DIFFICULTY_STR;
};

#endif