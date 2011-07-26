/**
 * Texture1D.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TEXTURE1D_H__
#define __TEXTURE1D_H__

#include "BasicIncludes.h"
#include "Texture.h"

class Texture1D : public Texture {

private:
	Texture1D(TextureFilterType texFilter);

public:
	virtual ~Texture1D();

	// Creator methods
	static Texture1D* CreateTexture1DFromBuffer(unsigned char* fileBuffer, long fileBufferLength, TextureFilterType texFilter);
	static Texture1D* CreateTexture1DFromImgFile(PHYSFS_File* fileHandle, TextureFilterType texFilter);
	static Texture1D* CreateTexture1DFromImgFile(const std::string& filepath, TextureFilterType texFilter);
	
};

#endif