#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "../Utils/Includes.h"

// Abstract Texture class
class Texture {

public:
	enum TextureFilterType { Nearest, Bilinear, Trilinear };

protected:
	int textureType;
	unsigned int texID;

	static void SetMipmapFilteringParams(TextureFilterType texFilter, int glTexType);
	static void SetNonMipmapFilteringParams(TextureFilterType texFilter, int glTexType);

	bool LoadTextureFromImg(const std::string& filepath, TextureFilterType texFilter = Nearest, bool useMipmapping = false);

public:
	Texture(int textureType);
	virtual ~Texture();

	unsigned int GetTextureID() const {
		return this->texID;
	}
	
	int GetTextureType() const {
		return this->textureType;
	}

	void BindTexture() const {
		glBindTexture(this->textureType, this->texID);
	}
};

#endif