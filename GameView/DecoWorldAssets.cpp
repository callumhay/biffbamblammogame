/**
 * DecoWorldAssets.cpp
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

#include "DecoWorldAssets.h"
#include "GameAssets.h"
#include "GameViewConstants.h"
#include "CgFxVolumetricEffect.h"
#include "Skybox.h"

#include "../ResourceManager.h"


// Basic constructor: Load all the basic assets for the deco world...
DecoWorldAssets::DecoWorldAssets(GameAssets* assets) : 
GameWorldAssets(assets, new Skybox(),
        ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BACKGROUND_MESH),
		ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_PADDLE_MESH),
		ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BLOCK_MESH)),

skybeam(ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SKYBEAM_MESH)),

beamEffect(new CgFxVolumetricEffect()),
beamRotationfg1(0.0),
beamRotationfg2(0.0),
beamRotationbg1(0.0),
beamRotationbg2(0.0),
rotationSpd(8.0),
rotationLimitfg(20.0),
rotationLimitbg(10.0),
rotationStatefg1(RotateCCW),
rotationStatefg2(RotateCW),
rotationStatebg1(RotateCW),
rotationStatebg2(RotateCCW),
spiralTexSm(NULL),
spiralTexMed(NULL),
spiralTexLg(NULL),
rotateEffectorCW(0, 5, ESPParticleRotateEffector::CLOCKWISE),
rotateEffectorCCW(0, 5, ESPParticleRotateEffector::COUNTER_CLOCKWISE)
{
	// Setup the beam effect
	this->beamEffect->SetColour(Colour(1, 1, 1));
	this->beamEffect->SetFadeExponent(2.0f);
	this->beamEffect->SetScale(0.05f);
	this->beamEffect->SetFrequency(4.0f);
	this->beamEffect->SetAlphaMultiplier(0.55f);

    // Change the default values for drawing outlines
    this->outlineMinDistance = 0.04f;
    this->outlineMaxDistance = 28.0f;
    this->outlineContrast    = 1.9f;
    this->outlineOffset      = 0.8f;

    // Makes sure the colours are dark enough not to saturate/blanche insanely
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
    colours.push_back(Colour(0.4f, 0.8039215f, 0.666667f));		   // deep aquamarine
    colours.push_back(Colour(0.3725f, 0.6196078f, 0.62745098f));   // cadet (olive-) blue

    this->UpdateColourChangeList(colours);

	// Setup the background emitters
	this->InitializeEmitters();
}

DecoWorldAssets::~DecoWorldAssets() {
	// Clean up deco specific background assets:
	// Beam effect
	delete this->beamEffect;
	this->beamEffect = NULL;

	// Textures
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->spiralTexSm);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->spiralTexMed);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->spiralTexLg);
	assert(success);

    // Meshes
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->skybeam);
    assert(success);
}

/**
 * Initialize the emitters in the background.
 */
void DecoWorldAssets::InitializeEmitters() {
	// Load the textures for the emitter particles
	assert(this->spiralTexSm == NULL);
	this->spiralTexSm = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPIRAL_SMALL, Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->spiralTexSm != NULL);
	
	assert(this->spiralTexMed == NULL);
	this->spiralTexMed = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPIRAL_MEDIUM, Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->spiralTexMed != NULL);

	assert(this->spiralTexLg == NULL);
	this->spiralTexLg = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPIRAL_LARGE, Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->spiralTexLg != NULL);

	// Setup the spiral emitters that come out at the very back, behind the buildings
	ESPInterval spiralSpawn(0.6f, 1.2f);
	ESPInterval spiralLife(20.0f, 25.0f);
	ESPInterval spiralSpd(5.0f, 7.0f);

	const Colour& spiralColour = this->GetColourChangeListAt(0);

	Point3D spiralMinPt(-70.0f, -30.0f, -70.0f);
	Point3D spiralMaxPt(70.0f, -25.0f, -65.0f);

	this->spiralEmitterSm.SetSpawnDelta(spiralSpawn);
	this->spiralEmitterSm.SetInitialSpd(spiralSpd);
	this->spiralEmitterSm.SetParticleLife(spiralLife);
	this->spiralEmitterSm.SetParticleSize(ESPInterval(2.5f, 4.0f));
	this->spiralEmitterSm.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->spiralEmitterSm.SetEmitVolume(spiralMinPt, spiralMaxPt);
	this->spiralEmitterSm.SetEmitDirection(Vector3D(0, 1, 0));
	this->spiralEmitterSm.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	this->spiralEmitterSm.SetParticleColour(ESPInterval(spiralColour.R()), ESPInterval(spiralColour.G()), ESPInterval(spiralColour.B()), ESPInterval(1.0f));
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		this->spiralEmitterSm.AddEffector(&this->rotateEffectorCW);
	}
	else {
		this->spiralEmitterSm.AddEffector(&this->rotateEffectorCCW);
	}
	this->spiralEmitterSm.SetParticles(23, this->spiralTexSm);
	
	this->spiralEmitterMed.SetSpawnDelta(spiralSpawn);
	this->spiralEmitterMed.SetInitialSpd(spiralSpd);
	this->spiralEmitterMed.SetParticleLife(spiralLife);
	this->spiralEmitterMed.SetParticleSize(ESPInterval(4.5f, 5.5f));
	this->spiralEmitterMed.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->spiralEmitterMed.SetEmitVolume(spiralMinPt, spiralMaxPt);
	this->spiralEmitterMed.SetEmitDirection(Vector3D(0, 1, 0));
	this->spiralEmitterMed.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	this->spiralEmitterMed.SetParticleColour(ESPInterval(spiralColour.R()), ESPInterval(spiralColour.G()), ESPInterval(spiralColour.B()), ESPInterval(1.0f));
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		this->spiralEmitterMed.AddEffector(&this->rotateEffectorCW);
	}
	else {
		this->spiralEmitterMed.AddEffector(&this->rotateEffectorCCW);
	}
	this->spiralEmitterMed.SetParticles(23, this->spiralTexMed);

	this->spiralEmitterLg.SetSpawnDelta(spiralSpawn);
	this->spiralEmitterLg.SetInitialSpd(spiralSpd);
	this->spiralEmitterLg.SetParticleLife(spiralLife);
	this->spiralEmitterLg.SetParticleSize(ESPInterval(6.0f, 7.0f));
	this->spiralEmitterLg.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->spiralEmitterLg.SetEmitVolume(spiralMinPt, spiralMaxPt);
	this->spiralEmitterLg.SetEmitDirection(Vector3D(0, 1, 0));
	this->spiralEmitterLg.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	this->spiralEmitterLg.SetParticleColour(ESPInterval(spiralColour.R()), ESPInterval(spiralColour.G()), ESPInterval(spiralColour.B()), ESPInterval(1.0f));
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		this->spiralEmitterLg.AddEffector(&this->rotateEffectorCW);
	}
	else {
		this->spiralEmitterLg.AddEffector(&this->rotateEffectorCCW);
	}
	this->spiralEmitterLg.SetParticles(23, this->spiralTexLg);

	// Tick all the emitters for a bit to get them to look like they've been spawning for awhile
	for (unsigned int i = 0; i < 60; i++) {
		this->spiralEmitterSm.Tick(0.5);
		this->spiralEmitterMed.Tick(0.5);
		this->spiralEmitterLg.Tick(0.5);
	}
}

void DecoWorldAssets::Tick(double dT, const GameModel& model) {
	// Rotate the background effect (sky beams)
	this->RotateSkybeams(dT);

	// Tick the effects with the appropriate colour and alpha set...
	const Colour& spiralColour = this->currBGMeshColourAnim.GetInterpolantValue();
	float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();

	this->spiralEmitterSm.Tick(dT);
	this->spiralEmitterSm.SetParticleColour(spiralColour);
    this->spiralEmitterSm.SetAliveParticleAlphaMax(currBGAlpha);
	this->spiralEmitterMed.Tick(dT);
	this->spiralEmitterMed.SetParticleColour(spiralColour);
    this->spiralEmitterMed.SetAliveParticleAlphaMax(currBGAlpha);
	this->spiralEmitterLg.Tick(dT);
	this->spiralEmitterLg.SetParticleColour(spiralColour);
    this->spiralEmitterLg.SetAliveParticleAlphaMax(currBGAlpha);
	
	GameWorldAssets::Tick(dT, model);
}

void DecoWorldAssets::LoadFGLighting(GameAssets* assets, const Vector3D& fgKeyPosOffset, const Vector3D& fgFillPosOffset) const {
    
    // Setup the foreground lights
    assets->GetLightAssets()->SetForegroundLightDefaults(
        BasicPointLight(Point3D(-15.0f, 10.0f, 45.0f) + fgKeyPosOffset, Colour(0.6f, 0.6f, 0.6f), 0.030f),
        BasicPointLight(Point3D(25.0f, 10.0f, 35.0f) + fgFillPosOffset, GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_COLOUR, 0.024f));
}

void DecoWorldAssets::LoadBGLighting(GameAssets* assets) const {
    // Change the positions of the background lights to something that complements the background more
    assets->GetLightAssets()->SetBackgroundLightDefaults(
        BasicPointLight(Point3D(41.0f, 15.0f, 60.0f), GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_COLOUR, 0.0125f),
        BasicPointLight(Point3D(-30.0f, 30.0f, 30.0f), GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_COLOUR,  0.02f));
}

/**
 * Private helper function for rotating the background skybeams.
 */
void DecoWorldAssets::RotateSkybeams(double dT) {
	// Rotate the beams...
	double rotationAmt = dT * this->rotationSpd;

	// Rotate and draw the skybeams...

	// Front beams
	if (this->rotationStatefg1 == RotateCCW) {
		this->beamRotationfg1 += rotationAmt;
	}
	else {
		this->beamRotationfg1 -= rotationAmt;
	}

	if (this->rotationStatefg2 == RotateCCW) {
		this->beamRotationfg2 += rotationAmt;
	}
	else {
		this->beamRotationfg2 -= rotationAmt;
	}

	// Back beams
	if (this->rotationStatebg1 == RotateCCW) {
		this->beamRotationbg1 += rotationAmt;
	}
	else {
		this->beamRotationbg1 -= rotationAmt;
	}

	if (this->rotationStatebg2 == RotateCCW) {
		this->beamRotationbg2 += rotationAmt;
	}
	else {
		this->beamRotationbg2 -= rotationAmt;
	}

	// Rotate in the opposite direction if we go past the limit...

	// Front beams...
	if (abs(this->beamRotationfg1) > rotationLimitfg) {
		this->beamRotationfg1 = rotationLimitfg * NumberFuncs::SignOf(static_cast<float>(this->beamRotationfg1));
		if (this->rotationStatefg1 == RotateCCW) {
			this->rotationStatefg1 = RotateCW;
		}
		else {
			this->rotationStatefg1 = RotateCCW;
		}
	}
	
	if (abs(this->beamRotationfg2) > rotationLimitfg) {
		this->beamRotationfg2 = rotationLimitfg * NumberFuncs::SignOf(static_cast<float>(this->beamRotationfg2));
		if (this->rotationStatefg2 == RotateCCW) {
			this->rotationStatefg2 = RotateCW;
		}
		else {
			this->rotationStatefg2 = RotateCCW;
		}
	}

	// Back beams...
	if (abs(this->beamRotationbg1) > rotationLimitbg) {
		this->beamRotationbg1 = rotationLimitbg * NumberFuncs::SignOf(static_cast<float>(this->beamRotationbg1));
		if (this->rotationStatebg1 == RotateCCW) {
			this->rotationStatebg1 = RotateCW;
		}
		else {
			this->rotationStatebg1 = RotateCCW;
		}
	}
	
	if (abs(this->beamRotationbg2) > rotationLimitbg) {
		this->beamRotationbg2 = rotationLimitbg * NumberFuncs::SignOf(static_cast<float>(this->beamRotationbg2));
		if (this->rotationStatebg2 == RotateCCW) {
			this->rotationStatebg2 = RotateCW;
		}
		else {
			this->rotationStatebg2 = RotateCCW;
		}
	}
}

void DecoWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight) {
	// Draw spiral effects behind the background model
	this->spiralEmitterSm.Draw(camera);
	this->spiralEmitterMed.Draw(camera);
	this->spiralEmitterLg.Draw(camera);

	const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void DecoWorldAssets::DrawBackgroundEffects(const Camera& camera) {
	float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();
	if (currBGAlpha == 0) {
		return;
	}
	
	glPushAttrib(GL_CURRENT_BIT);

	// Draw deco background beams:
    static const float BEAM_AMT = 0.95f;
    glColor4f(BEAM_AMT, BEAM_AMT, BEAM_AMT, std::min<float>(currBGAlpha, 0.85f));

	// Back beams...
	glPushMatrix();
	glTranslatef(-25.0f, -50.0f, -63.0f);
	glRotated(this->beamRotationbg1, 0, 0, 1);
	this->skybeam->Draw(camera, this->beamEffect);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(25.0f, -50.0f, -63.0f);
	glRotated(this->beamRotationbg2, 0, 0, 1);
	this->skybeam->Draw(camera, this->beamEffect);
	glPopMatrix();

	// Front beams...
	glPushMatrix();
	glTranslatef(-18.0f, -70.0f, -20.0f);
	glRotated(this->beamRotationfg1, 0, 0, 1);
	this->skybeam->Draw(camera, this->beamEffect);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(18.0f, -70.0f, -20.0f);
	glRotated(this->beamRotationfg2, 0, 0, 1);
	this->skybeam->Draw(camera, this->beamEffect);
	glPopMatrix();

	glPopAttrib();
}
