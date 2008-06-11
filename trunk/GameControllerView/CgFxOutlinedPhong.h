#ifndef __CGFXOUTLINEDPHONG_H__
#define __CGFXOUTLINEDPHONG_H__

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
class CgFxOutlinedPhong : public CgFxEffect {

protected:
	virtual void Load();
	virtual void Unload();

private:
	// Constants used for loading the CelShading effect
	static const std::string BASIC_TECHNIQUE_NAME;
	static const std::string TEXTURED_TECHNIQUE_NAME;

	// Cg Runtime Effect and its Techniques
	static CGeffect cgOutlinedPhongEffect;
	static std::map<std::string, CGtechnique> cgOutlinedPhongTechniques;
	static bool isOutlinedPhongEffectLoaded;

	// Cg parameter pointers
	// Transforms
	static CGparameter worldITMatrixParam;
	static CGparameter wvpMatrixParam;
	static CGparameter worldMatrixParam;
	static CGparameter viewInvMatrixParam;

	// Textures
	static CGparameter texSamplerParam;

	// Material properties
	static CGparameter diffuseColourParam;
	static CGparameter specularColourParam;
	static CGparameter shininessParam;
	
	// Lights
	static CGparameter diffuseLightPosParam;
	static CGparameter diffuseLightColourParam;

public:
	CgFxOutlinedPhong(MaterialProperties* properties);
	~CgFxOutlinedPhong();

	virtual void SetupBeforePasses(const Camera& camera);

};

#endif