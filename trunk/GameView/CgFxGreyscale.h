/**
 * CgFxGreyscale.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXGREYSCALE_H__
#define __CGFXGREYSCALE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class CgFxGreyscale : public CgFxEffectBase {
public:
	CgFxGreyscale();
	~CgFxGreyscale();

    void SetGreyscaleFactor(float factor) { assert(factor >= 0.0f && factor <= 1.0f); this->greyscaleFactor = factor; }
    void SetColourTexture(Texture2D* tex) { this->colourTexture = tex; }

private:
    static const char* GREYSCALE_TECHNIQUE_NAME;

    CGparameter greyscaleFactorParam;

    // Texture sampler parameters
    CGparameter colourSamplerParam;

    Texture2D* colourTexture;
    float greyscaleFactor;

    void SetupBeforePasses(const Camera& camera);

    DISALLOW_COPY_AND_ASSIGN(CgFxGreyscale);
};

#endif // __CGFXGREYSCALE_H__