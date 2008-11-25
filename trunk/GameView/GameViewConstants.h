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

};
#endif