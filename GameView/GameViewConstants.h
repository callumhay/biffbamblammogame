/**
 * GameViewConstants.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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

    static const int STARRY_BG_TILE_MULTIPLIER = 2;

	// Asset file path constants ***************************
	const char* RESOURCE_DIR;
	const char* FONT_DIR;
	const char* MESH_DIR;
	const char* SHADER_DIR;
	const char* TEXTURE_DIR;
	const char* SOUND_DIR;

    // Author, credits, Licensing Text **********************
    const char* GAME_CREDITS_TEXT;
    const char* LICENSE_TEXT;

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
    const char* CGFX_POSTBULLETTIME_SHADER;
    const char* CGFX_FULLSCREEN_POSTDEATH_SHADER;
    const char* CGFX_GREYSCALE_SHADER;

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
    const char* TEXTURE_CLEAN_CIRCLE_GRADIENT;
    const char* TEXTURE_HOOP;
    const char* TEXTURE_OUTLINED_HOOP;
    const char* TEXTURE_SPHERE;
	const char* TEXTURE_STAR;
	const char* TEXTURE_STAR_OUTLINE;
	const char* TEXTURE_EVIL_STAR;
	const char* TEXTURE_EVIL_STAR_OUTLINE;
	const char* TEXTURE_EXPLOSION_CLOUD;
	const char* TEXTURE_EXPLOSION_RAYS;
	const char* TEXTURE_LASER_BEAM;
	const char* TEXTURE_UP_ARROW;
    const char* TEXTURE_HEART;
    const char* TEXTURE_NO_HEART;
	const char* TEXTURE_SPARKLE;
	const char* TEXTURE_SPIRAL_SMALL;
	const char* TEXTURE_SPIRAL_MEDIUM;
	const char* TEXTURE_SPIRAL_LARGE;
	const char* TEXTURE_HALO;
	const char* TEXTURE_LENSFLARE;
	const char* TEXTURE_TWISTED_SPIRAL;
	const char* TEXTURE_SIDEBLAST;
	const char* TEXTURE_HUGE_EXPLOSION;
    const char* TEXTURE_BUBBLY_EXPLOSION;
	const char* TEXTURE_BRIGHT_FLARE;
	const char* TEXTURE_LIGHTNING_BOLT;
	const char* TEXTURE_SPHERE_NORMALS;
	const char* TEXTURE_RECT_PRISM_NORMALS;
	const char* TEXTURE_CLOUD;
	const char* TEXTURE_GRIT;
	const char* TEXTURE_FROST;
	const char* TEXTURE_FULLSCREEN_FROST;
    const char* TEXTURE_VAPOUR_TRAIL;
    const char* TEXTURE_FUTURISM_TRIANGLE;
    const char* TEXTURE_BOSS_ICON;
    const char* TEXTURE_PADLOCK;
    const char* TEXTURE_PLUS;
    const char* TEXTURE_CHEVRON;

    const char* TEXTURE_EXPLOSION_ANIMATION;
    const char* TEXTURE_LIGHTNING_ANIMATION;

    const char* TEXTURE_MULTIPLIER_BANG;
    const char* TEXTURE_MULTIPLIER_GAGE_OUTLINE;
    const char* TEXTURE_MULTIPLIER_GAGE_GRADIENT;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_1;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_2;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_3;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_4;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_5;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_6;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_7;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_8;
    const char* TEXTURE_MULTIPLIER_GAGE_FILL_9;

    const char* TEXTURE_BOOST_HUD_OUTLINE;
    const char* TEXTURE_BOOST_HUD_BALLFILL;
    const char* TEXTURE_BOOST_HUD_TRAILFILL_1;
    const char* TEXTURE_BOOST_HUD_TRAILFILL_2;
    const char* TEXTURE_BOOST_HUD_TRAILFILL_3;

	const char* TEXTURE_STARFIELD;
	
	const char* TEXTURE_INKSPLATTER;
	const char* TEXTURE_BALLTARGET;
	const char* TEXTURE_BULLET_CROSSHAIR;
	const char* TEXTURE_BEAM_CROSSHAIR;
	const char* TEXTURE_BARREL_OVERLAY;

	const char* TEXTURE_BALL_SAFETY_NET;
    const char* TEXTURE_OFF_SWITCH;
    const char* TEXTURE_GREEN_ON_SWITCH;
    const char* TEXTURE_RED_ON_SWITCH;

	const char* TEXTURE_CEL_GRADIENT;
	const char* TEXTURE_NOISE_OCTAVES;
    const char* TEXTURE_INFINITY_CHAR;

	const char* TEXTURE_LOCKED_BLAMMOPEDIA_ENTRY;

    const char* TEXTURE_BOOST_DIR_TUTORIAL;
    const char* TEXTURE_BOOST_HUD_TUTORIAL;
    const char* TEXTURE_MULTIPLIER_TUTORIAL;

    const char* TEXTURE_LIFE_HUD_TUTORIAL;
    const char* TEXTURE_LIFE_ITEM_DROP_TUTORIAL;

    const char* TEXTURE_BOOST_TUTORIAL_ITEM;
    const char* TEXTURE_MULTIPLIER_TUTORIAL_ITEM;
    const char* TEXTURE_LIFE_TUTORIAL_ITEM;

    enum XBoxButtonType { XBoxPushButton, XBoxAnalogStick, XBoxTrigger };
    enum KeyboardButtonType { KeyboardChar, KeyboardSpaceBar };
    enum MouseButtonType { LeftMouseButton, RightMouseButton };

    const char* TEXTURE_SHORT_KEYBOARD_KEY;
    const char* TEXTURE_LONG_KEYBOARD_KEY;
    const char* TEXTURE_LEFT_MOUSE_BUTTON;
    const char* TEXTURE_RIGHT_MOUSE_BUTTON;
    const char* TEXTURE_XBOX_CONTROLLER_BUTTON;
    const char* TEXTURE_XBOX_CONTROLLER_ANALOG_STICK;
    const char* TEXTURE_XBOX_CONTROLLER_TRIGGER;

    const char* GetXBoxButtonTextureName(XBoxButtonType buttonType) const;
    const char* GetKeyboardButtonTextureName(KeyboardButtonType buttonType) const;
    const char* GetMoustButtonTextureName(MouseButtonType buttonType) const;

    void GetXBoxButtonLabelOffset(XBoxButtonType buttonType, float buttonWidth, float buttonHeight,
        float labelWidth, float labelHeight, float& offsetX, float& offsetY) const;
    void GetKeyboardButtonLabelOffset(KeyboardButtonType buttonType, float buttonSize,
        float labelWidth, float labelHeight, float& offsetX, float& offsetY) const;
    void GetMouseButtonLabelOffset(MouseButtonType buttonType, float buttonSize,
        float labelWidth, float labelHeight, float& offsetX, float& offsetY) const;

	// Font path constants **********************************
	const char* FONT_SADBAD;
	const char* FONT_HAPPYGOOD;
	const char* FONT_EXPLOSIONBOOM;
	const char* FONT_ELECTRICZAP;
	const char* FONT_ALLPURPOSE;

	// Sound Script File (.msf) path constants *********************************
	const char* MAIN_MENU_SOUND_SCRIPT;
	const char* DECO_SOUND_SCRIPT;
    const char* FUTURISM_SOUND_SCRIPT;

	// Item-related constants *******************************
	const char* ITEM_LABEL_MATGRP;
	const char* ITEM_END_MATGRP;
	const char* ITEM_DROP_BLOCK_ITEMTYPE_MATGRP;
    const char* ALWAYS_DROP_BLOCK_ITEMTYPE_MATGRP;
    const char* ALWAYS_DROP_BLOCK_COLOUR_MATGRP;
    const char* REGEN_BLOCK_BASE_METAL_MATGRP;

	const Colour ITEM_GOOD_COLOUR;
	const Colour ITEM_BAD_COLOUR;
	const Colour ITEM_NEUTRAL_COLOUR;

    const char* SWITCH_BASE_METAL_MATGRP;
    const char* SWITCH_ON_MATGRP;
    const char* SWITCH_OFF_MATGRP;
    const char* SWITCH_CURRENT_MATGRP;

	const float DEFAULT_BALL_LIGHT_ATTEN;
	const float BLACKOUT_BALL_LIGHT_ATTEN;
	const Colour DEFAULT_PADDLE_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_PADDLE_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BOSS_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_BOSS_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BALL_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_FG_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_FG_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BG_KEY_LIGHT_COLOUR;
	const Colour DEFAULT_BG_FILL_LIGHT_COLOUR;
	const Colour DEFAULT_BALL_LIGHT_COLOUR;
	const Colour BLACKOUT_BALL_KEY_LIGHT_COLOUR;
	const Colour BLACKOUT_PADDLE_KEY_LIGHT_COLOUR;
	const Colour BLACKOUT_PADDLE_FILL_LIGHT_COLOUR;
	const Colour BLACKOUT_BOSS_KEY_LIGHT_COLOUR;
	const Colour BLACKOUT_BOSS_FILL_LIGHT_COLOUR;
	const Colour POISON_LIGHT_DEEP_COLOUR;
	const Colour POISON_LIGHT_LIGHT_COLOUR;
	const Colour INK_BLOCK_COLOUR;
	const Colour STICKYPADDLE_GOO_COLOUR;
	const Colour STICKYPADDLE_PLUS_BEAM_GOO_COLOUR;
	const Colour PRISM_BLOCK_COLOUR;
	const Colour LASER_BEAM_COLOUR;

    const Colour TUTORIAL_PANE_COLOUR;

    const Colour ACTIVE_POINT_STAR_COLOUR;
    const Colour INACTIVE_POINT_STAR_COLOUR;

    const Colour TWO_TIMES_MULTIPLIER_COLOUR;
    const Colour THREE_TIMES_MULTIPLIER_COLOUR;
    const Colour FOUR_TIMES_MULTIPLIER_COLOUR;

    const Colour XBOX_CONTROLLER_X_BUTTON_COLOUR;
    const Colour XBOX_CONTROLLER_Y_BUTTON_COLOUR;
    const Colour XBOX_CONTROLLER_A_BUTTON_COLOUR;
    const Colour XBOX_CONTROLLER_B_BUTTON_COLOUR;

	const Point3D DEFAULT_FG_KEY_LIGHT_POSITION;
	const Point3D DEFAULT_FG_FILL_LIGHT_POSITION;
	const Point3D DEFAULT_BG_KEY_LIGHT_POSITION;
	const Point3D DEFAULT_BG_FILL_LIGHT_POSITION;
	const Point3D DEFAULT_BALL_KEY_LIGHT_POSITION;
	const Point3D DEFAULT_BALL_FILL_LIGHT_POSITION;
	const Point3D DEFAULT_PADDLE_KEY_LIGHT_POSITION;
	const Point3D DEFAULT_PADDLE_FILL_LIGHT_POSITION;

	// General Mesh Constants ******************************
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
    const char* SWITCH_BLOCK_MESH;
    const char* NO_ENTRY_BLOCK_MESH;
    const char* TURRET_BASE_MESH;
    const char* LASER_TURRET_HEAD_MESH;
    const char* LASER_TURRET_BARREL_1_MESH;
    const char* LASER_TURRET_BARREL_2_MESH;
    const char* ROCKET_TURRET_HEAD_MESH;
    const char* ROCKET_TURRET_BARREL_MESH;
    const char* MINE_TURRET_HEAD_MESH;
    const char* MINE_TURRET_BARREL_MESH;
    const char* MINE_MESH;
    const char* ALWAYS_DROP_BLOCK_MESH;
    const char* REGEN_BLOCK_MESH;

	const char* SKYBOX_MESH;
	const char* ITEM_MESH;
	const char* PADDLE_LASER_ATTACHMENT_MESH;
	const char* PADDLE_BEAM_ATTACHMENT_MESH;
    const char* PADDLE_MINE_ATTACHMENT_MESH;
	const char* PADDLE_STICKY_ATTACHMENT_MESH;
	const char* PADDLE_ROCKET_MESH;
    const char* TURRET_ROCKET_MESH;

    const char* ONE_WAY_BLOCK_UP_MESH;
    const char* ONE_WAY_BLOCK_DOWN_MESH;
    const char* ONE_WAY_BLOCK_LEFT_MESH;
    const char* ONE_WAY_BLOCK_RIGHT_MESH;

    // World specific path constants ************************

    // GENERAL
    const char* SKYBEAM_MESH;

    // CLASSICAL WORLD
    const char* CLASSICAL_PADDLE_MESH;
    const char* CLASSICAL_BACKGROUND_MESH;
    const char* CLASSICAL_BLOCK_MESH;
    // CLASSICAL BOSS
    const char* CLASSICAL_BOSS_ARM_MESH;
    const char* CLASSICAL_BOSS_ARM_SQUARE_MESH;
    const char* CLASSICAL_BOSS_BASE_MESH;
    const char* CLASSICAL_BOSS_BODY_COLUMN_MESH;
    const char* CLASSICAL_BOSS_EYE_MESH;
    const char* CLASSICAL_BOSS_PEDIMENT_MESH;
    const char* CLASSICAL_BOSS_TABLATURE_MESH;

    // GOTHIC-ROMANTIC WORLD
    const char* GOTHIC_ROMANTIC_PADDLE_MESH;
    const char* GOTHIC_ROMANTIC_BACKGROUND_MESH;
    const char* GOTHIC_ROMANTIC_BLOCK_MESH;
    // GOTHIC-ROMANTIC BOSS
    const char* GOTHIC_ROMANTIC_BOSS_TOP_POINT_MESH;
    const char* GOTHIC_ROMANTIC_BOSS_BOTTOM_POINT_MESH;
    const char* GOTHIC_ROMANTIC_BOSS_BODY_MESH;
    const char* GOTHIC_ROMANTIC_BOSS_LEG_MESH;

	// DECO WORLD
	const char* DECO_PADDLE_MESH;
	const char* DECO_BACKGROUND_MESH;
	const char* DECO_BLOCK_MESH;

    // FUTURISM WORLD
    const char* FUTURISM_PADDLE_MESH;
    const char* FUTURISM_BACKGROUND_MESH;
    const char* FUTURISM_BLOCK_MESH;

    const Colour& GetItemColourFromDisposition(const GameItem::ItemDisposition& itemDisposition) const;
    const Colour& GetMultiplierColour(int multiplier);

    AnimationMultiLerp<Colour> BuildFlashingColourAnimation() const;

private:
	static GameViewConstants* Instance;

	GameViewConstants();
	~GameViewConstants();

	// Disallow copy and assign
	GameViewConstants(const GameViewConstants& g);
	GameViewConstants& operator=(const GameViewConstants& g);
};
#endif