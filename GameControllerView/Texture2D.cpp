#include "Texture2D.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "../Utils/Includes.h"
#include "../Utils/Algebra.h"

Texture::TextureFilterType Texture::texFilter = Trilinear;

Texture::Texture(int textureType) : textureType(textureType), texID(0) {
}

Texture::~Texture() {
	glDeleteTextures(1, &this->texID);
}

void Texture::SetMipmapFilteringParams(int glTexType) {
	switch(Texture::texFilter) {
		case Nearest:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			break;
		case Bilinear:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			break;
		case Trilinear:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			break;
		default:
			assert(false);
	}
}

void Texture::SetNonMipmapFilteringParams(int glTexType) {
	switch(Texture::texFilter) {
		case Nearest:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			break;
		case Bilinear:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		case Trilinear:
			// This is just bilinear - since you need mipmaps for trilinear
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		default:
			assert(false);
	}
}

// Default constructor for 2D textures
Texture2D::Texture2D() : Texture(GL_TEXTURE_2D) {
}

Texture2D::~Texture2D() {
}

/**
 * Static creator for making a 2D texture from a given file path to an image file.
 */
Texture2D* Texture2D::CreateTexture2DFromImgFile(const std::string& filepath) {
	Texture2D* newTex = new Texture2D();
	if (newTex->LoadTexture2DFromImg(filepath)) {
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
Texture2D* Texture2D::CreateTexture2DFromFTBMP(const FT_Bitmap& bmp) {

	// First create the texture
	Texture2D* newTex = new Texture2D();
	glGenTextures(1, &newTex->texID);
	if (!glIsTexture(newTex->texID)) {
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
	Texture::SetNonMipmapFilteringParams(newTex->textureType);

	// Fonts are 2 channel data, hence the use of GL_LUMINANCE_ALPHA
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
							  GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData );

	delete[] expandedData;
	return newTex;
}

/**
 * Private helper for loading a 2D texture from an image file, given the
 * file path to that image on disk.
 */
bool Texture2D::LoadTexture2DFromImg(const std::string& filepath) {

	glEnable(this->textureType);

	// Read in the texture, with mipmapping
	int imageID =	ilGenImage();
	ilBindImage(imageID);
	ILboolean resultOfImageLoad = ilLoadImage(filepath.c_str());
	if (!resultOfImageLoad) {
		debug_output("Failed to load texture image from " << filepath);
		return false;
	}
	this->texID = ilutGLBindMipmaps();

	// Set 2D texture filter parameters
	Texture::SetMipmapFilteringParams(this->textureType);

	// Set 2D texture wrap/clamp params
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	ilDeleteImage(imageID);
	return this->texID != 0;
}
