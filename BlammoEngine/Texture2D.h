/**
 * Texture2D.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	
	void RenderTextureToFullscreenQuad(float depth = 0.0f) const;
    void RenderTextureToFullscreenQuad(float depth, float uvX, float uvY) const;

	// Creator methods
	static Texture2D* CreateTexture2DFromBuffer(unsigned char* fileBuffer, long fileBufferLength, TextureFilterType texFilter);
	static Texture2D* CreateTexture2DFromImgFile(PHYSFS_File* fileHandle, TextureFilterType texFilter);
	static Texture2D* CreateTexture2DFromImgFile(const std::string& filepath, TextureFilterType texFilter);
	static Texture2D* CreateTexture2DFromFTBMP(const FT_Bitmap& bmp, TextureFilterType texFilter);
	static Texture2D* CreateEmptyTextureRectangle(int width, int height, Texture::TextureFilterType filter);
};

#endif