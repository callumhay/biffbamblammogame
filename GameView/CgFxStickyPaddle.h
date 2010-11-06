/**
 * CgFxStickyPaddle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	static const char* STICKYPADDLE_TECHNIQUE_NAME;

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