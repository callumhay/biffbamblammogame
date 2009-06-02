#include "ResourceManager.h"

#include "BlammoEngine/ObjReader.h"
#include "BlammoEngine/MtlReader.h"
#include "BlammoEngine/CgFxEffect.h"
#include "BlammoEngine/GeometryMaker.h"
#include "BlammoEngine/Mesh.h"
#include "BlammoEngine/Texture1D.h"
#include "BlammoEngine/Texture2D.h"
#include "BlammoEngine/TextureFontSet.h"

#include "GameModel/LevelPiece.h"

#include "GameView/CgFxInkBlock.h"
#include "GameView/GameViewConstants.h"

ResourceManager* ResourceManager::instance = NULL;

ResourceManager::ResourceManager(const std::string& resourceZip, const char* argv0) : 
cgContext(NULL), inkBlockMesh(NULL) {
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

	debug_output("PHYSFS Base Path: " << PHYSFS_getBaseDir());
}

ResourceManager::~ResourceManager() {
	// Clean up Physfs
	PHYSFS_deinit();

	// Clean up all loaded meshes - these must be deleted first so that the
	// effects go with them and make the assertions below correct
	for (std::map<std::string, Mesh*>::iterator iter = this->loadedMeshes.begin(); iter != this->loadedMeshes.end(); iter++) {
		delete iter->second;
		iter->second = NULL;
	}
	this->loadedMeshes.clear();

	if (this->inkBlockMesh != NULL) {
		delete this->inkBlockMesh;
		this->inkBlockMesh = NULL;
	}

	// Clean up all loaded effects - technically we shouldn't have to do this since
	// whoever is using the resource should have released it by now
	assert(this->numRefPerEffect.size() == 0);
	assert(this->loadedEffects.size() == 0);
	for (std::map<std::string, CGeffect>::iterator iter = this->loadedEffects.begin(); iter != this->loadedEffects.end(); iter++) {
		cgDestroyEffect(iter->second);
		iter->second = NULL;
	}
	this->loadedEffects.clear();
	this->numRefPerEffect.clear();
	this->loadedEffectTechniques.clear();

	// Destroy the cg context
	cgDestroyContext(this->cgContext);
	this->cgContext = NULL;

	// Clean up all loaded textures
	assert(this->numRefPerTexture.size() == 0);
	assert(this->loadedTextures.size() == 0);
	for (std::map<std::string, Texture*>::iterator iter = this->loadedTextures.begin(); iter != this->loadedTextures.end(); iter++) {
		delete iter->second;
		iter->second = NULL;
	}
	this->loadedTextures.clear();
	this->numRefPerTexture.clear();
}

/**
 * Private helper function for initializing the Cg context.
 */
void ResourceManager::InitCgContext() {
	if (this->cgContext != NULL) {
		return;
	}

	// Load the cg context and check for error
	this->cgContext = cgCreateContext();
	debug_cg_state();

	// Register OGL states for the context and allow texture mgmt
	cgGLRegisterStates(this->cgContext);
	cgGLSetManageTextureParameters(this->cgContext, true);
	debug_cg_state();
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
 * Obtain the mesh resource for the ink block, load it into memory
 * if it hasn't been loaded already.
 * Returns: Mesh resource for the game's ink block.
 */
Mesh* ResourceManager::GetInkBlockMeshResource() {
	// If it exists, return a pointer to it
	if (this->inkBlockMesh != NULL) {
		return this->inkBlockMesh;
	}

	// Otherwise, we load it into memory:
	// Create the geometry (eliptical pill shape) using the ball geometry
	Mesh* ballMesh = this->GetObjMeshResource(GameViewConstants::GetInstance()->BALL_MESH);
	PolygonGroup* inkBlockPolyGrp = new PolygonGroup(*ballMesh->GetMaterialGroups().begin()->second->GetPolygonGroup());
	Matrix4x4 scaleMatrix = Matrix4x4::scaleMatrix(Vector3D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT, 1.0f));
	inkBlockPolyGrp->Transform(scaleMatrix);
	assert(inkBlockPolyGrp != NULL);

	// Create the material properties and effect (ink block cgfx shader - makes the ink block all wiggly and stuff)
	MaterialProperties* inkMatProps = new MaterialProperties();
	inkMatProps->materialType	= MaterialProperties::MATERIAL_INKBLOCK_TYPE;
	inkMatProps->geomType			= MaterialProperties::MATERIAL_GEOM_FG_TYPE;
	inkMatProps->diffuse			= Colour(0.111f, 0.137f, 0.289f);
	inkMatProps->specular			= Colour(0.33f, 0.33f, 0.33f);
	inkMatProps->shininess		= 95.0f;
	CgFxInkBlock* inkBlockEffect = new CgFxInkBlock(inkMatProps);

	// Create the material group and its geometry (as defined above)
	MaterialGroup* inkMatGrp = new MaterialGroup(inkBlockEffect);
	inkMatGrp->SetPolygonGroup(inkBlockPolyGrp);

	std::map<std::string, MaterialGroup*> inkBlockMatGrps;
	inkBlockMatGrps.insert(std::make_pair("Ink Material", inkMatGrp));

	// Insert the material and its geometry it into the mesh
	this->inkBlockMesh = new Mesh("Ink Block", inkBlockMatGrps);
	return this->inkBlockMesh;
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
				break;

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

/**
 * Obtain a font texture resource from the physfs file system.
 * Returns: The font set on successful load, false otherwise.
 */
std::map<unsigned int, TextureFontSet*> ResourceManager::LoadFont(const std::string &filepath, const std::vector<unsigned int>& heights) {
	std::map<unsigned int, TextureFontSet*> fontSets;
	
	// First make sure the file exists in the archive
	int doesExist = PHYSFS_exists(filepath.c_str());
	if (doesExist == NULL) {
		debug_output("Texture file not found: " << filepath);
		debug_physfs_state(NULL);
		return fontSets;
	}

	// Open the file for reading
	PHYSFS_File* fileHandle = PHYSFS_openRead(filepath.c_str());
	if (fileHandle == NULL) {
		assert(false);
		return fontSets;
	}

	// Load the font sets using physfs file handle
	fontSets = TextureFontSet::CreateTextureFontFromTTF(fileHandle, heights);

	// Clean-up the file handle
	int closeWentWell = PHYSFS_close(fileHandle);
	debug_physfs_state(closeWentWell);
	fileHandle = NULL;

	return fontSets;
}

/**
 * Get the given cgfx effect file as a cgeffect and load it into the resource
 * manager. If the effect file has already been loaded the effect will simply be
 * given.
 * Returns: The CGeffect associated with the given effect file on successful load, NULL otherwise.
 */
void ResourceManager::GetCgFxEffectResource(const std::string& filepath, CGeffect &effect, std::map<std::string, CGtechnique>& techniques) {
	// Make sure the cg context has been initialized
	this->InitCgContext();
	effect = NULL;

	// Try to find the effect in the loaded effects first...
	std::map<std::string, CGeffect>::iterator loadedEffectIter = this->loadedEffects.find(filepath);
	bool needToReadFromFile = loadedEffectIter == this->loadedEffects.end();

	if (needToReadFromFile) {

		// First make sure the file exists in the archive
		int doesExist = PHYSFS_exists(filepath.c_str());
		if (doesExist == NULL) {
			debug_output("Effect file not found: " << filepath);
			debug_physfs_state(NULL);
			return;
		}

		// Open the file for reading
		PHYSFS_File* fileHandle = PHYSFS_openRead(filepath.c_str());
		if (fileHandle == NULL) {
			assert(false);
			return;
		}

		// Grab all the data out of the file so it can be used to compile the cgfx effect:
		PHYSFS_sint64 fileLength = PHYSFS_fileLength(fileHandle);
		char* fileBuffer = new char[fileLength+1];
	
		int readResult = PHYSFS_read(fileHandle, fileBuffer, sizeof(char), fileLength);
		if (readResult == NULL) {
			delete[] fileBuffer;
			fileBuffer = NULL;
			debug_output("Error reading obj file to bytes: " << filepath);
			assert(false);
			return;
		}
		fileBuffer[fileLength] = '\0';

		// Load the effect file using physfs and create the effect using the Cg runtime
		effect = cgCreateEffect(this->cgContext, fileBuffer, NULL);
		debug_cg_state();

		// Clean-up the buffer and file handle
		delete[] fileBuffer;
		fileBuffer = NULL;
		int closeWentWell = PHYSFS_close(fileHandle);
		debug_physfs_state(closeWentWell);
		fileHandle = NULL;

		// Add the effect as a resource
		assert(effect != NULL);
		this->numRefPerEffect[effect] = 1;
		this->loadedEffects[filepath] = effect;

		// Load all the techniques for the effect as well
		this->LoadEffectTechniques(effect, techniques);
		this->loadedEffectTechniques[effect] = techniques;
	}
	else {
		// Load the effect
		effect = loadedEffectIter->second;
		assert(effect != NULL);

		// Make sure techniques exist for the effect as well
		std::map<CGeffect, std::map<std::string, CGtechnique>>::iterator techniqueIter = this->loadedEffectTechniques.find(effect);
		assert(techniqueIter != this->loadedEffectTechniques.end());
		techniques = techniqueIter->second;
		assert(techniques.size() > 0);
		assert(this->numRefPerEffect.find(effect) != this->numRefPerEffect.end());

		// Increment the number of references to the effect
		this->numRefPerEffect[effect]++;
	}
}

/**
 * Private helper function that loads the techniques for the given effect into the given
 * map of techniques - these are hashed using their name as the key.
 * Returns: Mapping of techniques for the given effect on success, empty map otherwise.
 */
void ResourceManager::LoadEffectTechniques(const CGeffect effect, std::map<std::string, CGtechnique> &techniques) {
	assert(effect != NULL);

	// Obtain all the techniques associated with this effect
	CGtechnique currTechnique = cgGetFirstTechnique(effect);
	while (currTechnique) {
		const char* techniqueName = cgGetTechniqueName(currTechnique);
		if (cgValidateTechnique(currTechnique) == CG_FALSE) {
			debug_output("Could not validate Cg technique " << techniqueName);
			assert(false);
			currTechnique = cgGetNextTechnique(currTechnique);
			continue;
		}
		techniques[std::string(techniqueName)] = currTechnique;
		currTechnique = cgGetNextTechnique(currTechnique);
	}

	assert(techniques.size() != 0);
	debug_cg_state();	
}

/**
 * Release an effect resource and its techniques from the manager if 
 * the number of references has reached zero.
 * Return: true on successful release and/or decrement of references, false on error.
 */
bool ResourceManager::ReleaseCgFxEffectResource(CGeffect &effect) {
	assert(effect != NULL);

	// Find the effect resource
	std::map<std::string, CGeffect>::iterator effectResourceIter = this->loadedEffects.end();
	for (std::map<std::string, CGeffect>::iterator iter = this->loadedEffects.begin(); iter != this->loadedEffects.end(); iter++) {
		if (iter->second == effect) {
			effectResourceIter = iter;
		}
	}

	if (effectResourceIter == this->loadedEffects.end()) {
		return false;
	}

	// Check the number of references if we have reached the last reference then we delete the effect
	std::map<CGeffect, unsigned int>::iterator numRefIter = this->numRefPerEffect.find(effect);
	assert(numRefIter != this->numRefPerEffect.end());
	this->numRefPerEffect[effect]--;

	if (this->numRefPerEffect[effect] == 0) {
		// No more references: delete the effect resource and its associated techniques
		CGeffect effectResource = effectResourceIter->second;
		assert(effectResource != NULL);

		std::map<CGeffect, std::map<std::string, CGtechnique>>::iterator techniquesIter = this->loadedEffectTechniques.find(effectResource);
		assert(techniquesIter != this->loadedEffectTechniques.end());
		std::map<std::string, CGtechnique>& techniques = techniquesIter->second;
		assert(techniques.size() > 0);

		cgDestroyEffect(effectResource);
		debug_cg_state();

		this->loadedEffects.erase(effectResourceIter);
		this->loadedEffectTechniques.erase(techniquesIter);
		this->numRefPerEffect.erase(numRefIter);	
	}
	
	effect = NULL;
	return true;
}

std::istringstream* ResourceManager::FilepathToInStream(const std::string &filepath) {
	std::istringstream* inFile = NULL;

	// First make sure the file exists in the archive
	int doesExist = PHYSFS_exists(filepath.c_str());
	if (doesExist == NULL) {
		debug_output("File not found: " << filepath);
		debug_physfs_state(NULL);
		return inFile;
	}

	// Open the file for reading
	PHYSFS_File* fileHandle = PHYSFS_openRead(filepath.c_str());
	if (fileHandle == NULL) {
		assert(false);
		return inFile;
	}

	// Get a byte buffer of the entire file and convert it into a string stream so it
	// can be read in using the STL
	PHYSFS_sint64 fileLength = PHYSFS_fileLength(fileHandle);
	char* fileBuffer = new char[fileLength+1];
	
	int readResult = PHYSFS_read(fileHandle, fileBuffer, sizeof(char), fileLength);
	if (readResult == NULL) {
		delete[] fileBuffer;
		fileBuffer = NULL;
		debug_output("Error reading file to bytes: " << filepath);
		assert(false);
		return inFile;
	}
	fileBuffer[fileLength] = '\0';

	// Convert the bytes to a string stream 
	inFile = new std::istringstream(std::string(fileBuffer), std::ios_base::in | std::ios_base::binary);
	
	// Clean up
	delete[] fileBuffer;
	fileBuffer = NULL;
	int closeWentWell = PHYSFS_close(fileHandle);
	debug_physfs_state(closeWentWell);
	fileHandle = NULL;

	return inFile;
}
