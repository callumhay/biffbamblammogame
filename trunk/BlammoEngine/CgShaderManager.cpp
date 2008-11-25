#include "CgShaderManager.h"

CgShaderManager* CgShaderManager::instance = NULL;

CgShaderManager::CgShaderManager() : cgContext(NULL) {
	// Load the cg context and check for error
	this->cgContext = cgCreateContext();
	this->CheckForCgError("Creating Cg context");

	// Register OGL states for the context and allow texture mgmt
	cgGLRegisterStates(this->cgContext);
	cgGLSetManageTextureParameters(this->cgContext, true);
	this->CheckForCgError("Registering OGL Cg Runtime");
}

CgShaderManager::~CgShaderManager() {
	// Destroy the cg context
	cgDestroyContext(this->cgContext);
	this->CheckForCgError("Destroying cg context");
}

/**
 * Used to obtain the one instance of the CgShaderManager.
 */
CgShaderManager* CgShaderManager::Instance() {
	if (instance == NULL) {
		instance = new CgShaderManager();
	}
	return instance;
}

/**
 * Used to delete/clean the CgShaderManager from memory.
 */
void CgShaderManager::DeleteInstance() {
	if (instance != NULL) {
		delete instance;
		instance = NULL;
	}
}

/**
 * Used to check for errors in the Cg runtime.
 * This will print off the error in debug mode and pose an assertion.
 */
void CgShaderManager::CheckForCgError(std::string situation) {
	CGerror error = CG_NO_ERROR;
	const char* errorStr = cgGetLastErrorString(&error);
	
	if (error != CG_NO_ERROR) {
		debug_output("Cg Error: " << situation << " - " << errorStr);
		assert(false);
	}
}

/**
 * Load a CGeffect object given a CgFx filepath.
 */
CGeffect CgShaderManager::LoadEffectFromCgFxFile(const std::string& cgfxFilepath) {
	// Load the effect using the Cg runtime
	CGeffect temp = cgCreateEffectFromFile(this->cgContext, cgfxFilepath.c_str(), NULL);
	this->CheckForCgError("Loading effect file");
	return temp;
}

/**
 * Loads the techniques for the given effect into the given
 * map of techniques - these are hashed using their name as the key.
 */
void CgShaderManager::LoadEffectTechniques(CGeffect effect, std::map<std::string, CGtechnique>& techniques) {
	assert(effect != NULL);

	// Obtain all the techniques associated with this effect
	CGtechnique currTechnique = cgGetFirstTechnique(effect);
	while (currTechnique) {
		const char* techniqueName = cgGetTechniqueName(currTechnique);
		if (cgValidateTechnique(currTechnique) == CG_FALSE) {
			debug_output("Could not validate Cg technique " << techniqueName);
			assert(false);
			continue;
		}
		techniques[std::string(techniqueName)] = currTechnique;
		currTechnique = cgGetNextTechnique(currTechnique);
	}

	assert(techniques.size() != 0);
	CgShaderManager::Instance()->CheckForCgError("Loading techniques");
}

void CgShaderManager::LoadEffectTechnique(CGeffect effect, CGtechnique& technique) {
	assert(effect != NULL);

	// Obtain all the techniques associated with this effect
	CGtechnique currTechnique = cgGetFirstTechnique(effect);
	if (currTechnique) {
		const char* techniqueName = cgGetTechniqueName(currTechnique);
		if (cgValidateTechnique(currTechnique) == CG_FALSE) {
			debug_output("Could not validate Cg technique " << techniqueName);
			assert(false);
		}
		else {
			technique = currTechnique;
		}
	}

	assert(technique != NULL);
	CgShaderManager::Instance()->CheckForCgError("Loading technique");
}