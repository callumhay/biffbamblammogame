#include "Texture2D.h"

#include "../Utils/Algebra.h"

// Default constructor for 2D textures
Texture2D::Texture2D(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_2D) {
}

Texture2D::~Texture2D() {
}

/**
 * Static creator for making a 2D texture from a given file path to an image file.
 */
Texture2D* Texture2D::CreateTexture2DFromImgFile(const std::string& filepath, TextureFilterType texFilter) {
	Texture2D* newTex = new Texture2D(texFilter);
	if (newTex->Load2DOr1DTextureFromImg(filepath, texFilter, true)) {
		return newTex;
	}
	else {
		delete newTex;
	}
	return NULL;
}

/**
 * Static creator, for making a 2D texture given a true font bitmap.
 */
Texture2D* Texture2D::CreateTexture2DFromFTBMP(const FT_Bitmap& bmp, TextureFilterType texFilter) {

	// First create the texture
	Texture2D* newTex = new Texture2D(texFilter);
	glGenTextures(1, &newTex->texID);
	if (newTex->texID == 0) {
		delete newTex;
		return NULL;
	}

	// Obtain the proper power of two width/height of the font
	int width = NumberFuncs::NextPowerOfTwo(bmp.width);
	int height = NumberFuncs::NextPowerOfTwo(bmp.rows);

	// Allocate Memory For The Texture Data.
	GLubyte* expandedData = new GLubyte[ 2 * width * height];

	// Use A Two Channel Bitmap (One For Channel Luminosity And One For Alpha)
	// Assign Both Luminosity And Alpha To The Value That We Find In The FreeType Bitmap. 
	for (int j=0; j < height; j++) {
		for (int i=0; i < width; i++){
			// That Value Which We Use Will Be 0 If We Are In The Padding Zone, And Whatever
			// Is The FreeType Bitmap Otherwise.
			expandedData[2*(i+j*width)]= expandedData[2*(i+j*width)+1] = 
				(i>=bmp.width || j>=bmp.rows) ? 0 : bmp.buffer[i + bmp.width*j];
		}
	}
	
	// Bind the texture and create it in all its glory
	glBindTexture(GL_TEXTURE_2D, newTex->texID);
	Texture::SetNonMipmapFilteringParams(texFilter, newTex->textureType);

	// Fonts are 2 channel data, hence the use of GL_LUMINANCE_ALPHA
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
							  GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData );

	delete[] expandedData;
	return newTex;
}