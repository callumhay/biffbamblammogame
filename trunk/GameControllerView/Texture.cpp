#include "Texture.h"

#include <string>

Texture::Texture(int textureType) : textureType(textureType), texID(0) {
}

Texture::~Texture() {
	glDeleteTextures(1, &this->texID);
}

void Texture::SetMipmapFilteringParams(TextureFilterType texFilter, int glTexType) {
	switch(texFilter) {
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

void Texture::SetNonMipmapFilteringParams(TextureFilterType texFilter, int glTexType) {
	switch(texFilter) {
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

bool Texture::LoadTextureFromImg(const std::string& filepath, TextureFilterType texFilter, bool useMipmapping) {
	glEnable(this->textureType);

	// Read in the texture, with mipmapping
	int imageID =	ilGenImage();
	ilBindImage(imageID);
	ILboolean resultOfImageLoad = ilLoadImage(filepath.c_str());
	if (!resultOfImageLoad) {
		debug_output("Failed to load texture image from " << filepath);
		return false;
	}

	ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
	if (height == 1) {
		assert(this->textureType == GL_TEXTURE_1D);
		
		// 1D Texture
		ILubyte* texelData = ilGetData();
		ILint width = ilGetInteger(IL_IMAGE_WIDTH);

		glGenTextures(1, &this->texID);
		glBindTexture(this->textureType, this->texID);

		if (useMipmapping) {
			GLint result = gluBuild1DMipmaps(this->textureType, GL_RGB, width, GL_RGB, GL_UNSIGNED_BYTE, texelData);
			
			assert(result == 0);
			if (result != 0) {
				debug_output("Failed to load mipmaps for image " << filepath);
				glDeleteTextures(1, &this->texID);
				ilDeleteImage(imageID);
				return false;
			}
		}
		else {
			glTexImage1D(this->textureType, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, texelData);
		}
	}
	else {
		// 2D Texture
		if (useMipmapping) { 
			this->texID = ilutGLBindMipmaps();
		}
		else {
			this->texID = ilutGLBindTexImage();
		}
	}

	// Set texture wrap/clamp params
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Set texture filtering
	if (useMipmapping) { 
		Texture::SetMipmapFilteringParams(texFilter, this->textureType);
	}
	else {
		Texture::SetNonMipmapFilteringParams(texFilter, this->textureType);
	}

	ilDeleteImage(imageID);
	return this->texID != 0;
}