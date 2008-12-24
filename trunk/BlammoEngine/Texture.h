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
	unsigned int texID;
	unsigned int width, height;

	static void SetFilteringParams(TextureFilterType texFilter, int glTexType);
	static bool IsMipmappedFilter(TextureFilterType texFilter) {
		return !(texFilter == Nearest || texFilter == Linear);
	}

	bool Load2DOr1DTextureFromImg(const std::string& filepath, TextureFilterType texFilter = Nearest);

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

};

#endif