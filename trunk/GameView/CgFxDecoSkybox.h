#ifndef __CGFXDECOSKYBOX_H__
#define __CGFXDECOSKYBOX_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class Texture;

class CgFxDecoSkybox : public CgFxEffectBase {
private:
	static const std::string DECOSKYBOX_TECHNIQUE_NAME;

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