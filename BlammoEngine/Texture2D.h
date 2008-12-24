#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__

#include "BasicIncludes.h"
#include "Texture.h"

// Wraps a OpenGL 2D texture, takes care of texture ID and stuff like that
class Texture2D : public Texture {
private:
	Texture2D(TextureFilterType texFilter);

public:
	virtual ~Texture2D();
	
	void RenderTextureToFullscreenQuad();

	// Creator methods
	static Texture2D* CreateTexture2DFromImgFile(const std::string& filepath, TextureFilterType texFilter);
	static Texture2D* CreateTexture2DFromFTBMP(const FT_Bitmap& bmp, TextureFilterType texFilter);
	static Texture2D* CreateEmptyTexture2D(TextureFilterType texFilter, int width, int height);
};

#endif