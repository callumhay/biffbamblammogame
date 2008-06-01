#ifndef __TEXTUREFONTSET_H__
#define __TEXTUREFONTSET_H__

//#include "../Utils/Includes.h"
#include "../Utils/Point.h"

#include <string>
#include <vector>

class Texture2D;

/**
 * A font represented as a set of display list textured quads,
 * this can be used to form sentences, words etc. using a particular
 * true type font.
 */
class TextureFontSet {

private:
	TextureFontSet();

	static const unsigned char MAX_NUM_CHARS;

	unsigned int heightInPixels;					// Height of the font set in pixels
	std::vector<Texture2D*> charTextures;	// Holds each character as a texture
	std::vector<unsigned int> charDispLists;		// Holds display list IDs for each character

public:
	~TextureFontSet();
	void OrthoPrint(const Point2D& topLeftCorner, const std::string& s) const;

	// Obtain the height of this font set, measured in pixels
	unsigned int GetHeight() const {
		return this->heightInPixels;
	}

	// Creator Functions
	static TextureFontSet* CreateTextureFontFromTTF(const std::string& ttfFilepath, 
																								  unsigned int heightInPixels);
};
#endif