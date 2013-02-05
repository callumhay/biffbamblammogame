/**
 * GothicRomanticWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GothicRomanticWorldAssets.h"

#include "../ResourceManager.h"

// TODO: Get rid of this
#include "DecoSkybox.h"

GothicRomanticWorldAssets::GothicRomanticWorldAssets() : 
GameWorldAssets(new DecoSkybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BLOCK_MESH)) {
}

GothicRomanticWorldAssets::~GothicRomanticWorldAssets() {
}

void GothicRomanticWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                                    const BasicPointLight& bgFillLight) {

    const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();

    // Draw the model with the current background colour
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}