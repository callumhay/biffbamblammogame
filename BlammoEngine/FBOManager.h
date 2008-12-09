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

	// Regular FBO and render buffer IDs
	GLuint fboID;
	GLuint renderBuffID;
	
	// Multisample FBO and render buffer IDs
	//GLuint multisampleFBOID;
	//GLuint multisampleRenderBuffID;
	//GLuint multisampleColourBuffID;

	bool CheckFBOStatus();

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

	bool SetupFBO(Texture& texture);
	//bool SetupMultisampleFBO(int numSamples);

	// Functions for binding/unbinding the FBO
	void BindFBO() const {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fboID);
	}
	void UnbindFBO() const {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

};
#endif