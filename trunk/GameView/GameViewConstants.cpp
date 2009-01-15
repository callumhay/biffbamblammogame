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
TEXTURE_BANG1(TEXTURE_DIRECTORY "/Bang1_256x128.png"),
TEXTURE_BANG2(TEXTURE_DIRECTORY "/Bang2_256x128.png"),
TEXTURE_BANG3(TEXTURE_DIRECTORY "/Bang3_256x128.png"),
TEXTURE_CIRCLE_GRADIENT(TEXTURE_DIRECTORY "/Circle_Gradient_64x64.png"),

// Font Asset Paths
FONT_HAPPYGOOD(FONT_DIRECTORY "/happygood.ttf"),
FONT_GUNBLAM(FONT_DIRECTORY "/gunblam.ttf"),
FONT_EXPLOSIONBOOM(FONT_DIRECTORY "/explosionboom.ttf"),
FONT_ELECTRICZAP(FONT_DIRECTORY "/electriczap.ttf"),
FONT_ALLPURPOSE(FONT_DIRECTORY "/allpurpose.ttf"),

// World-Related Asset Paths
BALL_MESH(MESH_DIRECTORY "/ball.obj"),
SPIKEY_BALL_MESH(MESH_DIRECTORY "/spikey_ball.obj"),
BASIC_BLOCK_MESH_PATH(MESH_DIRECTORY "/block.obj"),
SKYBOX_MESH(MESH_DIRECTORY "/skybox.obj"),

// Deco Asset Paths
DECO_PADDLE_MESH(MESH_DIRECTORY "/deco_paddle.obj"),
DECO_BACKGROUND_MESH(MESH_DIRECTORY "/deco_background.obj"),
DECO_SKYBEAM_MESH(MESH_DIRECTORY "/deco_background_beam.obj"),
DECO_BLOCK_MESH_PATH(MESH_DIRECTORY "/deco_block.obj")
{
}

GameViewConstants::~GameViewConstants() {
}
