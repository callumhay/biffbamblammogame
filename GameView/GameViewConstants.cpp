#include "GameViewConstants.h"

GameViewConstants* GameViewConstants::Instance = NULL;

#define RESOURCE_DIRECTORY "resources"
#define FONT_DIRECTORY RESOURCE_DIRECTORY "/fonts"
#define MESH_DIRECTORY RESOURCE_DIRECTORY "/models"
#define SHADER_DIRECTORY RESOURCE_DIRECTORY "/shaders"
#define TEXTURE_DIRECTORY RESOURCE_DIRECTORY "/textures"

GameViewConstants::GameViewConstants() :
// Basic Asset Paths
RESOURCE_DIR(RESOURCE_DIRECTORY),
FONT_DIR(FONT_DIRECTORY),
MESH_DIR(MESH_DIRECTORY),
SHADER_DIR(SHADER_DIRECTORY),
TEXTURE_DIR(TEXTURE_DIRECTORY),

// Shader Asset Paths
CGFX_CEL_SHADER(SHADER_DIRECTORY "/CelShading.cgfx"),
CGFX_PHONG_SHADER(SHADER_DIRECTORY "/OutlinedPhong.cgfx"),
CGFX_POSTREFRACT_SHADER(SHADER_DIRECTORY "/PostRefract.cgfx"),
CGFX_VOLUMEMETRIC_SHADER(SHADER_DIRECTORY "/VolumetricEffect.cgfx"),

// Texture Asset Paths
TEXTURE_BANG1(TEXTURE_DIRECTORY  "/Bang1_256x128.png"),
TEXTURE_BANG2(TEXTURE_DIRECTORY  "/Bang2_256x128.png"),
TEXTURE_BANG3(TEXTURE_DIRECTORY  "/Bang3_256x128.png"),
TEXTURE_SMOKE1(TEXTURE_DIRECTORY "/Smoke_Puff1_64x64.png"),
TEXTURE_SMOKE2(TEXTURE_DIRECTORY "/Smoke_Puff2_64x64.png"),
TEXTURE_SMOKE3(TEXTURE_DIRECTORY "/Smoke_Puff3_64x64.png"),
TEXTURE_SMOKE4(TEXTURE_DIRECTORY "/Smoke_Puff4_64x64.png"),
TEXTURE_SMOKE5(TEXTURE_DIRECTORY "/Smoke_Puff5_64x64.png"),
TEXTURE_SMOKE6(TEXTURE_DIRECTORY "/Smoke_Puff6_64x64.png"),
TEXTURE_CIRCLE_GRADIENT(TEXTURE_DIRECTORY "/Circle_Gradient_64x64.png"),
TEXTURE_STAR(TEXTURE_DIRECTORY "/Star_Fill_64x64.png"),
TEXTURE_STAR_OUTLINE(TEXTURE_DIRECTORY "/Star_Outline_64x64.png"),

// Item Drop Texture Asset Paths
TEXTURE_ITEM_SLOWBALL(TEXTURE_DIRECTORY   "/slowball_powerup256x128.jpg"),
TEXTURE_ITEM_FASTBALL(TEXTURE_DIRECTORY   "/fastball_powerdown256x128.jpg"),
TEXTURE_ITEM_UBERBALL(TEXTURE_DIRECTORY   "/uberball_powerup256x128.jpg"),
TEXTURE_ITEM_INVISIBALL(TEXTURE_DIRECTORY "/invisiball_powerdown256x128.jpg"),
TEXTURE_ITEM_GHOSTBALL(TEXTURE_DIRECTORY	"/ghostball_powerneutral256x128.jpg"),

// Item Timer (Outline) Texture Asset Paths
TEXTURE_ITEM_TIMER_SLOWBALL(TEXTURE_DIRECTORY		"/slowball_timer_hud128x64.png"),
TEXTURE_ITEM_TIMER_FASTBALL(TEXTURE_DIRECTORY		"/fastball_timer_hud128x64.png"),
TEXTURE_ITEM_TIMER_UBERBALL(TEXTURE_DIRECTORY		"/uberball_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_INVISIBALL(TEXTURE_DIRECTORY	"/invisiball_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_GHOSTBALL(TEXTURE_DIRECTORY	"/ghostball_timer_hud256x128.png"),

// Item Timer (Fill) Texture Asset Paths
TEXTURE_ITEM_TIMER_FILLER_SPDBALL(TEXTURE_DIRECTORY			"/ballspeed_timer_fill_hud128x64.png"),
TEXTURE_ITEM_TIMER_FILLER_UBERBALL(TEXTURE_DIRECTORY		"/uberball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_INVISIBALL(TEXTURE_DIRECTORY	"/invisiball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL(TEXTURE_DIRECTORY		"/ghostball_timer_fill_hud256x128.png"),

// Font Asset Paths
FONT_HAPPYGOOD(FONT_DIRECTORY "/happygood.ttf"),
FONT_GUNBLAM(FONT_DIRECTORY "/gunblam.ttf"),
FONT_EXPLOSIONBOOM(FONT_DIRECTORY "/explosionboom.ttf"),
FONT_ELECTRICZAP(FONT_DIRECTORY "/electriczap.ttf"),
FONT_ALLPURPOSE(FONT_DIRECTORY "/allpurpose.ttf"),

// Item-related constants
ITEM_LABEL_MATGRP("ItemLabel"),	// Material group name for changing the label on the item mesh
ITEM_END_MATGRP("ColourEnd"),		// Material group name for changing the colour on the item mesh
ITEM_GOOD_COLOUR(0.0f, 0.8f, 0.0f),
ITEM_BAD_COLOUR(0.8f, 0.0f, 0.0f),
ITEM_NEUTRAL_COLOUR(0.0f, 0.6f, 1.0f),

// World-Related Asset Paths
BALL_MESH(MESH_DIRECTORY "/ball.obj"),
SPIKEY_BALL_MESH(MESH_DIRECTORY "/spikey_ball.obj"),
BASIC_BLOCK_MESH_PATH(MESH_DIRECTORY "/block.obj"),
BOMB_BLOCK_MESH(MESH_DIRECTORY "/bomb_block.obj"),
SKYBOX_MESH(MESH_DIRECTORY "/skybox.obj"),
ITEM_MESH(MESH_DIRECTORY "/item.obj"),

// Deco Asset Paths
DECO_PADDLE_MESH(MESH_DIRECTORY "/deco_paddle.obj"),
DECO_BACKGROUND_MESH(MESH_DIRECTORY "/deco_background.obj"),
DECO_SKYBEAM_MESH(MESH_DIRECTORY "/deco_background_beam.obj"),
DECO_BLOCK_MESH_PATH(MESH_DIRECTORY "/deco_block.obj")
{
}

GameViewConstants::~GameViewConstants() {
}
