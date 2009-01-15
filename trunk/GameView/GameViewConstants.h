#ifndef __GAMEVIEWCONSTANTS_H__
#define __GAMEVIEWCONSTANTS_H__

#include <string>

class GameViewConstants {

private:
	static GameViewConstants* Instance;

	void InitConstants();
	GameViewConstants();
	~GameViewConstants();

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
	const std::string RESOURCE_DIR;
	const std::string FONT_DIR;
	const std::string MESH_DIR;
	const std::string SHADER_DIR;
	const std::string TEXTURE_DIR;

	// Shader path constants ********************************
	const std::string CGFX_CEL_SHADER;
	const std::string CGFX_PHONG_SHADER;
	const std::string CGFX_POSTREFRACT_SHADER;
	const std::string CGFX_VOLUMEMETRIC_SHADER;

	// Texture path constants *******************************
	const std::string TEXTURE_BANG1;
	const std::string TEXTURE_BANG2;
	const std::string TEXTURE_BANG3;
	const std::string TEXTURE_CIRCLE_GRADIENT;

	// Font path constants **********************************
	const std::string FONT_HAPPYGOOD;
	const std::string FONT_GUNBLAM;
	const std::string FONT_EXPLOSIONBOOM;
	const std::string FONT_ELECTRICZAP;
	const std::string FONT_ALLPURPOSE;

	// World specific path constants ************************
	const std::string BALL_MESH;
	const std::string SPIKEY_BALL_MESH;
	const std::string BASIC_BLOCK_MESH_PATH;
	const std::string SKYBOX_MESH;

	// DECO WORLD
	const std::string DECO_PADDLE_MESH;
	const std::string DECO_BACKGROUND_MESH;
	const std::string DECO_SKYBEAM_MESH;
	const std::string DECO_BLOCK_MESH_PATH;


};
#endif