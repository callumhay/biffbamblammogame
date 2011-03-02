/**
 * GameFontAssetsManager.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"

GameFontAssetsManager* GameFontAssetsManager::instance = NULL;

GameFontAssetsManager::GameFontAssetsManager() : minimalFontsLoaded(false) {
	this->LoadMinimalFonts();
}

GameFontAssetsManager::~GameFontAssetsManager() {
	// Delete the regular fonts
	std::map<FontStyle, std::map<unsigned int, TextureFontSet*> >::iterator fontSetIter;
	std::map<unsigned int, TextureFontSet*>::iterator fontIter;
	for (fontSetIter = this->fonts.begin(); fontSetIter != this->fonts.end(); ++fontSetIter) {
		std::map<unsigned int, TextureFontSet*>& fontSet = fontSetIter->second;
		for (fontIter = fontSet.begin(); fontIter != fontSet.end(); ++fontIter) {
			delete fontIter->second;
			fontIter->second = NULL;
		}
		fontSet.clear();
	}
	this->fonts.clear();
}

/**
 * Load the minimal game fonts - these are always in memory since they are required
 * even for the load screen.
 */
void GameFontAssetsManager::LoadMinimalFonts() {
	// If we've already loaded the minimal fonts then don't do it again
	if (minimalFontsLoaded) {
		return;
	}
	
	debug_output("Loading minimal font sets");

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	// Prepare all sizes of each font type
	std::vector<unsigned int> sizeSet;
	sizeSet.reserve(4);

	sizeSet.push_back(Small);
	sizeSet.push_back(Medium);
	sizeSet.push_back(Big);
	sizeSet.push_back(Huge);
	this->fonts[ExplosionBoom]	= ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, sizeSet, Texture::Linear);
	this->fonts[AllPurpose]			= ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, sizeSet, Texture::Linear);

	sizeSet.clear();
	sizeSet.push_back(Medium);
	this->fonts[SadBadGoo]			= ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_SADBAD, sizeSet, Texture::Linear);
	this->fonts[HappyGood]			= ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_HAPPYGOOD, sizeSet, Texture::Linear);
	this->fonts[ElectricZap]		= ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, sizeSet, Texture::Linear);

	glPopAttrib();
	this->minimalFontsLoaded = true;

	debug_opengl_state();
}