/**
 * Texture3D.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TEXTURE3D_H__
#define __TEXTURE3D_H__

#include "Texture.h"

class Texture3D : public Texture {
private:
	unsigned int depth;

	Texture3D(TextureFilterType texFilter);

public:
	virtual ~Texture3D();

	static Texture3D* Create3DNoiseTexture(int size);

};
#endif