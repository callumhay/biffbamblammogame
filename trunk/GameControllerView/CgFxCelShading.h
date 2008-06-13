#ifndef __CGFXCELSHADING_H__
#define __CGFXCELSHADING_H__

#include "CgFxEffect.h"
#include "Texture1D.h"

#include <string>

/**
 * Represents CgFx CelShader material.
 */
class CgFxCelShading : public CgFxEffect {

private:
	// Constants used for loading the CelShading effect
	static const std::string EFFECT_FILE;
	static const std::string BASIC_TECHNIQUE_NAME;
	static const std::string TEXTURED_TECHNIQUE_NAME;

	// Cel shading texture: parameter and texture objects
	CGparameter celSamplerParam;
	Texture1D* celDiffuseTexture;

public:

	CgFxCelShading(MaterialProperties* properties);
	virtual ~CgFxCelShading();

};

#endif