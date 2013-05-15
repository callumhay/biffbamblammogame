/**
 * FBObj.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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