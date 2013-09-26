/**
 * Texture.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "BasicIncludes.h"

// Abstract Texture class
class Texture {

public:
	enum TextureFilterType { Nearest, Linear, NearestMipmap, Bilinear, Trilinear };

protected:
	TextureFilterType texFilter;
	int textureType;
	GLuint texID;
	unsigned int width, height;

	static void SetFilteringParams(TextureFilterType texFilter, int glTexType);
	static bool IsMipmappedFilter(TextureFilterType texFilter) {
		return !(texFilter == Nearest || texFilter == Linear);
	}

	bool Load2DOr1DTextureFromBuffer(unsigned char* fileBuffer, long fileBufferLength, TextureFilterType texFilter = Nearest);
	bool Load2DOr1DTextureFromImg(const std::string& filepath, TextureFilterType texFilter = Nearest);
	bool Load2DOr1DTextureFromImg(PHYSFS_File* fileHandle, TextureFilterType texFilter = Nearest);

public:
	Texture(TextureFilterType texFilter, int textureType);
	virtual ~Texture();

	unsigned int GetTextureID() const {
		return this->texID;
	}
	
	int GetTextureType() const {
		return this->textureType;
	}

	unsigned int GetWidth() const {
		return this->width;
	}

	unsigned int GetHeight() const {
		return this->height;
	}

	TextureFilterType GetFilter() const {
		return this->texFilter;
	}

	// Functions for binding and unbinding the texture - these should
	// ALWAYS be used over manually doing it - both
	// help isolate problems with the OGL state
	void BindTexture() const {
		glEnable(this->textureType);
		glBindTexture(this->textureType, this->texID);
	}
	void UnbindTexture() const {
		glBindTexture(this->textureType, 0);
		glDisable(this->textureType);
	}

    void SetWrapParams(GLint wrapS, GLint wrapT) const {
        this->BindTexture();
        glTexParameteri(this->textureType, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(this->textureType, GL_TEXTURE_WRAP_T, wrapT);
        this->UnbindTexture();
    }

	void GenerateMipmaps() {
		glGenerateMipmapEXT(this->textureType);
	}

};

#endif