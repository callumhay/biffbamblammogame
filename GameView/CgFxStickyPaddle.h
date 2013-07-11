/**
 * CgFxStickyPaddle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
public:
	CgFxStickyPaddle(MaterialProperties* properties);
	~CgFxStickyPaddle();

	void SetSceneTexture(const Texture2D* tex) {
		this->sceneTex = tex;
	}
    void SetAlpha(float alpha) {
        assert(alpha >= 0.0f && alpha <= 1.0f);
        this->alpha = alpha;
    }

private:
    static const char* STICKYPADDLE_TECHNIQUE_NAME;

    // Cg parameters for the sticky paddle effect
    CGparameter timerParam;
    CGparameter displacementParam;
    CGparameter speedParam;
    CGparameter waveScaleParam;
    CGparameter refractScaleParam;
    CGparameter noiseScaleParam;
    CGparameter sceneWidthParam;
    CGparameter sceneHeightParam;
    CGparameter alphaMultiplierParam;

    CGparameter sceneSamplerParam;

    // Values for the Cg parameters
    float timer, displacement, speed, waveScale, refractScale, noiseScale, alpha;
    const Texture2D* sceneTex;

    void SetupBeforePasses(const Camera& camera);

    DISALLOW_COPY_AND_ASSIGN(CgFxStickyPaddle);
};

#endif