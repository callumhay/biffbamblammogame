#include "ResourceManager.h"
#include "ObjReader.h"
#include "MtlReader.h"
#include "CgFxEffect.h"

#include "Mesh.h"
#include "Texture1D.h"
#include "Texture2D.h"

ResourceManager* ResourceManager::instance = NULL;

ResourceManager::ResourceManager(const std::string& resourceZip, const char* argv0) {
	// Initialize DevIL and make sure it loaded correctly
	ilInit();
	iluInit();
	ILboolean ilResult = ilutRenderer(ILUT_OPENGL);
	assert(ilResult);
	ilResult = ilutEnable(ILUT_OPENGL_CONV);
	assert(ilResult);

	// Initialize Physfs and make sure everything loaded alright
	int result = PHYSFS_init(argv0);
	debug_physfs_state(result);

	result = PHYSFS_addToSearchPath(resourceZip.c_str(), 0);
	debug_physfs_state(result);

	debug_output("PHYSFS Base Path:    " << PHYSFS_getBaseDir());	
}

ResourceManager::~ResourceManager() {
	// Clean up Physfs
	PHYSFS_deinit();

	// Clean up all loaded meshes
	for (std::map<std::string, Mesh*>::iterator iter = this->loadedMeshes.begin(); iter != this->loadedMeshes.end(); iter++) {
		delete iter->second;
		iter->second = NULL;
	}
	this->loadedMeshes.clear();

	// Clean up all loaded textures
	for (std::map<std::string, Texture*>::iterator iter = this->loadedTextures.begin(); iter != this->loadedTextures.end(); iter++) {
		delete iter->second;
		iter->second = NULL;
	}
	this->loadedTextures.clear();

	assert(this->numRefPerTexture.size() == 0);
	this->numRefPerTexture.clear();

}

/**
 * Initialize the resource manager (must be called before obtaining an instance of the class)
 * so that it loads resources from the given zip file.
 * Also must be provided with the directory that the application is running in: argv[0].
 */
void ResourceManager::InitResourceManager(const std::string& resourceZip, const char* argv0) {
	assert(ResourceManager::instance == NULL);
	if (ResourceManager::instance == NULL) {
		ResourceManager::instance = new ResourceManager(resourceZip, argv0);
	}
}

/**
 * Get a .obj mesh resource from the zip archive loaded with this resource manager.
 * If the mesh has already been loaded then the resource manager will return
 * the already loaded mesh unless specified otherwise.
 * Returns: Mesh resource requested by the given filepath in the resource archive.
 */
Mesh* ResourceManager::GetObjMeshResource(const std::string &filepath) {
	// We only need to load from file if the mesh has not already been loaded into memory
	std::map<std::string, Mesh*>::iterator meshesLoadedIter = this->loadedMeshes.find(filepath);
	bool needToReadFromFile = meshesLoadedIter == this->loadedMeshes.end();
	Mesh* mesh = NULL;

	if (needToReadFromFile) {
		// First make sure the file exists in the archive
		int doesExist = PHYSFS_exists(filepath.c_str());
		if (doesExist == NULL) {
			debug_output("Mesh file not found: " << filepath);
			debug_physfs_state(NULL);
			return NULL;
		}

		// Open the file for reading
		PHYSFS_File* fileHandle = PHYSFS_openRead(filepath.c_str());
		if (fileHandle == NULL) {
			assert(false);
			return NULL;
		}

		// Read the file as an obj
		mesh = ObjReader::ReadMesh(filepath, fileHandle);
		assert(mesh != NULL);
		this->loadedMeshes[filepath] = mesh;

		// Clean-up
		int closeWentWell = PHYSFS_close(fileHandle);
		debug_physfs_state(closeWentWell);
		fileHandle = NULL;
	}
	else {
		// The mesh has already been loaded from file - all we have to do is either return it
		// or make a copy if the user requested it
		mesh = meshesLoadedIter->second;
		assert(mesh != NULL);
	}

	return mesh;
}

/**
 * Read a MTL file for a material and return a map of all the material effects
 * created from it.
 * Returns: mapping of material named effects, empty map otherwise.
 */
std::map<std::string, CgFxMaterialEffect*> ResourceManager::GetMtlMeshResource(const std::string &filepath) {
	std::map<std::string, CgFxMaterialEffect*> materials;

	// First make sure the file exists in the archive
	int doesExist = PHYSFS_exists(filepath.c_str());
	if (doesExist == NULL) {
		debug_output("Material file not found: " << filepath);
		debug_physfs_state(NULL);
		return materials;
	}

	// Open the file for reading
	PHYSFS_File* fileHandle = PHYSFS_openRead(filepath.c_str());
	if (fileHandle == NULL) {
		assert(false);
		return materials;
	}

	// Read in the MTL file
	materials = MtlReader::ReadMaterialFile(filepath, fileHandle);
	assert(materials.size() > 0);

	// Clean-up
	int closeWentWell = PHYSFS_close(fileHandle);
	debug_physfs_state(closeWentWell);
	fileHandle = NULL;

	return materials;
}

/**
 * Release a mesh resource with the given pointer.
 * Returns: true if the mesh was found and released, false otherwise.
 */
bool ResourceManager::ReleaseMeshResource(Mesh* mesh) {
	assert(mesh != NULL);

	// Find the mesh resource
	std::map<std::string, Mesh*>::iterator meshResourceIter = this->loadedMeshes.end();
	for (std::map<std::string, Mesh*>::iterator iter = this->loadedMeshes.begin(); iter != this->loadedMeshes.end(); iter++) {
		if (iter->second == mesh) {
			meshResourceIter = iter;
		}
	}

	if (meshResourceIter == this->loadedMeshes.end()) {
		return false;
	}

	// Release it
	Mesh* meshResource = meshResourceIter->second;
	assert(meshResource != NULL);
	delete meshResource;
	meshResource = NULL;

	this->loadedMeshes.erase(meshResourceIter);
	return true;
}

/**
 * Read a texture resource from archive file into memory - this function will check
 * to see if the texture has already been loaded and if it has will return that object.
 * Returns: Texture loaded in memory on success, NULL otherwise.
 */
Texture* ResourceManager::GetImgTextureResource(const std::string &filepath, Texture::TextureFilterType filter, GLenum textureType) {
	// Try to find the texture in the loaded textures first...
	std::map<std::string, Texture*>::iterator loadedTexIter = this->loadedTextures.find(filepath);
	bool needToReadFromFile = loadedTexIter == this->loadedTextures.end();
	Texture* texture = NULL;

	if (needToReadFromFile) {
		// First make sure the file exists in the archive
		int doesExist = PHYSFS_exists(filepath.c_str());
		if (doesExist == NULL) {
			debug_output("Texture file not found: " << filepath);
			debug_physfs_state(NULL);
			return NULL;
		}

		// Open the file for reading
		PHYSFS_File* fileHandle = PHYSFS_openRead(filepath.c_str());
		if (fileHandle == NULL) {
			assert(false);
			return NULL;
		}

		// Load the texture based on its type
		switch (textureType) {

			case GL_TEXTURE_1D:
				texture = Texture1D::CreateTexture1DFromImgFile(fileHandle, filter);

			case GL_TEXTURE_2D:
				texture = Texture2D::CreateTexture2DFromImgFile(fileHandle, filter);
				break;

			default:
				assert(false);
				return NULL;
		}
		
		// First reference to the texture...
		assert(texture != NULL);
		this->numRefPerTexture[texture] = 1;
		this->loadedTextures[filepath] = texture;

		// Clean-up the file handle
		int closeWentWell = PHYSFS_close(fileHandle);
		debug_physfs_state(closeWentWell);
		fileHandle = NULL;
	}
	else {
		// Read the texture out of memory and increment the number of references past out
		texture = loadedTexIter->second;
		assert(this->numRefPerTexture.find(texture) != this->numRefPerTexture.end());
		this->numRefPerTexture[texture]++;
	}

	return texture;
}

/**
 * Release a texture resource from the manager if the number of references has
 * reached zero.
 * Return: true on successful release and/or decrement of references, false on error.
 */
bool ResourceManager::ReleaseTextureResource(Texture* texture) {
	assert(texture != NULL);

	// Find the texture resource
	std::map<std::string, Texture*>::iterator texResourceIter = this->loadedTextures.end();
	for (std::map<std::string, Texture*>::iterator iter = this->loadedTextures.begin(); iter != this->loadedTextures.end(); iter++) {
		if (iter->second == texture) {
			texResourceIter = iter;
		}
	}

	if (texResourceIter == this->loadedTextures.end()) {
		return false;
	}

	// Check the number of references if we have reached the last reference then
	// we delete the texture
	std::map<Texture*, unsigned int>::iterator numRefIter = this->numRefPerTexture.find(texture);
	assert(numRefIter != this->numRefPerTexture.end());
	this->numRefPerTexture[texture]--;

	if (this->numRefPerTexture[texture] == 0) {
		Texture* texResource = texResourceIter->second;
		assert(texResource != NULL);
		delete texResource;
		texResource = NULL;

		this->loadedTextures.erase(texResourceIter);
		this->numRefPerTexture.erase(numRefIter);
	}
	
	texture = NULL;
	return true;
}