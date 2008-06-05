#ifndef __CGFXCELSHADING_H__
#define __CGFXCELSHADING_H__

#include "../Utils/Includes.h"
#include "../Utils/Colour.h"

#include "CgFxEffect.h"
#include "Texture2D.h"

#include <string>
#include <map>

/**
 * Material properties of the celshader.
 */
struct CelShadingProperties {
	Colour diffuse, specular;
	float shininess;
	Texture2D* texture;
	
	CelShadingProperties() : diffuse(Colour(1,1,1)), specular(Colour(1,1,1)), shininess(30), texture(NULL) {}
	~CelShadingProperties() {
		if (texture != NULL) {
			delete this->texture;
		}
	}
};

/**
 * Represents CgFx CelShader material.
 */
class CgFxCelShading : public CgFxEffect {

private:

	// Constants used for loading the CelShading effect
	static const std::string BASIC_TECHNIQUE_NAME;
	static const std::string TEXTURED_TECHNIQUE_NAME;

	// Cg Runtime Effect and its Techniques
	static CGeffect cgCelShadingEffect;
	static std::map<std::string, CGtechnique> cgCelShadingTechniques;

	// Cg parameter pointers
	static CGparameter mvpMatrixParam;
	static CGparameter texSamplerParam;

	virtual void SetupBeforePasses();

	Colour diffuse, specular;
	float shininess;
	Texture2D* texture;

public:
	CgFxCelShading();
	~CgFxCelShading();

	void SetDiffuse(const Colour& diff) {
		this->diffuse = diff;
	}
	void SetSpecular(const Colour& spec) {
		this->specular = spec;
	}
	void SetShininess(float s) {
		this->shininess = s;
	}
	bool SetTexture(const std::string& filepath) {
		this->texture = Texture2D::CreateTexture2DFromImgFile(filepath);
		if (this->texture == NULL) {
			return false;
		}
		return true;
	}

	static void Load();

	// TODO: get rid of this function
	void DrawMaterial();

};

#endif