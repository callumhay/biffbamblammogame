/**
 * CgFxCelOutlines.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXCELOUTLINES_H__
#define __CGFXCELOUTLINES_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/FBObj.h"

class CgFxCelOutlines : public CgFxPostProcessingEffect {
public:
	CgFxCelOutlines();
	~CgFxCelOutlines();

    void Draw(FBObj* colourAndDepthSceneFBO, FBObj* outputFBO);   

    void SetMinDistance(float minDist) { this->minDistance = minDist; }
    void SetMaxDistance(float maxDist) { this->maxDistance = maxDist; }
    void SetContrastExponent(float contrast) { this->contrastExp = contrast; }
    void SetAlphaMultiplier(float alpha) { this->alphaMultiplier = alpha; }
    void SetOffsetMultiplier(float offsetMultiplier) { this->offsetMultiplier = offsetMultiplier; }

    void SetAmbientBrightness(float amt) { this->ambientBrightness = amt; }

private:
    static const char* TECHNIQUE_NAME;

    CGparameter nearZParam;
    CGparameter farZParam;
    CGparameter texOffsetsParam;
    CGparameter minDistanceParam;
    CGparameter maxDistanceParam;
    CGparameter ambientBrightnessParam;
    CGparameter ambientDarknessParam;
    CGparameter contrastParam;
    CGparameter alphaMultiplierParam;

    // Texture sampler parameters
    CGparameter colourSamplerParam;
    CGparameter depthSamplerParam;

    float minDistance;
    float maxDistance;
    float contrastExp;
    float alphaMultiplier;
    float offsetMultiplier;

    float ambientBrightness;

    void Draw(int, int, double) { assert(false); }

    DISALLOW_COPY_AND_ASSIGN(CgFxCelOutlines);
};

#endif // __CGFXCELOUTLINES_H__