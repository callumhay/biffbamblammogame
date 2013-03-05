/**
 * GameModelConstants.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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
PROB_OF_ITEM_DROP(0.14987654321),
PROB_OF_CONSECTUIVE_ITEM_DROP(0.85555555),
PROB_OF_CONSECUTIVE_SAME_ITEM_DROP(0.10),
MAX_LIVE_ITEMS(3),
PROB_OF_GHOSTBALL_BLOCK_MISS(0.322222222),
LENGTH_OF_GHOSTMODE(1.0),
INITIAL_WORLD_NUM(0),

INIT_SCORE(0),
INIT_LIVES_LEFT(3),

MAXIMUM_POSSIBLE_LIVES(3),

DEFAULT_DAMAGE_ON_BALL_HIT(50),
MIN_BALL_DAMAGE_MULTIPLIER(0.1f),
MAX_BALL_DAMAGE_MULTIPLIER(3.0f),
UBER_BALL_DAMAGE_MULTIPLIER(1.6f),

FIRE_DAMAGE_PER_SECOND(33),
MIN_FIRE_GLOB_DROP_CHANCE_INTERVAL(4.0),
MAX_FIRE_GLOB_DROP_CHANCE_INTERVAL(8.0),
FIRE_GLOB_CHANCE_MOD(2),

TWO_TIMES_MULTIPLIER_NUM_BLOCKS(3),
THREE_TIMES_MULTIPLIER_NUM_BLOCKS(6),
FOUR_TIMES_MULTIPLIER_NUM_BLOCKS(9),

MAX_MULTIPLIER(4),

GHOST_BALL_COLOUR(0.643f, 0.725f, 0.843f),
UBER_BALL_COLOUR(1.0f, 0.0f, 0.0f),
GRAVITY_BALL_COLOUR(0.75f, 0.24f, 1.0f),
CRAZY_BALL_COLOUR(0.93f, 0.0f, 0.93f),
FIRE_BALL_COLOUR(1.0f, 0.7f, 0.0f),
ICE_BALL_COLOUR(0.40f, 0.72f, 1.0f),
OMNI_LASER_BALL_COLOUR(0.5f, 0.85f, 0.9f),

SHIELD_PADDLE_COLOUR(0.53f, 0.81f, 1.0f) {
}

GameModelConstants::~GameModelConstants() {
}