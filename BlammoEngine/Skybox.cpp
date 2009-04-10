#include "Skybox.h"
#include "TextureCube.h"
#include "Mesh.h"
#include "ObjReader.h"
#include "CgShaderManager.h"
#include "Camera.h"

// Skybox shader loading constants
const std::string Skybox::SKYBOX_EFFECT_FILE	= "resources/shaders/Skybox.cgfx";
const std::string Skybox::TECHNIQUE_NAME			= "Skybox";

// Default, private constructor for the Skybox object
Skybox::Skybox(PolygonGroup* geom, TextureCube* tex) : cgEffect(NULL), technique(NULL), geometry(geom), cubemap(tex) {
	assert(geom != NULL);
	assert(tex != NULL);
	
	this->currColour = Colour(1,1,1);

	// Load the skybox effect, technique and its parameters
	this->cgEffect = CgShaderManager::Instance()->LoadEffectFromCgFxFile(SKYBOX_EFFECT_FILE);
	this->LoadSkyboxCgFxParameters();
	CgShaderManager::Instance()->LoadEffectTechnique(this->cgEffect, this->technique);
	assert(this->cgEffect != NULL);
	assert(this->technique != NULL);
}

Skybox::~Skybox() {
	delete this->cubemap;
	this->cubemap = NULL;

	delete this->geometry;
	this->geometry = NULL;

	cgDestroyEffect(this->cgEffect);
	CgShaderManager::Instance()->CheckForCgError("Destroying effect");
}

/**
 * Private helper function for loading the parameters for the CgFx for
 * this skybox.
 */
void Skybox::LoadSkyboxCgFxParameters() {
	// Initialize all parameters to NULL
	this->wvpMatrixParam					= NULL;
	this->worldMatrixParam				= NULL;
	this->skyboxCamParam					= NULL;
	this->skyboxCubeSamplerParam	= NULL;
	this->colourMultParam					= NULL;

	// Transforms
	this->wvpMatrixParam					=	cgGetNamedEffectParameter(this->cgEffect, "ModelViewProjXf");
	this->worldMatrixParam				= cgGetNamedEffectParameter(this->cgEffect, "WorldXf");
	// Location of the skybox-camera
	this->skyboxCamParam					= cgGetNamedEffectParameter(this->cgEffect, "SkyCamPos");
	// Cubemap for the skybox
	this->skyboxCubeSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "SkyboxSampler");
	// Multiply colour for the skybox
	this->colourMultParam					= cgGetNamedEffectParameter(this->cgEffect, "MultiplyColour");

	CgShaderManager::Instance()->CheckForCgError("Getting parameters for Skybox effect");
	assert(this->wvpMatrixParam && this->worldMatrixParam	&& this->skyboxCamParam && this->skyboxCubeSamplerParam);
}

/**
 * Protected helper (can be inherited and overridden), used to set up the 
 * cgfx parameters with their respective values.
 */
void Skybox::SetupCgFxParameters() {
	cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

	glPushMatrix();
	glLoadIdentity();
	cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glPopMatrix();
	
	cgGLSetParameter3f(this->skyboxCamParam, 0, 0, 0);
	cgGLSetTextureParameter(this->skyboxCubeSamplerParam, this->cubemap->GetTextureID());
	cgGLSetParameter3f(this->colourMultParam, this->currColour[0], this->currColour[1], this->currColour[2]);

	CgShaderManager::Instance()->CheckForCgError("Setting parameters for Skybox effect");
}

/**
 * Draw the skybox... in all its skyboxey glory!
 */
void Skybox::Draw(const Camera& camera) {

	this->SetupCgFxParameters();

	// There should be only ONE pass for a skybox...
	// Get the pass and draw away
	CGpass currPass = cgGetFirstPass(this->technique);
	while (currPass) {
		cgSetPassState(currPass);
		//glCallList(displayListID);
		this->geometry->Draw();
		cgResetPassState(currPass);
		currPass = cgGetNextPass(currPass);
	}
}


/**
 * Static creator method for making a Skybox object. 
 * It will use the given file paths to create the geometry and material
 * for initializing a skybox.
 */
Skybox* Skybox::CreateSkybox(const std::string& meshFilepath, 
														 const std::string cubeTexFilepaths[6]) {

	// Start by making the cubemap texture
	TextureCube* skyboxTex = TextureCube::CreateCubeTextureFromImgFiles(cubeTexFilepaths, Texture::Trilinear);
	if (skyboxTex == NULL) {
		return NULL;
	}

	// Now load the polygon group associated with the skybox
	PolygonGroup* skyboxGeom = ObjReader::ReadPolygonGroup(meshFilepath);
	// TODO: (OPTIMIZATION) Have ONLY the position geometry being loaded... THIS WILL REQUIRE MODIFICATIONS
	// TO THE OBJREADER AND POLYGONGROUP CLASSES
	if (skyboxGeom == NULL) {
		return NULL;
	}

	Skybox* newSkybox = new Skybox(skyboxGeom, skyboxTex);
	return newSkybox;
}