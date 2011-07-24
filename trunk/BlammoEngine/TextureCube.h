/**
 * TextureCube.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TEXTURECUBE_H__
#define __TEXTURECUBE_H__

#include "BasicIncludes.h"
#include "Texture.h"

/**
 * Cube map texture class - manages cube mapping textures.
 */
class TextureCube : public Texture {

private:
	static const int NUM_CUBE_FACES = 6;
	static const GLint CUBE_FACES[NUM_CUBE_FACES];

	TextureCube(TextureFilterType texFilter);
	bool LoadCubeTextureFromImgs(const std::string filepaths[NUM_CUBE_FACES], TextureFilterType texFilter);
	bool LoadCubeTextureFromImgs(const std::string& filepath2d, TextureFilterType texFilter);

public:
	virtual ~TextureCube();

	// Static creators
	static TextureCube* CreateCubeTextureFromImgFiles(const std::string filepaths[NUM_CUBE_FACES], 
		                                                TextureFilterType texFilter);
};

#endif
