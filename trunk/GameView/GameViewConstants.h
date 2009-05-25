#ifndef __GAMEVIEWCONSTANTS_H__
#define __GAMEVIEWCONSTANTS_H__

#include <string>
#include "../BlammoEngine/Colour.h"

class GameViewConstants {

private:
	static GameViewConstants* Instance;

	void InitConstants();
	GameViewConstants();
	~GameViewConstants();

public:
	
	// Obtain the singleton
	static GameViewConstants* GetInstance() {
		if (GameViewConstants::Instance == NULL) {
			GameViewConstants::Instance = new GameViewConstants();
		}
		return GameViewConstants::Instance;
	}
	// Delete the singleton
	static void DeleteInstance() {
		if (GameViewConstants::Instance != NULL) {
			delete GameViewConstants::Instance;
			GameViewConstants::Instance = NULL;
		}
	}

	// Constant listing...

	// Asset file path constants ***************************
	const std::string RESOURCE_DIR;
	const std::string FONT_DIR;
	const std::string MESH_DIR;
	const std::string SHADER_DIR;
	const std::string TEXTURE_DIR;

	// Shader path constants ********************************
	const std::string CGFX_CEL_SHADER;
	const std::string CGFX_PHONG_SHADER;
	const std::string CGFX_POSTREFRACT_SHADER;
	const std::string CGFX_VOLUMEMETRIC_SHADER;

	// Texture path constants *******************************
	const std::string TEXTURE_BANG1;
	const std::string TEXTURE_BANG2;
	const std::string TEXTURE_BANG3;
	const std::string TEXTURE_SMOKE1;
	const std::string TEXTURE_SMOKE2;
	const std::string TEXTURE_SMOKE3;
	const std::string TEXTURE_SMOKE4;
	const std::string TEXTURE_SMOKE5;
	const std::string TEXTURE_SMOKE6;
	const std::string TEXTURE_CIRCLE_GRADIENT;
	const std::string TEXTURE_STAR;
	const std::string TEXTURE_STAR_OUTLINE;
	const std::string TEXTURE_EXPLOSION_CLOUD;
	const std::string TEXTURE_EXPLOSION_RAYS;
	const std::string TEXTURE_LASER_BEAM;
	const std::string TEXTURE_UP_ARROW;
	const std::string TEXTURE_BALL_LIFE_HUD;
	const std::string TEXTURE_SPARKLE;

	const std::string TEXTURE_BALL_SAFETY_NET;

	const std::string TEXTURE_ITEM_SLOWBALL;
	const std::string TEXTURE_ITEM_FASTBALL;
	const std::string TEXTURE_ITEM_UBERBALL;
	const std::string TEXTURE_ITEM_INVISIBALL;
	const std::string TEXTURE_ITEM_GHOSTBALL;
	const std::string TEXTURE_ITEM_PADDLELASER;
	const std::string TEXTURE_ITEM_MULTIBALL3;
	const std::string TEXTURE_ITEM_MULTIBALL5;
	const std::string TEXTURE_ITEM_PADDLEGROW;
	const std::string TEXTURE_ITEM_PADDLESHRINK;
	const std::string TEXTURE_ITEM_BALLGROW;
	const std::string TEXTURE_ITEM_BALLSHRINK;
	const std::string TEXTURE_ITEM_BLACKOUT;
	const std::string TEXTURE_ITEM_UPSIDEDOWN;
	const std::string TEXTURE_ITEM_BALLSAFETYNET;
	const std::string TEXTURE_ITEM_1UP;

	const std::string TEXTURE_ITEM_TIMER_SLOWBALL;
	const std::string TEXTURE_ITEM_TIMER_FASTBALL;
	const std::string TEXTURE_ITEM_TIMER_UBERBALL;
	const std::string TEXTURE_ITEM_TIMER_INVISIBALL;
	const std::string TEXTURE_ITEM_TIMER_GHOSTBALL;
	const std::string TEXTURE_ITEM_TIMER_PADDLELASER;
	const std::string TEXTURE_ITEM_TIMER_BLACKOUT;
	const std::string TEXTURE_ITEM_TIMER_UPSIDEDOWN;

	const std::string TEXTURE_ITEM_TIMER_FILLER_SPDBALL;
	const std::string TEXTURE_ITEM_TIMER_FILLER_UBERBALL;
	const std::string TEXTURE_ITEM_TIMER_FILLER_INVISIBALL;
	const std::string TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL;
	const std::string TEXTURE_ITEM_TIMER_FILLER_PADDLELASER;
	const std::string TEXTURE_ITEM_TIMER_FILLER_BLACKOUT;
	const std::string TEXTURE_ITEM_TIMER_FILLER_UPSIDEDOWN;

	// Font path constants **********************************
	const std::string FONT_HAPPYGOOD;
	const std::string FONT_EXPLOSIONBOOM;
	const std::string FONT_ELECTRICZAP;
	const std::string FONT_ALLPURPOSE;

	// Item-related constants *******************************
	const std::string ITEM_LABEL_MATGRP;
	const std::string ITEM_END_MATGRP;

	const Colour ITEM_GOOD_COLOUR;
	const Colour ITEM_BAD_COLOUR;
	const Colour ITEM_NEUTRAL_COLOUR;
	
	const float DEFAULT_BALL_LIGHT_ATTEN;
	const Colour DEFAULT_FG_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_FG_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BG_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_BG_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BALL_LIGHT_COLOUR;
	const Colour GHOST_BALL_COLOUR;
	const Colour UBER_BALL_COLOUR;

	// World specific path constants ************************
	const std::string BALL_MESH;
	const std::string SPIKEY_BALL_MESH;
	const std::string BASIC_BLOCK_MESH_PATH;
	const std::string TRIANGLE_BLOCK_MESH_PATH;
	const std::string BOMB_BLOCK_MESH;
	const std::string SKYBOX_MESH;
	const std::string ITEM_MESH;
	const std::string PADDLE_LASER_ATTACHMENT_MESH;

	// DECO WORLD
	const std::string DECO_PADDLE_MESH;
	const std::string DECO_BACKGROUND_MESH;
	const std::string DECO_SKYBEAM_MESH;
	const std::string DECO_BLOCK_MESH_PATH;


};
#endif