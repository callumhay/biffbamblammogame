#include "CelShadingMaterial.h"
#include "Texture2D.h"

#include "../Utils/Includes.h"

// Default constructor, builds default, white material
CelShadingMaterial::CelShadingMaterial() : 
diffuse(Colour(1,1,1)), specular(Colour(0.33f, 0.33f, 0.33f)), shininess(100.0f), texture(NULL) {
}

CelShadingMaterial::~CelShadingMaterial() {
	if (this->texture != NULL) {
		delete this->texture;
	}
}

void CelShadingMaterial::DrawMaterial() {
	if (this->texture == NULL) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		glBindTexture(this->texture->GetTextureType(), this->texture->GetTextureID());
	}
}