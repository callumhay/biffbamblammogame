/**
 * TextureCube.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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