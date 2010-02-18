#ifndef __CGFXPOSTREFRACT_H__
#define __CGFXPOSTREFRACT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"

class Camera;

/**
 * Represents CgFx Post Refraction material.
 */
class CgFxPostRefract : public CgFxEffectBase {
public:
	static const char* BASIC_TECHNIQUE_NAME;

	CgFxPostRefract();
	virtual ~CgFxPostRefract();

	/**
	 * Set the FBO texture (the texture where the scene thus far
	 * has been rendered to).
	 */
	void SetFBOTexture(const Texture2D* tex) {
		this->sceneTex = tex;
	}
	/**
	 * Set the average (doesn't have to be perfect) amount to warp
   * the lookup into the scene texture.
	 */
	void SetWarpAmountParam(float amt) {
		this->warpAmount = amt;
	}
	/**
	 * Set the index of refraction for the background.
	 */
	void SetIndexOfRefraction(float eta) {
		this->indexOfRefraction = eta;
	}

protected:
	virtual void SetupBeforePasses(const Camera& camera);

private:
	// CG Transform params
	CGparameter worldITMatrixParam;
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;
	CGparameter viewInvMatrixParam;

	// CG Refract params
	CGparameter sceneSamplerParam;
	CGparameter indexOfRefactionParam;
	CGparameter warpAmountParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;

	// Actual values for parameters
	float indexOfRefraction, warpAmount;
	const Texture2D* sceneTex;
};

#endif