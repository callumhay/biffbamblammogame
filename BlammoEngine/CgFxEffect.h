/**
 * CgFxEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

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
	static const char* MATERIAL_CELBASIC_TYPE;
	static const char* MATERIAL_PHONG_TYPE;
	static const char* MATERIAL_INKBLOCK_TYPE;
	static const char* MATERIAL_STICKYGOO_TYPE;
	static const char* MATERIAL_PRISM_TYPE;
	static const char* MATERIAL_PORTAL_TYPE;

	// Geometry types
	static const char* MATERIAL_GEOM_FG_TYPE;
	static const char* MATERIAL_GEOM_BG_TYPE;

	Colour diffuse, specular, outlineColour;
	float shininess, outlineSize;
	float alphaMultiplier;
	Texture* diffuseTexture;
	
	std::string materialType;
	std::string geomType;

	MaterialProperties(): diffuseTexture(NULL), materialType(MATERIAL_CELBASIC_TYPE), 
        geomType(MATERIAL_GEOM_BG_TYPE), diffuse(Colour(1,1,1)), 
        specular(Colour(1,1,1)), outlineColour(Colour(0,0,0)), 
        shininess(128.0f), outlineSize(1.0f), alphaMultiplier(1.0f) {}

	~MaterialProperties() {
		if (this->diffuseTexture != NULL) {
			bool resourceRemoved = ResourceManager::GetInstance()->ReleaseTextureResource(this->diffuseTexture);
			assert(resourceRemoved);
            UNUSED_VARIABLE(resourceRemoved);

			this->diffuseTexture = NULL;
		}
	}
};

/**
 * Superclass for post-processing CgFX effects.
 */
class CgFxPostProcessingEffect {
public:
	CgFxPostProcessingEffect(const std::string& effectPath, FBObj* sceneFBO) : sceneFBO(sceneFBO) {
		// Load the effect and its techniques from file
		ResourceManager::GetInstance()->GetCgFxEffectResource(effectPath, this->cgEffect, this->techniques);
		assert(this->cgEffect != NULL);
		assert(this->techniques.size() > 0);
	}

	virtual ~CgFxPostProcessingEffect() {
		// Release the effect resource
		bool success = ResourceManager::GetInstance()->ReleaseCgFxEffectResource(this->cgEffect);
        UNUSED_VARIABLE(success);
		assert(success);
	}

	

	inline void SetInputFBO(FBObj* sceneFBO) {
		this->sceneFBO = sceneFBO;
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
			if (cgValidateTechnique(currTechnique) == CG_FALSE) {
				debug_output("Invalid Cg Technique found: " << techniqueName);
			}
			this->currTechnique = temp;
		}
	}

protected:
	// The Cg Effect pointer
	CGeffect cgEffect;

	// The current technique and a set of all techniques for this effect
	CGtechnique currTechnique;
	std::map<std::string, CGtechnique> techniques;

	// Pointer to the scene FBO - this must be passed to the effect
	FBObj* sceneFBO;

    virtual void Draw(int screenWidth, int screenHeight, double dT) = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(CgFxPostProcessingEffect);
};

/**
 * Superclass for most CgFX effects.
 */
class CgFxEffectBase {
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
			CgFxEffectBase::DrawPass(currPass, displayListID);
			currPass = cgGetNextPass(currPass);
		}
	}
	void Draw(const Camera& camera, const std::vector<GLuint> &displayListIDs) {
		this->SetupBeforePasses(camera);
		
		// Draw each pass of this effect
		CGpass currPass = cgGetFirstPass(this->currTechnique);
		while (currPass) {
			CgFxEffectBase::DrawPass(currPass, displayListIDs);
			currPass = cgGetNextPass(currPass);
		}
	}
    void DrawText(const Camera& camera, GLuint baseDispList, const std::string& s) {
		this->SetupBeforePasses(camera);
		
		// Draw each pass of this effect
		CGpass currPass = cgGetFirstPass(this->currTechnique);
		while (currPass) {
			CgFxEffectBase::DrawPass(currPass, baseDispList, s);
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
			if (cgValidateTechnique(currTechnique) == CG_FALSE) {
				debug_output("Invalid Cg Technique found: " << techniqueName);
			}
			this->currTechnique = temp;
		}
	}
protected:
	// The Cg Effect pointer
	CGeffect cgEffect;

	// The current technique and a set of all techniques for this effect
	CGtechnique currTechnique;
	std::map<std::string, CGtechnique> techniques;

	virtual void SetupBeforePasses(const Camera& camera) = 0;

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
    static void DrawPass(CGpass pass, GLuint baseDispList, const std::string& s) {
		cgSetPassState(pass);
	    glListBase(baseDispList);
	    glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());
		cgResetPassState(pass);
    }
	static void DrawPass(CGpass pass, const std::vector<GLuint> &displayListIDs) {
		assert(displayListIDs.size() > 0);
		cgSetPassState(pass);
		glListBase(0);
		glCallLists(displayListIDs.size(), GL_UNSIGNED_INT, &displayListIDs[0]);
		cgResetPassState(pass);
	}

    DISALLOW_COPY_AND_ASSIGN(CgFxEffectBase);
};

/**
 * Base class for loading and dealing with CgFx material shaders.
 */
class CgFxMaterialEffect : public CgFxEffectBase {	
public:
	CgFxMaterialEffect(const std::string& effectPath, MaterialProperties* props);
	virtual ~CgFxMaterialEffect();

	MaterialProperties* GetProperties() {
		return this->properties;
	}

	void SetKeyLight(const BasicPointLight& keyLight) {
        this->keyLight = keyLight;
	}
	void SetFillLight(const BasicPointLight& fillLight) {
		this->fillLight = fillLight;
	}
	void SetBallLight(const BasicPointLight& ballLight) {
		this->ballLight = ballLight;
	}

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
	CGparameter alphaMultiplierParam;

	// Lights
	CGparameter keyPointLightPosParam;
	CGparameter keyPointLightColourParam;

	CGparameter fillPointLightPosParam;
	CGparameter fillPointLightColourParam;
	CGparameter fillPointLightAttenParam;

	CGparameter ballPointLightPosParam;
	CGparameter ballPointLightDiffuseParam;
	CGparameter ballPointLightAttenParam;

	BasicPointLight keyLight;
	BasicPointLight fillLight;
	BasicPointLight ballLight;

	// Properties of this material
	MaterialProperties* properties;

private:
	void LoadParameters();
    DISALLOW_COPY_AND_ASSIGN(CgFxMaterialEffect);
};

#endif