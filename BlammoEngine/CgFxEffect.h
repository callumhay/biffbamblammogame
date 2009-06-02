#ifndef __CGFXEFFECT_H__
#define __CGFXEFFECT_H__

#include "BasicIncludes.h"
#include "Colour.h"
#include "Point.h"
#include "Light.h"
#include "Camera.h"
#include "Texture2D.h"

#include "../ResourceManager.h"

class PolygonGroup;
class FBObj;

/**
 * Holds all of the possible material properties in every type of material,
 * this acts as a general object for holding the properties so that they can
 * be set for the various types of materials.
 */
struct MaterialProperties {
	
	// Material types
	static const std::string MATERIAL_CELBASIC_TYPE;
	static const std::string MATERIAL_PHONG_TYPE;
	static const std::string MATERIAL_INKBLOCK_TYPE;

	// Geometry types
	static const std::string MATERIAL_GEOM_FG_TYPE;
	static const std::string MATERIAL_GEOM_BG_TYPE;

	Colour diffuse, specular, outlineColour;
	float shininess, outlineSize;
	Texture* diffuseTexture;
	
	std::string materialType;
	std::string geomType;

	MaterialProperties(): diffuseTexture(NULL), materialType(MATERIAL_CELBASIC_TYPE), geomType(MATERIAL_GEOM_BG_TYPE),
		diffuse(Colour(1,1,1)), specular(Colour(1,1,1)), outlineColour(Colour(0,0,0)), shininess(128.0f), outlineSize(1.0f) {}

	~MaterialProperties() {
		if (this->diffuseTexture != NULL) {
			bool resourceRemoved = ResourceManager::GetInstance()->ReleaseTextureResource(this->diffuseTexture);
			assert(resourceRemoved);
			this->diffuseTexture = NULL;
		}
	}
};

/**
 * Superclass for post-processing CgFX effects.
 */
class CgFxPostProcessingEffect {

protected:
	// The Cg Effect pointer
	CGeffect cgEffect;

	// The current technique and a set of all techniques for this effect
	CGtechnique currTechnique;
	std::map<std::string, CGtechnique> techniques;

	// Pointer to the scene FBO - this must be passed to the effect
	FBObj* sceneFBO;

public:
	CgFxPostProcessingEffect(const std::string& effectPath, FBObj* sceneFBO) : sceneFBO(sceneFBO) {
		assert(sceneFBO != NULL);
		// Load the effect and its techniques from file
		ResourceManager::GetInstance()->GetCgFxEffectResource(effectPath, this->cgEffect, this->techniques);
		assert(this->cgEffect != NULL);
		assert(this->techniques.size() > 0);
	}

	virtual ~CgFxPostProcessingEffect() {
		// Release the effect resource
		bool success = ResourceManager::GetInstance()->ReleaseCgFxEffectResource(this->cgEffect);
		assert(success);
	}

	virtual void Draw(int screenWidth, int screenHeight, double dT) = 0;
};

/**
 * Superclass for most CgFX effects.
 */
class CgFxEffectBase {

private:
	/**
	 * Draw the given display list in the given pass using
	 * the Cg runtime.
	 */
	static void DrawPass(CGpass pass, GLuint displayListID) {
		cgSetPassState(pass);
		glCallList(displayListID);
		cgResetPassState(pass);
	}
	static void DrawPass(CGpass pass, const std::list<GLuint> &displayListIDs) {
		cgSetPassState(pass);
		for (std::list<GLuint>::const_iterator iter = displayListIDs.begin(); iter != displayListIDs.end(); iter++) {
			glCallList(*iter);
		}
		cgResetPassState(pass);
	}

protected:
	// The Cg Effect pointer
	CGeffect cgEffect;

	// The current technique and a set of all techniques for this effect
	CGtechnique currTechnique;
	std::map<std::string, CGtechnique> techniques;

	virtual void SetupBeforePasses(const Camera& camera) = 0;

public:
	CgFxEffectBase(const std::string& effectPath);
	virtual ~CgFxEffectBase();

  /**
	 * Draw the given display list with this effect
	 * applied to it.
	 */
	void Draw(const Camera& camera, GLuint displayListID) {
		this->SetupBeforePasses(camera);
		
		// Draw each pass of this effect
		CGpass currPass = cgGetFirstPass(this->currTechnique);
		while (currPass) {
			this->DrawPass(currPass, displayListID);
			currPass = cgGetNextPass(currPass);
		}
	}
	void Draw(const Camera& camera, const std::list<GLuint> &displayListIDs) {
		this->SetupBeforePasses(camera);
		
		// Draw each pass of this effect
		CGpass currPass = cgGetFirstPass(this->currTechnique);
		while (currPass) {
			this->DrawPass(currPass, displayListIDs);
			currPass = cgGetNextPass(currPass);
		}
	}

	/**
	 * Obtain the current technique for this effect.
	 */
	CGtechnique GetCurrentTechnique() const {
		return this->currTechnique;
	}

	void SetTechnique(const std::string& techniqueName) {
		CGtechnique temp = this->techniques[techniqueName];
		if (temp != NULL) {
			this->currTechnique = temp;
		}
	}

};

/**
 * Base class for loading and dealing with CgFx material shaders.
 */
class CgFxMaterialEffect : public CgFxEffectBase {
private:
	void LoadParameters();

protected:
	virtual void SetupBeforePasses(const Camera& camera);

	// Cg Parameters shared by all effects
	// Transforms
	CGparameter worldITMatrixParam;
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;
	CGparameter viewInvMatrixParam;

	// Textures
	CGparameter texSamplerParam;

	// Material properties
	CGparameter diffuseColourParam;
	CGparameter shininessParam;
	CGparameter specularColourParam;
	
	// Lights
	CGparameter keyPointLightPosParam;
	CGparameter keyPointLightColourParam;

	CGparameter fillPointLightPosParam;
	CGparameter fillPointLightColourParam;
	CGparameter fillPointLightAttenParam;

	CGparameter ballPointLightPosParam;
	CGparameter ballPointLightDiffuseParam;
	CGparameter ballPointLightAttenParam;

	PointLight keyLight;
	PointLight fillLight;
	PointLight ballLight;

	// Properties of this material
	MaterialProperties* properties;
	
public:
	CgFxMaterialEffect(const std::string& effectPath, MaterialProperties* props);
	virtual ~CgFxMaterialEffect();

	MaterialProperties* GetProperties() {
		return this->properties;
	}

	void SetKeyLight(const PointLight& keyLight) {
		this->keyLight = keyLight;
	}
	void SetFillLight(const PointLight& fillLight) {
		this->fillLight = fillLight;
	}
	void SetBallLight(const PointLight& ballLight) {
		this->ballLight = ballLight;
	}

};

#endif