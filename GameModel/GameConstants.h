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
	static const float PROB_OF_ITEM_DROP;			// Probability that an item will drop from a destroyed block
	static const int DEFAULT_BLOCKS_HIT = 0;	// Initial number of blocks hit by a ball as it leaves the paddle
	static const int INIT_SCORE = 0;					// Initial player score when starting a game
	static const int INIT_LIVES = 3;					// Initial player lives when starting a game
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