/**
 * Texture3D.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Texture3D.h"
#include "Noise.h"
#include "../ResourceManager.h"

// Default constructor for 3D textures
Texture3D::Texture3D(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_3D), depth(0) {
}

Texture3D::~Texture3D() {
}

/**
 * Creates a 3D noise texture (perlin noise) of size x size x size.
 * Returns: A 3D perlin noise texture on success, NULL otherwise.
 */
Texture3D* Texture3D::Create3DNoiseTexture(int size) {
	Texture3D* newTex = new Texture3D(Linear);

	// Generate a texture...
	glGenTextures(1, &newTex->texID);
	if (newTex->texID == 0) {
		delete newTex;
		return NULL;
	}
	
	newTex->depth = size;
	
	glPushAttrib(GL_TEXTURE_BIT);
	newTex->BindTexture();

	// Grab the noise data
	GLubyte* texData = ResourceManager::GetInstance()->ReadNoiseOctave3DTextureData();

	// IF YOU NEED TO SAVE IT TO RAW DATA FILE...
	//if (texData == NULL) {
		//GLubyte* texData = Noise::GetInstance()->Make3DNoiseTexture(size);
		// Save the texture data to file...
		//std::ofstream outStream("noise_octaves.raw", std::ofstream::binary);
		//if (outStream.good()) {
		//	outStream.write((const char*)texData, size * size * size * 4);
		//	outStream.close();
		//}
	//}

	assert(texData != NULL);

	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	Texture::SetFilteringParams(Linear, newTex->textureType);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, size, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	newTex->UnbindTexture();
	glPopAttrib();
	delete[] texData;

	return newTex;
}