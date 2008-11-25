#ifndef __CGFXPOSTREFRACT_H__
#define __CGFXPOSTREFRACT_H__

#include "CgFxEffect.h"
#include "Colour.h"
class Camera;

/**
 * Represents CgFx CelShader material.
 */
class CgFxPostRefract : public CgFxEffectBase {

private:

	// Constants used for loading the CelShading effect
	static const std::string EFFECT_FILE;
	static const std::string BASIC_TECHNIQUE_NAME;
	static const std::string TEXTURED_TECHNIQUE_NAME;

	// CG Transform params
	CGparameter worldITMatrixParam;
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;
	CGparameter viewInvMatrixParam;

	// CG Refract params
	CGparameter sceneSamplerParam;
	CGparameter invisiColourParam;
	CGparameter indexOfRefactionParam;
	CGparameter warpAmountParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;

	// Actual values for parameters
	Colour invisiColour;
	float indexOfRefraction, warpAmount;
	Texture2D* sceneTex;

protected:
	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxPostRefract();
	virtual ~CgFxPostRefract();

	/**
	 * Set the FBO texture (the texture where the scene thus far
	 * has been rendered to).
	 */
	void SetFBOTexture(Texture2D* tex) {
		this->sceneTex = tex;
	}
	/**
	 * Set the average (doesn't have to be perfect) amount to warp
   * the lookup into the scene texture.
	 */
	void SetWarpAmountParam(float amt) {
		this->warpAmount = amt;
	}
};

#endif