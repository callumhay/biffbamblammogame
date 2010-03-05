/**
 * TextureFontSet.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TEXTUREFONTSET_H__
#define __TEXTUREFONTSET_H__

#include "BasicIncludes.h"
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

private:
	TextureFontSet();

	static const unsigned char MAX_NUM_CHARS;

	unsigned int heightInPixels;								// Height of the font set in pixels
	std::vector<float> widthOfChars;						// Width of each character in the font set
	std::vector<Texture2D*> charTextures;				// Holds each character as a texture
	std::vector<unsigned int> charDispLists;		// Holds display list IDs for each character

	static void CreateTextureFromFontLib(TextureFontSet* newFontSet, FT_Library library, FT_Face face, unsigned int heightInPixels);

public:
	~TextureFontSet();
	float OrthoPrint(const Point3D& topLeftCorner, const std::string& s, bool depthTestOn = false) const;
	void Print(const std::string& s) const;

	float GetWidth(const std::string& s) const;
	// Obtain the height of this font set, measured in pixels
	unsigned int GetHeight() const {
		return this->heightInPixels;
	}

	// Creator Functions
	static std::map<unsigned int, TextureFontSet*> CreateTextureFontFromTTF(PHYSFS_File* fileHandle, const std::vector<unsigned int>& heightsInPixels);
	static std::map<unsigned int, TextureFontSet*> CreateTextureFontFromTTF(const std::string& ttfFilepath, const std::vector<unsigned int>& heightsInPixels);
};
#endif