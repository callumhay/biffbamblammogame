/**
 * GameModelConstants.cpp
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

#include "GameModelConstants.h"

GameModelConstants* GameModelConstants::Instance = NULL;

GameModelConstants::GameModelConstants() :
RESOURCE_DIR("resources"),
WORLD_DIR("worlds"),
PROB_OF_ITEM_DROP(0.138),
PROB_OF_CONSECTUIVE_ITEM_DROP(0.8444444),
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

DEFAULT_DAMAGE_ON_PADDLE_HIT(75),
MIN_PADDLE_DAMAGE_MULTIPLIER(0.1f),
MAX_PADDLE_DAMAGE_MULTIPLIER(3.0f),

FIRE_DAMAGE_PER_SECOND(27),
MIN_FIRE_GLOB_DROP_CHANCE_INTERVAL(3.5),
MAX_FIRE_GLOB_DROP_CHANCE_INTERVAL(7.25),
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

SHIELD_PADDLE_COLOUR(0.53f, 0.81f, 1.0f),

PADDLE_LASER_BEAM_COLOUR(0.33f, 1.0f, 1.0f),
BOSS_LASER_BEAM_COLOUR(1.0f, 0.0f, 0.0f),

TESLA_LIGHTNING_BRIGHT_COLOUR(0.99f, 0.9f, 1.0f),
TESLA_LIGHTNING_MEDIUM_COLOUR(0.9f, 0.75f, 1.0f),
TESLA_LIGHTNING_DARK_COLOUR(0.8f, 0.7f, 1.0f)
{
}

GameModelConstants::~GameModelConstants() {
}