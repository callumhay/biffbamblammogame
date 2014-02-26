/**
 * FBObj.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __FBOBJ_H__
#define __FBOBJ_H__

#include "BasicIncludes.h"
#include "Texture.h"
#include "Texture2D.h"


class Camera;

class FBObj {
public:
	enum FBOAttachments {
        NoAttachment = 0x00000000, DepthAttachment = 0x00000001, StencilAttachment = 0x00000010,
        DepthTextureAttachment = 0x00000020
    };

	FBObj(int width, int height, Texture::TextureFilterType filter, int attachments);
	~FBObj();

	inline const Texture2D* GetFBOTexture() const { 
		return this->fboTex; 
	}
    inline const Texture2D* GetRenderToDepthTexture() const {
        return this->depthTex;
    }
	inline void BindFBObj() const {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fboID);
	}
	static void UnbindFBObj() {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	inline void BindDepthRenderBuffer() {
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->depthBuffID);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_RENDERBUFFER_EXT, this->depthBuffID);
	}
    inline void BindDepthRenderTexture() {
        glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, this->depthTex->GetTextureID(), 0);
    }

private:
	GLuint fboID;                       // OGL Framebuffer object ID
	GLuint depthBuffID;                 // OGL depth renderbuffer object ID
	GLuint stencilBuffID;               // OGL stencil renderbuffer object ID
	GLuint packedStencilDepthBuffID;	// OGL packed depth-stencil renderbuffer object ID

	Texture2D* fboTex;
    Texture2D* depthTex;

	static bool CheckFBOStatus();

    DISALLOW_COPY_AND_ASSIGN(FBObj);
};
#endif