#include "TextureFontSet.h"

#include "../Utils/Includes.h"

TextureFontSet::TextureFontSet() {
}

TextureFontSet::~TextureFontSet() {
}

/**
 * Creates a Texture font set from a given true type 
 * font file given as its filepath on disk. This function also requires
 * the height of the font in pixels - this will restrict the size of the font.
 * Returns: A new TextureFontSet of the particular font if all goes well, otherwise it will
 * return NULL.
 */
TextureFontSet* TextureFontSet::CreateTextureFontFromTTF(const std::string& ttfFilePath, unsigned int heightInPixels) {



	return NULL;
}
