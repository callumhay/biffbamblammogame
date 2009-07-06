#include "ConfigOptions.h"

const std::string ConfigOptions::INI_FILEPATH				= "BiffBamBlammo.ini";

const std::string ConfigOptions::WINDOW_HEIGHT_VAR			= "window_height";
const std::string ConfigOptions::WINDOW_WIDTH_VAR				= "window_width";
const std::string ConfigOptions::WINDOW_FULLSCREEN_VAR	= "fullscreen";
const std::string ConfigOptions::WINDOW_VSYNC_VAR				= "vsync";

const int  ConfigOptions::DEFAULT_WINDOW_WIDTH				= 1024;
const int  ConfigOptions::DEFAULT_WINDOW_HEIGHT				= 768;
const bool ConfigOptions::DEFAULT_FULLSCREEN_TOGGLE		= false;
const bool ConfigOptions::DEFAULT_VSYNC_TOGGLE				= false;

const int ConfigOptions::MIN_WINDOW_SIZE	= 480;
const int ConfigOptions::MAX_WINDOW_SIZE	= 2048;

ConfigOptions::ConfigOptions() : windowWidth(DEFAULT_WINDOW_WIDTH), windowHeight(DEFAULT_WINDOW_HEIGHT),
fullscreenIsOn(DEFAULT_FULLSCREEN_TOGGLE), vSyncIsOn(DEFAULT_VSYNC_TOGGLE) {
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

/**
 * Reads the configuration options from the .ini file on disk.
 * Returns: A heap object pointer to a set of configurations created from reading
 * the .ini file on disk, NULL otherwise.
 */
ConfigOptions* ConfigOptions::ReadConfigOptionsFromFile() {

	// Open the .ini file off disk
	std::ifstream inFile;
	inFile.open(ConfigOptions::INI_FILEPATH.c_str());
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

	while (inFile >> currStr) {
		
		// Window width config
		if (currStr == ConfigOptions::WINDOW_WIDTH_VAR) {
			inFile >> skipEquals;

			// Read the window width
			int tempWinWidth = ConfigOptions::DEFAULT_WINDOW_WIDTH;
			inFile >> tempWinWidth;

			if (tempWinWidth <= ConfigOptions::MAX_WINDOW_SIZE && tempWinWidth >= ConfigOptions::MIN_WINDOW_SIZE) {
				cfgOptions->windowWidth = tempWinWidth;
			}
		}
		// Window height config
		else if (currStr == ConfigOptions::WINDOW_HEIGHT_VAR) {
			inFile >> skipEquals;

			// Read the window width
			int tempWinHeight = ConfigOptions::DEFAULT_WINDOW_HEIGHT;
			inFile >> tempWinHeight;

			if (tempWinHeight <= ConfigOptions::MAX_WINDOW_SIZE && tempWinHeight >= ConfigOptions::MIN_WINDOW_SIZE) {
				cfgOptions->windowHeight = tempWinHeight;
			}
		}
		// Fullscreen config
		else if (currStr == ConfigOptions::WINDOW_FULLSCREEN_VAR) {
			inFile >> skipEquals;

			// Read whether the window is fullscreen or not
			int isFullscreen = ConfigOptions::DEFAULT_FULLSCREEN_TOGGLE ? 1 : 0;
			inFile >> isFullscreen;

			if (isFullscreen == 0) {
				cfgOptions->fullscreenIsOn = false;
			}
			else {
				cfgOptions->fullscreenIsOn = true;
			}
		}
		// Vertical Sync config
		else if (currStr == ConfigOptions::WINDOW_VSYNC_VAR) {
			inFile >> skipEquals;

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
	}
	
	inFile.close();
	return cfgOptions;
}

/**
 * Writes the currently stored configuration values from memory to file.
 * Returns: true on success, false otherwise.
 */
bool ConfigOptions::WriteConfigOptionsToFile() const {
	// Open or create the .ini file for writing
	std::ofstream outFile;
	outFile.open(ConfigOptions::INI_FILEPATH.c_str());
	if (!outFile.is_open()) {
		// Couldn't open for writing...
		outFile.close();
		return false;
	}

	// Write all the configuration values (and comments along with them):
	// Write a description of what this file is for...
	outFile << "// This is the Biff! Bam!! Blammo!?! game configuration file, it is used to initialize"	<< std::endl;
	outFile << "// various game options (video, audio, etc.) at startup. To change the options you may"	<< std::endl; 
	outFile << "// configure them manually in-game via the \"Options\" menu or manually, by following"	<< std::endl;
	outFile << "// the guidelines below."																																<< std::endl;
	outFile << "// Remember to always leave spaces between '=' characters!"															<< std::endl;
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

	outFile.close();
	return true;
}