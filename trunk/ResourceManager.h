#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "BlammoEngine/BasicIncludes.h"
#include "BlammoEngine/Texture.h"

class Mesh;
class CgFxMaterialEffect;
class TextureFontSet;

/**
 * This class is important for the quick loading of all resources relevant to BiffBlamBlammo
 * and its engine. This class is a singleton that can both load resources and manage them.
 * An important distinction is that between loading (which is a pure operation that does not track
 * any resources and expects the caller to manage what is returned (these are public, static functions) 
 * and resource 'getting' which track the number of instances lent out and recovered - the manager handles
 * all memory creation and destruction for these.
 */
class ResourceManager {

private:
	static ResourceManager* instance;

	ResourceManager(const std::string& resourceZip, const char* argv0);
	~ResourceManager();

	std::map<std::string, Mesh*> loadedMeshes;					// Meshes already loaded into the blammo engine from file

	std::map<std::string, Texture*> loadedTextures;			// Textures already loaded into the blammo engine
	std::map<Texture*, unsigned int> numRefPerTexture;	// Number of references per texture handed out

	// Cg Runtime objects and helper functions
	CGcontext cgContext;
	std::map<std::string, CGeffect> loadedEffects;																	// Effects already loaded into the blammo engine
	std::map<CGeffect, std::map<std::string, CGtechnique>> loadedEffectTechniques;	// Techniques associated with each effect
	std::map<CGeffect, unsigned int> numRefPerEffect;																// Number of references per effect

	void InitCgContext();
	static void LoadEffectTechniques(const CGeffect effect, std::map<std::string, CGtechnique>& techniques);

public:
	static ResourceManager* GetInstance() {
		if (ResourceManager::instance == NULL) {
			debug_output("ERROR: Did not properly initialize resource manager before querying instance.");
			assert(false);
			return NULL;
		}
		return ResourceManager::instance;
	}

	static void DeleteInstance() {
		if (ResourceManager::instance != NULL) {
			delete ResourceManager::instance;
			ResourceManager::instance = NULL;
		}
	}

	static void InitResourceManager(const std::string& resourceZip, const char* argv0);
	
	// Resource Managerment and loading functions *******************************************************************************
		
	// Mesh Resource Functions
	Mesh* GetObjMeshResource(const std::string &filepath);
	std::map<std::string, CgFxMaterialEffect*> GetMtlMeshResource(const std::string &filepath);
	bool ReleaseMeshResource(Mesh* mesh);

	// Texture Resource Functions
	Texture* GetImgTextureResource(const std::string &filepath, Texture::TextureFilterType filter, GLenum textureType = GL_TEXTURE_2D);
	bool ReleaseTextureResource(Texture* texture);
	
	// Effects Resource Functions
	void GetCgFxEffectResource(const std::string& filepath, CGeffect &effect, std::map<std::string, CGtechnique> &techniques);
	bool ReleaseCgFxEffectResource(CGeffect &effect);

	// Basic loading functions ****************************************************************************************************

	// Font loading functions
	static std::map<unsigned int, TextureFontSet*> LoadFont(const std::string &filepath, const std::vector<unsigned int> &heights);


	static std::istringstream* FilepathToInStream(const std::string &filepath);

};

#endif