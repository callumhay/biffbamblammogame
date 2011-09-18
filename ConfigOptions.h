/**
 * ConfigOptions.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CONFIGOPTIONS_H__
#define __CONFIGOPTIONS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../GameModel/GameModel.h"
#include "ResourceManager.h"


/**
 * Stores data for initializing video, audio, etc. options,
 * created by the resource manager on loading the game's .ini file.
 */
class ConfigOptions {
	// Befriend the reading and writing functions in the resource manager - we want the
	// resource manager to be able to use this class easily
	friend ConfigOptions ResourceManager::ReadConfigurationOptions(bool forceReadFromFile);
	friend bool ResourceManager::WriteConfigurationOptionsToFile(const ConfigOptions& configOptions);

public:
	// Boundry values for each variable
	static const int MIN_WINDOW_SIZE;
	static const int MAX_WINDOW_SIZE;
	static const int MIN_VOLUME;
	static const int MAX_VOLUME;

	ConfigOptions();
	~ConfigOptions() {}
	
	// Accessor functions for all of the configuration options
	inline int  GetWindowWidth() const { return this->windowWidth; }
	inline int  GetWindowHeight() const { return this->windowHeight; }
	inline std::string GetResolutionString() const { 
		std::stringstream resolutionStr;
		resolutionStr << this->windowWidth << " x " << this->windowHeight;
		return resolutionStr.str();
	}

	inline bool GetIsFullscreenOn() const { return this->fullscreenIsOn; }
	inline bool GetIsVSyncOn() const { return this->vSyncIsOn; }
	inline int GetVolume() const { return this->volume; }
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
	inline void SetVolume(int volume) {
		assert(volume >= MIN_VOLUME && volume <= MAX_VOLUME);
		this->volume = std::max<int>(MIN_VOLUME, std::min<int>(MAX_VOLUME, volume));
	}
    inline void SetDifficulty(GameModel::Difficulty difficulty) {
        this->difficulty = difficulty;
    }

    static std::vector<std::string> GetDifficultyItems() {
        std::vector<std::string> difficultyItems;
        difficultyItems.reserve(3);
        difficultyItems.push_back("Easy");
        difficultyItems.push_back("Medium");
        difficultyItems.push_back("Hard");
        return difficultyItems;
    }

private:
	// Filepath to the game's .ini file
	static const char* INI_FILEPATH;

	// The variable (initialization string) names for the various values in the .ini file
	static const char* WINDOW_HEIGHT_VAR;
	static const char* WINDOW_WIDTH_VAR;
	static const char* WINDOW_FULLSCREEN_VAR;
	static const char* WINDOW_VSYNC_VAR;
	static const char* VOLUME_VAR;
    static const char* DIFFICULTY_VAR;

	// Default values for each initialization variable
	static const int  DEFAULT_WINDOW_WIDTH;
	static const int  DEFAULT_WINDOW_HEIGHT;
	static const bool DEFAULT_FULLSCREEN_TOGGLE;
	static const bool DEFAULT_VSYNC_TOGGLE;
	static const int  DEFAULT_VOLUME;
    static const GameModel::Difficulty DEFAULT_DIFFICULTY;

	// Variables for storing the configuration values in memory
	int windowWidth, windowHeight;
	bool fullscreenIsOn;
	bool vSyncIsOn;
	int volume;
    GameModel::Difficulty difficulty;

	static ConfigOptions* ReadConfigOptionsFromFile();
	bool WriteConfigOptionsToFile() const;

    static const char* EASY_DIFFICULTY_STR;
    static const char* MEDIUM_DIFFICULTY_STR;
    static const char* HARD_DIFFICULTY_STR;

    static bool DifficultyToString(const GameModel::Difficulty& difficulty, std::string& difficultyStr);
    static bool StringToDifficulty(const std::string& difficultyStr, GameModel::Difficulty& difficulty);
};

#endif