#ifndef __GAMECONSTANTS_H__
#define __GAMECONSTANTS_H__

#include <string>
#include <vector>

class GameModelConstants {

private:
	static GameModelConstants* Instance;

	void InitConstArrays();

	// World paths
	std::string WORLD0_PATH;
	std::string WORLD1_PATH;

	GameModelConstants();
	~GameModelConstants(){};

public:
	
	static GameModelConstants* GetInstance() {
		if (GameModelConstants::Instance == NULL) {
			GameModelConstants::Instance = new GameModelConstants();
		}
		return GameModelConstants::Instance;
	}

	static void DeleteInstance() {
		if (GameModelConstants::Instance != NULL) {
			delete GameModelConstants::Instance;
			GameModelConstants::Instance = NULL;
		}
	}

	// FILE CONSTANTS ----------------------------------
	// Basic path stuffs
	const std::string RESOURCE_DIR;
	const std::string WORLD_DIR;

	// World path/file related constants
	std::vector<std::string> WORLD_PATHS;
	const int INITIAL_WORLD_NUM;
	// -------------------------------------------------

	// IN-GAME CONSTANTS -------------------------------
	const double PROB_OF_ITEM_DROP;							// Probability multiplier used for an item that drops from a destroyed block
	const double PROB_OF_CONSECTUIVE_ITEM_DROP;	// Probability multiplier used for consecutive item drops
	const int DEFAULT_BLOCKS_HIT;								// Initial number of blocks hit by a ball as it leaves the paddle
	const int INIT_SCORE;												// Initial player score when starting a game
	const int INIT_LIVES;												// Initial player lives when starting a game
	// -------------------------------------------------

};

#endif