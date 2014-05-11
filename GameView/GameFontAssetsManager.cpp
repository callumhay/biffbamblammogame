/**
 * GameFontAssetsManager.cpp
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
    this->fonts[ExplosionBoom]	= ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_EXPLOSIONBOOM, sizeSet, Texture::Trilinear);
	this->fonts[AllPurpose]		= ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_ALLPURPOSE, sizeSet, Texture::Trilinear);

	sizeSet.clear();
	sizeSet.push_back(Medium);
	this->fonts[SadBadGoo]   = ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_SADBAD, sizeSet, Texture::Trilinear);
	this->fonts[ElectricZap] = ResourceManager::LoadFont(GameViewConstants::GetInstance()->FONT_ELECTRICZAP, sizeSet, Texture::Trilinear);

	glPopAttrib();
	this->minimalFontsLoaded = true;

	debug_opengl_state();
}