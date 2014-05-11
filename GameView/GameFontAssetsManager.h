/**
 * GameFontAssetsManager.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	enum FontStyle { SadBadGoo, ExplosionBoom, ElectricZap, AllPurpose };

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