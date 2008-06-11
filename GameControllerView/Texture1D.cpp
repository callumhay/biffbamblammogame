#include "Texture1D.h"

Texture1D::Texture1D(TextureFilterType texFilter) : Texture(GL_TEXTURE_1D), texFilter(texFilter) {
}

Texture1D::~Texture1D() {
}

/**
 * Static creator method for the creation of 1D textures.
 */
Texture1D* Texture1D::CreateTexture1DFromImgFile(const std::string& filepath, TextureFilterType texFilter, bool useMipmapping) {
	Texture1D* newTex = new Texture1D(texFilter);
	if (newTex->LoadTextureFromImg(filepath, texFilter, useMipmapping)) {
		return newTex;
	}
	else {
		delete newTex;
	}
	return NULL;
}
