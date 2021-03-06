/**
 * GameWorldAssets.cpp
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

#include "GameWorldAssets.h"
#include "GameAssets.h"
#include "GameViewConstants.h"

// Subclasses that can be created...
#include "ClassicalWorldAssets.h"
#include "GothicRomanticWorldAssets.h"
#include "NouveauWorldAssets.h"
#include "DecoWorldAssets.h"
#include "FuturismWorldAssets.h"
#include "SurrealismDadaWorldAssets.h"

const float GameWorldAssets::COLOUR_CHANGE_TIME = 10.0f;	// Amount of time in seconds to change from one colour to the next

GameWorldAssets::GameWorldAssets(GameAssets* assets, Skybox* skybox, Mesh* bg, Mesh* paddle, Mesh* styleBlock) : 
skybox(skybox), background(bg), playerPaddle(paddle), styleBlock(styleBlock), bgFadeAnim(1),
outlineMinDistance(0.001f), outlineMaxDistance(30.0f), outlineContrast(1.0f), outlineOffset(0.75f) {

    assert(assets != NULL);
	assert(skybox != NULL);
	assert(bg != NULL);
	assert(paddle != NULL);

    UNUSED_PARAMETER(assets);

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

void GameWorldAssets::LoadFGLighting(GameAssets* assets, const Vector3D& fgKeyPosOffset, const Vector3D& fgFillPosOffset) const {
    // Set the foreground lights to their default positions, colours and attenuations...

    // Setup the foreground lights
    assets->GetLightAssets()->SetForegroundLightDefaults(
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_POSITION + fgKeyPosOffset, GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_COLOUR, 
        GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_ATTEN),
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_POSITION + fgFillPosOffset, GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_COLOUR, 
        GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_ATTEN));
}

void GameWorldAssets::LoadBGLighting(GameAssets* assets) const {
    // Setup the Background lights
    assets->GetLightAssets()->SetBackgroundLightDefaults(
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_POSITION, GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_COLOUR, 
        GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_ATTEN),
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_POSITION, GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_COLOUR,  
        GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_ATTEN));
}

void GameWorldAssets::LoadLightingForLevel(GameAssets* assets, const GameLevel& level) const {
    float levelWidth  = level.GetLevelUnitWidth();

    static const float MIN_WIDTH_BEFORE_Z_CHANGE = 15 * LevelPiece::PIECE_WIDTH;
    static const float MAX_WIDTH_BEFORE_Z_FIXED  = 23 * LevelPiece::PIECE_WIDTH;

    Vector3D addedFgKeyDist(0,0,0);
    Vector3D addedFgFillDist(0,0,0);

    static const float MAX_DIST_CHANGE = 9.0f;

    if (levelWidth > MIN_WIDTH_BEFORE_Z_CHANGE) {
        if (levelWidth > MAX_WIDTH_BEFORE_Z_FIXED) {
            addedFgFillDist[2] = addedFgKeyDist[2] = 10.0f;
            addedFgFillDist[0] = MAX_DIST_CHANGE;
            addedFgKeyDist[0]  = -MAX_DIST_CHANGE;
        }
        else {
            addedFgFillDist[2] = addedFgKeyDist[2] = 
                NumberFuncs::LerpOverFloat(MIN_WIDTH_BEFORE_Z_CHANGE, MAX_WIDTH_BEFORE_Z_FIXED, 0.0f, MAX_DIST_CHANGE, levelWidth);
            addedFgFillDist[0] = addedFgFillDist[2];
            addedFgKeyDist[0]  = -addedFgFillDist[0];
        }
    }

    this->LoadFGLighting(assets, addedFgKeyDist, addedFgFillDist);
    this->LoadBGLighting(assets);
}

void GameWorldAssets::DrawPaddle(const PlayerPaddle& p, const Camera& camera, CgFxEffectBase* replacementMat, 
                                 const BasicPointLight& keyLight, const BasicPointLight& fillLight,
                                 const BasicPointLight& ballLight) const {

    if (p.GetAlpha() <= 0.0f) {
        return;
    }
    
    ColourRGBA paddleColour = p.GetColour();

    glPushMatrix();

    float paddleScaleFactor  = p.GetPaddleScaleFactor();
    float paddleZRotationAmt = p.GetZRotation();

    glRotatef(paddleZRotationAmt, 0, 0, 1);
    glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
    glColor4f(paddleColour.R(), paddleColour.G(), paddleColour.B(), paddleColour.A());
    this->playerPaddle->Draw(camera, replacementMat, keyLight, fillLight, ballLight);
    glPopMatrix();
}

void GameWorldAssets::DrawGhostPaddle(const PlayerPaddle& p, const Camera& camera) {
    glPushMatrix();

    float paddleScaleFactor  = p.GetPaddleScaleFactor();

    glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
    this->playerPaddle->Draw(camera); //, replacementMat, keyLight, fillLight, ballLight);
    glPopMatrix();
}

void GameWorldAssets::ResetToInitialState() {
	this->bgFadeAnim.ClearLerp();
	this->bgFadeAnim.SetInterpolantValue(1.0f);
    this->currBGMeshColourAnim.SetToRandom();
}

// Static creation method
GameWorldAssets* GameWorldAssets::CreateWorldAssets(GameWorld::WorldStyle world, GameAssets* assets) {
	switch (world) {
        case GameWorld::Classical:
            return new ClassicalWorldAssets(assets);
        case GameWorld::GothicRomantic:
            return new GothicRomanticWorldAssets(assets);
        case GameWorld::Nouveau:
            return new NouveauWorldAssets(assets);
		case GameWorld::Deco:
			return new DecoWorldAssets(assets);
        case GameWorld::Futurism:
            return new FuturismWorldAssets(assets);
        
        
        case GameWorld::SurrealismDada:
            return new SurrealismDadaWorldAssets(assets);

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