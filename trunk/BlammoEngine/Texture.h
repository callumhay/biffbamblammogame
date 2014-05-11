/**
 * Texture.h
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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "BasicIncludes.h"

// Abstract Texture class
class Texture {

public:
	enum TextureFilterType { Nearest, Linear, NearestMipmap, Bilinear, Trilinear };

protected:
	TextureFilterType texFilter;
	int textureType;
	GLuint texID;
	unsigned int width, height;

	static void SetFilteringParams(TextureFilterType texFilter, int glTexType);
	static bool IsMipmappedFilter(TextureFilterType texFilter) {
		return !(texFilter == Nearest || texFilter == Linear);
	}

	bool Load2DOr1DTextureFromBuffer(unsigned char* fileBuffer, long fileBufferLength, TextureFilterType texFilter = Nearest);
	bool Load2DOr1DTextureFromImg(const std::string& filepath, TextureFilterType texFilter = Nearest);
	bool Load2DOr1DTextureFromImg(PHYSFS_File* fileHandle, TextureFilterType texFilter = Nearest);

public:
	Texture(TextureFilterType texFilter, int textureType);
	virtual ~Texture();

	unsigned int GetTextureID() const {
		return this->texID;
	}
	
	int GetTextureType() const {
		return this->textureType;
	}

	unsigned int GetWidth() const {
		return this->width;
	}

	unsigned int GetHeight() const {
		return this->height;
	}

	TextureFilterType GetFilter() const {
		return this->texFilter;
	}

	// Functions for binding and unbinding the texture - these should
	// ALWAYS be used over manually doing it - both
	// help isolate problems with the OGL state
	void BindTexture() const {
		glEnable(this->textureType);
		glBindTexture(this->textureType, this->texID);
	}
	void UnbindTexture() const {
		glBindTexture(this->textureType, 0);
		glDisable(this->textureType);
	}

    void SetWrapParams(GLint wrapS, GLint wrapT) const {
        this->BindTexture();
        glTexParameteri(this->textureType, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(this->textureType, GL_TEXTURE_WRAP_T, wrapT);
        this->UnbindTexture();
    }

	void GenerateMipmaps() {
		glGenerateMipmapEXT(this->textureType);
	}

};

#endif