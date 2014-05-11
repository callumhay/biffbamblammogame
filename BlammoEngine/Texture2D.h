/**
 * Texture2D.h
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

#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__

#include "BasicIncludes.h"
#include "Texture.h"

// Wraps a OpenGL 2D texture, takes care of texture ID and stuff like that
class Texture2D : public Texture {
private:
	Texture2D(TextureFilterType texFilter);

public:
	virtual ~Texture2D();
	
    void RenderTextureToFullscreenQuadNoDepth() const;
	void RenderTextureToFullscreenQuad(float depth = 0.0f) const;
    void RenderTextureToFullscreenQuad(float depth, float uvX, float uvY) const;

	// Creator methods
	static Texture2D* CreateTexture2DFromBuffer(unsigned char* fileBuffer, long fileBufferLength, TextureFilterType texFilter);
	static Texture2D* CreateTexture2DFromImgFile(PHYSFS_File* fileHandle, TextureFilterType texFilter);
	static Texture2D* CreateTexture2DFromImgFile(const std::string& filepath, TextureFilterType texFilter);
	static Texture2D* CreateTexture2DFromFTBMP(const FT_Bitmap& bmp, TextureFilterType texFilter);
	static Texture2D* CreateEmptyTextureRectangle(int width, int height, Texture::TextureFilterType filter);
    static Texture2D* CreateEmptyDepthTextureRectangle(int width, int height);
};

#endif