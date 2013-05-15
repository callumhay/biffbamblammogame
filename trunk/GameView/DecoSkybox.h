/**
 * DecoSkybox.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

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

	virtual void Tick(double){};
	virtual void Draw(const Camera& camera);
};

#endif