#ifndef __GAMECONSTANTS_H__
#define __GAMECONSTANTS_H__

#include "../BlammoEngine/Colour.h"

#include <string>
#include <vector>

class GameModelConstants {
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
	const unsigned int MAX_LIVE_ITEMS;					// Maximum number of items dropping simulataneously
	
	const double PROB_OF_GHOSTBALL_BLOCK_MISS;	// Probablity the ball will go into ghost mode when it hits a block
	const double LENGTH_OF_GHOSTMODE;						// Length of time in seconds of ghost mode

	const int DEFAULT_BLOCKS_HIT;								// Initial number of blocks hit by a ball as it leaves the paddle
	const int INIT_SCORE;												// Initial player score when starting a game
	const int INIT_LIVES_LEFT;									// Initial player lives when starting a game
	const int MAX_LIVES_LEFT;										// Maximum number of lives that a player may have at a given time
	// -------------------------------------------------

	// BALL COLOURS
	const Colour GHOST_BALL_COLOUR;
	const Colour UBER_BALL_COLOUR;
	const Colour GRAVITY_BALL_COLOUR;
	const Colour CRAZY_BALL_COLOUR;

	const Colour SHIELD_PADDLE_COLOUR;

private:
	static GameModelConstants* Instance;

	void InitConstArrays();

	// World paths
	std::string WORLD0_PATH;
	std::string WORLD1_PATH;

	GameModelConstants();
	~GameModelConstants(){};

	// Disallow copy and assignment
	GameModelConstants(const GameModelConstants& g);
	GameModelConstants& operator=(const GameModelConstants& g);
};

#endif