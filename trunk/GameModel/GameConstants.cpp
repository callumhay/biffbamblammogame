#include "GameConstants.h"

// Set all the constants
const std::string GameConstants::RESOURCE_DIR = "resources";
const std::string GameConstants::WORLD_DIR		= "worlds";

const std::string GameConstants::WORLD0_PATH = GameConstants::RESOURCE_DIR + "/" + GameConstants::WORLD_DIR	+ "/0_deco/0_deco.wld";
const std::string GameConstants::WORLD1_PATH = GameConstants::RESOURCE_DIR + "/" + GameConstants::WORLD_DIR	+ "/1_cyberpunk/1_cyberpunk.wld";

const float GameConstants::PROB_OF_ITEM_DROP = 0.2f;	

std::vector<std::string> GameConstants::WORLD_PATHS;
bool GameConstants::isInit = GameConstants::Init();