/**
 * Texture1D.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Texture1D.h"

Texture1D::Texture1D(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_1D) {
}

Texture1D::~Texture1D() {
}

Texture1D* Texture1D::CreateTexture1DFromBuffer(unsigned char* fileBuffer, long fileBufferLength, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

	Texture1D* newTex = new Texture1D(texFilter);
	if (!newTex->Load2DOr1DTextureFromBuffer(fileBuffer, fileBufferLength, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	return newTex;
}

/**
 * Static creator method for the creation of 1D textures.
 */
Texture1D* Texture1D::CreateTexture1DFromImgFile(const std::string& filepath, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

	Texture1D* newTex = new Texture1D(texFilter);
	if (!newTex->Load2DOr1DTextureFromImg(filepath, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	return newTex;
}

Texture1D* Texture1D::CreateTexture1DFromImgFile(PHYSFS_File* fileHandle, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

	Texture1D* newTex = new Texture1D(texFilter);
	if (!newTex->Load2DOr1DTextureFromImg(fileHandle, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	return newTex;
}