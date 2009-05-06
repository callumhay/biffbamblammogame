#include "DecoSkybox.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/BlammoEngine.h"

const float DecoSkybox::COLOUR_CHANGE_TIME = 10.0f;	// Amount of time in seconds to change from one colour to the next

const Colour DecoSkybox::COLOUR_CHANGE_LIST[NUM_COLOUR_CHANGES] = {
	Colour(0.4375f, 0.5f, 0.5647f),							// slate greyish-blue
	Colour(0.2745098f, 0.5098039f, 0.70588f),		// steel blue
	Colour(0.28235f, 0.2392f, 0.545098f),				// slate purple-blue
	Colour(0.51372549f, 0.4352941f, 1.0f),			// slate purple
	Colour(0.8588235f, 0.439215686f, 0.57647f),	// pale violet
	Colour(1.0f, 0.75686f, 0.75686f),						// rosy brown 
	Colour(0.7215686f, 0.52549f, 0.043f),				// goldenrod
	Colour(0.4196f, 0.5568627f, 0.1372549f),		// olive
	Colour(0.4f, 0.8039215f, 0.666667f),				// deep aquamarine
	Colour(0.3725f, 0.6196078f, 0.62745098f)		// cadet (olive-) blue
};

const std::string DecoSkybox::DECO_SKYBOX_TEXTURES[6]			= {
	GameViewConstants::GetInstance()->TEXTURE_DIR + "/deco_spirals1024x1024.jpg", 
	GameViewConstants::GetInstance()->TEXTURE_DIR + "/deco_spirals1024x1024.jpg",
	GameViewConstants::GetInstance()->TEXTURE_DIR + "/deco_spirals1024x1024.jpg", 
	GameViewConstants::GetInstance()->TEXTURE_DIR + "/deco_spirals1024x1024.jpg",
	GameViewConstants::GetInstance()->TEXTURE_DIR + "/deco_spirals1024x1024.jpg", 
	GameViewConstants::GetInstance()->TEXTURE_DIR + "/deco_spirals1024x1024.jpg"
};

DecoSkybox::DecoSkybox(PolygonGroup* geom, TextureCube* tex) : Skybox(geom, tex), currColourAnim(&currColour) {
	// Setup the colour interpolation member
	const int colourChangesPlusOne = NUM_COLOUR_CHANGES + 1;
	std::vector<double> timeValues;
	timeValues.reserve(colourChangesPlusOne);
	std::vector<Colour> colourValues;
	colourValues.reserve(colourChangesPlusOne);

	for (int i = 0; i < colourChangesPlusOne; i++) {
		timeValues.push_back(i * DecoSkybox::COLOUR_CHANGE_TIME);
		colourValues.push_back(DecoSkybox::COLOUR_CHANGE_LIST[i % NUM_COLOUR_CHANGES]);
	}

	currColourAnim.SetRepeat(true);
	currColourAnim.SetLerp(timeValues, colourValues);
}

DecoSkybox::~DecoSkybox() {
}

void DecoSkybox::Tick(double dT) {
	currColourAnim.Tick(dT);
}

void DecoSkybox::SetupCgFxParameters() {
	Skybox::SetupCgFxParameters();
}

DecoSkybox* DecoSkybox::CreateDecoSkybox(const std::string& meshFilepath) {
	// Start by making the cubemap texture
	TextureCube* skyboxTex = TextureCube::CreateCubeTextureFromImgFiles(DECO_SKYBOX_TEXTURES, Texture::Trilinear);
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

	DecoSkybox* newSkybox = new DecoSkybox(skyboxGeom, skyboxTex);
	return newSkybox;
}