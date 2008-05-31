#ifndef __TEXTUREFONTSET_H__
#define __TEXTUREFONTSET_H__

#include <string>

/**
 * A font represented as a set of display list textured quads,
 * this can be used to form sentences, words etc. using a particular
 * true type font.
 */
class TextureFontSet {
public:
	TextureFontSet();
	~TextureFontSet();

	TextureFontSet* CreateTextureFontFromTTF(const std::string& ttfFilePath, 
		                                       unsigned int heightInPixels);
};
#endif