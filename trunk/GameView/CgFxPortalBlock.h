/**
 * CgFxPortalBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXPORTALBLOCK_H__
#define __CGFXPORTALBLOCK_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"

class CgFxPortalBlock : public CgFxAbstractMaterialEffect {
public:
	static const char* PORTAL_BLOCK_TECHNIQUE_NAME;

	CgFxPortalBlock();
	~CgFxPortalBlock();

    void SetAlphaMultiplier(float a) {
        this->alphaMultiplier = a;
    }
	void SetFBOTexture(const Texture2D* tex) {
		this->sceneTex = tex;
	}
	void SetWarpAmountParam(float amt) {
		this->warpAmount = amt;
	}
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

private:
	// CG Portal Block parameters
    CGparameter worldITMatrixParam;
    CGparameter wvpMatrixParam;
    CGparameter worldMatrixParam;
    CGparameter viewInvMatrixParam;

    CGparameter alphaMultiplierParam;
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
    float alphaMultiplier;
	float indexOfRefraction, warpAmount;
	const Texture2D* sceneTex;
	double timer;
	float scale, freq;
	Vector3D flowDir;
	GLint noiseTexID;

    void SetupBeforePasses(const Camera& camera);

    DISALLOW_COPY_AND_ASSIGN(CgFxPortalBlock);
};

#endif // __CGFXPORTALBLOCK_H__