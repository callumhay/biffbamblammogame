#include "GameViewConstants.h"

GameViewConstants* GameViewConstants::Instance = NULL;

#define RESOURCE_DIRECTORY "resources"
#define FONT_DIRECTORY RESOURCE_DIRECTORY "/fonts"
#define MESH_DIRECTORY RESOURCE_DIRECTORY "/models"
#define SHADER_DIRECTORY RESOURCE_DIRECTORY "/shaders"
#define TEXTURE_DIRECTORY RESOURCE_DIRECTORY "/textures"
#define SOUND_DIRECTORY RESOURCE_DIRECTORY "/sounds"

GameViewConstants::GameViewConstants() :
// Basic Asset Paths
RESOURCE_DIR(RESOURCE_DIRECTORY),
FONT_DIR(FONT_DIRECTORY),
MESH_DIR(MESH_DIRECTORY),
SHADER_DIR(SHADER_DIRECTORY),
TEXTURE_DIR(TEXTURE_DIRECTORY),
SOUND_DIR(SOUND_DIRECTORY),

// Shader Asset Paths
CGFX_CEL_SHADER(SHADER_DIRECTORY										"/CelShading.cgfx"),
CGFX_PHONG_SHADER(SHADER_DIRECTORY									"/Phong.cgfx"),
CGFX_INKBLOCK_SHADER(SHADER_DIRECTORY								"/InkBlock.cgfx"),
CGFX_POSTREFRACT_SHADER(SHADER_DIRECTORY						"/PostRefract.cgfx"),
CGFX_VOLUMEMETRIC_SHADER(SHADER_DIRECTORY						"/VolumetricEffect.cgfx"),
CGFX_DECOSKYBOX_SHADER(SHADER_DIRECTORY							"/DecoSkybox.cgfx"),
CGFX_GAUSSIAN_SHADER(SHADER_DIRECTORY								"/GaussianBlur.cgfx"),
CGFX_AFTERIMAGE_SHADER(SHADER_DIRECTORY							"/MotionBlurAndAfterImage.cgfx"),
CGFX_BLOOM_SHADER(SHADER_DIRECTORY									"/Bloom.cgfx"),
CGFX_FULLSCREENGOO_SHADER(SHADER_DIRECTORY					"/FullscreenGoo.cgfx"),
CGFX_STICKYPADDLE_SHADER(SHADER_DIRECTORY						"/StickyPaddle.cgfx"),
CGFX_FULLSCREENSMOKEY_SHADER(SHADER_DIRECTORY				"/PostSmokey.cgfx"),
CGFX_FULLSCREEN_UBERINTENSE_SHADER(SHADER_DIRECTORY "/PostUberIntense.cgfx"),
CGFX_PRISM_SHADER(SHADER_DIRECTORY									"/Prism.cgfx"),
CGFX_PORTALBLOCK_SHADER(SHADER_DIRECTORY						"/PortalBlock.cgfx"),
CGFX_FIREBALL_SHADER(SHADER_DIRECTORY								"/FireBall.cgfx"),

// Texture Asset Paths
TEXTURE_BANG1(TEXTURE_DIRECTORY  "/Bang1_256x128.png"),
TEXTURE_BANG2(TEXTURE_DIRECTORY  "/Bang2_256x128.png"),
TEXTURE_BANG3(TEXTURE_DIRECTORY  "/Bang3_256x128.png"),
TEXTURE_SPLAT1(TEXTURE_DIRECTORY "/Splat1_256x128.png"),
TEXTURE_SMOKE1(TEXTURE_DIRECTORY "/Smoke_Puff1_64x64.png"),
TEXTURE_SMOKE2(TEXTURE_DIRECTORY "/Smoke_Puff2_64x64.png"),
TEXTURE_SMOKE3(TEXTURE_DIRECTORY "/Smoke_Puff3_64x64.png"),
TEXTURE_SMOKE4(TEXTURE_DIRECTORY "/Smoke_Puff4_64x64.png"),
TEXTURE_SMOKE5(TEXTURE_DIRECTORY "/Smoke_Puff5_64x64.png"),
TEXTURE_SMOKE6(TEXTURE_DIRECTORY "/Smoke_Puff6_64x64.png"),
TEXTURE_FIRE_GLOB1(TEXTURE_DIRECTORY "/fire_glob1_128x128.png"),
TEXTURE_FIRE_GLOB2(TEXTURE_DIRECTORY "/fire_glob2_128x128.png"),
TEXTURE_FIRE_GLOB3(TEXTURE_DIRECTORY "/fire_glob3_128x128.png"),
TEXTURE_CIRCLE_GRADIENT(TEXTURE_DIRECTORY "/Circle_Gradient_64x64.png"),
TEXTURE_STAR(TEXTURE_DIRECTORY "/Star_Fill_64x64.png"),
TEXTURE_STAR_OUTLINE(TEXTURE_DIRECTORY "/Star_Outline_64x64.png"),
TEXTURE_EVIL_STAR(TEXTURE_DIRECTORY "/Evil_Star_Fill_64x64.png"),
TEXTURE_EVIL_STAR_OUTLINE(TEXTURE_DIRECTORY "/Evil_Star_Outline_64x64.png"),
TEXTURE_EXPLOSION_CLOUD(TEXTURE_DIRECTORY "/Explosion_128x128.png"),
TEXTURE_EXPLOSION_RAYS(TEXTURE_DIRECTORY "/Explosion_Rays_128x128.png"),
TEXTURE_LASER_BEAM(TEXTURE_DIRECTORY "/Laser_32x64.png"),
TEXTURE_UP_ARROW(TEXTURE_DIRECTORY "/Arrow_32x64.png"),
TEXTURE_BALL_LIFE_HUD(TEXTURE_DIRECTORY "/Ball_life_hud32x32.png"),
TEXTURE_SPARKLE(TEXTURE_DIRECTORY "/Sparkle32x32.png"),
TEXTURE_SPIRAL_SMALL(TEXTURE_DIRECTORY "/SmallSpiral_32x32.png"),
TEXTURE_SPIRAL_MEDIUM(TEXTURE_DIRECTORY "/MedSpiral_64x64.png"),
TEXTURE_SPIRAL_LARGE(TEXTURE_DIRECTORY "/BigSpiral_128x128.png"),
TEXTURE_HALO(TEXTURE_DIRECTORY "/halo_64x64.png"),
TEXTURE_LENSFLARE(TEXTURE_DIRECTORY "/lens_flare_64x64.png"),
TEXTURE_TWISTED_SPIRAL(TEXTURE_DIRECTORY "/twisted_spiral_64x64.png"),
TEXTURE_SIDEBLAST(TEXTURE_DIRECTORY "/sideblast_128x128.png"),
TEXTURE_HUGE_EXPLOSION(TEXTURE_DIRECTORY "/huge_explosion_512x256.png"),
TEXTURE_BRIGHT_FLARE(TEXTURE_DIRECTORY "/flare_64x64.png"),
TEXTURE_LIGHTNING_BOLT(TEXTURE_DIRECTORY "/LightningBolt_32x64.png"),
TEXTURE_SPHERE_NORMALS(TEXTURE_DIRECTORY "/SphereNormals_128x128.png"),
TEXTURE_CLOUD(TEXTURE_DIRECTORY "/Cloud_64x64.png"),

TEXTURE_STARFIELD(TEXTURE_DIRECTORY "/starfield_1024x1024.jpg"),

TEXTURE_INKSPLATTER(TEXTURE_DIRECTORY "/ink_splatter_1024x512.png"),
TEXTURE_BALLTARGET(TEXTURE_DIRECTORY "/ball_target_64x64.png"),
TEXTURE_BULLET_CROSSHAIR(TEXTURE_DIRECTORY "/Crosshair_128x128.png"),
TEXTURE_BEAM_CROSSHAIR(TEXTURE_DIRECTORY "/beam_crosshairs_256x256.png"),
TEXTURE_BARREL_OVERLAY(TEXTURE_DIRECTORY "/barrel_overlay_1024x1024.png"),

TEXTURE_BALL_SAFETY_NET(TEXTURE_DIRECTORY "/warning_platform_128x64.jpg"),

TEXTURE_CEL_GRADIENT(TEXTURE_DIRECTORY "/celshading_texture1x256.jpg"),
TEXTURE_NOISE_OCTAVES(TEXTURE_DIRECTORY "/noise_octaves.raw"),

// Item Drop Texture Asset Paths
TEXTURE_ITEM_SLOWBALL(TEXTURE_DIRECTORY     "/slowball_powerup256x128.jpg"),
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
TEXTURE_ITEM_POISON(TEXTURE_DIRECTORY				"/poison_powerdown256x128.jpg"),
TEXTURE_ITEM_STICKYPADDLE(TEXTURE_DIRECTORY "/stickypaddle_powerup256x128.jpg"),
TEXTURE_ITEM_PADDLECAM(TEXTURE_DIRECTORY		"/paddlecam_powerdown_256x128.jpg"),
TEXTURE_ITEM_BALLCAM(TEXTURE_DIRECTORY			"/ballcam_powerdown_256x128.jpg"),
TEXTURE_ITEM_PADDLEBEAM(TEXTURE_DIRECTORY		"/laserbeampaddle_powerup_256x128.jpg"),
TEXTURE_ITEM_GRAVITYBALL(TEXTURE_DIRECTORY  "/gravityball_powerneutral256x128.jpg"),
TEXTURE_ITEM_PADDLEROCKET(TEXTURE_DIRECTORY "/paddlerocket_powerup256x128.jpg"),
TEXTURE_ITEM_CRAZYBALL(TEXTURE_DIRECTORY    "/crazyball_powerneutral256x128.jpg"),
TEXTURE_ITEM_SHIELDPADDLE(TEXTURE_DIRECTORY "/shieldpaddle_powerneutral_256x128.jpg"),
TEXTURE_ITEM_FIREBALL(TEXTURE_DIRECTORY			"/fireball_powerneutral256x128.jpg"),
TEXTURE_ITEM_RANDOM(TEXTURE_DIRECTORY       "/random_powerneutral_256x128.jpg"),

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
TEXTURE_ITEM_TIMER_STICKYPADDLE(TEXTURE_DIRECTORY "/stickypaddle_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_EYE(TEXTURE_DIRECTORY					"/cam_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_GRAVITYBALL(TEXTURE_DIRECTORY	"/gravityball_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_SHIELDPADDLE(TEXTURE_DIRECTORY "/shieldpaddle_timer_hud256x128.png"),
TEXTURE_ITEM_TIMER_FIREBALL(TEXTURE_DIRECTORY			"/fireball_timer_hud256x128.png"),

// Item Timer (Fill) Texture Asset Paths
TEXTURE_ITEM_TIMER_FILLER_SPDBALL(TEXTURE_DIRECTORY				"/ballspeed_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_UBERBALL(TEXTURE_DIRECTORY			"/uberball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_INVISIBALL(TEXTURE_DIRECTORY		"/invisiball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL(TEXTURE_DIRECTORY			"/ghostball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_PADDLELASER(TEXTURE_DIRECTORY		"/paddlelaser_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_BLACKOUT(TEXTURE_DIRECTORY			"/blackout_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_UPSIDEDOWN(TEXTURE_DIRECTORY		"/upsidedown_timer_fill256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_POISON(TEXTURE_DIRECTORY				"/poison_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_STICKYPADDLE(TEXTURE_DIRECTORY	"/stickypaddle_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_EYE(TEXTURE_DIRECTORY						"/cam_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_GRAVITYBALL(TEXTURE_DIRECTORY	  "/gravityball_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_SHIELDPADDLE(TEXTURE_DIRECTORY  "/shieldpaddle_timer_fill_hud256x128.png"),
TEXTURE_ITEM_TIMER_FILLER_FIREBALL(TEXTURE_DIRECTORY      "/fireball_timer_fill_hud256x128.png"),

// Font Asset Paths
FONT_SADBAD(FONT_DIRECTORY				"/sadbad.ttf"),
FONT_HAPPYGOOD(FONT_DIRECTORY			"/happygood.ttf"),
FONT_EXPLOSIONBOOM(FONT_DIRECTORY "/explosionboom.ttf"),
FONT_ELECTRICZAP(FONT_DIRECTORY		"/electriczap.ttf"),
FONT_ALLPURPOSE(FONT_DIRECTORY		"/allpurpose.ttf"),

// Sound script paths
MAIN_MENU_SOUND_SCRIPT(SOUND_DIRECTORY "/main_menu_sound.msf"),
DECO_SOUND_SCRIPT(SOUND_DIRECTORY "/deco_sound.msf"),

// Item-related constants
ITEM_LABEL_MATGRP("ItemLabel"),																// Material group name for changing the label on the item mesh
ITEM_END_MATGRP("ColourEnd"),																	// Material group name for changing the colour on the item mesh
ITEM_DROP_BLOCK_ITEMTYPE_MATGRP("item_drop_item_material"),		// Material group name for the item picture on the item drop block mesh

ITEM_GOOD_COLOUR(0.0f, 0.8f, 0.0f),
ITEM_BAD_COLOUR(0.8f, 0.0f, 0.0f),
ITEM_NEUTRAL_COLOUR(0.0f, 0.6f, 1.0f),

DEFAULT_BALL_LIGHT_ATTEN(0.5f),
BLACKOUT_BALL_LIGHT_ATTEN(0.85f),

DEFAULT_BALL_KEY_LIGHT_COLOUR(0.9f, 0.9f, 0.9f),
DEFAULT_PADDLE_KEY_LIGHT_COLOUR(0.65f, 0.65f, 0.7f),
DEFAULT_PADDLE_FILL_LIGHT_COLOUR(0.7f, 0.7f, 0.7f),
DEFAULT_FG_KEY_LIGHT_COLOUR(0.65f, 0.65f, 0.7f),
DEFAULT_FG_FILL_LIGHT_COLOUR(0.7f, 0.7f, 0.7f),
DEFAULT_BG_KEY_LIGHT_COLOUR(0.7f, 0.75f, 0.75f),
DEFAULT_BG_FILL_LIGHT_COLOUR(0.6f, 0.6f, 0.6f),
DEFAULT_BALL_LIGHT_COLOUR(0.85f, 0.85f, 0.95f),
BLACKOUT_BALL_KEY_LIGHT_COLOUR(0.45f, 0.45f, 0.45f),
BLACKOUT_PADDLE_KEY_LIGHT_COLOUR(0.3f, 0.3f, 0.32f),
BLACKOUT_PADDLE_FILL_LIGHT_COLOUR(0.35f, 0.35f, 0.35f),
POISON_LIGHT_DEEP_COLOUR(0.10f, 0.50f, 0.10f),
POISON_LIGHT_LIGHT_COLOUR(0.75f, 1.0f, 0.0f),
INK_BLOCK_COLOUR(0.49f, 0.239f, 0.90f),
STICKYPADDLE_GOO_COLOUR(1.0f, 0.81f, 0.333f),
STICKYPADDLE_PLUS_BEAM_GOO_COLOUR(0.8f, 0.9f, 0.777f),
PRISM_BLOCK_COLOUR(0.8f, 1.0f, 1.0f),
LASER_BEAM_COLOUR(0.33f, 1.0f, 1.0f),

DEFAULT_FG_KEY_LIGHT_POSITION(Point3D(-30.0f, 40.0f, 65.0f)),
DEFAULT_FG_FILL_LIGHT_POSITION(Point3D(25.0f, 0.0f, 40.0f)),
DEFAULT_BG_KEY_LIGHT_POSITION(Point3D(-25.0f, 20.0f, 55.0f)),
DEFAULT_BG_FILL_LIGHT_POSITION(Point3D(30.0f, 11.0f, -15.0f)),
DEFAULT_BALL_KEY_LIGHT_POSITION(Point3D(-30.0f, 40.0f, 65.0f)),
DEFAULT_BALL_FILL_LIGHT_POSITION(Point3D(25.0f, 0.0f, 40.0f)),
DEFAULT_PADDLE_KEY_LIGHT_POSITION(Point3D(-30.0f, 40.0f, 65.0f)),
DEFAULT_PADDLE_FILL_LIGHT_POSITION(Point3D(25.0f, 0.0f, 40.0f)),

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
SKYBOX_MESH(MESH_DIRECTORY "/skybox.obj"),
ITEM_MESH(MESH_DIRECTORY "/item.obj"),
PADDLE_LASER_ATTACHMENT_MESH(MESH_DIRECTORY "/paddle_laser_attachment.obj"),
PADDLE_BEAM_ATTACHMENT_MESH(MESH_DIRECTORY "/paddle_beam_attachment.obj"),
PADDLE_STICKY_ATTACHMENT_MESH(MESH_DIRECTORY "/paddle_sticky_attachment.obj"),
PADDLE_ROCKET_MESH(MESH_DIRECTORY "/paddle_rocket.obj"),

// Deco Asset Paths
DECO_PADDLE_MESH(MESH_DIRECTORY "/deco_paddle.obj"),
DECO_BACKGROUND_MESH(MESH_DIRECTORY "/deco_background.obj"),
DECO_SKYBEAM_MESH(MESH_DIRECTORY "/deco_background_beam.obj"),
DECO_BLOCK_MESH_PATH(MESH_DIRECTORY "/deco_block.obj")
{
	// Initialize the item and timer texture arrays
	this->InitItemTextures();
	this->InitItemTimerTextures();
	this->InitItemTimerFillerTextures();
}

void GameViewConstants::InitItemTextures() {
	this->itemTextures.insert(std::make_pair(GameItem::BallSlowDownItem,				this->TEXTURE_ITEM_SLOWBALL));
	this->itemTextures.insert(std::make_pair(GameItem::BallSpeedUpItem,					this->TEXTURE_ITEM_FASTBALL));
	this->itemTextures.insert(std::make_pair(GameItem::UberBallItem,						this->TEXTURE_ITEM_UBERBALL));
	this->itemTextures.insert(std::make_pair(GameItem::InvisiBallItem,					this->TEXTURE_ITEM_INVISIBALL));
	this->itemTextures.insert(std::make_pair(GameItem::GhostBallItem,						this->TEXTURE_ITEM_GHOSTBALL));
	this->itemTextures.insert(std::make_pair(GameItem::LaserBulletPaddleItem,		this->TEXTURE_ITEM_PADDLELASER));
	this->itemTextures.insert(std::make_pair(GameItem::MultiBall3Item,					this->TEXTURE_ITEM_MULTIBALL3));
	this->itemTextures.insert(std::make_pair(GameItem::MultiBall5Item,					this->TEXTURE_ITEM_MULTIBALL5));
	this->itemTextures.insert(std::make_pair(GameItem::PaddleGrowItem,					this->TEXTURE_ITEM_PADDLEGROW));
	this->itemTextures.insert(std::make_pair(GameItem::PaddleShrinkItem,				this->TEXTURE_ITEM_PADDLESHRINK));
	this->itemTextures.insert(std::make_pair(GameItem::BallGrowItem,						this->TEXTURE_ITEM_BALLGROW));
	this->itemTextures.insert(std::make_pair(GameItem::BallShrinkItem,					this->TEXTURE_ITEM_BALLSHRINK));
	this->itemTextures.insert(std::make_pair(GameItem::BlackoutItem,						this->TEXTURE_ITEM_BLACKOUT));
	this->itemTextures.insert(std::make_pair(GameItem::UpsideDownItem,					this->TEXTURE_ITEM_UPSIDEDOWN));
	this->itemTextures.insert(std::make_pair(GameItem::BallSafetyNetItem,				this->TEXTURE_ITEM_BALLSAFETYNET));
	this->itemTextures.insert(std::make_pair(GameItem::OneUpItem,								this->TEXTURE_ITEM_1UP));
	this->itemTextures.insert(std::make_pair(GameItem::PoisonPaddleItem,				this->TEXTURE_ITEM_POISON));
	this->itemTextures.insert(std::make_pair(GameItem::StickyPaddleItem,				this->TEXTURE_ITEM_STICKYPADDLE));
	this->itemTextures.insert(std::make_pair(GameItem::PaddleCamItem,						this->TEXTURE_ITEM_PADDLECAM));
	this->itemTextures.insert(std::make_pair(GameItem::BallCamItem,							this->TEXTURE_ITEM_BALLCAM));
	this->itemTextures.insert(std::make_pair(GameItem::LaserBeamPaddleItem,			this->TEXTURE_ITEM_PADDLEBEAM));
	this->itemTextures.insert(std::make_pair(GameItem::GravityBallItem,					this->TEXTURE_ITEM_GRAVITYBALL));
	this->itemTextures.insert(std::make_pair(GameItem::RocketPaddleItem,				this->TEXTURE_ITEM_PADDLEROCKET));
	this->itemTextures.insert(std::make_pair(GameItem::CrazyBallItem,						this->TEXTURE_ITEM_CRAZYBALL));
	this->itemTextures.insert(std::make_pair(GameItem::ShieldPaddleItem,				this->TEXTURE_ITEM_SHIELDPADDLE));
	this->itemTextures.insert(std::make_pair(GameItem::FireBallItem,						this->TEXTURE_ITEM_FIREBALL));
	this->itemTextures.insert(std::make_pair(GameItem::RandomItem,							this->TEXTURE_ITEM_RANDOM));
}

void GameViewConstants::InitItemTimerTextures() {
	this->itemTimerTextures.insert(std::make_pair(GameItem::BallSlowDownItem,				this->TEXTURE_ITEM_TIMER_SLOWBALL));
	this->itemTimerTextures.insert(std::make_pair(GameItem::BallSpeedUpItem,				this->TEXTURE_ITEM_TIMER_FASTBALL));
	this->itemTimerTextures.insert(std::make_pair(GameItem::UberBallItem,						this->TEXTURE_ITEM_TIMER_UBERBALL));
	this->itemTimerTextures.insert(std::make_pair(GameItem::InvisiBallItem,					this->TEXTURE_ITEM_TIMER_INVISIBALL));
	this->itemTimerTextures.insert(std::make_pair(GameItem::GhostBallItem,					this->TEXTURE_ITEM_TIMER_GHOSTBALL));
	this->itemTimerTextures.insert(std::make_pair(GameItem::LaserBulletPaddleItem,	this->TEXTURE_ITEM_TIMER_PADDLELASER));
	this->itemTimerTextures.insert(std::make_pair(GameItem::BlackoutItem,						this->TEXTURE_ITEM_TIMER_BLACKOUT));
	this->itemTimerTextures.insert(std::make_pair(GameItem::UpsideDownItem,					this->TEXTURE_ITEM_TIMER_UPSIDEDOWN));
	this->itemTimerTextures.insert(std::make_pair(GameItem::PoisonPaddleItem,				this->TEXTURE_ITEM_TIMER_POISON));
	this->itemTimerTextures.insert(std::make_pair(GameItem::StickyPaddleItem,				this->TEXTURE_ITEM_TIMER_STICKYPADDLE));
	this->itemTimerTextures.insert(std::make_pair(GameItem::PaddleCamItem,					this->TEXTURE_ITEM_TIMER_EYE));
	this->itemTimerTextures.insert(std::make_pair(GameItem::BallCamItem,						this->TEXTURE_ITEM_TIMER_EYE));
	this->itemTimerTextures.insert(std::make_pair(GameItem::GravityBallItem,				this->TEXTURE_ITEM_TIMER_GRAVITYBALL));
	this->itemTimerTextures.insert(std::make_pair(GameItem::ShieldPaddleItem,				this->TEXTURE_ITEM_TIMER_SHIELDPADDLE));
	this->itemTimerTextures.insert(std::make_pair(GameItem::FireBallItem,						this->TEXTURE_ITEM_TIMER_FIREBALL));
}

void GameViewConstants::InitItemTimerFillerTextures() {
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::BallSlowDownItem,				this->TEXTURE_ITEM_TIMER_FILLER_SPDBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::BallSpeedUpItem,				this->TEXTURE_ITEM_TIMER_FILLER_SPDBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::UberBallItem,						this->TEXTURE_ITEM_TIMER_FILLER_UBERBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::InvisiBallItem,					this->TEXTURE_ITEM_TIMER_FILLER_INVISIBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::GhostBallItem,					this->TEXTURE_ITEM_TIMER_FILLER_GHOSTBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::LaserBulletPaddleItem,	this->TEXTURE_ITEM_TIMER_FILLER_PADDLELASER));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::BlackoutItem,						this->TEXTURE_ITEM_TIMER_FILLER_BLACKOUT));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::UpsideDownItem,					this->TEXTURE_ITEM_TIMER_FILLER_UPSIDEDOWN));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::PoisonPaddleItem,				this->TEXTURE_ITEM_TIMER_FILLER_POISON));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::StickyPaddleItem,				this->TEXTURE_ITEM_TIMER_FILLER_STICKYPADDLE));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::PaddleCamItem,					this->TEXTURE_ITEM_TIMER_FILLER_EYE));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::BallCamItem,						this->TEXTURE_ITEM_TIMER_FILLER_EYE));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::GravityBallItem,				this->TEXTURE_ITEM_TIMER_FILLER_GRAVITYBALL));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::ShieldPaddleItem,				this->TEXTURE_ITEM_TIMER_FILLER_SHIELDPADDLE));
	this->itemTimerFillerTextures.insert(std::make_pair(GameItem::FireBallItem,						this->TEXTURE_ITEM_TIMER_FILLER_FIREBALL));
}

GameViewConstants::~GameViewConstants() {
	this->itemTextures.clear();
	this->itemTimerTextures.clear();
	this->itemTimerFillerTextures.clear();
}
