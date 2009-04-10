#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"

GameFontAssetsManager* GameFontAssetsManager::instance = NULL;

GameFontAssetsManager::GameFontAssetsManager() : minimalFontsLoaded(false) {
	this->LoadMinimalFonts();
}

GameFontAssetsManager::~GameFontAssetsManager() {
	// Delete the regular fonts
	std::map<FontStyle, std::map<FontSize, TextureFontSet*>>::iterator fontSetIter;
	std::map<FontSize, TextureFontSet*>::iterator fontIter;
	for (fontSetIter = this->fonts.begin(); fontSetIter != this->fonts.end(); fontSetIter++) {
		std::map<FontSize, TextureFontSet*>& fontSet = fontSetIter->second;
		for (fontIter = fontSet.begin(); fontIter != fontSet.end(); fontIter++) {
			delete fontIter->second;
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
	
	TextureFontSet* temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_HAPPYGOOD, Small);
	assert(temp != NULL);
	this->fonts[HappyGood][Small]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_HAPPYGOOD, Medium);
	assert(temp != NULL);
	this->fonts[HappyGood][Medium]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_HAPPYGOOD, Big);
	assert(temp != NULL);
	this->fonts[HappyGood][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_HAPPYGOOD, Huge);
	assert(temp != NULL);
	this->fonts[HappyGood][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, Small);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Small]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, Medium);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Medium]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, Big);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, Huge);
	assert(temp != NULL);
	this->fonts[ExplosionBoom][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, Small);
	assert(temp != NULL);
	this->fonts[ElectricZap][Small]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, Medium);
	assert(temp != NULL);
	this->fonts[ElectricZap][Medium]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, Big);
	assert(temp != NULL);
	this->fonts[ElectricZap][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, Huge);
	assert(temp != NULL);
	this->fonts[ElectricZap][Huge]	= temp;

	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, Small);
	assert(temp != NULL);
	this->fonts[AllPurpose][Small]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, Medium);
	assert(temp != NULL);
	this->fonts[AllPurpose][Medium]	= temp; 
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, Big);
	assert(temp != NULL);
	this->fonts[AllPurpose][Big]	= temp;
	temp = TextureFontSet::CreateTextureFontFromTTF(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, Huge);
	assert(temp != NULL);
	this->fonts[AllPurpose][Huge]	= temp;

	glPopAttrib();
	this->minimalFontsLoaded = true;

	debug_opengl_state();
}