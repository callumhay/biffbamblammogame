#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "BasicIncludes.h"
#include "Texture.h"

class Mesh;
class CgFxMaterialEffect;

class ResourceManager {
private:
	static ResourceManager* instance;

	ResourceManager(const std::string& resourceZip, const char* argv0);
	~ResourceManager();

	std::map<std::string, Mesh*> loadedMeshes;			// Meshes already loaded into the blammo engine
	
	std::map<std::string, Texture*> loadedTextures;				// Textures already loaded into the blammo engine
	std::map<Texture*, unsigned int> numRefPerTexture;	// Number of references per texture handed out

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
	
	Mesh* GetObjMeshResource(const std::string &filepath);
	std::map<std::string, CgFxMaterialEffect*> GetMtlMeshResource(const std::string &filepath);
	bool ReleaseMeshResource(Mesh* mesh);

	Texture* GetImgTextureResource(const std::string &filepath, Texture::TextureFilterType filter, GLenum textureType = GL_TEXTURE_2D);
	bool ReleaseTextureResource(Texture* texture);
	

};

#endif