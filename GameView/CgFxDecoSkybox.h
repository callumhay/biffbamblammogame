/**
 * CgFxDecoSkybox.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXDECOSKYBOX_H__
#define __CGFXDECOSKYBOX_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class Texture;

class CgFxDecoSkybox : public CgFxEffectBase {
private:
	static const char* DECOSKYBOX_TECHNIQUE_NAME;

	// CgFx parameters
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;

	CGparameter freqParam;
	CGparameter timerParam;
	CGparameter viewDirParam;

	CGparameter noiseSamplerParam;
	CGparameter skySamplerParam;

	// Stored values for parameters
	float timer, freq;
	GLint noiseTexID; 
	Texture* skyTex;

	Vector3D viewDir;

protected:
	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxDecoSkybox(Texture *skyTex);
	virtual ~CgFxDecoSkybox();

};
#endif