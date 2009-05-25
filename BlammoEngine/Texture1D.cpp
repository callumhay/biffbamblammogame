#include "Texture1D.h"

Texture1D::Texture1D(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_1D) {
}

Texture1D::~Texture1D() {
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