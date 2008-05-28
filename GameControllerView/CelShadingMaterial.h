#ifndef __CELSHADINGMATERIAL_H__
#define __CELSHADINGMATERIAL_H__

#include "Texture2D.h"

#include "../Utils/Colour.h"

class CelShadingMaterial {
private:

	Colour diffuse, specular;
	float shininess;
	Texture2D texture;

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
	void SetTexture(const Texture2D& tex) {
		this->texture = tex;
	}

};

#endif