/**
 * TextureFontSet.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TextureFontSet.h"
#include "Texture2D.h"
#include "Camera.h"

#include "BasicIncludes.h"
#include "Algebra.h"

//const unsigned char TextureFontSet::MAX_NUM_CHARS = 128;
const float TextureFontSet::TEXT_3D_SCALE	= 0.04;

TextureFontSet::TextureFontSet() : numGlyphs(0), heightInPixels(0) {
}

TextureFontSet::~TextureFontSet() {
	// Delete all the textures
	for (size_t i = 0; i < this->charTextures.size(); i++) {
		delete this->charTextures[i];
	}
	this->charTextures.clear();
}

/**
 * Used to obtain the width of a hypothetical string using this font set.
 * Returns: the width in pixels of a given string using this font set.
 */
float TextureFontSet::GetWidth(const std::string& s) const {
	float totalWidth = 0;

	for (std::string::const_iterator strIter = s.begin(); strIter != s.end(); ++strIter) {
		char currChar = (*strIter);
		totalWidth += this->widthOfChars[currChar];
	}
	return totalWidth;
}

/**
 * Parses the given text into lines that are at most the given width and scale for printing using this font.
 */
std::vector<std::string> TextureFontSet::ParseTextToWidth(const std::string& s, float width, float scale) const {
    assert(width > 0);
    std::vector<std::string> lines;
    if (s.empty()) {
        return lines;
    }

    float currCount = 0;
    size_t lastLineBeginIdx = 0;
    size_t lastSpaceIdx = 0;
    float countAtSpace = 0;

    for (size_t i = 0; i < s.size(); i++) {
        char currChar = s.at(i);
		currCount += scale * this->widthOfChars[currChar];
        

        if (currCount <= width && (currChar == ' ' || currChar == '\n' || currChar == '-')) {
            lastSpaceIdx = i;
            countAtSpace = currCount;
        }

        if (currCount >= width && lastSpaceIdx != 0) {
            lines.push_back(s.substr(lastLineBeginIdx, lastSpaceIdx - lastLineBeginIdx));
            currCount -= countAtSpace;
            lastLineBeginIdx = lastSpaceIdx+1;
        }
        if (i == s.size()-1 && lastLineBeginIdx != i) {
            lines.push_back(s.substr(lastLineBeginIdx, s.size() - lastLineBeginIdx));
        }
    }

    return lines;
}

/**
 * Prints a given string using this font, while printing such that
 * the string starts with its top-left corner at the point specified.
 * This prints in 2D, window coordinates.
 * Precondition: s cannot have any new lines in it!!
 * Returns: The length of the printed text.
 */
float TextureFontSet::OrthoPrint(const Point3D& topLeftCorner, const std::string& s, bool depthTestOn, float scale) const {
	assert(s.find('\n') == std::string::npos);
	assert(s.find('\r') == std::string::npos);

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	// Prepare OGL for drawing the text
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT | GL_COLOR_BUFFER_BIT); 

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);

	if (depthTestOn) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(topLeftCorner[0], topLeftCorner[1] - this->heightInPixels*scale, topLeftCorner[2]);
	glScalef(scale, scale, 1.0f);
	glRasterPos2i(0, 0);

	// Draw the text
	glListBase(this->baseDisplayList);
	glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());

	float rpos[4];
	glGetFloatv(GL_CURRENT_RASTER_POSITION, rpos);
	float textLength = rpos[0] - topLeftCorner[0];
	
	glPopMatrix();
	glPopAttrib();  

	// Pop the projection matrix
	Camera::PopWindowCoords();
	
	debug_opengl_state();

	return textLength;
}

float TextureFontSet::OrthoPrint(const Point3D& topLeftCorner, const std::string& s, 
                                 float rotationInDegs, float scale) const {
	assert(s.find('\n') == std::string::npos);
	assert(s.find('\r') == std::string::npos);

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	// Prepare OGL for drawing the text
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT | GL_COLOR_BUFFER_BIT); 

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(topLeftCorner[0], topLeftCorner[1] - this->heightInPixels*scale, topLeftCorner[2]);
	glRotatef(rotationInDegs, 0, 0, 1);
    glScalef(scale, scale, 1.0f);
	glRasterPos2i(0, 0);

	// Draw the text
	glListBase(this->baseDisplayList);
	glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());

	float rpos[4];
	glGetFloatv(GL_CURRENT_RASTER_POSITION, rpos);
	float textLength = rpos[0] - topLeftCorner[0];
	
	glPopMatrix();
	glPopAttrib();  

	// Pop the projection matrix
	Camera::PopWindowCoords();
	
	debug_opengl_state();

	return textLength;
}

/** 
 * A 3D version of printing text - this will print the text to a billboard that 
 * it initially centered at the origin and pointing in the direction of the z-axis,
 * with an up vector in the direction of +y.
 * Returns: text length in units.
 */
void TextureFontSet::Print(const std::string& s) const {
	assert(s.find('\n') == std::string::npos);
	assert(s.find('\r') == std::string::npos);

	// Draw the text
	glPushMatrix();
	glScalef(TextureFontSet::TEXT_3D_SCALE, TextureFontSet::TEXT_3D_SCALE, TextureFontSet::TEXT_3D_SCALE);
	glListBase(this->baseDisplayList);
	glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());
	glPopMatrix();
}

/**
 * Creates a Texture font set from a given true type 
 * font file given as its filepath on disk. This function also requires
 * the height of the font in pixels - this will restrict the size of the font.
 * Returns: A mapping of the given heights to texture sets - returns empty map on error.
 */
std::map<unsigned int, TextureFontSet*> TextureFontSet::CreateTextureFontFromTTF(const std::string& ttfFilepath, 
                                                                                 const std::vector<unsigned int>& heightsInPixels, 
                                                                                 Texture::TextureFilterType filterType) {
	
	std::map<unsigned int, TextureFontSet*> newFontSets;

	// Create And Initilize A FreeType Font Library.
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		debug_output("Could not initialize a freetype font library!");
		FT_Done_FreeType(library);
		return newFontSets;
	}

	// A Face Holds Information On A Given Font
	FT_Face face;

	// This Is Where We Load In The Font Information From The File.
	// Of All The Places Where The Code Might Die, This Is The Most Likely,
	// As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
	if (FT_New_Face(library, ttfFilepath.c_str(), 0, &face)) {
		debug_output("FT_New_Face failed - there is probably a problem with the font file: " << ttfFilepath);
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		return newFontSets;
	}

	// Go through each height and load that font
	for (unsigned int i = 0; i < heightsInPixels.size(); i++) {
		TextureFontSet* newFontSet = new TextureFontSet();
		unsigned int currHeight = heightsInPixels[i];
		TextureFontSet::CreateTextureFromFontLib(newFontSet, library, face, currHeight, filterType);
		
		assert(newFontSet != NULL);
		assert(newFontSets.find(currHeight) == newFontSets.end());
		newFontSets[currHeight] = newFontSet;
	}

	// Clean-up the ttf library stuff
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	// Check to make sure creation went well
	assert(newFontSets.size() == heightsInPixels.size());

	return newFontSets;
}

/**
 * Creates a Texture font set from a given true type font file handle given. This function also requires
 * the height of the font in pixels - this will restrict the size of the font.
 * Returns: A new TextureFontSet of the particular font if all goes well, otherwise it will
 * return NULL.
 */
std::map<unsigned int, TextureFontSet*> TextureFontSet::CreateTextureFontFromTTF(PHYSFS_File* fileHandle,
                                                                                 const std::vector<unsigned int>& heightsInPixels,
                                                                                 Texture::TextureFilterType filterType) {
	std::map<unsigned int, TextureFontSet*> newFontSets;

	// Grab the in-memory buffer for the file from the physfs filehandle
	PHYSFS_sint64 fileLength = PHYSFS_fileLength(fileHandle);
	unsigned char* fileBuffer = new unsigned char[fileLength];
	
	int readResult = PHYSFS_read(fileHandle, fileBuffer, sizeof(unsigned char), fileLength);
	if (readResult == 0) {
		delete[] fileBuffer;
		fileBuffer = NULL;
		debug_output("Error reading font file to bytes.");
		assert(false);
		return newFontSets;
	}

	// Create And Initilize A FreeType Font Library.
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		debug_output("Could not initialize a freetype font library!");
		FT_Done_FreeType(library);
		delete[] fileBuffer;
		fileBuffer = NULL;
		return newFontSets;
	}

	// A Face Holds Information On A Given Font
	FT_Face face;

	// This Is Where We Load In The Font Information From The File.
	// Of All The Places Where The Code Might Die, This Is The Most Likely,
	// As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
	if (FT_New_Memory_Face(library, fileBuffer, fileLength, 0, &face)) {
		debug_output("FT_New_Face failed - there is probably a problem with the physfs font.");
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		delete[] fileBuffer;
		fileBuffer = NULL;
		return newFontSets;
	}

	// Go through each height and load that font
	for (unsigned int i = 0; i < heightsInPixels.size(); i++) {
		TextureFontSet* newFontSet = new TextureFontSet();
		unsigned int currHeight = heightsInPixels[i];
		TextureFontSet::CreateTextureFromFontLib(newFontSet, library, face, currHeight, filterType);
		
		assert(newFontSet != NULL);
		assert(newFontSets.find(currHeight) == newFontSets.end());
		newFontSets[currHeight] = newFontSet;
	}

	delete[] fileBuffer;
	fileBuffer = NULL;

	// Clean-up the ttf library stuff
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	// Check to make sure creation went well
	assert(newFontSets.size() == heightsInPixels.size());

	return newFontSets;
}

std::map<unsigned int, TextureFontSet*> TextureFontSet::CreateTextureFontFromBuffer(unsigned char* buffer, long length,
                                                                                    const std::vector<unsigned int>& heightsInPixels,
                                                                                    Texture::TextureFilterType filterType) {
	std::map<unsigned int, TextureFontSet*> newFontSets;

	// Create And Initilize A FreeType Font Library.
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		debug_output("Could not initialize a freetype font library!");
		FT_Done_FreeType(library);
		return newFontSets;
	}

	// A Face Holds Information On A Given Font
	FT_Face face;

	// This Is Where We Load In The Font Information From The File.
	// Of All The Places Where The Code Might Die, This Is The Most Likely,
	// As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
	if (FT_New_Memory_Face(library, buffer, length, 0, &face)) {
		debug_output("FT_New_Face failed - there is probably a problem with the physfs font.");
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		return newFontSets;
	}

	// Go through each height and load that font
	for (unsigned int i = 0; i < heightsInPixels.size(); i++) {
		TextureFontSet* newFontSet = new TextureFontSet();
		unsigned int currHeight = heightsInPixels[i];
		TextureFontSet::CreateTextureFromFontLib(newFontSet, library, face, currHeight, filterType);
		
		assert(newFontSet != NULL);
		assert(newFontSets.find(currHeight) == newFontSets.end());
		newFontSets[currHeight] = newFontSet;
	}

	// Clean-up the ttf library stuff
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	// Check to make sure creation went well
	assert(newFontSets.size() == heightsInPixels.size());

	return newFontSets;
}

/**
 * Private helper function for reading a font from a library already setup.
 * Returns: A new TextureFontSet of the particular font if all goes well, otherwise it will
 * return NULL.
 */
void TextureFontSet::CreateTextureFromFontLib(TextureFontSet* newFontSet, FT_Library library,
                                              FT_Face face, unsigned int heightInPixels,
											  Texture::TextureFilterType filterType) {
	assert(library != NULL);
	assert(face != NULL);

	// FreeType Measures Font Size In Terms Of 1/64ths Of Pixels.  
	// Thus, To Make A Font h Pixels High, We Need To Request A Size Of h*64.
	if (FT_Set_Char_Size(face, heightInPixels << 6, heightInPixels << 6, 96, 96)) {
		debug_output("FT_Set_Char_Size failed - there is probably a problem with the font file.");
		return;	
	}

	newFontSet->heightInPixels = heightInPixels;
	long numGlyphs = std::max<long>(128, face->num_glyphs);
	newFontSet->numGlyphs = numGlyphs;
	newFontSet->charTextures.reserve(numGlyphs);

	// Create the font set by making the texture and display list for
	// each character in the font
	newFontSet->baseDisplayList = glGenLists(numGlyphs);
	glPushMatrix();
	for (int i = 0; i < numGlyphs; i++) {	
		// Render the current character into a bitmap
		// Load the glyph for the character
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT)) {
			debug_output("Could not load the glyph for character: " << i << " in font set file.");
			return;
		}
		
		// Move The Face's Glyph Into A Glyph Object.
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph)) {
			debug_output("Could not get glyph for character: " << i << " in font set file.");
			FT_Done_Glyph(glyph);
			return;			
		}

		// Convert The Glyph To A Bitmap.
		FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		// This Reference Will Make Accessing The Bitmap Easier.
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;
		Texture2D* newCharTexture = NULL;
		
		// Create a texture for the current character only if there's data in the buffer...
		if (bitmap.width > 0 && bitmap.rows > 0 && bitmap.buffer != NULL) {
			newCharTexture = Texture2D::CreateTexture2DFromFTBMP(bitmap, filterType);

			if (newCharTexture == NULL) {
				debug_output("Could not create texture from bitmap for character: " << i << " in font set file.");
				FT_Done_Glyph(glyph);
				return;
			}
			
			newFontSet->charTextures.push_back(newCharTexture);
		}

		// Create the display list for the character as well - in the case where there's no texture
		// all we're interested in is advancing, via translation, the current display list
		glNewList(newFontSet->baseDisplayList + i, GL_COMPILE);
		if (newCharTexture != NULL) {
			newCharTexture->BindTexture();
			glPushMatrix();

			// Move the character over so that it has the right
			// amount of space between it and the one before
			glTranslatef(bitmap_glyph->left, 0, 0);

			// Now We Move Down A Little In The Case That The Bitmap Extends Past The Bottom Of The Line 
			// This Is Only True For Characters Like 'g' Or 'y'.
			glTranslatef(0, bitmap_glyph->top - bitmap.rows, 0);

			// Textures Are Filled With Empty Padding Space. We Figure What Portion Of The Texture Is Used By 
			// The Actual Character And Store That Information In The x And y Variables, Then When We Draw The
			// Quad, We Will Only Reference The Parts Of The Texture That Contains The Character Itself.
			int width		= NumberFuncs::NextPowerOfTwo(bitmap.width);
			int height	= NumberFuncs::NextPowerOfTwo(bitmap.rows);
			float x = static_cast<float>(bitmap.width) / static_cast<float>(width);
			float y = static_cast<float>(bitmap.rows)  / static_cast<float>(height);

			// Draw The Texturemapped Quads. The Bitmap That We Got From FreeType Was Not 
			// Oriented Quite Like We Would Like It To Be, But We Link The Texture To The Quad
			// In Such A Way That The Result Will Be Properly Aligned.
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2f(0, bitmap.rows);
			glTexCoord2d(0, y); glVertex2f(0, 0);
			glTexCoord2d(x, y); glVertex2f(bitmap.width, 0);
			glTexCoord2d(x, 0); glVertex2f(bitmap.width, bitmap.rows);
			glEnd();
			glPopMatrix();
		}

		float charWidth = face->glyph->advance.x >> 6;
		newFontSet->widthOfChars.push_back(charWidth);
		glTranslatef(charWidth, 0, 0);
		// Increment The Raster Position As If We Were A Bitmap Font.
		// (Only Needed If You Want To Calculate Text Length)
		glBitmap(0,0,0,0,face->glyph->advance.x >> 6, 0, NULL);

		// Finish The Display List
		glEndList();

		FT_Done_Glyph(glyph);
	}
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
}
