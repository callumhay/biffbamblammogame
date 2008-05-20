#ifndef __GAMECONSTANTS_H__
#define __GAMECONSTANTS_H__

#include <string>
#include <vector>

class GameConstants {
public:
	
	// World related constants
	static std::vector<std::string> WORLD_PATHS;
	static const int INITIAL_WORLD_NUM = 0;

private:
	// Basic path stuffs
	static const std::string RESOURCE_DIR;
	static const std::string WORLD_DIR;

	// World paths
	static const std::string WORLD0_PATH;

	static bool isInit;
	static bool Init() {
		GameConstants::WORLD_PATHS.push_back(WORLD0_PATH);
		return true;
	};

};

// Set all the constants
const std::string GameConstants::RESOURCE_DIR = "resources";
const std::string GameConstants::WORLD_DIR		= "worlds";

const std::string GameConstants::WORLD0_PATH = GameConstants::RESOURCE_DIR + "/" + GameConstants::WORLD_DIR	+ "/0_deco/0_deco.wld";

std::vector<std::string> GameConstants::WORLD_PATHS;
bool GameConstants::isInit = GameConstants::Init();

#endif