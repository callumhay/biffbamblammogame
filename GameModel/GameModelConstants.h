/**
 * GameModelConstants.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	const double PROB_OF_ITEM_DROP;                  // Probability multiplier used for an item that drops from a destroyed block
	const double PROB_OF_CONSECTUIVE_ITEM_DROP;      // Probability multiplier used for consecutive item drops
    const double PROB_OF_CONSECUTIVE_SAME_ITEM_DROP; // Probability multiplier used for consecutive drops of the same type of item
	const unsigned int MAX_LIVE_ITEMS;               // Maximum number of items dropping simulataneously
	
	const double PROB_OF_GHOSTBALL_BLOCK_MISS;	// Probablity the ball will go into ghost mode when it hits a block
	const double LENGTH_OF_GHOSTMODE;           // Length of time in seconds of ghost mode

	const int INIT_SCORE;                       // Initial player score when starting a game
	const int INIT_LIVES_LEFT;                  // Initial player lives when starting a game

    const int MAXIMUM_POSSIBLE_LIVES;           // Max player lives possible at any given time during game play

	const int FIRE_DAMAGE_PER_SECOND;               // The damage that fire does per second
	const double MIN_FIRE_GLOB_DROP_CHANCE_INTERVAL;    // The min interval of time in which a fire glob will be dropped from a block
    const double MAX_FIRE_GLOB_DROP_CHANCE_INTERVAL;    // The maximum interval of time in which a fire glob will be dropped from a block
	const int FIRE_GLOB_CHANCE_MOD;                 // The 1/x where x is the value of this, of a fire glob dropping after FIRE_GLOB_DROP_CHANCE_INTERVAL seconds

    const int DEFAULT_DAMAGE_ON_BALL_HIT;    // Default damage that a ball tends to do when it hits stuff
    const float MIN_BALL_DAMAGE_MULTIPLIER;  // The minimum damage multiplier a ball can have
    const float MAX_BALL_DAMAGE_MULTIPLIER;  // The maximum damage multiplier a ball can have
    const float UBER_BALL_DAMAGE_MULTIPLIER; // Multiplier damage that the uberball adds to a ball

    const int DEFAULT_DAMAGE_ON_PADDLE_HIT;  // Default damage that a paddle tends to do when it hits stuff
    const float MIN_PADDLE_DAMAGE_MULTIPLIER;  // The minimum damage multiplier a paddle can have
    const float MAX_PADDLE_DAMAGE_MULTIPLIER;  // The maximum damage multiplier a paddle can have

    const int MAX_MULTIPLIER;

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

    const Colour PADDLE_LASER_BEAM_COLOUR;
    const Colour BOSS_LASER_BEAM_COLOUR;

    const Colour TESLA_LIGHTNING_BRIGHT_COLOUR;
    const Colour TESLA_LIGHTNING_MEDIUM_COLOUR;
    const Colour TESLA_LIGHTNING_DARK_COLOUR;

    double GenerateFireGlobDropTime() const;

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

inline double GameModelConstants::GenerateFireGlobDropTime() const {
    return this->MIN_FIRE_GLOB_DROP_CHANCE_INTERVAL + Randomizer::GetInstance()->RandomNumZeroToOne() * 
        (this->MAX_FIRE_GLOB_DROP_CHANCE_INTERVAL - this->MIN_FIRE_GLOB_DROP_CHANCE_INTERVAL);
}

inline std::string GameModelConstants::GetWorldDefinitonFilePath() const {
    return this->RESOURCE_DIR + std::string("/") + this->WORLD_DIR + std::string("/worlds");
}

#endif