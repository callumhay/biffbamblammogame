/**
 * SurrealismDadaWorldAssets.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "SurrealismDadaWorldAssets.h"
#include "GameAssets.h"
#include "Skybox.h"

#include "../GameModel/GameModel.h"
#include "../ResourceManager.h"


SurrealismDadaWorldAssets::SurrealismDadaWorldAssets(GameAssets* assets) : 
GameWorldAssets(assets, new Skybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SURREALISM_DADA_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SURREALISM_DADA_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SURREALISM_DADA_BLOCK_MESH)) {

    this->skybox->SetFGStarMoveSpd(0.001f);

    // Makes sure the colours are dark enough not to saturate/blanche insanely
    // and that they properly reflect the movement: ominous, moody, washed-out colours
    std::vector<Colour> colours;
    colours.reserve(14);
    colours.push_back(Colour(0x604848)); // Murky purple
    colours.push_back(Colour(0x45424B)); // Dark grey
    colours.push_back(Colour(0x603034)); // Mulberry-Brownish
    colours.push_back(Colour(0x4C6366)); // Corpse green
    colours.push_back(Colour(0x2C667E)); // Deep turquoise
    colours.push_back(Colour(0x784819)); // Poop-Brownish
    colours.push_back(Colour(0x849C6D)); // Sickly green
    colours.push_back(Colour(0x54113A)); // Mulberryish
    colours.push_back(Colour(0x481816)); // Brownish-purple
    colours.push_back(Colour(0x304848)); // Murky turqouise
    colours.push_back(Colour(0x353B5B)); // Shadowy Bluish Purple
    colours.push_back(Colour(0x745F68)); // MurkyMurky
    colours.push_back(Colour(0x660C0C)); // Burgundy
    colours.push_back(Colour(0x850B00)); // Blood

    this->UpdateColourChangeList(colours);

    this->InitializeEmitters();
}

SurrealismDadaWorldAssets::~SurrealismDadaWorldAssets() {
}

void SurrealismDadaWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                                    const BasicPointLight& bgFillLight) {

    const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();

    // Draw the model with the current background colour
	glPushAttrib(GL_CURRENT_BIT);
    glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void SurrealismDadaWorldAssets::DrawBackgroundPostOutlinePreEffects(const Camera& camera) {
    UNUSED_PARAMETER(camera);
}

void SurrealismDadaWorldAssets::Tick(double dT, const GameModel& model) {
    GameWorldAssets::Tick(dT, model);

    /*
    // Tick the emitters
    float alpha = this->bgFadeAnim.GetInterpolantValue();

    this->emitterX.Tick(dT);
    this->emitterX.SetAliveParticleAlphaMax(alpha);
    */
}

void SurrealismDadaWorldAssets::LoadBGLighting(GameAssets* assets) const {
    // Setup the Background lights
    assets->GetLightAssets()->SetBackgroundLightDefaults(
        BasicPointLight(Point3D(-25.0f, 15.0f, 55.0f), GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_COLOUR, 0.02f),
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_POSITION, GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_COLOUR,  
        GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_ATTEN));
}

void SurrealismDadaWorldAssets::InitializeEmitters() {
}

void SurrealismDadaWorldAssets::DrawBackgroundEffects(const Camera& camera) {
    UNUSED_PARAMETER(camera);
}
