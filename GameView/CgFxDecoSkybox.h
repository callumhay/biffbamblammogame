/**
 * CgFxDecoSkybox.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2013
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
public:
    CgFxDecoSkybox(Texture *skyTex);
    ~CgFxDecoSkybox();

protected:
    void SetupBeforePasses(const Camera& camera);

private:
	static const char* DECOSKYBOX_TECHNIQUE_NAME;

	// CgFx parameters
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;

    CGparameter fgScaleParam;
	CGparameter twinkleFreqParam;
    CGparameter moveFreqParam;
    CGparameter noiseScaleParam;
	CGparameter timerParam;
	CGparameter viewDirParam;

	CGparameter noiseSamplerParam;
	CGparameter skySamplerParam;

	// Stored values for parameters
	float timer, twinkleFreq, moveFreq, noiseScale, fgScale;
	GLint noiseTexID; 
	Texture* skyTex;

	Vector3D viewDir;

    DISALLOW_COPY_AND_ASSIGN(CgFxDecoSkybox);
};

#endif // __CGFXDECOSKYBOX_H__