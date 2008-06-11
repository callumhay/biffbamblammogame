#include "CgShaderManager.h"

#include "CgFxCelShading.h"
#include "CgFxOutlinedPhong.h"

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
	// Destroy each of the cg effects
	std::map<std::string, CGeffect>::iterator cgEffectIter;
	for (cgEffectIter = this->cgEffects.begin(); cgEffectIter != this->cgEffects.end(); cgEffectIter++) {
		cgDestroyEffect(cgEffectIter->second);
		this->CheckForCgError("Destroying cg effect");
	}
	this->cgEffects.clear();

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
	
	// Add the effect to the manager
	assert(this->cgEffects.find(cgfxFilepath) == this->cgEffects.end());
	this->cgEffects[cgfxFilepath] = temp;

	return temp;
}

/**
 * Obtain the techniques for a given CGeffect.
 * Returns: A map of techniques hashed by their name.
 */
std::map<std::string, CGtechnique> CgShaderManager::LoadTechniques(CGeffect effect) {
	std::map<std::string, CGtechnique> techniques;

	// Obtain all the techniques associated with the given effect
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
	this->CheckForCgError("Loading techniques");

	return techniques;
}