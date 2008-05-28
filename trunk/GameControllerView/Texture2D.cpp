#include "Texture2D.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "../Utils/Includes.h"

Texture::TextureFilterType Texture::texFilter = Bilinear;

Texture::Texture(int textureType) : textureType(textureType), texID(0) {
}

Texture::~Texture() {
	if (this->IsInitialized()) {
		glDeleteTextures(1, &this->texID);
	}
}

void Texture::SetFilteringParams(int glTexType) {
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

// Default constructor for 2D textures
Texture2D::Texture2D() : Texture(GL_TEXTURE_2D) {
}

Texture2D::Texture2D(const std::string& filepath) : Texture(GL_TEXTURE_2D) {
	this->LoadTexture2D(filepath);
}

Texture2D::~Texture2D() {
}

bool Texture2D::LoadTexture2D(const std::string& filepath) {
	// Clear out any previously loaded textures
	if (this->IsInitialized()) {
		glDeleteTextures(1, &this->texID);
		this->texID = 0;
	}

	// Set 2D texture filter parameters
	Texture::SetFilteringParams(this->textureType);

	// Set 2D texture wrap/clamp params
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Read in the texture...
	char* temp = new char[filepath.length()+1];
	filepath.copy(temp, filepath.length());
	temp[filepath.length()] = '\0';
	this->texID = ilutGLLoadImage(temp);
	delete[] temp;

	return this->IsInitialized();
}