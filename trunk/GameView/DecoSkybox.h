#ifndef __DECOSKYBOX_H__
#define __DECOSKYBOX_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Skybox.h"

#include "CgFxDecoSkybox.h"

class DecoSkybox : public Skybox {

private:
	GLuint skyboxDispList;
	CgFxDecoSkybox decoSkyboxEffect;

public:
	DecoSkybox();
	virtual ~DecoSkybox();

	virtual void Tick(double dT){};
	virtual void Draw(const Camera& camera);
};

#endif