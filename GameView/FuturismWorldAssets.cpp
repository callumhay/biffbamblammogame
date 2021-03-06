/**
 * FuturismWorldAssets.cpp
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

#include "FuturismWorldAssets.h"
#include "GameViewConstants.h"
#include "GameAssets.h"
#include "Skybox.h"

#define STARTING_BACK_BEAM_ANGLE  18.0f
//#define STARTING_FRONT_BEAM_ANGLE -15.0f

FuturismWorldAssets::FuturismWorldAssets(GameAssets* assets) :
GameWorldAssets(assets, new Skybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_BLOCK_MESH)),

beamMesh(ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SKYBEAM_MESH)),

rotBackLeftBeam1(STARTING_BACK_BEAM_ANGLE),
rotBackLeftBeam2(STARTING_BACK_BEAM_ANGLE),
rotBackLeftBeam3(STARTING_BACK_BEAM_ANGLE),
rotBackRightBeam1(-STARTING_BACK_BEAM_ANGLE),
rotBackRightBeam2(-STARTING_BACK_BEAM_ANGLE),
rotBackRightBeam3(-STARTING_BACK_BEAM_ANGLE),
/*
rotFrontLeftBeam1(STARTING_FRONT_BEAM_ANGLE),
rotFrontLeftBeam2(STARTING_FRONT_BEAM_ANGLE),
rotFrontLeftBeam3(STARTING_FRONT_BEAM_ANGLE),
rotFrontRightBeam1(-STARTING_FRONT_BEAM_ANGLE),
rotFrontRightBeam2(-STARTING_FRONT_BEAM_ANGLE),
rotFrontRightBeam3(-STARTING_FRONT_BEAM_ANGLE),
*/
futurismTriangleTex(NULL),
triangleFader(0.0f, 1.0f)
{

    // Make the background colours different, make sure they're all fairly dark, there's
    // a lot of light from the beams and emitters already!
    // Setup the default colour change list
    std::vector<Colour> colours;
    colours.reserve(11);
    colours.push_back(Colour(0x58706D));
    colours.push_back(Colour(0x4B5757));
    colours.push_back(Colour(0x7C8A6E));
    colours.push_back(Colour(0x7375A5));
    colours.push_back(Colour(0x4F475C));
    colours.push_back(Colour(0x6FA380));
    colours.push_back(Colour(0x8F968A));
    colours.push_back(Colour(0x8890AD));
    colours.push_back(Colour(0x525051));
    colours.push_back(Colour(0xADBF73));
    colours.push_back(Colour(0x336E80));

    this->UpdateColourChangeList(colours);
    
    // Change the default values for drawing outlines
    this->outlineMinDistance = 0.007f;
    this->outlineMaxDistance = 10.0f;
    this->outlineContrast    = 1.0f;
    this->outlineOffset      = 0.3f;

	// Setup the beam effects
	this->backBeamEffect.SetColour(Colour(1, 1, 1));
	this->backBeamEffect.SetFadeExponent(1.5f);
	this->backBeamEffect.SetScale(0.025f);
	this->backBeamEffect.SetFrequency(3.0f);
	this->backBeamEffect.SetAlphaMultiplier(0.7f);

	//this->frontBeamEffect->SetColour(Colour(1, 1, 1));
	//this->frontBeamEffect->SetFadeExponent(2.25f);
	//this->frontBeamEffect->SetScale(0.05f);
	//this->frontBeamEffect->SetFrequency(4.0f);
	//this->frontBeamEffect->SetAlphaMultiplier(0.45f);

    this->InitializeTextures();
    this->InitializeEmitters();
    this->InitializeAnimations();
}

FuturismWorldAssets::~FuturismWorldAssets() {
    bool success = true;
    
    // Clean up animations
    for (int i = 0; i < static_cast<int>(this->beamAnimators.size()); i++) {
        AnimationMultiLerp<float>* animator = this->beamAnimators[i];
        delete animator;
        animator = NULL;
    }
    this->beamAnimators.clear();

    // Clean up meshes
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->beamMesh);
    assert(success);

    // Clean up textures
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->futurismTriangleTex);

    UNUSED_VARIABLE(success);
}


void FuturismWorldAssets::TickSkybeams(double dT) {
    for (int i = 0; i < static_cast<int>(this->beamAnimators.size()); i++) {
        this->beamAnimators[i]->Tick(dT);
    }
}

void FuturismWorldAssets::DrawBackgroundEffects(const Camera& camera) {

	float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();
	if (currBGAlpha == 0) {
		return;
	}

	static const float BACK_BEAM_Z_OFFSET = -95.0f;
    static const float BACK_BEAM_X_OFFSET = 5.0f;
    static const float BACK_BEAM_SPACING  = 8.0f;
    
    static const float FRONT_BEAM_Z_OFFSET = -25.0f;
    static const float FRONT_BEAM_X_OFFSET = 8.0f;
    static const float FRONT_BEAM_SPACING  = 8.0f;
	
	glPushAttrib(GL_CURRENT_BIT);
    static const float BEAM_AMT = 0.95f;
    glColor4f(BEAM_AMT, BEAM_AMT, BEAM_AMT, std::min<float>(currBGAlpha, 0.85f));  

	// Draw futurism background beams:

    // Back Beams (3 beams on each side, behind the buildings)
    glPushMatrix();
	glTranslatef(-BACK_BEAM_X_OFFSET, -30.0f, BACK_BEAM_Z_OFFSET);

    // Left back beams...
    this->DrawBackBeam(camera, this->rotBackLeftBeam1);
    glTranslatef(-BACK_BEAM_SPACING, 0.0f, 0.0f);
    this->DrawBackBeam(camera, this->rotBackLeftBeam2);
    glTranslatef(-BACK_BEAM_SPACING, 0.0f, 0.0f);
    this->DrawBackBeam(camera, this->rotBackLeftBeam3);
    
    glTranslatef(2*BACK_BEAM_X_OFFSET + 2*BACK_BEAM_SPACING, 0.0f, 0.0f);

    // Right back beams...
    this->DrawBackBeam(camera, this->rotBackRightBeam1);
    glTranslatef(BACK_BEAM_SPACING, 0.0f, 0.0f);
    this->DrawBackBeam(camera, this->rotBackRightBeam2);
    glTranslatef(BACK_BEAM_SPACING, 0.0f, 0.0f);
    this->DrawBackBeam(camera, this->rotBackRightBeam3);

    glPopMatrix();

    /*
    // Front beams (3 beams on each side, infront of buildings)
    glPushMatrix();
    glTranslatef(-FRONT_BEAM_X_OFFSET, -50.0f, FRONT_BEAM_Z_OFFSET);

    // Left front beams...
    this->DrawFrontBeam(camera, this->rotFrontLeftBeam1);
    glTranslatef(-FRONT_BEAM_SPACING, 0.0f, 0.0f);
    this->DrawFrontBeam(camera, this->rotFrontLeftBeam2);
    glTranslatef(-FRONT_BEAM_SPACING, 0.0f, 0.0f);
    this->DrawFrontBeam(camera, this->rotFrontLeftBeam3);

    glTranslatef(2*FRONT_BEAM_X_OFFSET + 2*FRONT_BEAM_SPACING, 0.0f, 0.0f);

    // Right front beams...
    this->DrawFrontBeam(camera, this->rotFrontRightBeam1);
    glTranslatef(FRONT_BEAM_SPACING, 0.0f, 0.0f);
    this->DrawFrontBeam(camera, this->rotFrontRightBeam2);
    glTranslatef(FRONT_BEAM_SPACING, 0.0f, 0.0f);
    this->DrawFrontBeam(camera, this->rotFrontRightBeam3);


    glPopMatrix();
    */
    
    glPopAttrib();
}

void FuturismWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                              const BasicPointLight& bgFillLight) {

    float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();

    // Draw the triangle projectiles...
    this->triangleEmitterSm.Draw(camera);
    this->triangleEmitterMed.Draw(camera);
    this->triangleEmitterLg.Draw(camera);

    const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();

    // Draw the model with the current background colour
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), currBGAlpha);
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void FuturismWorldAssets::LoadFGLighting(GameAssets* assets, const Vector3D& fgKeyPosOffset, const Vector3D& fgFillPosOffset) const {
    assets->GetLightAssets()->SetForegroundLightDefaults(
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_POSITION + fgKeyPosOffset, 
        GameViewConstants::GetInstance()->DEFAULT_FG_KEY_LIGHT_COLOUR, 0.022f),
        BasicPointLight(GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_POSITION + fgFillPosOffset, 
        GameViewConstants::GetInstance()->DEFAULT_FG_FILL_LIGHT_COLOUR, 0.015f));
}

void FuturismWorldAssets::LoadBGLighting(GameAssets* assets) const {
    GameWorldAssets::LoadBGLighting(assets);
    // TODO?
}

void FuturismWorldAssets::InitializeTextures() {
   assert(this->futurismTriangleTex == NULL);
   this->futurismTriangleTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
       GameViewConstants::GetInstance()->TEXTURE_FUTURISM_TRIANGLE, Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->futurismTriangleTex != NULL);
}

void FuturismWorldAssets::InitializeEmitters() {

	static const int NUM_PARTICLES_PER_EMITTER = 25;
	ESPInterval triangleLife(25.0f, 30.0f);
    ESPInterval triangleSpawn(triangleLife.minValue / NUM_PARTICLES_PER_EMITTER, triangleLife.maxValue / NUM_PARTICLES_PER_EMITTER);
	ESPInterval triangleSpd(5.0f, 7.0f);

	Point3D triangleMinPt(-80.0f, -45.0f, -70.0f);
	Point3D triangleMaxPt(80.0f, -40.0f, -65.0f);

    this->triangleEmitterSm.SetSpawnDelta(triangleSpawn);
	this->triangleEmitterSm.SetInitialSpd(triangleSpd);
	this->triangleEmitterSm.SetParticleLife(triangleLife);
	this->triangleEmitterSm.SetParticleSize(ESPInterval(10.0f), ESPInterval(5.0f));
	this->triangleEmitterSm.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->triangleEmitterSm.SetEmitVolume(triangleMinPt, triangleMaxPt);
	this->triangleEmitterSm.SetEmitDirection(Vector3D(0, 1, 0));
	this->triangleEmitterSm.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	this->triangleEmitterSm.SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
    this->triangleEmitterSm.AddEffector(&this->triangleFader);
    this->triangleEmitterSm.SetParticles(NUM_PARTICLES_PER_EMITTER, this->futurismTriangleTex);
	
	this->triangleEmitterMed.SetSpawnDelta(triangleSpawn);
	this->triangleEmitterMed.SetInitialSpd(triangleSpd);
	this->triangleEmitterMed.SetParticleLife(triangleLife);
	this->triangleEmitterMed.SetParticleSize(ESPInterval(22.0f), ESPInterval(11.0f));
	this->triangleEmitterMed.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->triangleEmitterMed.SetEmitVolume(triangleMinPt, triangleMaxPt);
	this->triangleEmitterMed.SetEmitDirection(Vector3D(0, 1, 0));
	this->triangleEmitterMed.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	this->triangleEmitterMed.SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
    this->triangleEmitterMed.AddEffector(&this->triangleFader);
	this->triangleEmitterMed.SetParticles(NUM_PARTICLES_PER_EMITTER, this->futurismTriangleTex);

	this->triangleEmitterLg.SetSpawnDelta(triangleSpawn);
	this->triangleEmitterLg.SetInitialSpd(triangleSpd);
	this->triangleEmitterLg.SetParticleLife(triangleLife);
	this->triangleEmitterLg.SetParticleSize(ESPInterval(38.0f), ESPInterval(19.0f));
	this->triangleEmitterLg.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->triangleEmitterLg.SetEmitVolume(triangleMinPt, triangleMaxPt);
	this->triangleEmitterLg.SetEmitDirection(Vector3D(0, 1, 0));
	this->triangleEmitterLg.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	this->triangleEmitterLg.SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
    this->triangleEmitterLg.AddEffector(&this->triangleFader);
	this->triangleEmitterLg.SetParticles(NUM_PARTICLES_PER_EMITTER, this->futurismTriangleTex);

	// Tick all the emitters for a bit to get them to look like they've been spawning for awhile
	for (int i = 0; i < 60; i++) {
		this->triangleEmitterSm.Tick(0.5);
		this->triangleEmitterMed.Tick(0.5);
		this->triangleEmitterLg.Tick(0.5);
	}
}

void FuturismWorldAssets::InitializeAnimations() {
    assert(this->beamAnimators.empty());

    AnimationMultiLerp<float>* animation = NULL;

    static const float BACK_BEAM_SWEAP_ANGLE_MAX = 45.0f;
    static const float BACK_BEAM_SWEAP_ANGLE_MED = BACK_BEAM_SWEAP_ANGLE_MAX - 10.0f;
    static const float BACK_BEAM_SWEAP_ANGLE_MIN = BACK_BEAM_SWEAP_ANGLE_MED - 10.0f;

    std::vector<double> backTimes3;
    backTimes3.reserve(5);
    backTimes3.push_back(0.0);
    backTimes3.push_back(3.0);
    backTimes3.push_back(8.0);
    backTimes3.push_back(12.0);
    backTimes3.push_back(13.0);

    std::vector<double> backTimes2;
    backTimes2.reserve(6);
    backTimes2.push_back(0.0);
    backTimes2.push_back(1.0);
    backTimes2.push_back(4.0);
    backTimes2.push_back(7.0);
    backTimes2.push_back(11.0);
    backTimes2.push_back(13.0);

    std::vector<double> backTimes1;
    backTimes1.reserve(6);
    backTimes1.push_back(0.0);
    backTimes1.push_back(2.0);
    backTimes1.push_back(4.0);
    backTimes1.push_back(6.0);
    backTimes1.push_back(10.0);
    backTimes1.push_back(13.0);

    // Back Left Beams ***********************************************************
    {
        std::vector<float> angles;
        angles.reserve(5);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);
        angles.push_back(STARTING_BACK_BEAM_ANGLE + BACK_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(STARTING_BACK_BEAM_ANGLE + BACK_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotBackLeftBeam3);
        animation->SetLerp(backTimes3, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

    {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);
        angles.push_back(STARTING_BACK_BEAM_ANGLE + BACK_BEAM_SWEAP_ANGLE_MED);
        angles.push_back(STARTING_BACK_BEAM_ANGLE + BACK_BEAM_SWEAP_ANGLE_MED);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotBackLeftBeam2);
        animation->SetLerp(backTimes2, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

   {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);
        angles.push_back(STARTING_BACK_BEAM_ANGLE + BACK_BEAM_SWEAP_ANGLE_MIN);
        angles.push_back(STARTING_BACK_BEAM_ANGLE + BACK_BEAM_SWEAP_ANGLE_MIN);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);
        angles.push_back(STARTING_BACK_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotBackLeftBeam1);
        animation->SetLerp(backTimes1, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

    // Back Right Beams ***********************************************************
    {
        std::vector<float> angles;
        angles.reserve(5);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE - BACK_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE - BACK_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotBackRightBeam3);
        animation->SetLerp(backTimes3, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

    {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE - BACK_BEAM_SWEAP_ANGLE_MED);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE - BACK_BEAM_SWEAP_ANGLE_MED);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotBackRightBeam2);
        animation->SetLerp(backTimes2, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

   {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE - BACK_BEAM_SWEAP_ANGLE_MIN);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE - BACK_BEAM_SWEAP_ANGLE_MIN);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);
        angles.push_back(-STARTING_BACK_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotBackRightBeam1);
        animation->SetLerp(backTimes1, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }
    
   /*
    static const float FRONT_BEAM_SWEAP_ANGLE_MAX = 45.0f;

    std::vector<double> frontTimes3;
    frontTimes3.reserve(6);
    frontTimes3.push_back(0.0);
    frontTimes3.push_back(1.0);
    frontTimes3.push_back(8.0);
    frontTimes3.push_back(14.0);
    frontTimes3.push_back(19.0);
    frontTimes3.push_back(20.0);

    std::vector<double> frontTimes2;
    frontTimes2.reserve(6);
    frontTimes2.push_back(0.0);
    frontTimes2.push_back(2.0);
    frontTimes2.push_back(9.0);
    frontTimes2.push_back(13.0);
    frontTimes2.push_back(18.0);
    frontTimes2.push_back(20.0);

    std::vector<double> frontTimes1;
    frontTimes1.reserve(6);
    frontTimes1.push_back(0.0);
    frontTimes1.push_back(3.0);
    frontTimes1.push_back(10.0);
    frontTimes1.push_back(12.0);
    frontTimes1.push_back(17.0);
    frontTimes1.push_back(20.0);

    // Front Left Beams ***********************************************************
    {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE + FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE + FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotFrontLeftBeam3);
        animation->SetLerp(frontTimes3, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

    {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE + FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE + FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotFrontLeftBeam2);
        animation->SetLerp(frontTimes2, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

   {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE + FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE + FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(STARTING_FRONT_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotFrontLeftBeam1);
        animation->SetLerp(frontTimes1, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

    // Front Right Beams ***********************************************************
    {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE - FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE - FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotFrontRightBeam3);
        animation->SetLerp(frontTimes3, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

    {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE - FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE - FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotFrontRightBeam2);
        animation->SetLerp(frontTimes2, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }

   {
        std::vector<float> angles;
        angles.reserve(6);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE - FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE - FRONT_BEAM_SWEAP_ANGLE_MAX);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);
        angles.push_back(-STARTING_FRONT_BEAM_ANGLE);

        animation = new AnimationMultiLerp<float>(&this->rotFrontRightBeam1);
        animation->SetLerp(frontTimes1, angles);
        animation->SetRepeat(true);
        this->beamAnimators.push_back(animation);
    }
    */
}