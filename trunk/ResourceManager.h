/**
 * ResourceManager.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "BlammoEngine/BasicIncludes.h"
#include "BlammoEngine/Texture.h"

class ConfigOptions;
class Mesh;
class CgFxMaterialEffect;
class TextureFontSet;
class Blammopedia;

/**
 * This class is important for the quick loading of all resources relevant to BiffBlamBlammo
 * and its engine. This class is a singleton that can both load resources and manage them.
 * An important distinction is that between loading (which is a pure operation that does not track
 * any resources and expects the caller to manage what is returned (these are public, static functions) 
 * and resource 'getting' which track the number of instances lent out and recovered - the manager handles
 * all memory creation and destruction for these.
 */
class ResourceManager {
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

	// Resource Management and loading functions *******************************************************************************
	bool LoadBlammopedia(const std::string& blammopediaFile);	
	Blammopedia* GetBlammopedia() const;

	// Mesh Resource Functions
	Mesh* GetObjMeshResource(const std::string &filepath);
	Mesh* GetInkBlockMeshResource();
	Mesh* GetPortalBlockMeshResource();
	std::map<std::string, CgFxMaterialEffect*> GetMtlMeshResource(const std::string &filepath);
	bool ReleaseMeshResource(Mesh* mesh);

	// Texture Resource Functions
	Texture* GetImgTextureResource(const std::string &filepath, Texture::TextureFilterType filter, GLenum textureType = GL_TEXTURE_2D);
	bool ReleaseTextureResource(Texture* texture);
	Texture* GetCelShadingTexture();
	GLubyte* ReadNoiseOctave3DTextureData();
	
	// Effects Resource Functions
	void GetCgFxEffectResource(const std::string &filepath, CGeffect &effect, std::map<std::string, CGtechnique> &techniques);
	bool ReleaseCgFxEffectResource(CGeffect &effect);

	// Initialization configuration loading
    static void SetLoadDir(const char* loadDir);
    static inline const std::string& GetLoadDir() { return baseLoadDir; }

	static ConfigOptions ReadConfigurationOptions(bool forceReadFromFile);
	static bool WriteConfigurationOptionsToFile(const ConfigOptions& cfgOptions);

	// Basic loading functions ****************************************************************************************************
	static std::map<unsigned int, TextureFontSet*> LoadFont(const std::string &filepath, const std::vector<unsigned int> &heights, Texture::TextureFilterType filterType);
	static std::istringstream* FilepathToInStream(const std::string &filepath);
    static std::stringstream* FilepathToInOutStream(const std::string &filepath);
	static char* FilepathToMemoryBuffer(const std::string &filepath, long &length);

	// Public Resource Directories
	static std::string GetTextureResourceDir();
	static std::string GetBlammopediaResourceDir();

private:
	static ResourceManager* instance;

	ResourceManager(const std::string& resourceZip, const char* argv0);
	~ResourceManager();

	static const char* RESOURCE_DIRECTORY;
	static const char* TEXTURE_DIRECTORY;
	static const char* BLAMMOPEDIA_DIRECTORY;
	static const char* MOD_DIRECTORY;

    static std::string baseLoadDir;

	std::map<std::string, Mesh*> loadedMeshes;	// Meshes already loaded into the blammo engine from file
    std::map<Mesh*, unsigned int> numRefPerMesh; // Number of references per mesh handed out

	// Independant meshes for specific special-material things in the game
	Mesh* inkBlockMesh;
	Mesh* portalBlockMesh;

	std::map<std::string, Texture*> loadedTextures;			// Textures already loaded into the blammo engine
	Texture* celShadingTexture;													// Cel-shading 1D texture
	std::map<Texture*, unsigned int> numRefPerTexture;	// Number of references per texture handed out

	// Cg Runtime objects and helper functions
	CGcontext cgContext;
	std::map<std::string, CGeffect> loadedEffects;																	// Effects already loaded into the blammo engine
	std::map<CGeffect, std::map<std::string, CGtechnique> > loadedEffectTechniques;	// Techniques associated with each effect
	std::map<CGeffect, unsigned int> numRefPerEffect;																// Number of references per effect

	// Blammopedia - where all information and textures for items, blocks, etc. are archived for easy lookup
	Blammopedia* blammopedia;

	static ConfigOptions* configOptions;	// The configuration options read from the game's ini file

	void InitCgContext();
	static void LoadEffectTechniques(const CGeffect effect, std::map<std::string, CGtechnique>& techniques);

	static std::string ConvertResourceFilepathToModFilepath(const std::string& resourceFilepath);

	//static std::string GetAsModDirectoryPath(const std::string& resourceFilepath);
	//static std::string GetAsResourceDirectoryPath(const std::string& resourceFilepath);
};

inline Blammopedia* ResourceManager::GetBlammopedia() const {
	return this->blammopedia;
}

inline std::string ResourceManager::GetTextureResourceDir() {
	return std::string(ResourceManager::RESOURCE_DIRECTORY) + std::string("/") + std::string(ResourceManager::TEXTURE_DIRECTORY) + std::string("/");
}

inline std::string ResourceManager::GetBlammopediaResourceDir() {
	return std::string(ResourceManager::RESOURCE_DIRECTORY) + std::string("/") + std::string(ResourceManager::BLAMMOPEDIA_DIRECTORY) + std::string("/");
}

#endif