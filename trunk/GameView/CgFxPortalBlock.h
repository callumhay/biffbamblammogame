/**
 * CgFxPortalBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXPORTALBLOCK_H__
#define __CGFXPORTALBLOCK_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"

class Camera;

/**
 * Represents CgFx Post Refraction material.
 */
class CgFxPortalBlock : public CgFxMaterialEffect {
public:
	static const char* PORTAL_BLOCK_TECHNIQUE_NAME;

	CgFxPortalBlock(MaterialProperties* properties);
	~CgFxPortalBlock();

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

	void SetScale(float s) {
		this->scale = s;
	}
	void SetFrequency(float f) {
		this->freq = f;
	}
	void SetFlowDirection(const Vector3D& v) {
		this->flowDir = v;
	}
	void AddToTimer(double dT) {
		this->timer += dT;
	}

protected:
	virtual void SetupBeforePasses(const Camera& camera);

private:
	// CG Portal Block parameters
	CGparameter sceneSamplerParam;
	CGparameter indexOfRefactionParam;
	CGparameter warpAmountParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;

	CGparameter noiseSamplerParam;
	CGparameter timerParam;
	CGparameter noiseScaleParam;
	CGparameter noiseFreqParam;
	CGparameter noiseFlowDirParam;

	// Actual values for parameters
	float indexOfRefraction, warpAmount;
	const Texture2D* sceneTex;
	double timer;
	float scale, freq;
	Vector3D flowDir;
	GLint noiseTexID;
};

#endif // __CGFXPORTALBLOCK_H__