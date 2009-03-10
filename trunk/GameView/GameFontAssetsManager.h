#ifndef __GAMEFONTASSETSMANAGER_H__
#define __GAMEFONTASSETSMANAGER_H__

#include <map>

#include "../BlammoEngine/TextureFontSet.h"

/**
 * Singleton manager class for font assets in the game.
 */
class GameFontAssetsManager {

public:
	// Font enumerations for use in defining the font we want
	enum FontSize  { Small = 16, Medium = 24, Big = 32, Huge = 60 };
	enum FontStyle { HappyGood, GunBlam, ExplosionBoom, ElectricZap, AllPurpose }; //Decoish, Cyberpunkish };

private:
	static GameFontAssetsManager* instance;
	
	// Set of fonts for use in the game, indexed by their style and height
	std::map<FontStyle, std::map<FontSize, TextureFontSet*>> fonts;
	bool minimalFontsLoaded;

	GameFontAssetsManager();
	~GameFontAssetsManager();

public:
	
	// Obtain the singleton
	static GameFontAssetsManager* GetInstance() {
		if (GameFontAssetsManager::instance == NULL) {
			GameFontAssetsManager::instance = new GameFontAssetsManager();
		}
		return GameFontAssetsManager::instance;
	}
	
	// Delete the singleton
	static void DeleteInstance() {
		if (GameFontAssetsManager::instance != NULL) {
			delete GameFontAssetsManager::instance;
			GameFontAssetsManager::instance = NULL;
		}
	}

	// Obtain a particular font
	const TextureFontSet* GetFont(FontStyle style, FontSize height) const {
		std::map<FontStyle, std::map<FontSize, TextureFontSet*>>::const_iterator fontSetIter = this->fonts.find(style);
		if (fontSetIter != this->fonts.end()) {
			std::map<FontSize, TextureFontSet*>::const_iterator fontIter = fontSetIter->second.find(height);
			if (fontIter != fontSetIter->second.end()) {
				return fontIter->second;
			}
		}
		return NULL;
	}

	void LoadMinimalFonts();
};
#endif