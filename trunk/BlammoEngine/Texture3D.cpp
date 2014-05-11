/**
 * Texture3D.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

    /*
	// IF YOU NEED TO SAVE IT TO RAW DATA FILE...
	if (texData == NULL) {
		texData = Noise::GetInstance()->Make3DNoiseTexture(size);
		// Save the texture data to file...
		std::ofstream outStream("noise_octaves.raw", std::ofstream::binary);
		if (outStream.good()) {
			outStream.write((const char*)texData, size * size * size * 4);
			outStream.close();
		}
	}
    */

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