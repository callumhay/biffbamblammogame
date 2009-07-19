#include "Texture.h"

Texture::Texture(TextureFilterType texFilter, int textureType) : texFilter(texFilter), textureType(textureType), texID(0) {
}

Texture::~Texture() {
	glDeleteTextures(1, &this->texID);
	this->texID = 0;
}

void Texture::SetFilteringParams(TextureFilterType texFilter, int glTexType) {
	switch(texFilter) {
		case Nearest:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			break;
		case Linear:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		case NearestMipmap:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		case Bilinear:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			break;
		case Trilinear:
			glTexParameteri(glTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(glTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			break;
		default:
			assert(false);
	}
}

bool Texture::Load2DOr1DTextureFromImg(const std::string& filepath, TextureFilterType texFilter) {
	assert(this->textureType == GL_TEXTURE_2D || this->textureType == GL_TEXTURE_1D);

	// Read in the texture
	int imageID =	ilGenImage();
	ilBindImage(imageID);
	ILboolean resultOfImageLoad = ilLoadImage((const wchar_t*)filepath.c_str());
	if (!resultOfImageLoad) {
		debug_output("Failed to load texture image from " << filepath);
		ilDeleteImage(imageID);
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

		if (Texture::IsMipmappedFilter(texFilter)) {
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
		if (Texture::IsMipmappedFilter(texFilter)) { 
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
	Texture::SetFilteringParams(texFilter, this->textureType);
	glBindTexture(this->textureType, 0);

	ilDeleteImage(imageID);
	
	debug_opengl_state();
	return this->texID != 0;
}

bool Texture::Load2DOr1DTextureFromImg(PHYSFS_File* fileHandle, TextureFilterType texFilter) {
	assert(this->textureType == GL_TEXTURE_2D || this->textureType == GL_TEXTURE_1D);
	
	// Grab all the data needed from the file handle
	PHYSFS_sint64 fileLength = PHYSFS_fileLength(fileHandle);
	unsigned char* fileBuffer = new unsigned char[fileLength];
	
	int readResult = PHYSFS_read(fileHandle, fileBuffer, sizeof(unsigned char), fileLength);
	if (readResult == NULL) {
		delete[] fileBuffer;
		fileBuffer = NULL;
		debug_output("Error reading texture file to bytes");
		assert(false);
		return false;
	}

	// Read in the texture
	int imageID =	ilGenImage();
	ilBindImage(imageID);
	ILboolean resultOfImageLoad = ilLoadL(IL_TYPE_UNKNOWN, fileBuffer, fileLength);
	
	delete[] fileBuffer;
	fileBuffer = NULL;
	
	if (!resultOfImageLoad) {
		debug_output("Failed to load texture image.");
		ilDeleteImage(imageID);
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

		if (Texture::IsMipmappedFilter(texFilter)) {
			GLint result = gluBuild1DMipmaps(this->textureType, internalFormat, width, imgFormat, GL_UNSIGNED_BYTE, texelData);
			
			assert(result == 0);
			if (result != 0) {
				debug_output("Failed to load mipmaps for image.");
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
		if (Texture::IsMipmappedFilter(texFilter)) { 
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
	Texture::SetFilteringParams(texFilter, this->textureType);
	glBindTexture(this->textureType, 0);

	ilDeleteImage(imageID);
	
	debug_opengl_state();
	return this->texID != 0;
}