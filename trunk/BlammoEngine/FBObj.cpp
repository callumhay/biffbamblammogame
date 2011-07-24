/**
 * FBObj.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "FBObj.h"
#include "Camera.h"
#include "Texture.h"

FBObj::FBObj(int width, int height, Texture::TextureFilterType filter, int attachments) : 
fboID(0), depthBuffID(0), stencilBuffID(0), packedStencilDepthBuffID(0), fboTex(NULL) {
	// Generate the framebuffer object
	glGenFramebuffersEXT(1, &this->fboID);
	assert(this->fboID != 0);
	
	// Set up the texture to render the framebuffer into
	this->fboTex = Texture2D::CreateEmptyTextureRectangle(width, height, filter);
	assert(this->fboTex != NULL);

	// Bind this FBO for setup
	this->BindFBObj();

	// Depending on the types of attachments establish the appropriate renderbuffers...
	if ((attachments & DepthAttachment) == DepthAttachment && (attachments & StencilAttachment) == StencilAttachment) {
		// Both the depth and stencil render buffers are requested to use a packed depth-stencil render buffer
		glGenRenderbuffersEXT(1, &this->packedStencilDepthBuffID);
		assert(this->packedStencilDepthBuffID != 0);
		
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->packedStencilDepthBuffID);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, width, height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->packedStencilDepthBuffID);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->packedStencilDepthBuffID);
	}
	else {

		// Create the depth render buffer if requested...
		if ((attachments & DepthAttachment) == DepthAttachment) {
			glGenRenderbuffersEXT(1, &this->depthBuffID);
			assert(this->depthBuffID != 0);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->depthBuffID);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_RENDERBUFFER_EXT, this->depthBuffID);
		}

		// Create the stencil render buffer if requested...
		if ((attachments & StencilAttachment) == StencilAttachment) {
			glGenRenderbuffersEXT(1, &this->stencilBuffID);
			assert(this->stencilBuffID != 0);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->stencilBuffID);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, width, height);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->stencilBuffID);
		}

	}

	// Bind the given texture to the FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, this->fboTex->GetTextureType(), this->fboTex->GetTextureID(), 0);

	// Unbind the FBO for now
	this->UnbindFBObj();

	debug_opengl_state();	
	assert(FBObj::CheckFBOStatus());
}

FBObj::~FBObj() {
	// Clean-up all FBO and renderbuffer objects in OGL
	glDeleteFramebuffersEXT(1,  &this->fboID);

	if (this->depthBuffID != 0) {
		glDeleteRenderbuffersEXT(1, &this->depthBuffID);
		this->depthBuffID = 0;
	}

	if (this->stencilBuffID != 0) {
		glDeleteRenderbuffersEXT(1, &this->stencilBuffID);
		this->stencilBuffID = 0;
	}
	
	if (this->packedStencilDepthBuffID != 0) {
		glDeleteRenderbuffersEXT(1, &this->packedStencilDepthBuffID);
		this->packedStencilDepthBuffID = 0;
	}

	// Clean-up the fbo texture
	delete this->fboTex;
	this->fboTex = NULL;

	debug_opengl_state();
	assert(FBObj::CheckFBOStatus());
}

/**
 * Private helper function that checks the status of the frame buffer object and reports any errors.
 * Returns: true on successful status, false if badness.
 */
bool FBObj::CheckFBOStatus() {
	debug_opengl_state();

	int status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		switch (status) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				debug_output("Framebuffer Object error detected: Incomplete attachment.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				debug_output("Framebuffer Object error detected: Incomplete dimensions.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				debug_output("Framebuffer Object error detected: Incomplete draw buffer.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				debug_output("Framebuffer Object error detected: Incomplete formats.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT:
				debug_output("Framebuffer Object error detected: Incomplete layer count.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
				debug_output("Framebuffer Object error detected: Incomplete layer targets.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				debug_output("Framebuffer Object error detected: Incomplete, missing attachment.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
				debug_output("Framebuffer Object error detected: Incomplete multisample.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				debug_output("Framebuffer Object error detected: Incomplete read buffer.");
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				debug_output("Framebuffer Object error detected: Framebuffer unsupported.");
				break;
			default:
				debug_output("Framebuffer Object error detected: Unknown Error");
				break;
		}
		return false;
	}

	return true;
}