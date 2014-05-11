/**
 * TextureFontSet.h
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

#ifndef __TEXTUREFONTSET_H__
#define __TEXTUREFONTSET_H__


#include "BasicIncludes.h"
#include "Texture.h"
#include "Point.h"

class Texture2D;

/**
 * A font represented as a set of display list textured quads,
 * this can be used to form sentences, words etc. using a particular
 * true type font.
 */
class TextureFontSet {
public:
	static const float TEXT_3D_SCALE;

	~TextureFontSet();
	void OrthoPrint(const Point3D& topLeftCorner, const std::string& s, bool depthTestOn = false, float scale = 1.0f) const;
    void OrthoPrint(const Point3D& topLeftCorner, const std::string& s, float rotationInDegs, float scale = 1.0f) const;
	void Print(const std::string& s) const;
    void BasicPrint(const std::string& s) const;

	float GetWidth(const std::string& s) const;
    std::vector<std::string> ParseTextToWidth(const std::string& s, float width, float scale) const;

	// Obtain the height of this font set, measured in pixels
	unsigned int GetHeight() const {
		return this->heightInPixels;
	}

    GLuint GetBaseDisplayList() const {
        return this->baseDisplayList;
    }

	// Creator Functions
	static std::map<unsigned int, TextureFontSet*> CreateTextureFontFromTTF(PHYSFS_File* fileHandle, 
		const std::vector<unsigned int>& heightsInPixels, Texture::TextureFilterType filterType);
	static std::map<unsigned int, TextureFontSet*> CreateTextureFontFromTTF(const std::string& ttfFilepath, 
		const std::vector<unsigned int>& heightsInPixels, Texture::TextureFilterType filterType);
	static std::map<unsigned int, TextureFontSet*> CreateTextureFontFromBuffer(unsigned char* buffer, long length, 
		const std::vector<unsigned int>& heightsInPixels, Texture::TextureFilterType filterType);

private:
	TextureFontSet();

	unsigned int numGlyphs;
	unsigned int heightInPixels;            // Height of the font set in pixels
	std::vector<float> widthOfChars;        // Width of each character in the font set
	std::vector<Texture2D*> charTextures;   // Holds each character as a texture
	GLuint baseDisplayList;

	static void CreateTextureFromFontLib(TextureFontSet* newFontSet, FT_Face face, unsigned int heightInPixels, Texture::TextureFilterType filterType);

    DISALLOW_COPY_AND_ASSIGN(TextureFontSet);
};


inline void TextureFontSet::BasicPrint(const std::string& s) const {
	glListBase(this->baseDisplayList);
	glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());
}

#endif