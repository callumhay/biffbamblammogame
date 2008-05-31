#ifndef __CELSHADINGMATERIAL_H__
#define __CELSHADINGMATERIAL_H__

#include "Texture2D.h"

#include "../Utils/Colour.h"

#include <string>

class CelShadingMaterial {
private:

	Colour diffuse, specular;
	float shininess;
	Texture2D* texture;

public:
	CelShadingMaterial();
	~CelShadingMaterial();

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

	void DrawMaterial();
	

};

#endif