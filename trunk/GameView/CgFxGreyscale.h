/**
 * CgFxGreyscale.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXGREYSCALE_H__
#define __CGFXGREYSCALE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class CgFxGreyscale : public CgFxEffectBase {
public:
	CgFxGreyscale();
	~CgFxGreyscale();

    void SetColourTexture(Texture2D* tex) { this->colourTexture = tex; }

private:
    static const char* GREYSCALE_TECHNIQUE_NAME;

    // Texture sampler parameters
    CGparameter colourSamplerParam;

    Texture2D* colourTexture;

    void SetupBeforePasses(const Camera& camera);

    DISALLOW_COPY_AND_ASSIGN(CgFxGreyscale);
};

#endif // __CGFXGREYSCALE_H__