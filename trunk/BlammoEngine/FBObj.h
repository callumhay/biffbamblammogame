/**
 * FBObj.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __FBOBJ_H__
#define __FBOBJ_H__

#include "BasicIncludes.h"
#include "Texture.h"
#include "Texture2D.h"


class Camera;

class FBObj {
private:
	GLuint fboID;											// OGL Framebuffer object ID
	GLuint depthBuffID;								// OGL depth renderbuffer object ID
	GLuint stencilBuffID;							// OGL stencil renderbuffer object ID
	GLuint packedStencilDepthBuffID;	// OGL packed depth-stencil renderbuffer object ID

	Texture2D* fboTex;

	static bool CheckFBOStatus();

public:
	enum FBOAttachments {NoAttachment = 0x00000000, DepthAttachment = 0x00000001, StencilAttachment = 0x00000010};

	FBObj(int width, int height, Texture::TextureFilterType filter, int attachments);
	~FBObj();

	inline const Texture2D* GetFBOTexture() const { 
		return this->fboTex; 
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

};
#endif