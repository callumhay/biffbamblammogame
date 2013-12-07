/**
 * CgFxBossWeakpoint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXBOSSWEAKPOINT_H__
#define __CGFXBOSSWEAKPOINT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Light.h"

class CgFxBossWeakpoint : public CgFxTextureEffectBase {
public:
    static const char* BASIC_TECHNIQUE_NAME;
    static const char* TEXTURED_TECHNIQUE_NAME;

    CgFxBossWeakpoint();
    ~CgFxBossWeakpoint();

    void SetTexture(const Texture2D* texture);
    void SetLightAmt(float amt);
    void SetLightAmt(const BasicPointLight& keyLight, const BasicPointLight& fillLight);

private:
    CGparameter wvpMatrixParam;
    CGparameter colourTexSamplerParam;
    CGparameter lightAmtParam;
    
    const Texture2D* colourTex;
    float lightAmt;

    void SetupBeforePasses(const Camera& camera);

    DISALLOW_COPY_AND_ASSIGN(CgFxBossWeakpoint);
};

inline void CgFxBossWeakpoint::SetLightAmt(float amt) {
    assert(amt >= 0.0f && amt <= 1.0f);
    this->lightAmt = amt;
}

inline void CgFxBossWeakpoint::SetLightAmt(const BasicPointLight& keyLight, 
                                           const BasicPointLight& fillLight) {

    this->SetLightAmt(std::min<float>(1.0f, 2*std::max<float>(keyLight.GetDiffuseColour().GetGreyscaleAmt(), 
        fillLight.GetDiffuseColour().GetGreyscaleAmt())));
}

#endif // __CGFXBOSSWEAKPOINT_H__