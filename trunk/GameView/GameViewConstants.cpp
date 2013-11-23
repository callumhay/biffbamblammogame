/**
 * GameViewConstants.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GameViewConstants.h"

GameViewConstants* GameViewConstants::Instance = NULL;

#define RESOURCE_DIRECTORY "resources"
#define FONT_DIRECTORY RESOURCE_DIRECTORY "/fonts"
#define MESH_DIRECTORY RESOURCE_DIRECTORY "/models"
#define SHADER_DIRECTORY RESOURCE_DIRECTORY "/shaders"
#define TEXTURE_DIRECTORY RESOURCE_DIRECTORY "/textures"
#define SOUND_DIRECTORY RESOURCE_DIRECTORY "/sounds"

#define BOSS_DIRECTORY "/boss"

#define CLASSICAL_MESH_DIRECTORY MESH_DIRECTORY "/classical"
#define CLASSICAL_BOSS_MESH_DIRECTORY CLASSICAL_MESH_DIRECTORY BOSS_DIRECTORY

#define GOTHIC_ROMANTIC_MESH_DIRECTORY MESH_DIRECTORY "/gothic_romantic"
#define GOTHIC_ROMANTIC_BOSS_MESH_DIRECTORY GOTHIC_ROMANTIC_MESH_DIRECTORY BOSS_DIRECTORY


#define NOUVEAU_MESH_DIRECTORY MESH_DIRECTORY "/nouveau"
#define NOUVEAU_BOSS_MESH_DIRECTORY NOUVEAU_MESH_DIRECTORY BOSS_DIRECTORY

#define DECO_MESH_DIRECTORY MESH_DIRECTORY "/deco"
#define DECO_BOSS_MESH_DIRECTORY DECO_MESH_DIRECTORY BOSS_DIRECTORY

#define FUTURISM_MESH_DIRECTORY MESH_DIRECTORY "/futurism"
#define FUTURISM_BOSS_MESH_DIRECTORY FUTURISM_MESH_DIRECTORY BOSS_DIRECTORY


GameViewConstants::GameViewConstants() :
// Basic Asset Paths
RESOURCE_DIR(RESOURCE_DIRECTORY),
FONT_DIR(FONT_DIRECTORY),
MESH_DIR(MESH_DIRECTORY),
SHADER_DIR(SHADER_DIRECTORY),
TEXTURE_DIR(TEXTURE_DIRECTORY),
SOUND_DIR(SOUND_DIRECTORY),

GAME_CREDITS_TEXT("A game by Callum Hay"),
LICENSE_TEXT("(CC) BY-NC, 2013"),

// Shader Asset Paths
CGFX_CEL_SHADER(SHADER_DIRECTORY                    "/CelShading.cgfx"),
CGFX_PHONG_SHADER(SHADER_DIRECTORY                  "/Phong.cgfx"),
CGFX_INKBLOCK_SHADER(SHADER_DIRECTORY               "/InkBlock.cgfx"),
CGFX_POSTREFRACT_SHADER(SHADER_DIRECTORY            "/PostRefract.cgfx"),
CGFX_VOLUMEMETRIC_SHADER(SHADER_DIRECTORY           "/VolumetricEffect.cgfx"),
CGFX_SKYBOX_SHADER(SHADER_DIRECTORY                 "/Skybox.cgfx"),
CGFX_GAUSSIAN_SHADER(SHADER_DIRECTORY               "/GaussianBlur.cgfx"),
CGFX_AFTERIMAGE_SHADER(SHADER_DIRECTORY             "/MotionBlurAndAfterImage.cgfx"),
CGFX_BLOOM_SHADER(SHADER_DIRECTORY                  "/Bloom.cgfx"),
CGFX_FULLSCREENGOO_SHADER(SHADER_DIRECTORY          "/FullscreenGoo.cgfx"),
CGFX_STICKYPADDLE_SHADER(SHADER_DIRECTORY           "/StickyPaddle.cgfx"),
CGFX_FULLSCREENSMOKEY_SHADER(SHADER_DIRECTORY       "/PostSmokey.cgfx"),
CGFX_FULLSCREEN_UBERINTENSE_SHADER(SHADER_DIRECTORY "/PostUberIntense.cgfx"),
CGFX_PRISM_SHADER(SHADER_DIRECTORY                  "/Prism.cgfx"),
CGFX_PORTALBLOCK_SHADER(SHADER_DIRECTORY            "/PortalBlock.cgfx"),
CGFX_FIREBALL_SHADER(SHADER_DIRECTORY               "/FireBall.cgfx"),
CGFX_FULLSCREEN_POSTFIREY_SHADER(SHADER_DIRECTORY   "/PostFirey.cgfx"),
CGFX_POSTBULLETTIME_SHADER(SHADER_DIRECTORY         "/PostBulletTime.cgfx"),
CGFX_FULLSCREEN_POSTDEATH_SHADER(SHADER_DIRECTORY   "/PostDeath.cgfx"),
CGFX_GREYSCALE_SHADER(SHADER_DIRECTORY              "/Greyscale.cgfx"),
CGFX_CELOUTLINES_SHADER(SHADER_DIRECTORY            "/CelOutlines.cgfx"),
CGFX_POSTTUTORIALATTENTION_SHADER(SHADER_DIRECTORY  "/PostTutorialAttention.cgfx"),
CGFX_CLOUD_SHADER(SHADER_DIRECTORY                  "/CloudEffect.cgfx"),

// Texture Asset Paths
TEXTURE_BANG1(TEXTURE_DIRECTORY  "/Bang1_256x128.png"),
TEXTURE_BANG2(TEXTURE_DIRECTORY  "/Bang2_256x128.png"),
TEXTURE_BANG3(TEXTURE_DIRECTORY  "/Bang3_256x128.png"),
TEXTURE_SPLAT1(TEXTURE_DIRECTORY "/Splat1_256x128.png"),
TEXTURE_SMOKE1(TEXTURE_DIRECTORY "/smoke_puff1_128x128.png"),
TEXTURE_SMOKE2(TEXTURE_DIRECTORY "/smoke_puff2_128x128.png"),
TEXTURE_SMOKE3(TEXTURE_DIRECTORY "/smoke_puff3_128x128.png"),
TEXTURE_SMOKE4(TEXTURE_DIRECTORY "/smoke_puff4_128x128.png"),
TEXTURE_SMOKE5(TEXTURE_DIRECTORY "/smoke_puff5_128x128.png"),
TEXTURE_SMOKE6(TEXTURE_DIRECTORY "/smoke_puff6_128x128.png"),
TEXTURE_SNOWFLAKE1(TEXTURE_DIRECTORY "/snowflake1_64x64.png"),
TEXTURE_SNOWFLAKE2(TEXTURE_DIRECTORY "/snowflake2_64x64.png"),
TEXTURE_SNOWFLAKE3(TEXTURE_DIRECTORY "/snowflake3_64x64.png"),
TEXTURE_ROCK1(TEXTURE_DIRECTORY "/rock1_64x64.png"),
TEXTURE_ROCK2(TEXTURE_DIRECTORY "/rock2_64x64.png"),
TEXTURE_ROCK3(TEXTURE_DIRECTORY "/rock3_64x64.png"),
TEXTURE_ROCK4(TEXTURE_DIRECTORY "/rock4_64x64.png"),
TEXTURE_ROCK5(TEXTURE_DIRECTORY "/rock5_64x64.png"),
TEXTURE_BOLT1(TEXTURE_DIRECTORY "/bolt1_32x64.png"),
TEXTURE_BOLT2(TEXTURE_DIRECTORY "/bolt2_32x64.png"),
TEXTURE_BOLT3(TEXTURE_DIRECTORY "/bolt3_32x64.png"),
TEXTURE_BOLT4(TEXTURE_DIRECTORY "/bolt4_32x64.png"),
TEXTURE_BOLT5(TEXTURE_DIRECTORY "/bolt5_32x64.png"),
TEXTURE_CIRCLE_GRADIENT(TEXTURE_DIRECTORY "/Circle_Gradient_64x64.png"),
TEXTURE_CLEAN_CIRCLE_GRADIENT(TEXTURE_DIRECTORY "/clean_gradient_circle_32x32.png"),
TEXTURE_HOOP(TEXTURE_DIRECTORY "/hoop_64x64.png"),
TEXTURE_SPHERE(TEXTURE_DIRECTORY "/sphere_32x32.png"),
TEXTURE_STAR(TEXTURE_DIRECTORY "/Star_Fill_128x128.png"),
TEXTURE_STAR_OUTLINE(TEXTURE_DIRECTORY "/Star_Outline_128x128.png"),
TEXTURE_EVIL_STAR(TEXTURE_DIRECTORY "/Evil_Star_Fill_64x64.png"),
TEXTURE_EXPLOSION_CLOUD(TEXTURE_DIRECTORY "/Explosion_Cloud_128x128.png"),
TEXTURE_EXPLOSION_RAYS(TEXTURE_DIRECTORY "/Explosion_Rays_256x256.png"),
TEXTURE_LASER_BEAM(TEXTURE_DIRECTORY "/Laser_32x64.png"),
TEXTURE_UP_ARROW(TEXTURE_DIRECTORY "/Arrow_32x64.png"),
TEXTURE_HEART(TEXTURE_DIRECTORY "/heart_64x64.png"),
TEXTURE_NO_HEART(TEXTURE_DIRECTORY "/no_heart_64x64.png"),
TEXTURE_SPARKLE(TEXTURE_DIRECTORY "/Sparkle32x32.png"),
TEXTURE_SPIRAL_SMALL(TEXTURE_DIRECTORY "/SmallSpiral_32x32.png"),
TEXTURE_SPIRAL_MEDIUM(TEXTURE_DIRECTORY "/MedSpiral_64x64.png"),
TEXTURE_SPIRAL_LARGE(TEXTURE_DIRECTORY "/BigSpiral_128x128.png"),
TEXTURE_HALO(TEXTURE_DIRECTORY "/halo_64x64.png"),
TEXTURE_LENSFLARE(TEXTURE_DIRECTORY "/lens_flare_64x64.png"),
TEXTURE_TWISTED_SPIRAL(TEXTURE_DIRECTORY "/twisted_spiral_64x64.png"),
TEXTURE_SIDEBLAST(TEXTURE_DIRECTORY "/sideblast_128x128.png"),
TEXTURE_HUGE_EXPLOSION(TEXTURE_DIRECTORY "/huge_explosion_512x256.png"),
TEXTURE_BUBBLY_EXPLOSION(TEXTURE_DIRECTORY "/bubbly_explosion_128x128.png"),
TEXTURE_BRIGHT_FLARE(TEXTURE_DIRECTORY "/flare_64x64.png"),
TEXTURE_LIGHTNING_BOLT(TEXTURE_DIRECTORY "/LightningBolt_32x64.png"),
TEXTURE_SPHERE_NORMALS(TEXTURE_DIRECTORY "/SphereNormals_128x128.png"),
TEXTURE_RECT_PRISM_NORMALS(TEXTURE_DIRECTORY "/rect_prism_normals_64x32.jpg"),
TEXTURE_CLOUD1(TEXTURE_DIRECTORY "/cloud1_128x128.png"),
TEXTURE_CLOUD2(TEXTURE_DIRECTORY "/cloud2_128x128.png"),
TEXTURE_CLOUD3(TEXTURE_DIRECTORY "/cloud3_128x128.png"),
TEXTURE_NORMAL_CLOUD(TEXTURE_DIRECTORY "/cloud_normal_64x64.png"),
TEXTURE_GRIT(TEXTURE_DIRECTORY "/Grit_64x64.png"),
TEXTURE_FROST(TEXTURE_DIRECTORY "/frost_128x64.png"),
TEXTURE_FULLSCREEN_FROST(TEXTURE_DIRECTORY "/fullscreen_frost_512x256.png"),
TEXTURE_VAPOUR_TRAIL(TEXTURE_DIRECTORY "/vapour_ball_256x256.png"),
TEXTURE_FUTURISM_TRIANGLE(TEXTURE_DIRECTORY "/futurism_triangle_256x128.png"),
TEXTURE_BOSS_ICON(TEXTURE_DIRECTORY "/boss_skull_32x32.png"),
TEXTURE_PADLOCK(TEXTURE_DIRECTORY "/padlock_128x128.png"),
TEXTURE_CHEVRON(TEXTURE_DIRECTORY "/chevron_64x32.png"),
TEXTURE_MOON(TEXTURE_DIRECTORY "/moon_512x512.png"),

TEXTURE_DROPLET(TEXTURE_DIRECTORY "/water_drop_32x32.png"),

TEXTURE_PLUS(TEXTURE_DIRECTORY "/plus_64x64.png"),
TEXTURE_OUTLINED_PLUS(TEXTURE_DIRECTORY "/outlined_plus_64x64.png"),
TEXTURE_CIRCLE(TEXTURE_DIRECTORY "/circle_64x64.png"),
TEXTURE_OUTLINED_HOOP(TEXTURE_DIRECTORY "/outlined_hoop_64x64.png"),
TEXTURE_X(TEXTURE_DIRECTORY "/x_64x64.png"),
TEXTURE_OUTLINED_X(TEXTURE_DIRECTORY "/outlined_x_64x64.png"),

TEXTURE_HAPPY_FACE(TEXTURE_DIRECTORY   "/happy_face_64x64.png"),
TEXTURE_NEUTRAL_FACE(TEXTURE_DIRECTORY "/neutral_face_64x64.png"),
TEXTURE_SAD_FACE(TEXTURE_DIRECTORY     "/sad_face_64x64.png"),

TEXTURE_EXPLOSION_ANIMATION(TEXTURE_DIRECTORY "/explosion_animation_1024x1024.png"),
TEXTURE_LIGHTNING_ANIMATION(TEXTURE_DIRECTORY "/electricity_animation_256x256.png"),

TEXTURE_CLASSICAL_BLOCK_IMG(TEXTURE_DIRECTORY "/classical_block256x128.png"),
TEXTURE_GOTHIC_ROMANTIC_BLOCK_IMG(TEXTURE_DIRECTORY "/gothic_romantic_block256x128.png"),
TEXTURE_NOUVEAU_BLOCK_IMG(TEXTURE_DIRECTORY "/nouveau_block256x128.png"),
TEXTURE_DECO_BLOCK_IMG(TEXTURE_DIRECTORY "/deco_block256x128.png"),

TEXTURE_MULTIPLIER_BANG(TEXTURE_DIRECTORY           "/multipler_bang_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_OUTLINE(TEXTURE_DIRECTORY   "/multiplier_gage_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_GRADIENT(TEXTURE_DIRECTORY  "/multiplier_gage_gradient_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_1(TEXTURE_DIRECTORY    "/multiplier_gage_1_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_2(TEXTURE_DIRECTORY    "/multiplier_gage_2_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_3(TEXTURE_DIRECTORY    "/multiplier_gage_3_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_4(TEXTURE_DIRECTORY    "/multiplier_gage_4_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_5(TEXTURE_DIRECTORY    "/multiplier_gage_5_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_6(TEXTURE_DIRECTORY    "/multiplier_gage_6_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_7(TEXTURE_DIRECTORY    "/multiplier_gage_7_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_8(TEXTURE_DIRECTORY    "/multiplier_gage_8_128x128.png"),
TEXTURE_MULTIPLIER_GAGE_FILL_9(TEXTURE_DIRECTORY    "/multiplier_gage_9_128x128.png"),

TEXTURE_BOOST_HUD_OUTLINE(TEXTURE_DIRECTORY "/boost_hud_256x64.png"),
TEXTURE_BOOST_HUD_BALLFILL(TEXTURE_DIRECTORY "/boost_hud_ballfill_256x64.png"),
TEXTURE_BOOST_HUD_TRAILFILL_1(TEXTURE_DIRECTORY "/boost_hud_trailfill1_256x64.png"),
TEXTURE_BOOST_HUD_TRAILFILL_2(TEXTURE_DIRECTORY "/boost_hud_trailfill2_256x64.png"),
TEXTURE_BOOST_HUD_TRAILFILL_3(TEXTURE_DIRECTORY "/boost_hud_trailfill3_256x64.png"),

TEXTURE_STARFIELD(TEXTURE_DIRECTORY "/starfield_1024x1024.jpg"),

TEXTURE_INKSPLATTER(TEXTURE_DIRECTORY      "/ink_splatter_1024x512.png"),
TEXTURE_CIRCLE_TARGET(TEXTURE_DIRECTORY    "/ball_target_64x64.png"),
TEXTURE_SQUARE_TARGET(TEXTURE_DIRECTORY    "/square_target_64x64.png"),
TEXTURE_BULLET_CROSSHAIR(TEXTURE_DIRECTORY "/Crosshair_128x128.png"),
TEXTURE_BEAM_CROSSHAIR(TEXTURE_DIRECTORY   "/beam_crosshairs_256x256.png"),
TEXTURE_BARREL_OVERLAY(TEXTURE_DIRECTORY   "/barrel_overlay_1024x1024.png"),

TEXTURE_BALL_SAFETY_NET(TEXTURE_DIRECTORY "/warning_platform_128x64.jpg"),
TEXTURE_OFF_SWITCH(TEXTURE_DIRECTORY      "/switch_off_128x64.jpg"),
TEXTURE_GREEN_ON_SWITCH(TEXTURE_DIRECTORY "/switch_green_128x64.jpg"),
TEXTURE_RED_ON_SWITCH(TEXTURE_DIRECTORY   "/switch_red_128x64.jpg"),

TEXTURE_CEL_GRADIENT(TEXTURE_DIRECTORY  "/celshading_texture1x256.jpg"),
TEXTURE_NOISE_OCTAVES(TEXTURE_DIRECTORY "/noise_octaves.raw"),
TEXTURE_INFINITY_CHAR(TEXTURE_DIRECTORY "/infinity_char_32x32.png"),

TEXTURE_LOCKED_BLAMMOPEDIA_ENTRY(TEXTURE_DIRECTORY "/locked_item_256x128.jpg"),

TEXTURE_BOOST_DIR_TUTORIAL(TEXTURE_DIRECTORY  "/boost_dir_tutorial_image_256x256.jpg"),
TEXTURE_BOOST_HUD_TUTORIAL(TEXTURE_DIRECTORY  "/boost_hud_tutorial_image_256x256.jpg"),
TEXTURE_MULTIPLIER_TUTORIAL(TEXTURE_DIRECTORY "/multiplier_tutorial_256x512.jpg"),

TEXTURE_LIFE_HUD_TUTORIAL(TEXTURE_DIRECTORY "/life_hud_tutorial_256x128.jpg"),
TEXTURE_LIFE_ITEM_DROP_TUTORIAL(TEXTURE_DIRECTORY "/life_item_drop_256x256.jpg"),

TEXTURE_BOOST_TUTORIAL_ITEM(TEXTURE_DIRECTORY      "/boost_tutorial_item_512x256.png"),
TEXTURE_MULTIPLIER_TUTORIAL_ITEM(TEXTURE_DIRECTORY "/multiplier_tutorial_item_512x256.png"),
TEXTURE_LIFE_TUTORIAL_ITEM(TEXTURE_DIRECTORY       "/life_tutorial_item_512x256.png"),

TEXTURE_DEFAULT_MOUSE(TEXTURE_DIRECTORY "/default_cursor.png"),
TEXTURE_BOOST_MOUSE(TEXTURE_DIRECTORY   "/boost_cursor.png"),

TEXTURE_BBB_LOGO(TEXTURE_DIRECTORY                 "/bbb_logo.png"),

TEXTURE_SHORT_KEYBOARD_KEY(TEXTURE_DIRECTORY "/keyboard_button_64x64.png"),
TEXTURE_LONG_KEYBOARD_KEY(TEXTURE_DIRECTORY "/keyboard_long_button_128x64.png"),
TEXTURE_LEFT_MOUSE_BUTTON(TEXTURE_DIRECTORY "/left_mouse_button_64x64.png"),
TEXTURE_RIGHT_MOUSE_BUTTON(TEXTURE_DIRECTORY "/right_mouse_button_64x64.png"),
TEXTURE_XBOX_CONTROLLER_BUTTON(TEXTURE_DIRECTORY "/xbox_360_controller_button_64x64.png"),
TEXTURE_XBOX_CONTROLLER_ANALOG_STICK(TEXTURE_DIRECTORY "/xbox_360_controller_analog_stick_64x64.png"),
TEXTURE_XBOX_CONTROLLER_TRIGGER(TEXTURE_DIRECTORY "/xbox_360_controller_trigger_64x64.png"),

// Font Asset Paths
FONT_SADBAD(FONT_DIRECTORY          "/sadbad.ttf"),
FONT_HAPPYGOOD(FONT_DIRECTORY       "/happygood.ttf"),
FONT_EXPLOSIONBOOM(FONT_DIRECTORY   "/explosionboom.ttf"),
FONT_ELECTRICZAP(FONT_DIRECTORY     "/electriczap.ttf"),
FONT_ALLPURPOSE(FONT_DIRECTORY      "/allpurpose.ttf"),

// Sound script paths
GAME_SOUND_SCRIPT(SOUND_DIRECTORY  "/game_sound.msf"),

// Item-related constants
ITEM_LABEL_MATGRP("ItemLabel"),                             // Material group name for changing the label on the item mesh
ITEM_END_MATGRP("ColourEnd"),                               // Material group name for changing the colour on the item mesh
ITEM_DROP_BLOCK_ITEMTYPE_MATGRP("item_drop_item_material"), // Material group name for the item picture on the item drop block mesh
ALWAYS_DROP_BLOCK_ITEMTYPE_MATGRP("always_drop_item_material"),      // Material group name for the item picture on the always drop block mesh
ALWAYS_DROP_BLOCK_COLOUR_MATGRP("always_drop_item_colour_material"), // Material group name for the colour of the item on the always drop block mesh
REGEN_BLOCK_BASE_METAL_MATGRP("RegenBlockBaseColourMetal"), // Material group name for the base metal of the regen block mesh

ITEM_GOOD_COLOUR(0.0f, 0.8f, 0.0f),
ITEM_BAD_COLOUR(0.8f, 0.0f, 0.0f),
ITEM_NEUTRAL_COLOUR(0.0f, 0.6f, 1.0f),

SWITCH_BASE_METAL_MATGRP("switch_metal"),   // Material group name for the base texture of the switch block, which doesn't change
SWITCH_ON_MATGRP("switch_on"),              // Material group name for the 'on' texture of the switch block
SWITCH_OFF_MATGRP("switch_off"),            // Material group name for the 'off' texture of the switch block
SWITCH_CURRENT_MATGRP("switch_current"),    // Material group name for currently active mode (on or off) of the switch block

DEFAULT_BALL_LIGHT_ATTEN(0.68f),
BLACKOUT_BALL_LIGHT_ATTEN(0.9f),
DEFAULT_BALL_KEY_LIGHT_ATTEN(0.035f),
DEFAULT_BALL_FILL_LIGHT_ATTEN(0.025f),

DEFAULT_FG_KEY_LIGHT_ATTEN(0.022f),
DEFAULT_FG_FILL_LIGHT_ATTEN(0.022f),
DEFAULT_BG_KEY_LIGHT_ATTEN(0.005f),
DEFAULT_BG_FILL_LIGHT_ATTEN(0.025f),

DEFAULT_BALL_KEY_LIGHT_COLOUR(0.9f, 0.9f, 0.9f),
DEFAULT_BALL_FILL_LIGHT_COLOUR(0.9f, 0.9f, 0.9f),
DEFAULT_PADDLE_KEY_LIGHT_COLOUR(0.65f, 0.65f, 0.7f),
DEFAULT_PADDLE_FILL_LIGHT_COLOUR(0.7f, 0.7f, 0.7f),
DEFAULT_BOSS_KEY_LIGHT_COLOUR(0.6f, 0.6f, 0.65f),
DEFAULT_BOSS_FILL_LIGHT_COLOUR(0.65f, 0.65f, 0.65f),
DEFAULT_FG_KEY_LIGHT_COLOUR(0.6f, 0.6f, 0.65f),
DEFAULT_FG_FILL_LIGHT_COLOUR(0.65f, 0.65f, 0.65f),
DEFAULT_BG_KEY_LIGHT_COLOUR(0.5f, 0.55f, 0.55f),
DEFAULT_BG_FILL_LIGHT_COLOUR(0.6f, 0.6f, 0.6f),
DEFAULT_BALL_LIGHT_COLOUR(0.85f, 0.85f, 0.95f),
BLACKOUT_BALL_KEY_LIGHT_COLOUR(0.45f, 0.45f, 0.45f),
BLACKOUT_PADDLE_KEY_LIGHT_COLOUR(0.3f, 0.3f, 0.32f),
BLACKOUT_PADDLE_FILL_LIGHT_COLOUR(0.35f, 0.35f, 0.35f),
BLACKOUT_BOSS_KEY_LIGHT_COLOUR(0.0f, 0.0f, 0.0f),
BLACKOUT_BOSS_FILL_LIGHT_COLOUR(0.0f, 0.0f, 0.0f),
POISON_LIGHT_DEEP_COLOUR(0.10f, 0.50f, 0.10f),
POISON_LIGHT_LIGHT_COLOUR(0.75f, 1.0f, 0.0f),
INK_BLOCK_COLOUR(0.49f, 0.239f, 0.90f),
STICKYPADDLE_GOO_COLOUR(1.0f, 0.81f, 0.333f),
STICKYPADDLE_PLUS_BEAM_GOO_COLOUR(0.8f, 0.9f, 0.777f),
PRISM_BLOCK_COLOUR(0.4f, 0.5f, 0.5f),
BOSS_ORB_BASE_COLOUR(0.8f, 0.0f, 0.0f),
BOSS_ORB_BRIGHT_COLOUR(1.0f, 0.25f, 0.25f),

TUTORIAL_PANE_COLOUR(0.39f, 0.5843f, 0.9294f),

BRIGHT_POINT_STAR_COLOUR(1.0f, 1.0f, 0.0f),
ACTIVE_POINT_STAR_COLOUR(1.0f, 0.85f, 0.0f),
INACTIVE_POINT_STAR_COLOUR(0.5f, 0.5f, 0.5f),

TWO_TIMES_MULTIPLIER_COLOUR(1.0f, 0.4f, 0.0f),
THREE_TIMES_MULTIPLIER_COLOUR(0.0f, 0.8f, 0.0f),
FOUR_TIMES_MULTIPLIER_COLOUR(0.0f, 0.6f, 1.0f),

XBOX_CONTROLLER_X_BUTTON_COLOUR(0.0f, 0.29f, 1.0f),
XBOX_CONTROLLER_Y_BUTTON_COLOUR(1.0f, 0.78f, 0.16f),
XBOX_CONTROLLER_A_BUTTON_COLOUR(0.48f, 0.745f, 0.0f),
XBOX_CONTROLLER_B_BUTTON_COLOUR(1.0f, 0.1f, 0.0f),

DEFAULT_FG_KEY_LIGHT_POSITION(Point3D(-15.0f, 5.0f, 65.0f)),
DEFAULT_FG_FILL_LIGHT_POSITION(Point3D(15.0f, 10.0f, 50.0f)),
DEFAULT_BG_KEY_LIGHT_POSITION(Point3D(-25.0f, 20.0f, 55.0f)),
DEFAULT_BG_FILL_LIGHT_POSITION(Point3D(30.0f, 11.0f, -15.0f)),

// World-Related Asset Paths
BALL_MESH(MESH_DIRECTORY "/ball.obj"),
SPIKEY_BALL_MESH(MESH_DIRECTORY "/spikey_ball.obj"),
BASIC_BLOCK_MESH_PATH(MESH_DIRECTORY "/block.obj"),
TRIANGLE_BLOCK_MESH_PATH(MESH_DIRECTORY "/triangle_block.obj"),
BOMB_BLOCK_MESH(MESH_DIRECTORY "/bomb_block.obj"),
PRISM_DIAMOND_BLOCK_MESH(MESH_DIRECTORY "/prism_block.obj"),
PRISM_TRIANGLE_BLOCK_MESH(MESH_DIRECTORY "/triangle_prism_block.obj"),
CANNON_BLOCK_BASE_MESH(MESH_DIRECTORY "/cannon_block_base.obj"),
CANNON_BLOCK_BARREL_MESH(MESH_DIRECTORY "/cannon_block_barrel.obj"),
COLLATERAL_BLOCK_MESH(MESH_DIRECTORY "/collateral_block.obj"),
TESLA_BLOCK_BASE_MESH(MESH_DIRECTORY "/tesla_block_base.obj"),
TESLA_BLOCK_COIL_MESH(MESH_DIRECTORY "/tesla_block_coil.obj"),
ITEM_DROP_BLOCK_MESH(MESH_DIRECTORY  "/item_drop_block.obj"),
SWITCH_BLOCK_MESH(MESH_DIRECTORY "/switch_block.obj"),
NO_ENTRY_BLOCK_MESH(MESH_DIRECTORY "/no_entry_block.obj"),
TURRET_BASE_MESH(MESH_DIRECTORY "/turret_block_base.obj"),
LASER_TURRET_HEAD_MESH(MESH_DIRECTORY "/laser_turret_block_head.obj"),
LASER_TURRET_BARREL_1_MESH(MESH_DIRECTORY "/laser_turret_block_barrel_1.obj"),
LASER_TURRET_BARREL_2_MESH(MESH_DIRECTORY "/laser_turret_block_barrel_2.obj"),
ROCKET_TURRET_HEAD_MESH(MESH_DIRECTORY "/rocket_turret_block_head.obj"),
ROCKET_TURRET_BARREL_MESH(MESH_DIRECTORY "/rocket_turret_block_barrel.obj"),
MINE_TURRET_HEAD_MESH(MESH_DIRECTORY   "/mine_turret_block_head.obj"),
MINE_TURRET_BARREL_MESH(MESH_DIRECTORY "/mine_turret_block_barrel.obj"),
MINE_MESH(MESH_DIRECTORY "/mine.obj"),
ALWAYS_DROP_BLOCK_MESH(MESH_DIRECTORY "/always_drop_block.obj"),
REGEN_BLOCK_MESH(MESH_DIRECTORY "/regen_block.obj"),

SKYBOX_MESH(MESH_DIRECTORY "/skybox.obj"),
ITEM_MESH(MESH_DIRECTORY "/item.obj"),
PADDLE_LASER_ATTACHMENT_MESH(MESH_DIRECTORY "/paddle_laser_attachment.obj"),
PADDLE_BEAM_ATTACHMENT_MESH(MESH_DIRECTORY "/paddle_beam_attachment.obj"),
PADDLE_MINE_ATTACHMENT_MESH(MESH_DIRECTORY "/paddle_mine_attachment.obj"),
PADDLE_STICKY_ATTACHMENT_MESH(MESH_DIRECTORY "/paddle_sticky_attachment.obj"),
PADDLE_ROCKET_MESH(MESH_DIRECTORY "/paddle_rocket.obj"),
PADDLE_REMOTE_CONTROL_ROCKET_MESH(MESH_DIRECTORY "/remote_control_rocket.obj"),
TURRET_ROCKET_MESH(MESH_DIRECTORY "/turret_rocket.obj"),

ONE_WAY_BLOCK_UP_MESH(MESH_DIRECTORY    "/one_way_block_up.obj"),
ONE_WAY_BLOCK_DOWN_MESH(MESH_DIRECTORY  "/one_way_block_down.obj"),
ONE_WAY_BLOCK_LEFT_MESH(MESH_DIRECTORY  "/one_way_block_left.obj"),
ONE_WAY_BLOCK_RIGHT_MESH(MESH_DIRECTORY "/one_way_block_right.obj"),

SKYBEAM_MESH(MESH_DIRECTORY "/skybeam.obj"),

// PRE-MODERNISM ==================================================================

// Classical Asset Paths
CLASSICAL_PADDLE_MESH(CLASSICAL_MESH_DIRECTORY "/classical_paddle.obj"),
CLASSICAL_BACKGROUND_MESH(CLASSICAL_MESH_DIRECTORY "/classical_background.obj"),
CLASSICAL_BLOCK_MESH(CLASSICAL_MESH_DIRECTORY "/classical_block.obj"),

CLASSICAL_BOSS_ARM_MESH(CLASSICAL_BOSS_MESH_DIRECTORY "/classical_boss_arm.obj"),
CLASSICAL_BOSS_ARM_SQUARE_MESH(CLASSICAL_BOSS_MESH_DIRECTORY "/classical_boss_arm_square.obj"),
CLASSICAL_BOSS_BASE_MESH(CLASSICAL_BOSS_MESH_DIRECTORY "/classical_boss_base.obj"),
CLASSICAL_BOSS_BODY_COLUMN_MESH(CLASSICAL_BOSS_MESH_DIRECTORY "/classical_boss_body_column.obj"),
CLASSICAL_BOSS_EYE_MESH(CLASSICAL_BOSS_MESH_DIRECTORY "/classical_boss_eye.obj"),
CLASSICAL_BOSS_PEDIMENT_MESH(CLASSICAL_BOSS_MESH_DIRECTORY "/classical_boss_pediment.obj"),
CLASSICAL_BOSS_TABLATURE_MESH(CLASSICAL_BOSS_MESH_DIRECTORY "/classical_boss_tablature.obj"),


// Gothic-Romanticism Asset Paths
GOTHIC_ROMANTIC_PADDLE_MESH(GOTHIC_ROMANTIC_MESH_DIRECTORY "/gothic_romantic_paddle.obj"),
GOTHIC_ROMANTIC_BACKGROUND_MESH(GOTHIC_ROMANTIC_MESH_DIRECTORY "/gothic_romantic_background.obj"),
GOTHIC_ROMANTIC_BLOCK_MESH(GOTHIC_ROMANTIC_MESH_DIRECTORY "/gothic_romantic_block.obj"),

GOTHIC_ROMANTIC_BOSS_TOP_POINT_MESH(GOTHIC_ROMANTIC_BOSS_MESH_DIRECTORY "/gothic_romantic_boss_top_point.obj"),
GOTHIC_ROMANTIC_BOSS_BOTTOM_POINT_MESH(GOTHIC_ROMANTIC_BOSS_MESH_DIRECTORY "/gothic_romantic_boss_bottom_point.obj"),
GOTHIC_ROMANTIC_BOSS_BODY_MESH(GOTHIC_ROMANTIC_BOSS_MESH_DIRECTORY "/gothic_romantic_boss_body.obj"),
GOTHIC_ROMANTIC_BOSS_LEG_MESH(GOTHIC_ROMANTIC_BOSS_MESH_DIRECTORY "/gothic_romantic_boss_leg.obj"),


// MODERNISM =======================================================================

// Nouveau Asset Paths
NOUVEAU_PADDLE_MESH(NOUVEAU_MESH_DIRECTORY "/nouveau_paddle.obj"),
NOUVEAU_BACKGROUND_MESH(NOUVEAU_MESH_DIRECTORY "/nouveau_background.obj"),
NOUVEAU_BLOCK_MESH(NOUVEAU_MESH_DIRECTORY "/nouveau_block.obj"),

NOUVEAU_BOSS_BODY_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_body.obj"),
NOUVEAU_BOSS_BOTTOM_CURL_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_bottom_curl.obj"),
NOUVEAU_BOSS_BOTTOM_HEX_SPHERE_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_bottom_hexsphere.obj"),
NOUVEAU_BOSS_SIDE_CURLS_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_side_curls.obj"),
NOUVEAU_BOSS_SIDE_SPHERE_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_side_sphere.obj"),
NOUVEAU_BOSS_SIDE_SPHERE_FRILLS_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_side_sphere_frills.obj"),
NOUVEAU_BOSS_SIDE_SPHERE_HOLDER_CURL_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_side_sphere_holder_curl.obj"),
NOUVEAU_BOSS_TOP_ENCLOSURE_DOME_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_top_enclosure_dome.obj"),
NOUVEAU_BOSS_TOP_ENCLOSURE_GAZEBO_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_top_enclosure_gazebo.obj"),
NOUVEAU_BOSS_TOP_SPHERE_MESH(NOUVEAU_BOSS_MESH_DIRECTORY "/nouveau_boss_top_sphere.obj"),


// Deco Asset Paths
DECO_PADDLE_MESH(DECO_MESH_DIRECTORY "/deco_paddle.obj"),
DECO_BACKGROUND_MESH(DECO_MESH_DIRECTORY "/deco_background.obj"),
DECO_BLOCK_MESH(DECO_MESH_DIRECTORY "/deco_block.obj"),

DECO_BOSS_CORE_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_core.obj"),
DECO_BOSS_LIGHTNING_RELAY_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_lightning_relay.obj"),
DECO_BOSS_GEAR_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_gear.obj"),
DECO_BOSS_SCOPING_ARM1_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_scoping_arm1.obj"),
DECO_BOSS_SCOPING_ARM2_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_scoping_arm2.obj"),
DECO_BOSS_SCOPING_ARM3_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_scoping_arm3.obj"),
DECO_BOSS_SCOPING_ARM4_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_scoping_arm4.obj"),
DECO_BOSS_HAND_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_hand.obj"),
DECO_BOSS_LEFT_BODY_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_left_body.obj"),
DECO_BOSS_RIGHT_BODY_MESH(DECO_BOSS_MESH_DIRECTORY "/deco_boss_right_body.obj"),

// Futurism Asset Paths
FUTURISM_PADDLE_MESH(FUTURISM_MESH_DIRECTORY "/futurism_paddle.obj"),
FUTURISM_BACKGROUND_MESH(FUTURISM_MESH_DIRECTORY "/futurism_background.obj"),
FUTURISM_BLOCK_MESH(FUTURISM_MESH_DIRECTORY "/futurism_block.obj")

// Surrealism-Dada Asset Paths
// TODO

// POST-MODERNISM ==================================================================
// ...

{
}

GameViewConstants::~GameViewConstants() {
}

// Gets the string of the texture file name for the given xbox 360 button type
const char* GameViewConstants::GetXBoxButtonTextureName(GameViewConstants::XBoxButtonType buttonType) const {
    switch (buttonType) {
        case XBoxPushButton:
            return this->TEXTURE_XBOX_CONTROLLER_BUTTON;
        case XBoxAnalogStick:
            return this->TEXTURE_XBOX_CONTROLLER_ANALOG_STICK;
        case XBoxTrigger:
            return this->TEXTURE_XBOX_CONTROLLER_TRIGGER;
        default:
            assert(false);
            break;
    }
    return NULL;
}

// Gets the string of the texture file name for the given keyboard key type
const char* GameViewConstants::GetKeyboardButtonTextureName(GameViewConstants::KeyboardButtonType buttonType) const {
    switch (buttonType) {
        case KeyboardChar:
            return this->TEXTURE_SHORT_KEYBOARD_KEY;
        case KeyboardSpaceBar:
            return this->TEXTURE_LONG_KEYBOARD_KEY;
        default:
            assert(false);
            break;
    }
    return NULL;
}

const char* GameViewConstants::GetMoustButtonTextureName(MouseButtonType buttonType) const {
    switch (buttonType) {
        case LeftMouseButton:
            return this->TEXTURE_LEFT_MOUSE_BUTTON;
        case RightMouseButton:
            return this->TEXTURE_RIGHT_MOUSE_BUTTON;
        default:
            assert(false);
            break;
    }
    return NULL;
}


// Get the offsets from the top left corner of the button for the label
void GameViewConstants::GetXBoxButtonLabelOffset(XBoxButtonType buttonType, float buttonWidth, float buttonHeight,
                                                 float labelWidth, float labelHeight,
                                                 float& offsetX, float& offsetY) const {
    switch (buttonType) {
        case XBoxPushButton:
            offsetX = (buttonWidth - labelWidth)  / 2.0f;
            offsetY = -(buttonHeight - labelHeight) / 2.0f;
            break;
        case XBoxAnalogStick:
            offsetX = (buttonHeight - labelWidth) / 2.0f;
            offsetY = -(1.1f * buttonHeight);
            break;
        case XBoxTrigger:
            offsetX = (buttonHeight - labelWidth) / 2.0f;
            offsetY = -buttonHeight + labelHeight + 0.2*buttonHeight;
            break;
        default:
            assert(false);
            break;
    }
}

// Get the offsets from the top left corner of the button for the label
void GameViewConstants::GetKeyboardButtonLabelOffset(KeyboardButtonType buttonType, float buttonSize,
                                                     float labelWidth, float labelHeight,
                                                     float& offsetX, float& offsetY) const {
    switch (buttonType) {
        case KeyboardChar:
            offsetX = (buttonSize - labelWidth)  / 3.25f;
            offsetY = -(buttonSize - labelHeight) / 3.0f;
            break;
        case KeyboardSpaceBar:
            offsetX = (buttonSize - labelWidth)  / 3.25f;
            offsetY = -(buttonSize - labelHeight) / 3.0f;
            break;
        default:
            assert(false);
            break;
    }
}

void GameViewConstants::GetMouseButtonLabelOffset(MouseButtonType buttonType, float buttonSize,
                                                  float labelWidth, float labelHeight,
                                                  float& offsetX, float& offsetY) const {
    UNUSED_PARAMETER(labelHeight);
    switch (buttonType) {
        case LeftMouseButton:
            offsetX = (buttonSize - labelWidth)  / 2.0f;
            offsetY = -(1.1f * buttonSize);
            break;
        case RightMouseButton:
            offsetX = (buttonSize - labelWidth)  / 2.0f;
            offsetY = -(1.1f * buttonSize);
            break;
        default:
            assert(false);
            break;
    }
}

void GameViewConstants::GetWorldTypeBlockTextureNameMap(std::map<GameWorld::WorldStyle, const char*>& styleToTexName) const {
    styleToTexName.clear();
    styleToTexName.insert(std::make_pair(GameWorld::Classical, TEXTURE_CLASSICAL_BLOCK_IMG));
    styleToTexName.insert(std::make_pair(GameWorld::GothicRomantic, TEXTURE_GOTHIC_ROMANTIC_BLOCK_IMG));
    styleToTexName.insert(std::make_pair(GameWorld::Nouveau, TEXTURE_NOUVEAU_BLOCK_IMG));
    styleToTexName.insert(std::make_pair(GameWorld::Deco, TEXTURE_DECO_BLOCK_IMG));
}

/**
 * Gets the colour associated with the given item disposition.
 */
const Colour& GameViewConstants::GetItemColourFromDisposition(const GameItem::ItemDisposition& itemDisposition) const {
    switch (itemDisposition) {
        case GameItem::Good:
            return this->ITEM_GOOD_COLOUR;
        case GameItem::Neutral:
            return this->ITEM_NEUTRAL_COLOUR;
        case GameItem::Bad:
            return this->ITEM_BAD_COLOUR;
        default:
            assert(false);
            break;
    }
    return this->ITEM_NEUTRAL_COLOUR;
}

const Colour& GameViewConstants::GetMultiplierColour(int multiplier) {
    switch (multiplier) {
        case 1:
            break;
        case 2:
            return GameViewConstants::GetInstance()->TWO_TIMES_MULTIPLIER_COLOUR;
        case 3:
            return GameViewConstants::GetInstance()->THREE_TIMES_MULTIPLIER_COLOUR;
        case 4:
            return GameViewConstants::GetInstance()->FOUR_TIMES_MULTIPLIER_COLOUR;
        default:
            assert(false);
            break;
    }
    return GameViewConstants::GetInstance()->TWO_TIMES_MULTIPLIER_COLOUR;
}

void GameViewConstants::GetItemColourRandomnessFromDisposition(const GameItem::ItemDisposition& itemDisposition,
                                                               ESPInterval& redRandomColour, ESPInterval& greenRandomColour, ESPInterval& blueRandomColour,
                                                               ESPInterval& redColour, ESPInterval& greenColour, ESPInterval& blueColour) const {
    redColour   = ESPInterval(0);
    greenColour = ESPInterval(0);
    blueColour  = ESPInterval(0);

    switch (itemDisposition) {
        case GameItem::Good:
            greenRandomColour = ESPInterval(0.8f, 1.0f);
            redColour         = ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.R());
            greenColour       = ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.G());
            blueColour        = ESPInterval(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR.B());
            break;

        case GameItem::Bad:
            redRandomColour	  = ESPInterval(0.8f, 1.0f);
            greenRandomColour = ESPInterval(0.0f, 0.70f);
            blueRandomColour  = ESPInterval(0.0f, 0.70f);
            redColour         = ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.R());
            greenColour       = ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.G());
            blueColour	      = ESPInterval(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR.B());
            break;

        case GameItem::Neutral:
            blueRandomColour = ESPInterval(0.8f, 1.0f);
            redColour        = ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.R());
            greenColour      = ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.G());
            blueColour       = ESPInterval(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR.B());
            break;

        default:
            assert(false);
            break;
    }
}

AnimationMultiLerp<Colour> GameViewConstants::BuildFlashingColourAnimation() const {
    static const double FLASH_TIME = 0.5;

	std::vector<double> timeVals;
	timeVals.reserve(3);
	timeVals.push_back(0.0);
	timeVals.push_back(FLASH_TIME);
	timeVals.push_back(2 * FLASH_TIME);
	std::vector<Colour> colourVals;
	colourVals.reserve(3);
	colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
	colourVals.push_back(Colour(1.0f, 0.8f, 0.0f));
	colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
	
    AnimationMultiLerp<Colour> flashingColourAnimation;
    flashingColourAnimation.SetLerp(timeVals, colourVals);
	flashingColourAnimation.SetRepeat(true);
    return flashingColourAnimation;
}