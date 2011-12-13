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
#include "DecoWorldAssets.h"
#include "FuturismWorldAssets.h"

GameWorldAssets::GameWorldAssets(Skybox* skybox, Mesh* bg, Mesh* paddle, Mesh* styleBlock) : 
		skybox(skybox), background(bg), playerPaddle(paddle), styleBlock(styleBlock), bgFadeAnim(1) {
	assert(skybox != NULL);
	assert(bg != NULL);
	assert(paddle != NULL);

	// No animation to start for the background fade (needs to be activated via the appropriate member function)
	this->bgFadeAnim.SetRepeat(false);

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
		case GameWorld::Deco:
			return new DecoWorldAssets();
        case GameWorld::Futurism:
            return new FuturismWorldAssets();
		default:
			break;
	}

	return NULL;
}