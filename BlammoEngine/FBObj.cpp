/**
 * FBObj.cpp
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

#include "FBObj.h"
#include "Camera.h"
#include "Texture.h"

FBObj::FBObj(int width, int height, Texture::TextureFilterType filter, int attachments) : 
fboID(0), depthBuffID(0), stencilBuffID(0), packedStencilDepthBuffID(0), fboTex(NULL), depthTex(NULL) {
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

    // Check to see if we're also creating and binding a depth texture...
    if ((attachments & DepthTextureAttachment) == DepthTextureAttachment) {

        assert((attachments & DepthAttachment) == 0x00000000); // There will be no rendering to depth texture if the depth renderbuffer is also active!

        this->depthTex = Texture2D::CreateEmptyDepthTextureRectangle(width, height);
        assert(this->depthTex != NULL);

        // Bind the depth texture to the depth component of the FBO
        glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, this->depthTex->GetTextureID(), 0);
    }

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

    // Clean up any depth texture
    if (this->depthTex != NULL) {
        delete this->depthTex;
        this->depthTex = NULL;
    }

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