/**
 * FuturismWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "FuturismWorldAssets.h"
#include "GameViewConstants.h"

// TODO: Get rid of this
#include "DecoSkybox.h"

FuturismWorldAssets::FuturismWorldAssets() :
GameWorldAssets(new DecoSkybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_BLOCK_MESH)) {
}

FuturismWorldAssets::~FuturismWorldAssets() {
}

void FuturismWorldAssets::Tick(double dT) {
    GameWorldAssets::Tick(dT);
}

void FuturismWorldAssets::DrawBackgroundEffects(const Camera& camera) {

}

void FuturismWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                              const BasicPointLight& bgFillLight) {
    
    this->background->Draw(camera, bgKeyLight, bgFillLight);
}

void FuturismWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
    GameWorldAssets::FadeBackground(fadeout, fadeTime);

}

void FuturismWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();

}