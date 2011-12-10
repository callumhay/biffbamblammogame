/**
 * ResourceManager.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ResourceManager.h"
#include "ConfigOptions.h"
#include "Blammopedia.h"

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
#include "GameView/CgFxPortalBlock.h"
#include "GameView/CgFxStickyPaddle.h"
#include "GameView/GameViewConstants.h"

ResourceManager* ResourceManager::instance = NULL;
ConfigOptions* ResourceManager::configOptions = NULL;

const char* ResourceManager::RESOURCE_DIRECTORY			= "resources";
const char* ResourceManager::TEXTURE_DIRECTORY			= "textures";
const char* ResourceManager::BLAMMOPEDIA_DIRECTORY	= "blammopedia";

// The modifications resource directory - this takes loading presedant over the 
// resource zip file when in debug mode, otherwise it is ignored.
const char* ResourceManager::MOD_DIRECTORY = "mod";

std::string ResourceManager::baseLoadDir;

ResourceManager::ResourceManager(const std::string& resourceZip, const char* argv0) : 
cgContext(NULL), inkBlockMesh(NULL), portalBlockMesh(NULL), celShadingTexture(NULL), blammopedia(NULL) {
	// Initialize DevIL and make sure it loaded correctly
	ilInit();
	iluInit();
	ILboolean ilResult = ilutRenderer(ILUT_OPENGL);
	assert(ilResult);
	ilResult = ilutEnable(ILUT_OPENGL_CONV);
	assert(ilResult);

	//ilEnable(IL_ORIGIN_SET);
	//ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	// Initialize OpenAL for audio, make sure everything loaded alright
//	alutInit(NULL, NULL);
//	debug_output("Supported OpenAL Sound MIME types:");
//	
//#ifdef _DEBUG
//	const char* mimeTypes = alutGetMIMETypes(ALUT_LOADER_BUFFER);
//	if (mimeTypes != NULL) {
//		debug_output(mimeTypes);
//	}
//#endif
//	debug_openal_state();

	// Initialize Physfs and make sure everything loaded alright
	int result = PHYSFS_init(argv0);
	debug_physfs_state(result);
    //result = PHYSFS_mount(resourceZip.c_str(), NULL, 0);
	result = PHYSFS_addToSearchPath(resourceZip.c_str(), 0);
	debug_physfs_state(result);

	debug_output("PHYSFS Base Path: " << PHYSFS_getBaseDir());
}

ResourceManager::~ResourceManager() {
	// Clean up blammopedia - this should always be done first since it depends
	// on physfs and various textures / resources
	if (this->blammopedia != NULL) {
        bool success = this->blammopedia->WriteAsEntryStatusFile();
        UNUSED_VARIABLE(success);
        assert(success);
		delete this->blammopedia;
		this->blammopedia = NULL;
	}

	// Clean up Physfs
	PHYSFS_deinit();

	// Clean up sound memory
	for (std::map<std::string, Mix_Chunk*>::iterator iter = this->loadedEventSounds.begin(); iter != this->loadedEventSounds.end(); ++iter) {
		Mix_Chunk* chunk = iter->second;
		Mix_FreeChunk(chunk);
		chunk = NULL;
	}
	this->loadedEventSounds.clear();

	for (std::map<std::string, Mix_Music*>::iterator iter = this->loadedMusicSounds.begin(); iter != this->loadedMusicSounds.end(); ++iter) {
		Mix_Music* music = iter->second;
		Mix_FreeMusic(music);
		music = NULL;
	}
	this->loadedMusicSounds.clear();

	for (std::map<Mix_Music*, SDL_RWops*>::iterator iter = this->musicSDLMemory.begin(); iter != this->musicSDLMemory.end(); ++iter) {
		SDL_RWops* rw = iter->second;
		SDL_FreeRW(rw);
		rw = NULL;
	}
	this->musicSDLMemory.clear();

	for (std::map<Mix_Music*, char*>::iterator iter = this->musicBuffers.begin(); iter != this->musicBuffers.end(); ++iter) {
		char* buffer = iter->second;
		delete[] buffer;
		buffer = NULL;
	}
	this->musicBuffers.clear();

	// Clean up all loaded meshes - these must be deleted first so that the
	// effects go with them and make the assertions below correct
    assert(numRefPerMesh.empty());
	for (std::map<std::string, Mesh*>::iterator iter = this->loadedMeshes.begin(); iter != this->loadedMeshes.end(); ++iter) {
		delete iter->second;
		iter->second = NULL;
	}
	this->loadedMeshes.clear();

	if (this->inkBlockMesh != NULL) {
		delete this->inkBlockMesh;
		this->inkBlockMesh = NULL;
	}
	if (this->portalBlockMesh != NULL) {
		delete this->portalBlockMesh;
		this->portalBlockMesh = NULL;
	}

	// Clean up all loaded effects - technically we shouldn't have to do this since
	// whoever is using the resource should have released it by now
	assert(this->numRefPerEffect.size() == 0);
	assert(this->loadedEffects.size() == 0);
	for (std::map<std::string, CGeffect>::iterator iter = this->loadedEffects.begin(); iter != this->loadedEffects.end(); ++iter) {
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
	assert(this->numRefPerTexture.empty());
	assert(this->loadedTextures.empty());
	for (std::map<std::string, Texture*>::iterator iter = this->loadedTextures.begin(); iter != this->loadedTextures.end(); ++iter) {
		delete iter->second;
		iter->second = NULL;
	}
	this->loadedTextures.clear();
	this->numRefPerTexture.clear();

	// Clean up the cel-texture
	if (this->celShadingTexture != NULL) {
		delete this->celShadingTexture;
		this->celShadingTexture = NULL;
	}

	// Clean up configuration options
	if (ResourceManager::configOptions != NULL) {
		delete ResourceManager::configOptions;
		ResourceManager::configOptions = NULL;
	}
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
	cgGLSetManageTextureParameters(this->cgContext, CG_TRUE);
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

bool ResourceManager::LoadBlammopedia(const std::string& blammopediaFile) {
	if (this->blammopedia != NULL) {
		delete this->blammopedia;
	}

	// Initialize blammopedia
	this->blammopedia = Blammopedia::BuildFromBlammopediaFile(blammopediaFile);
	if (this->blammopedia == NULL) {
		assert(false);
		debug_output("Could not load blammopedia!!!");
		return false;
	}
	return true;
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
	assert(ballMesh != NULL);

	PolygonGroup* inkBlockPolyGrp = new PolygonGroup(*ballMesh->GetMaterialGroups().begin()->second->GetPolygonGroup());
    
    bool success = this->ReleaseMeshResource(ballMesh);
    assert(success);
    UNUSED_VARIABLE(success);

	Matrix4x4 scaleMatrix = Matrix4x4::scaleMatrix(Vector3D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT, 1.0f));
	inkBlockPolyGrp->Transform(scaleMatrix);
	assert(inkBlockPolyGrp != NULL);

	// Create the material properties and effect (ink block cgfx shader - makes the ink block all wiggly and stuff)
	MaterialProperties* inkMatProps = new MaterialProperties();
	inkMatProps->materialType	= MaterialProperties::MATERIAL_INKBLOCK_TYPE;
	inkMatProps->geomType			= MaterialProperties::MATERIAL_GEOM_FG_TYPE;
	inkMatProps->diffuse			= GameViewConstants::GetInstance()->INK_BLOCK_COLOUR;
	inkMatProps->specular			= Colour(0.44f, 0.44f, 0.44f);
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
 * Obtain the mesh resource for the portal block, load it into memory if it
 * hasn't already been created yet.
 */
Mesh* ResourceManager::GetPortalBlockMeshResource() {
	// If we've already loaded it, return it
	if (this->portalBlockMesh != NULL) {
		return this->portalBlockMesh;
	}

	// Otherwise, load it...
	// Use the typical level block mesh but modify its material
	Mesh* levelMesh = this->GetObjMeshResource(GameViewConstants::GetInstance()->BASIC_BLOCK_MESH_PATH);
	assert(levelMesh != NULL);
	PolygonGroup* portalBlockPolyGrp = new PolygonGroup(*levelMesh->GetMaterialGroups().begin()->second->GetPolygonGroup());
    
    bool success = this->ReleaseMeshResource(levelMesh);
    assert(success);
    UNUSED_VARIABLE(success);

	// Create the material properties and effect (portal block cgfx shader)
	MaterialProperties* portalMatProps = new MaterialProperties();
	portalMatProps->materialType	= MaterialProperties::MATERIAL_PORTAL_TYPE;
	portalMatProps->geomType			= MaterialProperties::MATERIAL_GEOM_FG_TYPE;
	//portalMatProps->diffuse			= GameViewConstants::GetInstance()->INK_BLOCK_COLOUR;
	//portalMatProps->specular			= Colour(0.44f, 0.44f, 0.44f);
	//portalMatProps->shininess		= 95.0f;
	CgFxPortalBlock* portalBlockEffect = new CgFxPortalBlock(portalMatProps);
	
	// Create the material group and its geometry (as defined above)
	MaterialGroup* portalMatGrp = new MaterialGroup(portalBlockEffect);
	portalMatGrp->SetPolygonGroup(portalBlockPolyGrp);

	std::map<std::string, MaterialGroup*> portalBlockMatGrps;
	portalBlockMatGrps.insert(std::make_pair("Portal Material", portalMatGrp));

	// Insert the material and its geometry it into the mesh
	this->portalBlockMesh = new Mesh("Portal Block", portalBlockMatGrps);
	return this->portalBlockMesh;
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
		std::istringstream* iStrStream = this->FilepathToInStream(filepath);
		if (iStrStream == NULL) {
			debug_output("Mesh file not found: " << filepath);
			return NULL;
		}
		mesh = ObjReader::ReadMeshFromStream(filepath, *iStrStream);
		assert(mesh != NULL);

		// First reference to the mesh...
		this->numRefPerMesh[mesh] = 1;
		this->loadedMeshes[filepath] = mesh;

		// Clean-up the stream
		delete iStrStream;
		iStrStream = NULL;
	}
	else {
		// The mesh has already been loaded from file - all we have to do is either return it
		// or make a copy if the user requested it
		mesh = meshesLoadedIter->second;
		assert(mesh != NULL);

		assert(this->numRefPerMesh.find(mesh) != this->numRefPerMesh.end());
		this->numRefPerMesh[mesh]++;
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

	std::istringstream* iStrStream = this->FilepathToInStream(filepath);
	if (iStrStream == NULL) {
		debug_output("Material file not found: " << filepath);
		return materials;
	}

	// Read in the MTL file
	materials = MtlReader::ReadMaterialFileFromStream(filepath, *iStrStream);
	assert(materials.size() > 0);

	// Clean-up the stream
	delete iStrStream;
	iStrStream = NULL;

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
	for (std::map<std::string, Mesh*>::iterator iter = this->loadedMeshes.begin(); iter != this->loadedMeshes.end(); ++iter) {
		if (iter->second == mesh) {
			meshResourceIter = iter;
		}
	}

	if (meshResourceIter == this->loadedMeshes.end()) {
		return false;
	}

	// Check the number of references if we have reached the last reference then
	// we delete the mesh
	std::map<Mesh*, unsigned int>::iterator numRefIter = this->numRefPerMesh.find(mesh);
	assert(numRefIter != this->numRefPerMesh.end());
	this->numRefPerMesh[mesh]--;

	if (this->numRefPerMesh[mesh] == 0) {
	    // Release it
	    Mesh* meshResource = meshResourceIter->second;
	    assert(meshResource != NULL);
	    delete meshResource;
	    meshResource = NULL;
	    this->loadedMeshes.erase(meshResourceIter);
		this->numRefPerMesh.erase(numRefIter);
	}
	
	mesh = NULL;
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
		long texBufferLength;
		unsigned char* texBuffer = (unsigned char*)this->FilepathToMemoryBuffer(filepath, texBufferLength);
		if (texBuffer == NULL) {
			debug_output("Texture file not found: " << filepath);
			return NULL;
		}

		// Load the texture based on its type
		switch (textureType) {

			case GL_TEXTURE_1D:
				texture = Texture1D::CreateTexture1DFromBuffer(texBuffer, texBufferLength, filter);
				break;

			case GL_TEXTURE_2D:
				texture = Texture2D::CreateTexture2DFromBuffer(texBuffer, texBufferLength, filter);
				break;

			default:
				assert(false);
				return NULL;
		}
		
		// Clean up the texture buffer...
		delete[] texBuffer;
		texBuffer = NULL;

        if (texture == NULL) {
            debug_output("Failed to load texture into memory: " << filepath);
            return NULL;
        }

		// First reference to the texture...
		this->numRefPerTexture[texture] = 1;
		this->loadedTextures[filepath] = texture;
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
	for (std::map<std::string, Texture*>::iterator iter = this->loadedTextures.begin(); iter != this->loadedTextures.end(); ++iter) {
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
 * Return the cel-shader texture used as the step function for cel-shading.
 * Returns: cel shading texture.
 */
Texture* ResourceManager::GetCelShadingTexture() {
	// If we've already loaded the texture, just return it
	if (this->celShadingTexture != NULL) {
		return this->celShadingTexture;
	}

	const std::string& filepath = GameViewConstants::GetInstance()->TEXTURE_CEL_GRADIENT;
	long texBufferLength;
	unsigned char* textureBuffer = (unsigned char*)this->FilepathToMemoryBuffer(filepath, texBufferLength);
	if (textureBuffer == NULL) {
		debug_output("Texture file not found: " << filepath);
		return NULL;
	}

	this->celShadingTexture = Texture1D::CreateTexture1DFromBuffer(textureBuffer, texBufferLength, Texture::Nearest);
	assert(this->celShadingTexture != NULL);

	delete[] textureBuffer;
	textureBuffer = NULL;

	return this->celShadingTexture;
}

/**
 * Does a direct read of the raw noise octave texture data from the relevant file system
 * and returns that data without taking ownership.
 * The caller is responsible for the returned memory!
 * Returns: NULL if could not find data.
 */
GLubyte* ResourceManager::ReadNoiseOctave3DTextureData() {
	long length;
	GLubyte* noiseBuffer = (GLubyte*)this->FilepathToMemoryBuffer(GameViewConstants::GetInstance()->TEXTURE_NOISE_OCTAVES, length);
	if (noiseBuffer == NULL) {
		debug_output("Error reading noise octave data to bytes.");
		assert(false);
		return NULL;
	}

	return noiseBuffer;
}

/**
 * Obtain a font texture resource from the physfs file system.
 * Returns: The font set on successful load, false otherwise.
 */
std::map<unsigned int, TextureFontSet*> ResourceManager::LoadFont(const std::string &filepath,
                                                                  const std::vector<unsigned int>& heights,
                                                                  Texture::TextureFilterType filterType) {
	std::map<unsigned int, TextureFontSet*> fontSets;
	
	long bufferLength = 0;
	unsigned char* fileBuffer = (unsigned char*)ResourceManager::FilepathToMemoryBuffer(filepath, bufferLength);
	if (fileBuffer == NULL) {
		debug_output("Font file not found: " << filepath);
		return fontSets;
	}

	// Load the font sets using the file buffer
	fontSets = TextureFontSet::CreateTextureFontFromBuffer(fileBuffer, bufferLength, heights, filterType);
	
	// Clean up the file buffer
	delete[] fileBuffer;
	fileBuffer = NULL;

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

		long fileBufferLength;
		char* fileBuffer = this->FilepathToMemoryBuffer(filepath, fileBufferLength);
		if (fileBuffer == NULL) {
			debug_output("Effect file not found: " << filepath);
			return;
		}

		// Load the effect file using physfs and create the effect using the Cg runtime
		effect = cgCreateEffect(this->cgContext, fileBuffer, NULL);
		debug_cg_state();

		// Clean-up the buffer and file handle
		delete[] fileBuffer;
		fileBuffer = NULL;

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
		std::map<CGeffect, std::map<std::string, CGtechnique> >::iterator techniqueIter = this->loadedEffectTechniques.find(effect);
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
	for (std::map<std::string, CGeffect>::iterator iter = this->loadedEffects.begin(); iter != this->loadedEffects.end(); ++iter) {
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

		std::map<CGeffect, std::map<std::string, CGtechnique> >::iterator techniquesIter = this->loadedEffectTechniques.find(effectResource);
		assert(techniquesIter != this->loadedEffectTechniques.end());

		std::map<std::string, CGtechnique>& techniques = techniquesIter->second;
        UNUSED_VARIABLE(techniques);
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

// Get an event sound resource by instantiating its mix chunk in SDL - this will load the resource from
// the game's resource zip file into memory.
// Returns: true on success, false otherwise.
Mix_Chunk* ResourceManager::GetEventSoundResource(const std::string &filepath) {
	// Try to find the sound in loaded sounds
	std::map<std::string, Mix_Chunk*>::iterator loadedSoundIter = this->loadedEventSounds.find(filepath);
	bool needToReadFromFile = loadedSoundIter == this->loadedEventSounds.end();

	if (needToReadFromFile) {
		long dataLength = 0;
		char* soundMemData = ResourceManager::GetInstance()->FilepathToMemoryBuffer(filepath, dataLength);
		if (soundMemData == NULL) {
			std::cerr << "Could not find file: " << filepath << std::endl;
			return NULL;
		}

		SDL_RWops* sdlSoundMem = SDL_RWFromMem(soundMemData, static_cast<int>(dataLength));
		Mix_Chunk* sound = Mix_LoadWAV_RW(sdlSoundMem, 1);	// Load the sound and free the SDL_RWops memory
		
		delete[] soundMemData;
		soundMemData = NULL;

		if (sound == NULL) {
			std::cerr << "SDL Error loading sound: " << Mix_GetError() << std::endl;
			return NULL;
		}

		this->loadedEventSounds.insert(std::make_pair(filepath, sound));
		this->numReservedEventSounds[sound] += 1;
		return sound;
	}

	// The sound had already been loaded, just return the pointer
	return loadedSoundIter->second;
}

Mix_Music* ResourceManager::GetMusicSoundResource(const std::string &filepath) {
	// Try to find the sound in loaded sounds
	std::map<std::string, Mix_Music*>::iterator loadedSoundIter = this->loadedMusicSounds.find(filepath);
	bool needToReadFromFile = loadedSoundIter == this->loadedMusicSounds.end();

	if (needToReadFromFile) {
		long dataLength = 0;
		char* soundMemData = ResourceManager::GetInstance()->FilepathToMemoryBuffer(filepath, dataLength);
		if (soundMemData == NULL) {
			std::cerr << "Could not find file: " << filepath << std::endl;
			return NULL;
		}

		SDL_RWops* sdlSoundMem = SDL_RWFromMem(soundMemData, static_cast<int>(dataLength));
		Mix_Music* music = Mix_LoadMUS_RW(sdlSoundMem);

		if (music == NULL) {
			std::cerr << "SDL Error loading music: " << Mix_GetError() << std::endl;
			return NULL;
		}

		this->loadedMusicSounds.insert(std::make_pair(filepath, music));
		this->musicSDLMemory.insert(std::make_pair(music, sdlSoundMem));
		this->musicBuffers.insert(std::make_pair(music, soundMemData));
		this->numReservedMusicSounds[music] += 1;
		return music;
	}

	// The sound had already been loaded, just return the pointer
	return loadedSoundIter->second;
}

bool ResourceManager::ReleaseEventSoundResource(Mix_Chunk* sound) {
	if (sound == NULL) {
		return true;
	}

	// Make sure the sound resource was even allocated
	std::map<Mix_Chunk*, int>::iterator reservedFindIter = this->numReservedEventSounds.find(sound);
	if (reservedFindIter == this->numReservedEventSounds.end()) {
		assert(false);
		return false;
	}

	// If this is the last reserved sound then we delete it completely from memory
	if (reservedFindIter->second - 1 == 0) {
		std::map<std::string, Mix_Chunk*>::iterator nameToChunkIter = this->loadedEventSounds.begin();
		for (; nameToChunkIter != this->loadedEventSounds.end(); ++nameToChunkIter) {
			if (nameToChunkIter->second == sound) {
				break;
			}
		}

		if (nameToChunkIter == this->loadedEventSounds.end()) {
			assert(false);
			return false;
		}

		Mix_FreeChunk(sound);
		sound = NULL;
		this->loadedEventSounds.erase(nameToChunkIter);
		this->numReservedEventSounds.erase(reservedFindIter);
	}
	else {
		reservedFindIter->second -= 1;
	}
	
	return true;
}

bool ResourceManager::ReleaseMusicSoundResource(Mix_Music* music) {
	if (music == NULL) {
		return true;
	}

	// Make sure the sound resource was even allocated
	std::map<Mix_Music*, int>::iterator reservedFindIter = this->numReservedMusicSounds.find(music);
	if (reservedFindIter == this->numReservedMusicSounds.end()) {
		assert(false);
		return false;
	}

	// If this is the last reserved sound then we delete it completely from memory
	if (reservedFindIter->second - 1 == 0) {
		std::map<std::string, Mix_Music*>::iterator nameToMusicIter = this->loadedMusicSounds.begin();
		for (; nameToMusicIter != this->loadedMusicSounds.end(); ++nameToMusicIter) {
			if (nameToMusicIter->second == music) {
				break;
			}
		}

		if (nameToMusicIter == this->loadedMusicSounds.end()) {
			assert(false);
			return false;
		}

		std::map<Mix_Music*, SDL_RWops*>::iterator sdlMemIter = this->musicSDLMemory.find(music);
		if (sdlMemIter == this->musicSDLMemory.end()) {
			assert(false);
			return false;
		}

		std::map<Mix_Music*, char*>::iterator musicBufferIter = this->musicBuffers.find(music);
		if (musicBufferIter == this->musicBuffers.end()) {
			assert(false);
			return false;
		}

		Mix_FreeMusic(music);
		music = NULL;
		delete[] musicBufferIter->second;
		musicBufferIter->second = NULL;

		this->loadedMusicSounds.erase(nameToMusicIter);
		this->musicSDLMemory.erase(sdlMemIter);
		this->musicBuffers.erase(musicBufferIter);
		this->numReservedMusicSounds.erase(reservedFindIter);
	}
	else {
		reservedFindIter->second -= 1;
	}
	
	return true;
}

void ResourceManager::SetLoadDir(const char* loadDir) {
    assert(loadDir != NULL);

    baseLoadDir = std::string(loadDir);
    size_t lastSlashPos = baseLoadDir.find_last_of('/');
    if (lastSlashPos == std::string::npos) {
        lastSlashPos = baseLoadDir.find_last_of('\\');
    }
    if (lastSlashPos != std::string::npos) {
        lastSlashPos++;
    }

    baseLoadDir = baseLoadDir.substr(0, lastSlashPos);
}

/**
 * Read the initialization configuration options in from the .ini file off disk
 * (should be in the same directory as the game). If the file has already been loaded
 * and force read is false then the options cached in memory will be given.
 * Returns: configuration options object according to values in .ini file; in cases
 * where the file could not be read a new one is created with default settings and
 * the values from it will be returned.
 */
ConfigOptions ResourceManager::ReadConfigurationOptions(bool forceReadFromFile) {
	
	// If we don't have to read from file and we already
	// loaded the configuration options then return them immediately
	if (!forceReadFromFile && ResourceManager::configOptions != NULL) {
		return *ResourceManager::configOptions;
	}

	// No matter what we need to read the configuration options from file
	
	// If any config options exist we must delete and overwrite them
	if (ResourceManager::configOptions != NULL) {
		delete ResourceManager::configOptions;
		ResourceManager::configOptions = NULL;
	}
	
	ResourceManager::configOptions = ConfigOptions::ReadConfigOptionsFromFile();
	if (ResourceManager::configOptions == NULL) {
		// Couldn't find, open or properly read the file so try to make a 
		// new one with the default settings and values in it
		
		ResourceManager::configOptions = new ConfigOptions(); // By calling the default construtor 
                                                              // we populate the object with default settings

		bool writeResult = ResourceManager::configOptions->WriteConfigOptionsToFile();
        UNUSED_VARIABLE(writeResult);
		assert(writeResult);
	}

	// Try to return configuration options no matter what
	assert(ResourceManager::configOptions != NULL);
	return *ResourceManager::configOptions;
}

/**
 * Write the given configuration options to the .ini file for the game.
 * Returns: true on successful write to .ini; false otherwise.
 */
bool ResourceManager::WriteConfigurationOptionsToFile(const ConfigOptions& cfgOptions) {
	// First thing we do is update the configuration options
	// that have already been loaded (or not)
	if (ResourceManager::configOptions == NULL) {
		ResourceManager::configOptions = new ConfigOptions(cfgOptions);
	}
	else {
		*ResourceManager::configOptions = cfgOptions;
	}

	// Now write to file
	bool writeResult = ResourceManager::configOptions->WriteConfigOptionsToFile();
	return writeResult;
}

/**
 * Convert a file stored in the resource zip filesystem into an input stream using physfs.
 */ 
std::istringstream* ResourceManager::FilepathToInStream(const std::string &filepath) {
	std::istringstream* inFile = NULL;
	long fileBufferlength = 0;

	char* fileBuffer = ResourceManager::FilepathToMemoryBuffer(filepath, fileBufferlength);
	if (fileBuffer == NULL) {
		assert(false);
		return NULL;
	}
	
	// Convert the bytes to a string stream 
	inFile = new std::istringstream(std::string(fileBuffer), std::ios_base::in | std::ios_base::binary);
	
	// Clean up
	delete[] fileBuffer;
	fileBuffer = NULL;

	return inFile;
}

/**
 * Convert a file stored in the resource zip filesystem into an input stream using physfs.
 */ 
std::stringstream* ResourceManager::FilepathToInOutStream(const std::string &filepath) {
	std::stringstream* inFile = NULL;
	long fileBufferlength = 0;

	char* fileBuffer = ResourceManager::FilepathToMemoryBuffer(filepath, fileBufferlength);
	if (fileBuffer == NULL) {
		assert(false);
		return NULL;
	}
	
	// Convert the bytes to a string stream 
    inFile = new std::stringstream(std::string(fileBuffer), std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	
	// Clean up
	delete[] fileBuffer;
	fileBuffer = NULL;

	return inFile;
}

// Convert a filepath with the resource directory in it into one with the mod directory instead
std::string ResourceManager::ConvertResourceFilepathToModFilepath(const std::string& resourceFilepath) {
	// On failure then we return an empty string...
	size_t resourceDirIdx = resourceFilepath.find_first_of(ResourceManager::RESOURCE_DIRECTORY);
	if (resourceDirIdx == std::string::npos) {
		return std::string("");
	}

	// Replace the resource directory name with the modifications directory name...
	std::string resourceDirName(ResourceManager::RESOURCE_DIRECTORY);
	std::string result = resourceFilepath;
	result.replace(resourceDirIdx, resourceDirName.size(), ResourceManager::MOD_DIRECTORY);

	return result;
}

/**
 * Convert a file stored in the resource zip filesystem into a memory stream of type T.
 */
char* ResourceManager::FilepathToMemoryBuffer(const std::string &filepath, long &length) {

#ifdef _DEBUG
	// Get the corresponding file path in the modifications directory...
	std::string modDirFilepath = ConvertResourceFilepathToModFilepath(filepath);
	std::ifstream iStream(modDirFilepath.c_str(), std::ios::binary);
	
	// If the file was not found then the stream will simply be closed and 
	// the file system will fall back to loading from the zip file
	if (iStream.is_open()) {
		iStream.seekg(0, std::ios::end);
		length = static_cast<long>(iStream.tellg());
		iStream.seekg(0, std::ios::beg);

		char* fileBuffer = new char[length+1];
		fileBuffer[length] = '\0';

		iStream.read(fileBuffer, length);
		iStream.close();

		return fileBuffer;
	}
#endif

	// First make sure the file exists in the archive
	int doesExist = PHYSFS_exists(filepath.c_str());
	if (doesExist == 0) {
		debug_output("File not found: " << filepath);
		debug_physfs_state(NULL);
		return NULL;
	}

	// Open the file for reading
	PHYSFS_File* fileHandle = PHYSFS_openRead(filepath.c_str());
	if (fileHandle == NULL) {
		return NULL;
	}

	// Get a byte buffer of the entire file and convert it into a string stream so it
	// can be read in using the STL
	PHYSFS_sint64 fileLength = PHYSFS_fileLength(fileHandle);
	char* fileBuffer = new char[fileLength+1];
	
	int readResult = PHYSFS_read(fileHandle, fileBuffer, sizeof(char), fileLength);
	if (readResult != fileLength) {
		delete[] fileBuffer;
		fileBuffer = NULL;
		debug_output("Error reading file to bytes: " << filepath);
		return NULL;
	}
	fileBuffer[fileLength] = '\0';

	int closeWentWell = PHYSFS_close(fileHandle);
	debug_physfs_state(closeWentWell);
	fileHandle = NULL;

	length = static_cast<long>(fileLength);
	return fileBuffer;
}