#include "CelShadingMaterial.h"
#include "Texture2D.h"

// Default constructor, builds default, white material
CelShadingMaterial::CelShadingMaterial() : 
diffuse(Colour(1,1,1)), specular(Colour(0.33f, 0.33f, 0.33f)), shininess(100.0f) {
}

CelShadingMaterial::~CelShadingMaterial() {
}