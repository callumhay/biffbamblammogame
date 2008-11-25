#include "GameViewConstants.h"

GameViewConstants* GameViewConstants::Instance = NULL;

GameViewConstants::GameViewConstants() :
RESOURCE_DIR("resources"),
FONT_DIR("fonts"),
MESH_DIR("models"),
SHADER_DIR("shaders"),
TEXTURE_DIR("textures") 
{
}

GameViewConstants::~GameViewConstants() {
}
