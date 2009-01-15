#include "GameModelConstants.h"

GameModelConstants* GameModelConstants::Instance = NULL;

GameModelConstants::GameModelConstants() :
RESOURCE_DIR("resources"),
WORLD_DIR("worlds"),
PROB_OF_ITEM_DROP(0.10),
PROB_OF_CONSECTUIVE_ITEM_DROP(0.80),
INITIAL_WORLD_NUM(0),
DEFAULT_BLOCKS_HIT(0),
INIT_SCORE(0),
INIT_LIVES(3)
{
	this->WORLD0_PATH = this->RESOURCE_DIR + "/" + this->WORLD_DIR	+ "/0_deco/0_deco.wld";
	this->WORLD1_PATH = this->RESOURCE_DIR + "/" + this->WORLD_DIR	+ "/1_cyberpunk/1_cyberpunk.wld";
	this->InitConstArrays();
}

void GameModelConstants::InitConstArrays() {
	this->WORLD_PATHS.push_back(WORLD0_PATH);
	this->WORLD_PATHS.push_back(WORLD1_PATH);
}