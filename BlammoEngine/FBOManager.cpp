#include "FBOManager.h"
#include "Camera.h"
#include "Texture.h"

FBOManager* FBOManager::instance = NULL;

FBOManager::FBOManager() : 
fboID(0), multisampleFBOID(0), depthBuffID(0), colourBuffID(0), numSupportedSamples(0), allowMultisampling(false) { 
	// Init the framebuffer and renderbuffer ID for binding, but don't bind yet
	glGenFramebuffersEXT(1, &this->fboID);
	glGenFramebuffersEXT(1,  &this->multisampleFBOID);
	glGenRenderbuffersEXT(1, &this->depthBuffID);
	glGenRenderbuffersEXT(1, &this->colourBuffID);

	FBOManager::IsFBOMultisamplingEnabled(this->numSupportedSamples);
	debug_opengl_state();
}

FBOManager::~FBOManager() {
	glDeleteFramebuffersEXT(1,  &this->fboID);
	glDeleteFramebuffersEXT(1,  &this->multisampleFBOID);
	glDeleteRenderbuffersEXT(1, &this->depthBuffID);
	glDeleteRenderbuffersEXT(1, &this->colourBuffID);

	debug_opengl_state();
}

/**
 * Private helper function for setting up the FBO to render without multisampling.
 */
void FBOManager::SetupFBOWithoutMultisampling(const Texture& texture) {
	texture.BindTexture();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fboID);


	// Create the depth render buffer...
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->depthBuffID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, texture.GetWidth(), texture.GetHeight());
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_RENDERBUFFER_EXT, this->depthBuffID);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texture.GetTextureType(), texture.GetTextureID(), 0);
	texture.UnbindTexture();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	debug_opengl_state();	
}

/**
 * Private helper function for setting up the FBO to render with multisampling.
 */
void FBOManager::SetupFBOWithMultisampling(const Texture& texture) {
	assert(allowMultisampling);
  int tempFBOSamplesNum = 0;

	// The first FBO will be the resolve target. This will store the final
  // offscreen texture that will be applied to the full screen quad. Remember
  // that the only reason why we use this resolve target FBO is because FBOs
  // don't support the creation of multisample texture images. We're using
  // the resolve target FBO as a means to get a copy of the pixel data stored
  // in the proper multisample color render buffer image attached to the
  // second FBO (that supports multisample rendering).
  //
  // The resolve target FBO is just a plain FBO. Just attach the offscreen
  // texture to a color attachment point. We don't need a depth buffer or a
  // stencil buffer.
	texture.BindTexture();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fboID);

  // The second FBO will support multisample rendering. For the second
  // FBO we need to create two render buffer images: a color buffer and a
  // depth buffer. Both are multisampled.
  // When creating the multisample render buffer images the samples argument
  // of the glRenderbufferStorageMultisampleEXT() function is a request for
  // the desired minimum number of samples. The actual number is guaranteed 
	// to be greater than or equal to the requested sample
  // count and no more than the next larger sample count supported by the
  // implementation.
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->multisampleFBOID);

	// Create the multisample color render buffer image and attach it to the second FBO.
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->colourBuffID);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, this->numSupportedSamples, GL_RGBA8, texture.GetWidth(), texture.GetHeight());
  glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_SAMPLES_EXT, &tempFBOSamplesNum);
  assert(tempFBOSamplesNum >= this->numSupportedSamples);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, this->colourBuffID);

  // Create the multisample depth render buffer image and attach it to the second FBO.
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->depthBuffID);
  glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, this->numSupportedSamples, GL_DEPTH_COMPONENT, texture.GetWidth(), texture.GetHeight());
  glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_SAMPLES_EXT, &tempFBOSamplesNum);
  assert(tempFBOSamplesNum >= this->numSupportedSamples);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->depthBuffID);
	
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texture.GetTextureType(), texture.GetTextureID(), 0);
	texture.UnbindTexture();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	debug_opengl_state();
}

/**
 * Setup the Framebuffer object to render to a specific texture type with a specific texture ID
 * at a given height and width - call this BEFORE binding.
 * Returns: true on success, false otherwise.
 */
bool FBOManager::SetupFBO(const Texture& texture) {
	
	// TODO: Make sure rectangle textures are supported and make sure the provided texture is rectangular!!!

	// Check for multisampling...
	if (this->allowMultisampling && FBOManager::IsFBOMultisamplingEnabled(this->numSupportedSamples)) {
		this->SetupFBOWithMultisampling(texture);
	}
	else {
		this->SetupFBOWithoutMultisampling(texture);
	}

	// Verify that the FBO dimensions are of the expected size.  
	int actualWidth = 0;
  int actualHeight = 0;
  glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT,  &actualWidth);
  glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_HEIGHT_EXT, &actualHeight);
  
	if (actualWidth != texture.GetWidth() || actualHeight != texture.GetHeight()) {
		debug_output("FBO Dimensions do not match that of the their render target texture!");
		assert(false);
		FBOManager::CheckFBOStatus();
		return false;
	}

	// Make sure everything FBO is good to go
	return FBOManager::CheckFBOStatus();
}

/**
 * Public function for unbinding the FBO and in the case of multisampling, this function
 * will copy the multisampled rendering into the bound, off-screen texture.
 */
void FBOManager::UnbindFBO(const Texture& texture) const {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	if (this->numSupportedSamples > 0 && this->allowMultisampling) {
		// In this case we are dealing with multisampling FBO: copy the pixel 
		// data in the multisample colour render buffer image to the FBO 
		// containing the offscreen texture image.

		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, this->multisampleFBOID);
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, this->fboID);
		glBlitFramebufferEXT(0, 0, texture.GetWidth(), texture.GetHeight(), 0, 0, texture.GetWidth(), texture.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	debug_opengl_state();
}

/**
 * Private helper function that checks the status of the frame buffer object and reports any errors.
 * Returns: true on successful status, false if badness.
 */
bool FBOManager::CheckFBOStatus() {
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

/**
 * Private helper function for checking to see if multisampling is enabled for FBO.
 * Returns: true on supported, false otherwise.
 */
bool FBOManager::IsFBOMultisamplingEnabled(int &maxFBOSamples) {
	if (glewGetExtension("GL_EXT_framebuffer_multisample") == GL_TRUE) {
		if (glewGetExtension("GL_EXT_framebuffer_blit") == GL_TRUE) {
			glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxFBOSamples);
			if (maxFBOSamples > 0) {
				maxFBOSamples = 4;
				return true;
			}
		}
	}
	return false;
}