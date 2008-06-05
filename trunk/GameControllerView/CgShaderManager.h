#ifndef __CGSHADERMANAGER_H__
#define __CGSHADERMANAGER_H__

#include "../Utils/Includes.h"

#include <string>
#include <map>

/**
 * Singleton class for dealing with Cg shader runtime
 * classes, pointers, etc.
 */
class CgShaderManager {

private:
	static CgShaderManager* instance;
	
	// Cg Runtime objects
	CGcontext cgContext;
	std::map<std::string, CGeffect> cgEffects;	// Hash of all CgEffects, hashed by their filepath

	CgShaderManager();

public:
	virtual ~CgShaderManager();

	// Creation and deletion of the singleton
	static CgShaderManager* Instance();
	static void DeleteInstance();

	void CheckForCgError(std::string situation);

	CGeffect LoadEffectFromCgFxFile(const std::string& cgfxFilepath);
	std::map<std::string, CGtechnique> LoadTechniques(CGeffect effect);
	//CGeffect GetEffect(const std::string& cgfxFilepath);
	void InitCgFx();
};

#endif