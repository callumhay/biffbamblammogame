#include "TextureFontSet.h"
#include "Texture2D.h"
#include "Camera.h"

#include "../Utils/Includes.h"
#include "../Utils/Algebra.h"

const unsigned char TextureFontSet::MAX_NUM_CHARS = 128;

TextureFontSet::TextureFontSet() {
}

TextureFontSet::~TextureFontSet() {
	// Delete all the textures
	for (size_t i = 0; i < this->charTextures.size(); i++) {
		delete this->charTextures[i];
	}
	this->charTextures.clear();
}

/**
 * Prints a given string using this font, while printing such that
 * the string starts with its top-left corner at the point specified.
 * This prints in 2D, window coordinates.
 * Precondition: s cannot have any new lines in it!!
 * Returns: The length of the printed text.
 */
float TextureFontSet::OrthoPrint(const Point2D& topLeftCorner, const std::string& s) const {
	assert(s.find('\n') == std::string::npos);
	assert(s.find('\r') == std::string::npos);

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	// Prepare OGL for drawing the text
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

	// Draw the text
	glListBase(this->charDispLists[0]);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(topLeftCorner[0], topLeftCorner[1]-this->heightInPixels, 0);
	glRasterPos2f(0,0);
	glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());
	
	float rpos[4];
	glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
	float textLength = rpos[0] - topLeftCorner[0];
	
	glPopMatrix();
	glPopAttrib();  

	// Pop the projection matrix
	Camera::PopWindowCoords();
	
	return textLength;
}

/**
 * Creates a Texture font set from a given true type 
 * font file given as its filepath on disk. This function also requires
 * the height of the font in pixels - this will restrict the size of the font.
 * Returns: A new TextureFontSet of the particular font if all goes well, otherwise it will
 * return NULL.
 */
TextureFontSet* TextureFontSet::CreateTextureFontFromTTF(const std::string& ttfFilepath, unsigned int heightInPixels) {
	
	TextureFontSet* newFontSet = new TextureFontSet();
	newFontSet->heightInPixels = heightInPixels;
	
	// Create And Initilize A FreeType Font Library.
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		debug_output("Could not initialize a freetype font library!");
		delete newFontSet;
		return NULL;
	}

	// A Face Holds Information On A Given Font
	FT_Face face;

	// This Is Where We Load In The Font Information From The File.
	// Of All The Places Where The Code Might Die, This Is The Most Likely,
	// As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
	if (FT_New_Face(library, ttfFilepath.c_str(), 0, &face)) {
		debug_output("FT_New_Face failed - there is probably a problem with the font file: " << ttfFilepath);
		delete newFontSet;
		return NULL;
	}

	// FreeType Measures Font Size In Terms Of 1/64ths Of Pixels.  
	// Thus, To Make A Font h Pixels High, We Need To Request A Size Of h*64.
	FT_Set_Char_Size( face, heightInPixels << 6, heightInPixels << 6, 96, 96);
	
	// Create the font set by making the texture and display list for
	// each character in the font
	glPushMatrix();
	for (unsigned char i=0; i < MAX_NUM_CHARS; i++) {
		
		// Render the current character into a bitmap
		// Load the glyph for the character
		if(FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT )) {
			debug_output("Could not load the glyph for character: " << i << " in font set file: " << ttfFilepath);
			delete newFontSet;
			return NULL;
		}
		
		// Move The Face's Glyph Into A Glyph Object.
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph)) {
			debug_output("Could not get glyph for character: " << i << " in font set file: " << ttfFilepath);
			delete newFontSet;
			return NULL;			
		}

		// Convert The Glyph To A Bitmap.
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		// This Reference Will Make Accessing The Bitmap Easier.
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;
		
		// Create a texture for the current character
		Texture2D* newCharTexture = Texture2D::CreateTexture2DFromFTBMP(bitmap, Texture::Trilinear);
		if (newCharTexture == NULL) {
			debug_output("Could not create texture from bitmap for character: " << i << " in font set file: " << ttfFilepath);
			delete newFontSet;
			return NULL;
		}
		newFontSet->charTextures.push_back(newCharTexture);

		// Create the display list for the character as well
		GLuint dList = glGenLists(1);
		glNewList(dList, GL_COMPILE);

		newCharTexture->BindTexture();
		glPushMatrix();

		// Move the character over so that it has the right
		// amount of space between it and the one before
		glTranslatef(bitmap_glyph->left,0,0);

		// Now We Move Down A Little In The Case That The Bitmap Extends Past The Bottom Of The Line 
		// This Is Only True For Characters Like 'g' Or 'y'.
		glTranslatef(0,bitmap_glyph->top-bitmap.rows,0);

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
		
		glTranslatef(face->glyph->advance.x >> 6, 0, 0);

		// Increment The Raster Position As If We Were A Bitmap Font.
		// (Only Needed If You Want To Calculate Text Length)
		glBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

		// Finish The Display List
		glEndList();
		newCharTexture->UnbindTexture();
		newFontSet->charDispLists.push_back(dList);
	}
	glPopMatrix();

	// Clean up
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	assert(newFontSet->charDispLists.size() == newFontSet->charTextures.size());
	return newFontSet;
}
