/**
 * TextureFontSet.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
	float OrthoPrint(const Point3D& topLeftCorner, const std::string& s, bool depthTestOn = false, float scale = 1.0f) const;
    float OrthoPrint(const Point3D& topLeftCorner, const std::string& s, float rotationInDegs, float scale = 1.0f) const;
	void Print(const std::string& s) const;

	float GetWidth(const std::string& s) const;
    std::vector<std::string> ParseTextToWidth(const std::string& s, float width, float scale) const;

	// Obtain the height of this font set, measured in pixels
	unsigned int GetHeight() const {
		return this->heightInPixels;
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

	static void CreateTextureFromFontLib(TextureFontSet* newFontSet, FT_Library library, FT_Face face, unsigned int heightInPixels, Texture::TextureFilterType filterType);

    DISALLOW_COPY_AND_ASSIGN(TextureFontSet);
};
#endif