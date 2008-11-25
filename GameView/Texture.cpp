#include "Texture.h"

#include <string>

Texture::Texture(TextureFilterType texFilter, int textureType) : texFilter(texFilter), textureType(textureType), texID(0) {
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

bool Texture::Load2DOr1DTextureFromImg(const std::string& filepath, TextureFilterType texFilter, bool useMipmapping) {
	assert(this->textureType == GL_TEXTURE_2D || this->textureType == GL_TEXTURE_1D);

	// Read in the texture
	int imageID =	ilGenImage();
	ilBindImage(imageID);
	ILboolean resultOfImageLoad = ilLoadImage(filepath.c_str());
	if (!resultOfImageLoad) {
		debug_output("Failed to load texture image from " << filepath);
		return false;
	}

	ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
	ILint width = ilGetInteger(IL_IMAGE_WIDTH);
	this->width = width;
	this->height = height;

	if (height == 1) {
		assert(this->textureType == GL_TEXTURE_1D);
		
		// 1D Texture
		ILubyte* texelData = ilGetData();
		ILint internalFormat = ilGetInteger(IL_IMAGE_BPP);
		ILint imgFormat = ilGetInteger(IL_IMAGE_FORMAT);

		glGenTextures(1, &this->texID);
		glBindTexture(this->textureType, this->texID);

		if (useMipmapping) {
			GLint result = gluBuild1DMipmaps(this->textureType, internalFormat, width, imgFormat, GL_UNSIGNED_BYTE, texelData);
			
			assert(result == 0);
			if (result != 0) {
				debug_output("Failed to load mipmaps for image " << filepath);
				ilDeleteImage(imageID);
				return false;
			}
		}
		else {
			glTexImage1D(this->textureType, 0, internalFormat, width, 0, imgFormat, GL_UNSIGNED_BYTE, texelData);
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