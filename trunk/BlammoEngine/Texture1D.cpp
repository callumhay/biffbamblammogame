#include "Texture1D.h"

Texture1D::Texture1D(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_1D) {
}

Texture1D::~Texture1D() {
}

/**
 * Static creator method for the creation of 1D textures.
 */
Texture1D* Texture1D::CreateTexture1DFromImgFile(const std::string& filepath, TextureFilterType texFilter) {
	Texture1D* newTex = new Texture1D(texFilter);
	if (newTex->Load2DOr1DTextureFromImg(filepath, texFilter)) {
		return newTex;
	}
	else {
		delete newTex;
	}
	return NULL;
}
