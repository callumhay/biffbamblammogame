#include "GameViewConstants.h"

#include "../GameModel/GameItem.h"
#include "../GameModel/BallSpeedItem.h"
#include "../GameModel/UberBallItem.h"
#include "../GameModel/InvisiBallItem.h"
#include "../GameModel/GhostBallItem.h"
#include "../GameModel/LaserPaddleItem.h"
#include "../GameModel/MultiBallItem.h"
#include "../GameModel/PaddleSizeItem.h"
#include "../GameModel/BallSizeItem.h"
#include "../GameModel/BlackoutItem.h"
#include "../GameModel/UpsideDownItem.h"
#include "../GameModel/BallSafetyNetItem.h"
#include "../GameModel/OneUpItem.h"
#include "../GameModel/PoisonPaddleItem.h"

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
CGFX_PHONG_SHADER(SHADER_DIRECTORY "/Phong.cgfx"),
CGFX_INKBLOCK_SHADER(SHADER_DIRECTORY "/InkBlock.cgfx"),
CGFX_POSTREFRACT_SHADER(SHADER_DIRECTORY "/PostRefract.cgfx"),
CGFX_VOLUMEMETRIC_SHADER(SHADER_DIRECTORY "/VolumetricEffect.cgfx"),
CGFX_DECOSKYBOX_SHADER(SHADER_DIRECTORY "/DecoSkybox.cgfx"),
CGFX_GAUSSIAN_SHADER(SHADER_DIRECTORY "/GaussianBlur.cgfx"),
CGFX_AFTERIMAGE_SHADER(SHADER_DIRECTORY "/MotionBlurAndAfterImage.cgfx"),
CGFX_BLOOM_SHADER(SHADER_DIRECTORY "/Bloom.cgfx"),

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
TEXTURE_EXPLOSION_CLOUD(TEXTURE_DIRECTORY "/Explosion_128x128.png"),
TEXTURE_EXPLOSION_RAYS(TEXTURE_DIRECTORY "/Explosion_Rays_128x128.png"),
TEXTURE_LASER_BEAM(TEXTURE_DIRECTORY "/Laser_32x64.png"),
TEXTURE_UP_ARROW(TEXTURE_DIRECTORY "/Arrow_32x64.png"),
TEXTURE_BALL_LIFE_HUD(TEXTURE_DIRECTORY "/Ball_life_hud32x32.png"),
TEXTURE_SPARKLE(TEXTURE_DIRECTORY "/Sparkle32x32.png"),
TEXTURE_SPIRAL_SMALL(TEXTURE_DIRECTORY "/SmallSpiral_32x32.png"),
TEXTURE_SPIRAL_MEDIUM(TEXTURE_DIRECTORY "/MedSpiral_64x64.png"),
TEXTURE_SPIRAL_LARGE(TEXTURE_DIRECTORY "/BigSpiral_128x128.png"),

TEXTURE_STARFIELD(TEXTURE_DIRECTORY "/starfield_1024x1024.jpg"),

TEXTURE_BALL_SAFETY_NET(TEXTURE_DIRECTORY "/warning_platform_128x64.jpg"),

// Item Drop Texture Asset Paths
TEXTURE_ITEM_SLOWBALL(TEXTURE_DIRECTORY			"/slowball_powerup256x128.jpg"),
TEXTURE_ITEM_FASTBALL(TEXTURE_DIRECTORY			"/fastball_powerdown256x128.jpg"),
TEXTURE_ITEM_UBERBALL(TEXTURE_DIRECTORY			"/uberball_powerup256x128.jpg"),
TEXTURE_ITEM_INVISIBALL(TEXTURE_DIRECTORY		"/invisiball_powerdown256x128.jpg"),
TEXTURE_ITEM_GHOSTBALL(TEXTURE_DIRECTORY		"/ghostball_powerneutral256x128.jpg"),
TEXTURE_ITEM_PADDLELASER(TEXTURE_DIRECTORY	"/paddlelaser_powerup256x128.jpg"),
TEXTURE_ITEM_MULTIBALL3(TEXTURE_DIRECTORY		"/multiball3_powerup256x128.jpg"),
TEXTURE_ITEM_MULTIBALL5(TEXTURE_DIRECTORY		"/multiball5_powerup256x128.jpg"),
TEXTURE_ITEM_PADDLEGROW(TEXTURE_DIRECTORY		"/paddlegrow_powerup256x128.jpg"),
TEXTURE_ITEM_PADDLESHRINK(TEXTURE_DIRECTORY "/paddleshrink_powerdown256x128.jpg"),
TEXTURE_ITEM_BALLGROW(TEXTURE_DIRECTORY			"/ballgrow_powerup256x128.jpg"),
TEXTURE_ITEM_BALLSHRINK(TEXTURE_DIRECTORY		"/ballshrink_powerdown256x128.jpg"),
TEXTURE_ITEM_BLACKOUT(TEXTURE_DIRECTORY			"/blackout_powerdown256x128.jpg"),
TEXTURE_ITEM_UPSIDEDOWN(TEXTURE_DIRECTORY		"/upsidedown_powerdown256x128.jpg"),
TEXTURE_ITEM_BALLSAFETYNET(TEXTURE_DIRECTORY "/ballsafetynet_powerup256x128.jpg"),
TEXTURE_ITEM_1UP(TEXTURE_DIRECTORY					"/1up_powerup256x128.jpg"),
TEXTURE_ITEM_POISON(TEXTURE_DIRECTORY				"/poison_powerdown_256x128.jpg"),

// Item Timer (Outline) Texture Asset Paths
TEXTURE_ITEM_TIMER_SLOWBALL(TEXTURE_DIRECTORY			"/slowball_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_FASTBALL(TEXTURE_DIRECTORY			"/fastball_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_UBERBALL(TEXTURE_DIRECTORY			"/uberball_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_INVISIBALL(TEXTURE_DIRECTORY		"/invisiball_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_GHOSTBALL(TEXTURE_DIRECTORY		"/ghostball_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_PADDLELASER(TEXTURE_DIRECTORY  "/paddlelaser_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_BLACKOUT(TEXTURE_DIRECTORY			"/blackout_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_UPSIDEDOWN(TEXTURE_DIRECTORY		"/upsidedown_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_POISON(TEXTURE_DIRECTORY				"/poison_timer_hud256x128.png"),

// Item Timer (Fill) Texture Asset Paths
TEXTURE_ITEM_TIMER_FILLER_SPDBALL(TEXTURE_DIRECTORY			"/ballspeed_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_UBERBALL(TEXTURE_DIRECTORY		"/uberball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_INVISIBALL(TEXTURE_DIRECTORY	"/invisiball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL(TEXTURE_DIRECTORY		"/ghostball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_PADDLELASER(TEXTURE_DIRECTORY "/paddlelaser_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_BLACKOUT(TEXTURE_DIRECTORY		"/blackout_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_UPSIDEDOWN(TEXTURE_DIRECTORY	"/upsidedown_timer_fill256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_POISON(TEXTURE_DIRECTORY			"/poison_timer_fill_hud256x128.png"),

// Font Asset Paths
FONT_SADBAD(FONT_DIRECTORY				"/sadbad.ttf"),
FONT_HAPPYGOOD(FONT_DIRECTORY			"/happygood.ttf"),
FONT_EXPLOSIONBOOM(FONT_DIRECTORY "/explosionboom.ttf"),
FONT_ELECTRICZAP(FONT_DIRECTORY		"/electriczap.ttf"),
FONT_ALLPURPOSE(FONT_DIRECTORY		"/allpurpose.ttf"),

// Item-related constants
ITEM_LABEL_MATGRP("ItemLabel"),	// Material group name for changing the label on the item mesh
ITEM_END_MATGRP("ColourEnd"),		// Material group name for changing the colour on the item mesh

ITEM_GOOD_COLOUR(0.0f, 0.8f, 0.0f),
ITEM_BAD_COLOUR(0.8f, 0.0f, 0.0f),
ITEM_NEUTRAL_COLOUR(0.0f, 0.6f, 1.0f),

DEFAULT_BALL_LIGHT_ATTEN(0.5f),
DEFAULT_BALL_KEY_LIGHT_COLOUR(0.9f, 0.9f, 0.9f),
DEFAULT_FG_KEY_LIGHT_COLOUR(0.65f, 0.65f, 0.7f),
DEFAULT_FG_FILL_LIGHT_COLOUR(0.7, 0.7f, 0.7f),
DEFAULT_BG_KEY_LIGHT_COLOUR(0.7f, 0.75f, 0.75f),
DEFAULT_BG_FILL_LIGHT_COLOUR(0.6f, 0.6f, 0.6f),
DEFAULT_BALL_LIGHT_COLOUR(0.85f, 0.85f, 0.95f),
BLACKOUT_LIGHT_COLOUR(0.0f, 0.0f, 0.0f),
POISON_LIGHT_DEEP_COLOUR(0.10f, 0.50f, 0.10f),
POISON_LIGHT_LIGHT_COLOUR(0.75f, 1.0f, 0.0f),
GHOST_BALL_COLOUR(0.643f, 0.725f, 0.843f),
UBER_BALL_COLOUR(1.0f, 0.0f, 0.0f),
INK_BLOCK_COLOUR(0.111f, 0.137f, 0.289f),

// World-Related Asset Paths
BALL_MESH(MESH_DIRECTORY "/ball.obj"),
SPIKEY_BALL_MESH(MESH_DIRECTORY "/spikey_ball.obj"),
BASIC_BLOCK_MESH_PATH(MESH_DIRECTORY "/block.obj"),
TRIANGLE_BLOCK_MESH_PATH(MESH_DIRECTORY "/triangle_block.obj"),
BOMB_BLOCK_MESH(MESH_DIRECTORY "/bomb_block.obj"),
SKYBOX_MESH(MESH_DIRECTORY "/skybox.obj"),
ITEM_MESH(MESH_DIRECTORY "/item.obj"),
PADDLE_LASER_ATTACHMENT_MESH(MESH_DIRECTORY "/paddle_laser_attachment.obj"),

// Deco Asset Paths
DECO_PADDLE_MESH(MESH_DIRECTORY "/deco_paddle.obj"),
DECO_BACKGROUND_MESH(MESH_DIRECTORY "/deco_background.obj"),
DECO_SKYBEAM_MESH(MESH_DIRECTORY "/deco_background_beam.obj"),
DECO_BLOCK_MESH_PATH(MESH_DIRECTORY "/deco_block.obj")
{

	// Initialize the timer texture arrays
	this->InitItemTextures();
	this->InitItemTimerTextures();
	this->InitItemTimerFillerTextures();
}

void GameViewConstants::InitItemTextures() {
	this->itemTextures.insert(std::make_pair(BallSpeedItem::SLOW_BALL_ITEM_NAME,						this->TEXTURE_ITEM_SLOWBALL));
	this->itemTextures.insert(std::make_pair(BallSpeedItem::FAST_BALL_ITEM_NAME,						this->TEXTURE_ITEM_FASTBALL));
	this->itemTextures.insert(std::make_pair(UberBallItem::UBER_BALL_ITEM_NAME,							this->TEXTURE_ITEM_UBERBALL));
	this->itemTextures.insert(std::make_pair(InvisiBallItem::INVISI_BALL_ITEM_NAME,					this->TEXTURE_ITEM_INVISIBALL));
	this->itemTextures.insert(std::make_pair(GhostBallItem::GHOST_BALL_ITEM_NAME,						this->TEXTURE_ITEM_GHOSTBALL));
	this->itemTextures.insert(std::make_pair(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,				this->TEXTURE_ITEM_PADDLELASER));
	this->itemTextures.insert(std::make_pair(MultiBallItem::MULTI3_BALL_ITEM_NAME,					this->TEXTURE_ITEM_MULTIBALL3));
	this->itemTextures.insert(std::make_pair(MultiBallItem::MULTI5_BALL_ITEM_NAME,					this->TEXTURE_ITEM_MULTIBALL5));
	this->itemTextures.insert(std::make_pair(PaddleSizeItem::PADDLE_GROW_ITEM_NAME,					this->TEXTURE_ITEM_PADDLEGROW));
	this->itemTextures.insert(std::make_pair(PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME,				this->TEXTURE_ITEM_PADDLESHRINK));
	this->itemTextures.insert(std::make_pair(BallSizeItem::BALL_GROW_ITEM_NAME,							this->TEXTURE_ITEM_BALLGROW));
	this->itemTextures.insert(std::make_pair(BallSizeItem::BALL_SHRINK_ITEM_NAME,						this->TEXTURE_ITEM_BALLSHRINK));
	this->itemTextures.insert(std::make_pair(BlackoutItem::BLACKOUT_ITEM_NAME,							this->TEXTURE_ITEM_BLACKOUT));
	this->itemTextures.insert(std::make_pair(UpsideDownItem::UPSIDEDOWN_ITEM_NAME,					this->TEXTURE_ITEM_UPSIDEDOWN));
	this->itemTextures.insert(std::make_pair(BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME,	this->TEXTURE_ITEM_BALLSAFETYNET));
	this->itemTextures.insert(std::make_pair(OneUpItem::ONE_UP_ITEM_NAME,										this->TEXTURE_ITEM_1UP));
	this->itemTextures.insert(std::make_pair(PoisonPaddleItem::POISON_PADDLE_ITEM_NAME,			this->TEXTURE_ITEM_POISON));
}

void GameViewConstants::InitItemTimerTextures() {
	this->itemTimerTextures.insert(std::make_pair(BallSpeedItem::SLOW_BALL_ITEM_NAME,					this->TEXTURE_ITEM_TIMER_SLOWBALL));
	this->itemTimerTextures.insert(std::make_pair(BallSpeedItem::FAST_BALL_ITEM_NAME,					this->TEXTURE_ITEM_TIMER_FASTBALL));
	this->itemTimerTextures.insert(std::make_pair(UberBallItem::UBER_BALL_ITEM_NAME,					this->TEXTURE_ITEM_TIMER_UBERBALL));
	this->itemTimerTextures.insert(std::make_pair(InvisiBallItem::INVISI_BALL_ITEM_NAME,			this->TEXTURE_ITEM_TIMER_INVISIBALL));
	this->itemTimerTextures.insert(std::make_pair(GhostBallItem::GHOST_BALL_ITEM_NAME,				this->TEXTURE_ITEM_TIMER_GHOSTBALL));
	this->itemTimerTextures.insert(std::make_pair(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,		this->TEXTURE_ITEM_TIMER_PADDLELASER));
	this->itemTimerTextures.insert(std::make_pair(BlackoutItem::BLACKOUT_ITEM_NAME,						this->TEXTURE_ITEM_TIMER_BLACKOUT));
	this->itemTimerTextures.insert(std::make_pair(UpsideDownItem::UPSIDEDOWN_ITEM_NAME,				this->TEXTURE_ITEM_TIMER_UPSIDEDOWN));
	this->itemTimerTextures.insert(std::make_pair(PoisonPaddleItem::POISON_PADDLE_ITEM_NAME,	this->TEXTURE_ITEM_TIMER_POISON));
}

void GameViewConstants::InitItemTimerFillerTextures() {
	this->itemTimerFillerTextures.insert(std::make_pair(BallSpeedItem::SLOW_BALL_ITEM_NAME,					this->TEXTURE_ITEM_TIMER_FILLER_SPDBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(BallSpeedItem::FAST_BALL_ITEM_NAME,					this->TEXTURE_ITEM_TIMER_FILLER_SPDBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(UberBallItem::UBER_BALL_ITEM_NAME,					this->TEXTURE_ITEM_TIMER_FILLER_UBERBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(InvisiBallItem::INVISI_BALL_ITEM_NAME,			this->TEXTURE_ITEM_TIMER_FILLER_INVISIBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(GhostBallItem::GHOST_BALL_ITEM_NAME,				this->TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,		this->TEXTURE_ITEM_TIMER_FILLER_PADDLELASER));
	this->itemTimerFillerTextures.insert(std::make_pair(BlackoutItem::BLACKOUT_ITEM_NAME,						this->TEXTURE_ITEM_TIMER_FILLER_BLACKOUT));
	this->itemTimerFillerTextures.insert(std::make_pair(UpsideDownItem::UPSIDEDOWN_ITEM_NAME,				this->TEXTURE_ITEM_TIMER_FILLER_UPSIDEDOWN));
	this->itemTimerFillerTextures.insert(std::make_pair(PoisonPaddleItem::POISON_PADDLE_ITEM_NAME,	this->TEXTURE_ITEM_TIMER_FILLER_POISON));
}

GameViewConstants::~GameViewConstants() {
}
