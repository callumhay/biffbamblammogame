/**
 * NouveauWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "NouveauWorldAssets.h"
#include "GameViewConstants.h"
#include "DecoSkybox.h"

#include "../ResourceManager.h"

// Basic constructor: Load all the basic assets for the deco world...
NouveauWorldAssets::NouveauWorldAssets() : 
GameWorldAssets(new DecoSkybox(),
        ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BACKGROUND_MESH),
		ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_PADDLE_MESH),
		ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BLOCK_MESH)) {
}

NouveauWorldAssets::~NouveauWorldAssets() {
}

void NouveauWorldAssets::Tick(double dT) {
    // TODO
	GameWorldAssets::Tick(dT);
}

void NouveauWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                             const BasicPointLight& bgFillLight) {

	const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void NouveauWorldAssets::DrawBackgroundEffects(const Camera& camera) {
    UNUSED_PARAMETER(camera);

	//float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();
	//if (currBGAlpha == 0) {
	//	return;
	//}

}

/**
 * Reset the world to its initial state - makes sure that nothing is faded/invisible etc.
 * and is ready to be see for the first time by the player.
 */
void NouveauWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();
}