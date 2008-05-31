#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__

#include <string>

#include "ft2build.h"
#include FT_FREETYPE_H

// Abstract Texture class
class Texture {

public:
	enum TextureFilterType { Nearest, Bilinear, Trilinear };

protected:
	int textureType;
	unsigned int texID;
	static TextureFilterType texFilter;

	static void SetMipmapFilteringParams(int glTexType);
	static void SetNonMipmapFilteringParams(int glTexType);

public:
	Texture(int textureType);
	virtual ~Texture();

	unsigned int GetTextureID() const {
		return this->texID;
	}
	
	int GetTextureType() const {
		return this->textureType;
	}

	static void SetTextureFiltering(TextureFilterType filterType) {
		Texture::texFilter = filterType;
	}
};

// Wraps a OpenGL 2D texture, takes care of texture ID and stuff like that
class Texture2D : public Texture {
private:
	Texture2D();
	bool LoadTexture2DFromImg(const std::string& filepath);

public:
	virtual ~Texture2D();
	
	// Creator methods
	static Texture2D* CreateTexture2DFromImgFile(const std::string& filepath);
	static Texture2D* CreateTexture2DFromFTBMP(const FT_Bitmap& bmp);
};

#endif