#ifndef __FBOMANAGER_H__
#define __FBOMANAGER_H__

#include "BasicIncludes.h"

#include "Texture.h"
#include "Texture2D.h"
#include "TextureCube.h"

class Camera;

/**
 * Singleton manager and wrapper for OpenGL's FrameBuffer Object.
 * The framebuffer can be bound, unbound, and generally obtained from
 * this object.
 */
class FBOManager {
private:
	static FBOManager* instance;	// Singleton instance for this class

	bool allowMultisampling;
	int numSupportedSamples;

	// Regular FBO and render buffer IDs
	GLuint fboID;
	GLuint depthBuffID;
	
	// Multisample FBO and render buffer IDs
	GLuint multisampleFBOID;
	GLuint colourBuffID;

	static bool CheckFBOStatus();
	static bool IsFBOMultisamplingEnabled(int &maxFBOSamples);
	
	void SetupFBOWithMultisampling(const Texture& texture);
	void SetupFBOWithoutMultisampling(const Texture& texture);

	FBOManager();
	~FBOManager();

public:

	static FBOManager* GetInstance() {
		if (FBOManager::instance == NULL) {
			FBOManager::instance = new FBOManager();
		}
		return FBOManager::instance;
	}

	static void DeleteInstance() {
		if (FBOManager::instance != NULL) {
			delete FBOManager::instance;
			FBOManager::instance = NULL;
		}
	}

	bool SetupFBO(const Texture& texture);

	void SetAllowMultisamplingFBO(bool allowMultisampling) {
		this->allowMultisampling = allowMultisampling;
	}
	bool GetAllowMultisamplingFBO() const {
		return this->allowMultisampling;
	}

	// Functions for binding/unbinding the FBO
	void BindFBO() const {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fboID);
	}
	void UnbindFBO(const Texture& texture) const;
};
#endif