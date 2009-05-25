#ifndef __CGFXCELSHADING_H__
#define __CGFXCELSHADING_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Texture1D.h"

/**
 * Represents CgFx CelShader material.
 */
class CgFxCelShading : public CgFxMaterialEffect {

private:
	// Constants used for loading the CelShading effect
	static const std::string BASIC_FG_TECHNIQUE_NAME;
	static const std::string TEXTURED_FG_TECHNIQUE_NAME;
	static const std::string BASIC_BG_TECHNIQUE_NAME;
	static const std::string TEXTURED_BG_TECHNIQUE_NAME;
	static Texture* CelDiffuseTexture;

	// Cel shading texture: parameter and texture objects
	CGparameter celSamplerParam;
	
	CGparameter outlineWidthParam;
	CGparameter outlineColourParam;

protected:
	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxCelShading(MaterialProperties* properties);
	virtual ~CgFxCelShading();

};

#endif