/**
 * GameWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameWorldAssets.h"

// Subclasses that can be created...
#include "ClassicalWorldAssets.h"
#include "DecoWorldAssets.h"
#include "FuturismWorldAssets.h"
#include "GothicRomanticWorldAssets.h"

const float GameWorldAssets::COLOUR_CHANGE_TIME = 10.0f;	// Amount of time in seconds to change from one colour to the next

GameWorldAssets::GameWorldAssets(Skybox* skybox, Mesh* bg, Mesh* paddle, Mesh* styleBlock) : 
		skybox(skybox), background(bg), playerPaddle(paddle), styleBlock(styleBlock), bgFadeAnim(1) {
	assert(skybox != NULL);
	assert(bg != NULL);
	assert(paddle != NULL);

	// No animation to start for the background fade (needs to be activated via the appropriate member function)
	this->bgFadeAnim.SetRepeat(false);

    // Setup the default colour change list
    std::vector<Colour> colours;
    colours.reserve(10);
    colours.push_back(Colour(0.4375f, 0.5f, 0.5647f));             // slate greyish-blue
    colours.push_back(Colour(0.2745098f, 0.5098039f, 0.70588f));   // steel blue
    colours.push_back(Colour(0.28235f, 0.2392f, 0.545098f));       // slate purple-blue
    colours.push_back(Colour(0.51372549f, 0.4352941f, 1.0f));      // slate purple
    colours.push_back(Colour(0.8588235f, 0.439215686f, 0.57647f)); // pale violet
    colours.push_back(Colour(0.8f, 0.55686f, 0.55686f));           // rosy brown 
    colours.push_back(Colour(0.7215686f, 0.52549f, 0.043f));       // goldenrod
    colours.push_back(Colour(0.4196f, 0.5568627f, 0.1372549f));    // olive
    colours.push_back(Colour(0.4f, 0.8039215f, 0.666667f));		  // deep aquamarine
    colours.push_back(Colour(0.3725f, 0.6196078f, 0.62745098f));	  // cadet (olive-) blue

    this->UpdateColourChangeList(colours);
}

GameWorldAssets::~GameWorldAssets() {
	delete this->skybox;
	this->skybox = NULL;

	// Tell the resource manager to release any of the loaded meshes
    bool success = false;
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->background);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->playerPaddle);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->styleBlock);
    assert(success);
}

void GameWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
	// Set the appropriate end alpha based on whether we are fading out or not
	float finalAlpha = 1.0f;
	if (fadeout) {
		finalAlpha = 0.0f;
	}

	this->bgFadeAnim.SetLerp(fadeTime, finalAlpha);
}
void GameWorldAssets::ResetToInitialState() {
	this->bgFadeAnim.ClearLerp();
	this->bgFadeAnim.SetInterpolantValue(1.0f);
}

// Static creation method
GameWorldAssets* GameWorldAssets::CreateWorldAssets(GameWorld::WorldStyle world) {
	switch (world) {
        case GameWorld::Classical:
            return new ClassicalWorldAssets();
        case GameWorld::GothicRomantic:
            return new GothicRomanticWorldAssets();
		case GameWorld::Deco:
			return new DecoWorldAssets();
        case GameWorld::Futurism:
            return new FuturismWorldAssets();
		default:
			break;
	}

	return NULL;
}

void GameWorldAssets::UpdateColourChangeList(const std::vector<Colour>& newList) {
    this->colourChangeList = newList;

    // Setup the colour animations for the background mesh
	const int colourChangesPlusOne = static_cast<int>(this->colourChangeList.size()) + 1;
	std::vector<double> timeValues;
	timeValues.reserve(colourChangesPlusOne);
	std::vector<Colour> colourValues;
	colourValues.reserve(colourChangesPlusOne);

	for (int i = 0; i < colourChangesPlusOne; i++) {
		timeValues.push_back(i * GameWorldAssets::COLOUR_CHANGE_TIME);
        colourValues.push_back(this->colourChangeList[i % this->colourChangeList.size()]);
	}

    this->currBGMeshColourAnim.ClearLerp();
	this->currBGMeshColourAnim.SetRepeat(true);
	this->currBGMeshColourAnim.SetLerp(timeValues, colourValues);
}