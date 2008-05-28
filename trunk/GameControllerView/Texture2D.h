#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__

#include <string>

// Abstract Texture class
class Texture {

public:
	enum TextureFilterType { Nearest, Bilinear, Trilinear };

protected:
	int textureType;
	unsigned int texID;
	static TextureFilterType texFilter;

	static void SetFilteringParams(int glTexType);

public:
	Texture(int textureType);
	virtual ~Texture();

	unsigned int GetTextureID() const {
		return this->texID;
	}
	
	int GetTextureType() const {
		return this->textureType;
	}

	bool IsInitialized() const {
		return texID != 0;
	}

	static void SetTextureFiltering(TextureFilterType filterType) {
		Texture::texFilter = filterType;
	}
};

// 2D Texture class
class Texture2D : public Texture {

public:
	Texture2D();
	Texture2D(const std::string& filepath);
	virtual ~Texture2D();
	
	bool LoadTexture2D(const std::string& filepath);
	
};

#endif