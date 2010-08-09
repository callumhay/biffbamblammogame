#ifndef __GAMEVIEWCONSTANTS_H__
#define __GAMEVIEWCONSTANTS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Point.h"

#include "../GameModel/GameItem.h"

class GameViewConstants {

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
	const std::string TEXTURE_ITEM_POISON;
	const std::string TEXTURE_ITEM_STICKYPADDLE;
	const std::string TEXTURE_ITEM_PADDLECAM;
	const std::string TEXTURE_ITEM_BALLCAM;
	const std::string TEXTURE_ITEM_PADDLEBEAM;
	const std::string TEXTURE_ITEM_GRAVITYBALL;
	const std::string TEXTURE_ITEM_PADDLEROCKET;
	const std::string TEXTURE_ITEM_CRAZYBALL;

	const std::string TEXTURE_ITEM_TIMER_SLOWBALL;
	const std::string TEXTURE_ITEM_TIMER_FASTBALL;
	const std::string TEXTURE_ITEM_TIMER_UBERBALL;
	const std::string TEXTURE_ITEM_TIMER_INVISIBALL;
	const std::string TEXTURE_ITEM_TIMER_GHOSTBALL;
	const std::string TEXTURE_ITEM_TIMER_PADDLELASER;
	const std::string TEXTURE_ITEM_TIMER_BLACKOUT;
	const std::string TEXTURE_ITEM_TIMER_UPSIDEDOWN;
	const std::string TEXTURE_ITEM_TIMER_POISON;
	const std::string TEXTURE_ITEM_TIMER_STICKYPADDLE;
	const std::string TEXTURE_ITEM_TIMER_EYE;
	const std::string TEXTURE_ITEM_TIMER_GRAVITYBALL;
	
	const std::string TEXTURE_ITEM_TIMER_FILLER_SPDBALL;
	const std::string TEXTURE_ITEM_TIMER_FILLER_UBERBALL;
	const std::string TEXTURE_ITEM_TIMER_FILLER_INVISIBALL;
	const std::string TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL;
	const std::string TEXTURE_ITEM_TIMER_FILLER_PADDLELASER;
	const std::string TEXTURE_ITEM_TIMER_FILLER_BLACKOUT;
	const std::string TEXTURE_ITEM_TIMER_FILLER_UPSIDEDOWN;
	const std::string TEXTURE_ITEM_TIMER_FILLER_POISON;
	const std::string TEXTURE_ITEM_TIMER_FILLER_STICKYPADDLE;
	const std::string TEXTURE_ITEM_TIMER_FILLER_EYE;
	const std::string TEXTURE_ITEM_TIMER_FILLER_GRAVITYBALL;

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
	const std::string RESOURCE_DIR;
	const std::string FONT_DIR;
	const std::string MESH_DIR;
	const std::string SHADER_DIR;
	const std::string TEXTURE_DIR;
	const std::string SOUND_DIR;

	// Shader path constants ********************************
	const std::string CGFX_CEL_SHADER;
	const std::string CGFX_PHONG_SHADER;
	const std::string CGFX_INKBLOCK_SHADER;
	const std::string CGFX_POSTREFRACT_SHADER;
	const std::string CGFX_VOLUMEMETRIC_SHADER;
	const std::string CGFX_DECOSKYBOX_SHADER;
	const std::string CGFX_GAUSSIAN_SHADER;
	const std::string CGFX_AFTERIMAGE_SHADER;
	const std::string CGFX_BLOOM_SHADER;
	const std::string CGFX_FULLSCREENGOO_SHADER;
	const std::string CGFX_STICKYPADDLE_SHADER;
	const std::string CGFX_FULLSCREENSMOKEY_SHADER;
	const std::string CGFX_FULLSCREEN_UBERINTENSE_SHADER;
	const std::string CGFX_PRISM_SHADER;
	const std::string CGFX_PORTALBLOCK_SHADER;

	// Texture path constants *******************************
	const std::string TEXTURE_BANG1;
	const std::string TEXTURE_BANG2;
	const std::string TEXTURE_BANG3;
	const std::string TEXTURE_SPLAT1;
	const std::string TEXTURE_SMOKE1;
	const std::string TEXTURE_SMOKE2;
	const std::string TEXTURE_SMOKE3;
	const std::string TEXTURE_SMOKE4;
	const std::string TEXTURE_SMOKE5;
	const std::string TEXTURE_SMOKE6;
	const std::string TEXTURE_CIRCLE_GRADIENT;
	const std::string TEXTURE_STAR;
	const std::string TEXTURE_STAR_OUTLINE;
	const std::string TEXTURE_EVIL_STAR;
	const std::string TEXTURE_EVIL_STAR_OUTLINE;
	const std::string TEXTURE_EXPLOSION_CLOUD;
	const std::string TEXTURE_EXPLOSION_RAYS;
	const std::string TEXTURE_LASER_BEAM;
	const std::string TEXTURE_UP_ARROW;
	const std::string TEXTURE_BALL_LIFE_HUD;
	const std::string TEXTURE_SPARKLE;
	const std::string TEXTURE_SPIRAL_SMALL;
	const std::string TEXTURE_SPIRAL_MEDIUM;
	const std::string TEXTURE_SPIRAL_LARGE;
	const std::string TEXTURE_HALO;
	const std::string TEXTURE_LENSFLARE;
	const std::string TEXTURE_TWISTED_SPIRAL;
	const std::string TEXTURE_SIDEBLAST;
	const std::string TEXTURE_HUGE_EXPLOSION;
	const std::string TEXTURE_BRIGHT_FLARE;
	const std::string TEXTURE_LIGHTNING_BOLT;
	const std::string TEXTURE_SPHERE_NORMALS;

	const std::string TEXTURE_STARFIELD;
	
	const std::string TEXTURE_INKSPLATTER;
	const std::string TEXTURE_BALLTARGET;
	const std::string TEXTURE_BULLET_CROSSHAIR;
	const std::string TEXTURE_BEAM_CROSSHAIR;
	const std::string TEXTURE_BARREL_OVERLAY;

	const std::string TEXTURE_BALL_SAFETY_NET;

	const std::string TEXTURE_CEL_GRADIENT;
	const std::string TEXTURE_NOISE_OCTAVES;

	// Font path constants **********************************
	const std::string FONT_SADBAD;
	const std::string FONT_HAPPYGOOD;
	const std::string FONT_EXPLOSIONBOOM;
	const std::string FONT_ELECTRICZAP;
	const std::string FONT_ALLPURPOSE;

	// Sound Script File (.msf) path constants *********************************
	const std::string MAIN_MENU_SOUND_SCRIPT;
	const std::string DECO_SOUND_SCRIPT;

	// Item-related constants *******************************
	const std::string ITEM_LABEL_MATGRP;
	const std::string ITEM_END_MATGRP;

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
	const std::string BALL_MESH;
	const std::string SPIKEY_BALL_MESH;
	const std::string BASIC_BLOCK_MESH_PATH;
	const std::string TRIANGLE_BLOCK_MESH_PATH;
	const std::string BOMB_BLOCK_MESH;
	const std::string PRISM_DIAMOND_BLOCK_MESH;
	const std::string PRISM_TRIANGLE_BLOCK_MESH;
	const std::string CANNON_BLOCK_BASE_MESH;
	const std::string CANNON_BLOCK_BARREL_MESH;
	const std::string COLLATERAL_BLOCK_MESH;
	const std::string TESLA_BLOCK_BASE_MESH;
	const std::string TESLA_BLOCK_COIL_MESH;
	const std::string SKYBOX_MESH;
	const std::string ITEM_MESH;
	const std::string PADDLE_LASER_ATTACHMENT_MESH;
	const std::string PADDLE_BEAM_ATTACHMENT_MESH;
	const std::string PADDLE_STICKY_ATTACHMENT_MESH;
	const std::string PADDLE_ROCKET_MESH;

	// DECO WORLD
	const std::string DECO_PADDLE_MESH;
	const std::string DECO_BACKGROUND_MESH;
	const std::string DECO_SKYBEAM_MESH;
	const std::string DECO_BLOCK_MESH_PATH;


};
#endif