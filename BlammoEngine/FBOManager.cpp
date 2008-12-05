#include "FBOManager.h"
#include "Camera.h"
#include "Texture.h"

FBOManager* FBOManager::instance = NULL;

FBOManager::FBOManager() : fboID(0), renderBuffID(0) {
	// Init the framebuffer and renderbuffer ID for binding, but don't bind yet
	glGenFramebuffersEXT(1, &this->fboID);
	glGenRenderbuffersEXT(1, &this->renderBuffID);
}

FBOManager::~FBOManager() {
	glDeleteFramebuffersEXT(1, &this->fboID);
	glDeleteRenderbuffersEXT(1, &this->renderBuffID);
}

/**
 * Setup the Framebuffer object to render to a specific texture type with a specific texture ID
 * at a given height and width - call this BEFORE binding.
 * Returns: true on success, false otherwise.
 */
bool FBOManager::SetupFBO(Texture& texture) {
	// TODO: test without renderbuffer...

	texture.BindTexture();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fboID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->renderBuffID);

	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, texture.GetWidth(), texture.GetHeight());
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->renderBuffID);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texture.GetTextureType(), texture.GetTextureID(), 0);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT };
	glDrawBuffers(1, buffers);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	texture.UnbindTexture();

	// Make sure everything FBO is good to go
	return this->CheckFBOStatus();
}		

/**
 * Private helper function that checks the status of the frame buffer object and reports any errors.
 * Returns: true on successful status, false if badness.
 */
bool FBOManager::CheckFBOStatus() {
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
