#include "TextureCube.h"

// Every array of cubemapped textures MUST be represented in this order
const GLint TextureCube::CUBE_FACES[NUM_CUBE_FACES] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
																											 GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
																											 GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};



TextureCube::TextureCube(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_CUBE_MAP) {
}

TextureCube::~TextureCube() {
}

bool TextureCube::LoadCubeTextureFromImgs(const std::string filepaths[NUM_CUBE_FACES], TextureFilterType texFilter) {
	
	// Generate the cubemap texture and bind it
	glGenTextures(1, &this->texID);
	glBindTexture(this->textureType, this->texID);
	
	// Load the faces of the cube map
	for (int i = 0; i < NUM_CUBE_FACES; i++) {
		
		// Read the image data using DevIL
		int imageID =	ilGenImage();
		ilBindImage(imageID);
		ILboolean resultOfImageLoad = ilLoadImage(filepaths[i].c_str());
		if (!resultOfImageLoad) {
			debug_output("Failed to load cube face texture image from " << filepaths[i]);
			return false;
		}

		ILubyte* texelData = ilGetData();
		ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
		ILint width = ilGetInteger(IL_IMAGE_WIDTH);
		ILint internalFormat = ilGetInteger(IL_IMAGE_BPP);
		ILint imgFormat = ilGetInteger(IL_IMAGE_FORMAT);

		glTexImage2D(CUBE_FACES[i], 0, internalFormat, width, height, 0, imgFormat, GL_UNSIGNED_BYTE, texelData);
		
		// Delete the DevIL image
		ilDeleteImage(imageID);
	}

	// Set the filtering and wrap parameters
	Texture::SetNonMipmapFilteringParams(texFilter, this->textureType);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(this->textureType, GL_TEXTURE_WRAP_R, GL_REPEAT);

	return true;
}

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