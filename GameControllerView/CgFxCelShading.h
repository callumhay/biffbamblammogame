#ifndef __CGFXCELSHADING_H__
#define __CGFXCELSHADING_H__

#include "../Utils/Includes.h"
#include "../Utils/Colour.h"

#include "CgFxEffect.h"
#include "Texture2D.h"
#include "Texture1D.h"

#include <string>
#include <map>

class Camera;

/**
 * Represents CgFx CelShader material.
 */
class CgFxCelShading : public CgFxEffect {

protected:
	virtual void Load();
	virtual void Unload();

private:
	// Constants used for loading the CelShading effect
	static const std::string BASIC_TECHNIQUE_NAME;
	static const std::string TEXTURED_TECHNIQUE_NAME;

	// Cg Runtime Effect and its Techniques
	static CGeffect cgCelShadingEffect;
	static std::map<std::string, CGtechnique> cgCelShadingTechniques;

	static bool isCelShadingEffectLoaded;

	// Cg parameter pointers
	// Transforms
	static CGparameter worldITMatrixParam;
	static CGparameter wvpMatrixParam;
	static CGparameter worldMatrixParam;
	static CGparameter viewInvMatrixParam;

	// Textures
	static CGparameter texSamplerParam;
	static CGparameter celSamplerParam;

	// Material properties
	static CGparameter diffuseColourParam;
	static CGparameter shininessParam;
	static CGparameter specularColourParam;
	
	// Lights
	static CGparameter diffuseLightPosParam;
	static CGparameter diffuseLightColourParam;
	
	// Cel shading texture
	static Texture1D* celDiffuseTexture;

public:
	CgFxCelShading(MaterialProperties* properties);
	~CgFxCelShading();

	virtual void SetupBeforePasses(const Camera& camera);

};

#endif