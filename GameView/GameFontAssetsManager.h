/**
 * GameFontAssetsManager.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEFONTASSETSMANAGER_H__
#define __GAMEFONTASSETSMANAGER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextureFontSet.h"

/**
 * Singleton manager class for font assets in the game.
 */
class GameFontAssetsManager {

public:
	// Font enumerations for use in defining the font we want
	enum FontSize  { Small = 16, Medium = 24, Big = 32, Huge = 60 };
	enum FontStyle { SadBadGoo, HappyGood, ExplosionBoom, ElectricZap, AllPurpose };

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
		std::map<FontStyle, std::map<unsigned int, TextureFontSet*> >::const_iterator fontSetIter = this->fonts.find(style);
		if (fontSetIter != this->fonts.end()) {
			std::map<unsigned int, TextureFontSet*>::const_iterator fontIter = fontSetIter->second.find(height);
			if (fontIter != fontSetIter->second.end()) {
				return fontIter->second;
			}
			else if (!fontSetIter->second.empty()) {
				// Just return whatever we can
				return fontSetIter->second.begin()->second;
			}	
		}
		
		assert(false);
		return NULL;
	}

	void LoadMinimalFonts();

private:
	static GameFontAssetsManager* instance;
	
	// Set of fonts for use in the game, indexed by their style and height
	std::map<FontStyle, std::map<unsigned int, TextureFontSet*> > fonts;
	bool minimalFontsLoaded;

	GameFontAssetsManager();
	~GameFontAssetsManager();

    DISALLOW_COPY_AND_ASSIGN(GameFontAssetsManager);
};
#endif