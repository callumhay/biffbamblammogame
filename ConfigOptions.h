#ifndef __CONFIGOPTIONS_H__
#define __CONFIGOPTIONS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "ResourceManager.h"

/**
 * Stores data for initializing video, audio, etc. options,
 * created by the resource manager on loading the game's .ini file.
 */
class ConfigOptions {
private:
	// Filepath to the game's .ini file
	static const std::string INI_FILEPATH;

	// The variable (initialization string) names for the various values in the .ini file
	static const std::string WINDOW_HEIGHT_VAR;
	static const std::string WINDOW_WIDTH_VAR;
	static const std::string WINDOW_FULLSCREEN_VAR;
	static const std::string WINDOW_VSYNC_VAR;

	// Default values for each initialization variable
	static const int  DEFAULT_WINDOW_WIDTH;
	static const int  DEFAULT_WINDOW_HEIGHT;
	static const bool DEFAULT_FULLSCREEN_TOGGLE;
	static const bool DEFAULT_VSYNC_TOGGLE;

	// Boundry values for each variable
	static const int MIN_WINDOW_SIZE;
	static const int MAX_WINDOW_SIZE; 

	// Variables for storing the configuration values in memory
	int windowWidth, windowHeight;
	bool fullscreenIsOn;
	bool vSyncIsOn;

	static ConfigOptions* ReadConfigOptionsFromFile();
	bool WriteConfigOptionsToFile() const;

public:
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

	// Setter functions for all the configuration options
	inline void SetWindowWidth(int width) { 
		assert(width >= MIN_WINDOW_SIZE && width <= MAX_WINDOW_SIZE);
		this->windowWidth = width;
	}
	inline void SetWindowHeight(int height) { 
		assert(height >= MIN_WINDOW_SIZE && height <= MAX_WINDOW_SIZE);
		this->windowHeight = height;
	}
	void SetResolutionByString(const std::string& resStr);
	inline void SetIsFullscreenOn(bool isOn) { this->fullscreenIsOn = isOn; }
	inline void SetIsVSyncOn(bool isOn) { this->vSyncIsOn = isOn; }

	// Befriend the reading and writing functions in the resource manager - we want the
	// resource manager to be able to use this class easily
	friend ConfigOptions ResourceManager::ReadConfigurationOptions(bool forceReadFromFile);
	friend bool ResourceManager::WriteConfigurationOptionsToFile(const ConfigOptions& configOptions);
};

#endif