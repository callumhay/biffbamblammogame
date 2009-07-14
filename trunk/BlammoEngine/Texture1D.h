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
	static Texture1D* CreateTexture1DFromImgFile(PHYSFS_File* fileHandle, TextureFilterType texFilter);
	static Texture1D* CreateTexture1DFromImgFile(const std::string& filepath, TextureFilterType texFilter);
	
};

#endif