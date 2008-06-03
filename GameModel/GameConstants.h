#ifndef __GAMECONSTANTS_H__
#define __GAMECONSTANTS_H__

#include <string>
#include <vector>

class GameConstants {
public:
	
	// FILE CONSTANTS ----------------------------------
	// Basic path stuffs
	static const std::string RESOURCE_DIR;
	static const std::string WORLD_DIR;

	// World path/file related constants
	static std::vector<std::string> WORLD_PATHS;
	static const int INITIAL_WORLD_NUM = 0;
	// -------------------------------------------------

	// IN-GAME CONSTANTS -------------------------------
	static const int POINTS_PER_BLOCK_HIT = 1;
	static const int DEFAULT_SCORE_MULTIPLIER = 1;
	static const int MAX_SCORE_MULTIPLIER = 0;
	// -------------------------------------------------

private:
	GameConstants(){};

	// World paths
	static const std::string WORLD0_PATH;
	static const std::string WORLD1_PATH;

	static bool isInit;
	static bool Init() {
		GameConstants::WORLD_PATHS.push_back(WORLD0_PATH);
		GameConstants::WORLD_PATHS.push_back(WORLD1_PATH);
		return true;
	};

};

#endif