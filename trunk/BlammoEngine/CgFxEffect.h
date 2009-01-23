#ifndef __CGFXEFFECT_H__
#define __CGFXEFFECT_H__

#include "BasicIncludes.h"
#include "Colour.h"
#include "Point.h"

#include "Texture2D.h"

class Camera;
class PolygonGroup;

/**
 * Holds all of the possible material properties in every type of material,
 * this acts as a general object for holding the properties so that they can
 * be set for the various types of materials.
 */
struct MaterialProperties {
	
	// Material types
	static const std::string MATERIAL_CELBASIC_TYPE;
	static const std::string MATERIAL_CELPHONG_TYPE;

	Colour diffuse, specular, outlineColour;
	float shininess, outlineSize;
	Texture2D* diffuseTexture;
	std::string materialType;

	MaterialProperties(): diffuseTexture(NULL), materialType(MATERIAL_CELBASIC_TYPE), 
		diffuse(Colour(1,1,1)), specular(Colour(1,1,1)), outlineColour(Colour(0,0,0)), shininess(128.0f), outlineSize(1.0f) {}

	~MaterialProperties() {
		if (this->diffuseTexture != NULL) {
			delete this->diffuseTexture;
		}
	}
};

/**
 * Base superclass for all CgFX effects.
 */
class CgFxEffectBase {

private:
	/**
	 * Draw the given display list in the given pass using
	 * the Cg runtime.
	 */
	void DrawPass(CGpass pass, GLint displayListID) {
		cgSetPassState(pass);
		glCallList(displayListID);
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
	void Draw(const Camera& camera, GLint displayListID) {
		this->SetupBeforePasses(camera);
		
		// Draw each pass of this effect
		CGpass currPass = cgGetFirstPass(this->currTechnique);
		while (currPass) {
			this->DrawPass(currPass, displayListID);
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
	CGparameter sceneLightPosParam;
	CGparameter sceneLightColourParam;

	// Properties of this material
	MaterialProperties* properties;
	
public:
	CgFxMaterialEffect(const std::string& effectPath, MaterialProperties* props);
	virtual ~CgFxMaterialEffect();

	MaterialProperties* GetProperties() {
		return this->properties;
	}

	// Setter methods for the Cg parameters -------------------
	void SetWorldInverseMatrixParam() {
		cgGLSetStateMatrixParameter(this->worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	}
	void SetWorldViewProjMatrixParam() {
		cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	}
	void SetWorldMatrixParam() {
		cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	}
	void SetViewInverseMatrixParam() {
		cgGLSetStateMatrixParameter(this->viewInvMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	}
	void SetDiffuseTextureSamplerParam() {
		if (this->properties->diffuseTexture != NULL) {
			cgGLSetTextureParameter(this->texSamplerParam, this->properties->diffuseTexture->GetTextureID());
		}
	}
	void SetDiffuseColourParam() {
		cgGLSetParameter3f(this->diffuseColourParam, this->properties->diffuse.R(), this->properties->diffuse.G(), this->properties->diffuse.B());
	}
	void SetSpecularColourParam() {
		cgGLSetParameter3f(this->specularColourParam, this->properties->specular.R(), this->properties->specular.G(), this->properties->specular.B());
	}
	void SetShininessParam() {
		cgGLSetParameter1f(this->shininessParam, this->properties->shininess);
	}
	void SetSceneLightPositionParam(const Point3D& pos) {
		// TODO...
		cgGLSetParameter3f(this->sceneLightPosParam, 0.0f, 20.0f, 50.0f);
	}
	void SetSceneLightColourParam(const Colour& colour) {
		// TODO...
		cgGLSetParameter3f(this->sceneLightColourParam, 1.0f, 1.0f, 1.0f);
	}
	// ---------------------------------------------------------

};

#endif