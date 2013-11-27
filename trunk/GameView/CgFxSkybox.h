/**
 * CgFxSkybox.h
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

class CgFxSkybox : public CgFxEffectBase {
public:
    CgFxSkybox(Texture *skyTex);
    ~CgFxSkybox();

    void SetMoveFrequency(float freq) { this->moveFreq = freq; }
    void SetFGScale(float scale) { this->fgScale = scale; }
    void SetAlpha(float alpha) { this->alpha = alpha; }
    void SetUVTranslation(float s, float t) {
        this->uvTranslation[0] = s;
        this->uvTranslation[1] = t;
    }

    Texture* GetSkyTexture() const { return this->skyTex; }

protected:
    void SetupBeforePasses(const Camera& camera);

private:
	static const char* SKYBOX_TECHNIQUE_NAME;

	// CgFx parameters
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;

    CGparameter fgScaleParam;
	CGparameter twinkleFreqParam;
    CGparameter moveFreqParam;
    CGparameter noiseScaleParam;
	CGparameter timerParam;
    CGparameter alphaParam;
	CGparameter viewDirParam;
    CGparameter uvTranslateParam;

	CGparameter noiseSamplerParam;
	CGparameter skySamplerParam;

	// Stored values for parameters
	float timer, twinkleFreq, moveFreq, noiseScale, fgScale, alpha;
	Vector2D uvTranslation;
    GLint noiseTexID; 
	Texture* skyTex;

	Vector3D viewDir;

    DISALLOW_COPY_AND_ASSIGN(CgFxSkybox);
};

#endif // __CGFXDECOSKYBOX_H__