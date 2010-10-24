/**
 * GameModelConstants.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameModelConstants.h"

GameModelConstants* GameModelConstants::Instance = NULL;

GameModelConstants::GameModelConstants() :
RESOURCE_DIR("resources"),
WORLD_DIR("worlds"),
PROB_OF_ITEM_DROP(0.23),
PROB_OF_CONSECTUIVE_ITEM_DROP(0.90),
MAX_LIVE_ITEMS(5),
PROB_OF_GHOSTBALL_BLOCK_MISS(0.33),
LENGTH_OF_GHOSTMODE(1.0),
INITIAL_WORLD_NUM(0),
DEFAULT_BLOCKS_HIT(0),

INIT_SCORE(0),
INIT_LIVES_LEFT(3),
MAX_LIVES_LEFT(10),


GHOST_BALL_COLOUR(0.643f, 0.725f, 0.843f),
UBER_BALL_COLOUR(1.0f, 0.0f, 0.0f),
GRAVITY_BALL_COLOUR(0.75f, 0.24f, 1.0f),
CRAZY_BALL_COLOUR(0.93f, 0.0f, 0.93f),

SHIELD_PADDLE_COLOUR(0.53f, 0.81f, 1.0f)
{
	this->WORLD0_PATH = this->RESOURCE_DIR + "/" + this->WORLD_DIR	+ "/0_deco/0_deco.wld";
	this->WORLD1_PATH = this->RESOURCE_DIR + "/" + this->WORLD_DIR	+ "/1_cyberpunk/1_cyberpunk.wld";
	this->InitConstArrays();
}

void GameModelConstants::InitConstArrays() {
	this->WORLD_PATHS.push_back(WORLD0_PATH);
	this->WORLD_PATHS.push_back(WORLD1_PATH);
}