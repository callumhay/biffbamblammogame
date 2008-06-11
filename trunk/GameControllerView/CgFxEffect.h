#ifndef __CGFXEFFECT_H__
#define __CGFXEFFECT_H__

#include "../Utils/Includes.h"
#include "../Utils/Colour.h"

#include "Texture2D.h"

#include <string>

class Camera;

/**
 * Holds all of the possible material properties in every type of material,
 * this acts as a general object for holding the properties so that they can
 * be set for the various types of materials.
 */
struct MaterialProperties {
	
	// Material types
	static const std::string MATERIAL_CELBASIC_TYPE;
	static const std::string MATERIAL_CELPHONG_TYPE;

	Colour diffuse, specular;
	float shininess;
	Texture2D* diffuseTexture;
	std::string materialType;

	MaterialProperties(): diffuseTexture(NULL), materialType(MATERIAL_CELBASIC_TYPE), 
		diffuse(Colour(1,1,1)), specular(Colour(1,1,1)), shininess(128.0f) {}

	~MaterialProperties() {
		if (this->diffuseTexture != NULL) {
			delete this->diffuseTexture;
		}
	}
};

/**
 * Base class for loading and dealing with CgFx shaders.
 */
class CgFxEffect {
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
	MaterialProperties* properties;
	CGtechnique currTechnique;

	virtual void Load()		= 0;
	virtual void Unload() = 0;

public:
	CgFxEffect(MaterialProperties* props) : properties(props), currTechnique(NULL) {
		assert(props != NULL);
	};
	virtual ~CgFxEffect() {};

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

	MaterialProperties* GetProperties() const {
		return this->properties;
	}

	CGtechnique GetCurrentTechnique() const {
		return this->currTechnique;
	}

	virtual void SetupBeforePasses(const Camera& camera) = 0;

};

#endif