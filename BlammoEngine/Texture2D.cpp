/**
 * Texture2D.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Texture2D.h"
#include "Camera.h"
#include "FBObj.h"
#include "GeometryMaker.h"
#include "Algebra.h"

// Default constructor for 2D textures
Texture2D::Texture2D(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_2D) {
}

Texture2D::~Texture2D() {
}


/**
 * Renders this texture to a full screen quad in the viewport.
 */
void Texture2D::RenderTextureToFullscreenQuad(float depth) const {
	// Draw the full screen quad
	this->BindTexture();
	GeometryMaker::GetInstance()->DrawFullScreenQuad(this->width, this->height, depth);
	this->UnbindTexture();
	debug_opengl_state();
}

Texture2D* Texture2D::CreateEmptyTextureRectangle(int width, int height, Texture::TextureFilterType filter) {


	// First check to see that rectangular textures are supported
	int textureType = GL_TEXTURE_2D;//
		/*
	if (glewGetExtension("GL_EXT_texture_rectangle") == GL_TRUE) {
		textureType = GL_TEXTURE_RECTANGLE_EXT;
	}
	else if (glewGetExtension("GL_ARB_texture_rectangle") == GL_TRUE) {
		textureType = GL_TEXTURE_RECTANGLE_ARB;
	}
	else {
		return NULL;
	}
*/
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);
	Texture2D* newTex = new Texture2D(filter);
	newTex->textureType = textureType;
	
	glEnable(newTex->textureType);
	glGenTextures(1, &newTex->texID);
	if (newTex->texID == 0) {
		delete newTex;
		return NULL;
	}

	newTex->width  = width;
	newTex->height = height;
	
	newTex->BindTexture();
	glTexImage2D(newTex->textureType, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	Texture::SetFilteringParams(newTex->texFilter, newTex->textureType);
	
	// If it's mipmapped then we should generate the mipmaps
	if (Texture::IsMipmappedFilter(newTex->texFilter)) {
		glGenerateMipmapEXT(newTex->textureType);
	}
	newTex->UnbindTexture();

	glPopAttrib();
	debug_opengl_state();

	return newTex;
}

Texture2D* Texture2D::CreateTexture2DFromBuffer(unsigned char* fileBuffer, long fileBufferLength, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);
	
	Texture2D* newTex = new Texture2D(texFilter);
	if (!newTex->Load2DOr1DTextureFromBuffer(fileBuffer, fileBufferLength, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	debug_opengl_state();

	return newTex;
}

/**
 * Static creator for making a 2D texture from a given file path to an image file.
 * Returns: 2D Texture with given image, NULL otherwise.
 */
Texture2D* Texture2D::CreateTexture2DFromImgFile(const std::string& filepath, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);
	
	Texture2D* newTex = new Texture2D(texFilter);
	if (!newTex->Load2DOr1DTextureFromImg(filepath, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	debug_opengl_state();

	return newTex;
}
/**
 * Static creator for making a 2D texture from a given file path to an image file handle
 * using physfs.
 * Returns: 2D Texture with given image, NULL otherwise.
 */
Texture2D* Texture2D::CreateTexture2DFromImgFile(PHYSFS_File* fileHandle, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);
	
	Texture2D* newTex = new Texture2D(texFilter);
	if (!newTex->Load2DOr1DTextureFromImg(fileHandle, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	debug_opengl_state();

	return newTex;
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

	glPushAttrib(GL_TEXTURE_BIT);
	
	// Obtain the proper power of two width/height of the font
	int width = std::max<int>(2, NumberFuncs::NextPowerOfTwo(bmp.width));
	int height = std::max<int>(2, NumberFuncs::NextPowerOfTwo(bmp.rows));
	newTex->width = width;
	newTex->height = height;

	// Allocate Memory For The Texture Data.
	GLubyte* expandedData = new GLubyte[2 * width * height];

	// Use A Two Channel Bitmap (One For Channel Luminosity And One For Alpha)
	// Assign Both Luminosity And Alpha To The Value That We Find In The FreeType Bitmap. 
	for (int j=0; j < height; j++) {
		for (int i=0; i < width; i++){
			// That Value Which We Use Will Be 0 If We Are In The Padding Zone, And Whatever
			// Is The FreeType Bitmap Otherwise.
				expandedData[2*(i+j*width)] = 255;
        expandedData[2*(i+j*width)+1] = (i >= bmp.width || j >= bmp.rows) ? 0 : bmp.buffer[i + bmp.width*j];
		}
	}

	// Bind the texture and create it in all its glory
	newTex->BindTexture();
	// Fonts are 2 channel data, hence the use of GL_LUMINANCE_ALPHA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData);
	Texture::SetFilteringParams(texFilter, newTex->textureType);

	if (Texture::IsMipmappedFilter(texFilter)) {
		glGenerateMipmapEXT(newTex->textureType);
	}
	
	newTex->UnbindTexture();
	glPopAttrib();

	delete[] expandedData;
	debug_opengl_state();

	return newTex;
}