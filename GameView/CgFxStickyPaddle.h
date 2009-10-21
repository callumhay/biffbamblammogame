#ifndef __CGFXSTICKYPADDLE_H__
#define __CGFXSTICKYPADDLE_H__

#include "../BlammoEngine/CgFxEffect.h"

class Texture2D;

/**
 * Shader effect for the sticky paddle goo that sits on top of the player
 * paddle when they have the sticky paddle power-up active.
 */
class CgFxStickyPaddle : public CgFxMaterialEffect {
private:
	static const std::string STICKYPADDLE_TECHNIQUE_NAME;

protected:
	// Cg parameters for the sticky paddle effect
	CGparameter timerParam;
	CGparameter displacementParam;
	CGparameter speedParam;
	CGparameter waveScaleParam;
	CGparameter refractScaleParam;
	CGparameter noiseScaleParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;
	
	CGparameter sceneSamplerParam;

	// Values for the Cg parameters
	float timer, displacement, speed, waveScale, refractScale, noiseScale;
	const Texture2D* sceneTex;

	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxStickyPaddle(MaterialProperties* properties);
	virtual ~CgFxStickyPaddle();

	void SetSceneTexture(const Texture2D* tex) {
		this->sceneTex = tex;
	}

};

#endif