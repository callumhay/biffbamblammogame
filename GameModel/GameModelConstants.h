/**
 * GameModelConstants.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

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

    std::string GetResourceWorldDir() const;
    std::string GetWorldDefinitonFilePath() const;

	// World path/file related constants
	const int INITIAL_WORLD_NUM;
	// -------------------------------------------------

	// IN-GAME CONSTANTS -------------------------------
	const double PROB_OF_ITEM_DROP;             // Probability multiplier used for an item that drops from a destroyed block
	const double PROB_OF_CONSECTUIVE_ITEM_DROP;	// Probability multiplier used for consecutive item drops
	const unsigned int MAX_LIVE_ITEMS;          // Maximum number of items dropping simulataneously
	
	const double PROB_OF_GHOSTBALL_BLOCK_MISS;	// Probablity the ball will go into ghost mode when it hits a block
	const double LENGTH_OF_GHOSTMODE;           // Length of time in seconds of ghost mode

	const int INIT_SCORE;                       // Initial player score when starting a game
	const int INIT_LIVES_LEFT;                  // Initial player lives when starting a game
	const int MAX_LIVES_LEFT;                   // Maximum number of lives that a player may have at a given time
	
	const int FIRE_DAMAGE_PER_SECOND;               // The damage that fire does per second
	const double FIRE_GLOB_DROP_CHANCE_INTERVAL;    // The interval of time in seconds over which a fire glob might be dropped from a block
	const int FIRE_GLOB_CHANCE_MOD;                 // The 1/x where x is the value of this, of a fire glob dropping after FIRE_GLOB_DROP_CHANCE_INTERVAL seconds

    const int TWO_TIMES_MULTIPLIER_NUM_BLOCKS;      // The number of blocks that must be destroyed between paddle hits to get a 2x multiplier
    const int THREE_TIMES_MULTIPLIER_NUM_BLOCKS;    // ... 3x multiplier
    const int FOUR_TIMES_MULTIPLIER_NUM_BLOCKS;     // ... 4x multiplier

	// -------------------------------------------------

	// BALL COLOURS
	const Colour GHOST_BALL_COLOUR;
	const Colour UBER_BALL_COLOUR;
	const Colour GRAVITY_BALL_COLOUR;
	const Colour CRAZY_BALL_COLOUR;
	const Colour FIRE_BALL_COLOUR;
	const Colour ICE_BALL_COLOUR;
    const Colour OMNI_LASER_BALL_COLOUR;

	const Colour SHIELD_PADDLE_COLOUR;

private:
	static GameModelConstants* Instance;

	GameModelConstants();
	~GameModelConstants();

	// Disallow copy and assignment
    DISALLOW_COPY_AND_ASSIGN(GameModelConstants);
};

inline std::string GameModelConstants::GetResourceWorldDir() const {
    return this->RESOURCE_DIR + std::string("/") + this->WORLD_DIR;
}
inline std::string GameModelConstants::GetWorldDefinitonFilePath() const {
    return this->RESOURCE_DIR + std::string("/") + this->WORLD_DIR + std::string("/worlds");
}

#endif