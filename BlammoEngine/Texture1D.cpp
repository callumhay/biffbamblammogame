/**
 * Texture1D.cpp
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

#include "Texture1D.h"

Texture1D::Texture1D(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_1D) {
}

Texture1D::~Texture1D() {
}

Texture1D* Texture1D::CreateTexture1DFromBuffer(unsigned char* fileBuffer, long fileBufferLength, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

	Texture1D* newTex = new Texture1D(texFilter);
	if (!newTex->Load2DOr1DTextureFromBuffer(fileBuffer, fileBufferLength, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	return newTex;
}

/**
 * Static creator method for the creation of 1D textures.
 */
Texture1D* Texture1D::CreateTexture1DFromImgFile(const std::string& filepath, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

	Texture1D* newTex = new Texture1D(texFilter);
	if (!newTex->Load2DOr1DTextureFromImg(filepath, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	return newTex;
}

Texture1D* Texture1D::CreateTexture1DFromImgFile(PHYSFS_File* fileHandle, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT);

	Texture1D* newTex = new Texture1D(texFilter);
	if (!newTex->Load2DOr1DTextureFromImg(fileHandle, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	return newTex;
}