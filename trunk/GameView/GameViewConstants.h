#ifndef __GAMEVIEWCONSTANTS_H__
#define __GAMEVIEWCONSTANTS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Point.h"

#include "../GameModel/GameItem.h"

class GameViewConstants {
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
	const char* RESOURCE_DIR;
	const char* FONT_DIR;
	const char* MESH_DIR;
	const char* SHADER_DIR;
	const char* TEXTURE_DIR;
	const char* SOUND_DIR;

	// Shader path constants ********************************
	const char* CGFX_CEL_SHADER;
	const char* CGFX_PHONG_SHADER;
	const char* CGFX_INKBLOCK_SHADER;
	const char* CGFX_POSTREFRACT_SHADER;
	const char* CGFX_VOLUMEMETRIC_SHADER;
	const char* CGFX_DECOSKYBOX_SHADER;
	const char* CGFX_GAUSSIAN_SHADER;
	const char* CGFX_AFTERIMAGE_SHADER;
	const char* CGFX_BLOOM_SHADER;
	const char* CGFX_FULLSCREENGOO_SHADER;
	const char* CGFX_STICKYPADDLE_SHADER;
	const char* CGFX_FULLSCREENSMOKEY_SHADER;
	const char* CGFX_FULLSCREEN_UBERINTENSE_SHADER;
	const char* CGFX_PRISM_SHADER;
	const char* CGFX_PORTALBLOCK_SHADER;
	const char* CGFX_FIREBALL_SHADER;
	const char* CGFX_FULLSCREEN_POSTFIREY_SHADER;

	// Texture path constants *******************************
	const char* TEXTURE_BANG1;
	const char* TEXTURE_BANG2;
	const char* TEXTURE_BANG3;
	const char* TEXTURE_SPLAT1;
	const char* TEXTURE_SMOKE1;
	const char* TEXTURE_SMOKE2;
	const char* TEXTURE_SMOKE3;
	const char* TEXTURE_SMOKE4;
	const char* TEXTURE_SMOKE5;
	const char* TEXTURE_SMOKE6;
	//const char* TEXTURE_FIRE_GLOB1;
	//const char* TEXTURE_FIRE_GLOB2;
	//const char* TEXTURE_FIRE_GLOB3;
	const char* TEXTURE_SNOWFLAKE1;
	const char* TEXTURE_SNOWFLAKE2;
	const char* TEXTURE_SNOWFLAKE3;
	const char* TEXTURE_ROCK1;
	const char* TEXTURE_ROCK2;
	const char* TEXTURE_ROCK3;
	const char* TEXTURE_ROCK4;
	const char* TEXTURE_ROCK5;
	const char* TEXTURE_CIRCLE_GRADIENT;
	const char* TEXTURE_STAR;
	const char* TEXTURE_STAR_OUTLINE;
	const char* TEXTURE_EVIL_STAR;
	const char* TEXTURE_EVIL_STAR_OUTLINE;
	const char* TEXTURE_EXPLOSION_CLOUD;
	const char* TEXTURE_EXPLOSION_RAYS;
	const char* TEXTURE_LASER_BEAM;
	const char* TEXTURE_UP_ARROW;
	const char* TEXTURE_BALL_LIFE_HUD;
	const char* TEXTURE_SPARKLE;
	const char* TEXTURE_SPIRAL_SMALL;
	const char* TEXTURE_SPIRAL_MEDIUM;
	const char* TEXTURE_SPIRAL_LARGE;
	const char* TEXTURE_HALO;
	const char* TEXTURE_LENSFLARE;
	const char* TEXTURE_TWISTED_SPIRAL;
	const char* TEXTURE_SIDEBLAST;
	const char* TEXTURE_HUGE_EXPLOSION;
	const char* TEXTURE_BRIGHT_FLARE;
	const char* TEXTURE_LIGHTNING_BOLT;
	const char* TEXTURE_SPHERE_NORMALS;
	const char* TEXTURE_RECT_PRISM_NORMALS;
	const char* TEXTURE_CLOUD;
	const char* TEXTURE_GRIT;
	const char* TEXTURE_FROST;
	const char* TEXTURE_FULLSCREEN_FROST;

	const char* TEXTURE_STARFIELD;
	
	const char* TEXTURE_INKSPLATTER;
	const char* TEXTURE_BALLTARGET;
	const char* TEXTURE_BULLET_CROSSHAIR;
	const char* TEXTURE_BEAM_CROSSHAIR;
	const char* TEXTURE_BARREL_OVERLAY;

	const char* TEXTURE_BALL_SAFETY_NET;

	const char* TEXTURE_CEL_GRADIENT;
	const char* TEXTURE_NOISE_OCTAVES;

	const char* TEXTURE_LOCKED_BLAMMOPEDIA_ENTRY;

	// Font path constants **********************************
	const char* FONT_SADBAD;
	const char* FONT_HAPPYGOOD;
	const char* FONT_EXPLOSIONBOOM;
	const char* FONT_ELECTRICZAP;
	const char* FONT_ALLPURPOSE;

	// Sound Script File (.msf) path constants *********************************
	const char* MAIN_MENU_SOUND_SCRIPT;
	const char* DECO_SOUND_SCRIPT;

	// Item-related constants *******************************
	const char* ITEM_LABEL_MATGRP;
	const char* ITEM_END_MATGRP;
	const char* ITEM_DROP_BLOCK_ITEMTYPE_MATGRP;

	const Colour ITEM_GOOD_COLOUR;
	const Colour ITEM_BAD_COLOUR;
	const Colour ITEM_NEUTRAL_COLOUR;
	
	const float DEFAULT_BALL_LIGHT_ATTEN;
	const float BLACKOUT_BALL_LIGHT_ATTEN;
	const Colour DEFAULT_PADDLE_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_PADDLE_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BALL_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_FG_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_FG_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BG_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_BG_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BALL_LIGHT_COLOUR;
	const Colour BLACKOUT_BALL_KEY_LIGHT_COLOUR;
	const Colour BLACKOUT_PADDLE_KEY_LIGHT_COLOUR;
	const Colour BLACKOUT_PADDLE_FILL_LIGHT_COLOUR;
	const Colour POISON_LIGHT_DEEP_COLOUR;
	const Colour POISON_LIGHT_LIGHT_COLOUR;
	const Colour INK_BLOCK_COLOUR;
	const Colour STICKYPADDLE_GOO_COLOUR;
	const Colour STICKYPADDLE_PLUS_BEAM_GOO_COLOUR;
	const Colour PRISM_BLOCK_COLOUR;
	const Colour LASER_BEAM_COLOUR;

	const Point3D DEFAULT_FG_KEY_LIGHT_POSITION;
	const Point3D DEFAULT_FG_FILL_LIGHT_POSITION;
	const Point3D DEFAULT_BG_KEY_LIGHT_POSITION;
	const Point3D DEFAULT_BG_FILL_LIGHT_POSITION;
	const Point3D DEFAULT_BALL_KEY_LIGHT_POSITION;
	const Point3D DEFAULT_BALL_FILL_LIGHT_POSITION;
	const Point3D DEFAULT_PADDLE_KEY_LIGHT_POSITION;
	const Point3D DEFAULT_PADDLE_FILL_LIGHT_POSITION;

	// World specific path constants ************************
	const char* BALL_MESH;
	const char* SPIKEY_BALL_MESH;
	const char* BASIC_BLOCK_MESH_PATH;
	const char* TRIANGLE_BLOCK_MESH_PATH;
	const char* BOMB_BLOCK_MESH;
	const char* PRISM_DIAMOND_BLOCK_MESH;
	const char* PRISM_TRIANGLE_BLOCK_MESH;
	const char* CANNON_BLOCK_BASE_MESH;
	const char* CANNON_BLOCK_BARREL_MESH;
	const char* COLLATERAL_BLOCK_MESH;
	const char* TESLA_BLOCK_BASE_MESH;
	const char* TESLA_BLOCK_COIL_MESH;
	const char* ITEM_DROP_BLOCK_MESH;
	const char* SKYBOX_MESH;
	const char* ITEM_MESH;
	const char* PADDLE_LASER_ATTACHMENT_MESH;
	const char* PADDLE_BEAM_ATTACHMENT_MESH;
	const char* PADDLE_STICKY_ATTACHMENT_MESH;
	const char* PADDLE_ROCKET_MESH;

	// DECO WORLD
	const char* DECO_PADDLE_MESH;
	const char* DECO_BACKGROUND_MESH;
	const char* DECO_SKYBEAM_MESH;
	const char* DECO_BLOCK_MESH_PATH;

private:
	static GameViewConstants* Instance;

	GameViewConstants();
	~GameViewConstants();

	// Disallow copy and assign
	GameViewConstants(const GameViewConstants& g);
	GameViewConstants& operator=(const GameViewConstants& g);
};
#endif