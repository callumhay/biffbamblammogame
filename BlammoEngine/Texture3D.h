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