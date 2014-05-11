/**
 * TextureCube.cpp
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

#include "TextureCube.h"

// Every array of cubemapped textures MUST be represented in this order
const GLint TextureCube::CUBE_FACES[NUM_CUBE_FACES] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
																											 GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
																											 GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

TextureCube::TextureCube(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_CUBE_MAP) {
}

TextureCube::~TextureCube() {
}

/**
 * Create a cube map/texture from a set of 6 2D texture files.
 */
bool TextureCube::LoadCubeTextureFromImgs(const std::string filepaths[NUM_CUBE_FACES], TextureFilterType texFilter) {
	
	// Generate the cubemap texture and bind it
	glGenTextures(1, &this->texID);
	glBindTexture(this->textureType, this->texID);
	
	// Load the faces of the cube map
	for (int i = 0; i < NUM_CUBE_FACES; i++) {
		
		// Read the image data using DevIL
		int imageID =	ilGenImage();
		ilBindImage(imageID);
		ILboolean resultOfImageLoad = ilLoadImage((const ILstring)filepaths[i].c_str());
		if (!resultOfImageLoad) {
			debug_output("Failed to load cube face texture image from " << filepaths[i]);
			return false;
		}

		ILubyte* texelData = ilGetData();
		ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
		ILint width = ilGetInteger(IL_IMAGE_WIDTH);

		// Assign width and height and make sure they are equal for all textures
		// in the cube map
		if (i == 0) {
			this->width = width;
			this->height = height;
		}
		else {
			assert(this->width == static_cast<unsigned int>(width));
			assert(this->height == static_cast<unsigned int>(height));
		}

		ILint internalFormat = ilGetInteger(IL_IMAGE_BPP);
		ILint imgFormat = ilGetInteger(IL_IMAGE_FORMAT);

		if (this->texFilter == Texture::Nearest) {
			glTexImage2D(CUBE_FACES[i], 0, internalFormat, this->width, this->height, 0, imgFormat, GL_UNSIGNED_BYTE, texelData);
		}
		else {
			GLint result = gluBuild2DMipmaps(CUBE_FACES[i], internalFormat, this->width, this->height, imgFormat, GL_UNSIGNED_BYTE, texelData);
			assert(result == 0);
			if (result != 0) {
				debug_output("Failed to to set OpenGL cube face texture.");
				return false;
			}	
		}

		// Delete the DevIL image
		ilDeleteImage(imageID);
	}

	// Set the filtering and wrap parameters
	Texture::SetFilteringParams(texFilter, this->textureType);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(this->textureType, 0);
	return true;
}

// Static creator methods ---------------------------------------------------------------------------

TextureCube* TextureCube::CreateCubeTextureFromImgFiles(const std::string filepaths[NUM_CUBE_FACES], 
		                                                    TextureFilterType texFilter) {
	TextureCube* newTex = new TextureCube(texFilter);
	if (newTex->LoadCubeTextureFromImgs(filepaths, texFilter)) {
		return newTex;
	}
	else {
		delete newTex;
	}
	return NULL;
}