/**
 * Texture2D.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

void Texture2D::RenderTextureToFullscreenQuadNoDepth() const {
	// Draw the full screen quad
	this->BindTexture();
	GeometryMaker::GetInstance()->DrawFullScreenQuadNoDepth(this->width, this->height);
	this->UnbindTexture();
	debug_opengl_state();
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

void Texture2D::RenderTextureToFullscreenQuad(float depth, float uvX, float uvY) const {
	// Draw the full screen quad
	this->BindTexture();
	GeometryMaker::GetInstance()->DrawFullScreenQuad(this->width, this->height, depth, uvX, uvY);
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
        glPopAttrib();
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

Texture2D* Texture2D::CreateEmptyDepthTextureRectangle(int width, int height) {
    glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

    Texture2D* newTex = new Texture2D(Texture::Linear);

    glEnable(newTex->textureType);
    glGenTextures(1, &newTex->texID);
	if (newTex->texID == 0) {
        glPopAttrib();
		delete newTex;
		return NULL;
	}

    newTex->BindTexture();

    Texture::SetFilteringParams(newTex->texFilter, newTex->textureType);
    glTexParameteri(newTex->textureType, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(newTex->textureType, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(newTex->textureType, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

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
	// Clamp the texture so we don't get any accidental spillage of texture, text doesn't repeat within itself, duh.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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