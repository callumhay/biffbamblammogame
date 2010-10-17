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

	// Getter functions for the item texture mappings
	const std::map<GameItem::ItemType, std::string>& GetItemTextures() const { return this->itemTextures; }
	const std::map<GameItem::ItemType, std::string>& GetItemTimerTextures() const { return this->itemTimerTextures; }
	const std::map<GameItem::ItemType, std::string>& GetItemTimerFillerTextures() const { return this->itemTimerFillerTextures; }

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

	const char* TEXTURE_STARFIELD;
	
	const char* TEXTURE_INKSPLATTER;
	const char* TEXTURE_BALLTARGET;
	const char* TEXTURE_BULLET_CROSSHAIR;
	const char* TEXTURE_BEAM_CROSSHAIR;
	const char* TEXTURE_BARREL_OVERLAY;

	const char* TEXTURE_BALL_SAFETY_NET;

	const char* TEXTURE_CEL_GRADIENT;
	const char* TEXTURE_NOISE_OCTAVES;

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

	// Maps of the item identifiers and their corresponding textures for
	// the items that drop in the game and timers and their fillers
	std::map<GameItem::ItemType, std::string> itemTextures;
	std::map<GameItem::ItemType, std::string> itemTimerTextures;
	std::map<GameItem::ItemType, std::string> itemTimerFillerTextures;

	// Initializer methods for filling the item texture arrays
	void InitItemTextures();
	void InitItemTimerTextures();
	void InitItemTimerFillerTextures();
	
	// Item drop, timer and filler textures ************************
	const char* TEXTURE_ITEM_SLOWBALL;
	const char* TEXTURE_ITEM_FASTBALL;
	const char* TEXTURE_ITEM_UBERBALL;
	const char* TEXTURE_ITEM_INVISIBALL;
	const char* TEXTURE_ITEM_GHOSTBALL;
	const char* TEXTURE_ITEM_PADDLELASER;
	const char* TEXTURE_ITEM_MULTIBALL3;
	const char* TEXTURE_ITEM_MULTIBALL5;
	const char* TEXTURE_ITEM_PADDLEGROW;
	const char* TEXTURE_ITEM_PADDLESHRINK;
	const char* TEXTURE_ITEM_BALLGROW;
	const char* TEXTURE_ITEM_BALLSHRINK;
	const char* TEXTURE_ITEM_BLACKOUT;
	const char* TEXTURE_ITEM_UPSIDEDOWN;
	const char* TEXTURE_ITEM_BALLSAFETYNET;
	const char* TEXTURE_ITEM_1UP;
	const char* TEXTURE_ITEM_POISON;
	const char* TEXTURE_ITEM_STICKYPADDLE;
	const char* TEXTURE_ITEM_PADDLECAM;
	const char* TEXTURE_ITEM_BALLCAM;
	const char* TEXTURE_ITEM_PADDLEBEAM;
	const char* TEXTURE_ITEM_GRAVITYBALL;
	const char* TEXTURE_ITEM_PADDLEROCKET;
	const char* TEXTURE_ITEM_CRAZYBALL;
	const char* TEXTURE_ITEM_SHIELDPADDLE;
	const char* TEXTURE_ITEM_RANDOM;

	const char* TEXTURE_ITEM_TIMER_SLOWBALL;
	const char* TEXTURE_ITEM_TIMER_FASTBALL;
	const char* TEXTURE_ITEM_TIMER_UBERBALL;
	const char* TEXTURE_ITEM_TIMER_INVISIBALL;
	const char* TEXTURE_ITEM_TIMER_GHOSTBALL;
	const char* TEXTURE_ITEM_TIMER_PADDLELASER;
	const char* TEXTURE_ITEM_TIMER_BLACKOUT;
	const char* TEXTURE_ITEM_TIMER_UPSIDEDOWN;
	const char* TEXTURE_ITEM_TIMER_POISON;
	const char* TEXTURE_ITEM_TIMER_STICKYPADDLE;
	const char* TEXTURE_ITEM_TIMER_EYE;
	const char* TEXTURE_ITEM_TIMER_GRAVITYBALL;
	const char* TEXTURE_ITEM_TIMER_SHIELDPADDLE;
	
	const char* TEXTURE_ITEM_TIMER_FILLER_SPDBALL;
	const char* TEXTURE_ITEM_TIMER_FILLER_UBERBALL;
	const char* TEXTURE_ITEM_TIMER_FILLER_INVISIBALL;
	const char* TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL;
	const char* TEXTURE_ITEM_TIMER_FILLER_PADDLELASER;
	const char* TEXTURE_ITEM_TIMER_FILLER_BLACKOUT;
	const char* TEXTURE_ITEM_TIMER_FILLER_UPSIDEDOWN;
	const char* TEXTURE_ITEM_TIMER_FILLER_POISON;
	const char* TEXTURE_ITEM_TIMER_FILLER_STICKYPADDLE;
	const char* TEXTURE_ITEM_TIMER_FILLER_EYE;
	const char* TEXTURE_ITEM_TIMER_FILLER_GRAVITYBALL;
	const char* TEXTURE_ITEM_TIMER_FILLER_SHIELDPADDLE;

	// Disallow copy and assign
	GameViewConstants(const GameViewConstants& g);
	GameViewConstants& operator=(const GameViewConstants& g);
};
#endif