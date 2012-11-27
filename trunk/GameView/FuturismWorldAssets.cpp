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

#define STARTING_BACK_BEAM_ANGLE  18.0f
#define STARTING_FRONT_BEAM_ANGLE -15.0f

FuturismWorldAssets::FuturismWorldAssets() :
GameWorldAssets(new DecoSkybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->FUTURISM_BLOCK_MESH)),

beamMesh(ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SKYBEAM_MESH)),
backBeamEffect(new CgFxVolumetricEffect()),
frontBeamEffect(new CgFxVolumetricEffect()),

rotBackLeftBeam1(STARTING_BACK_BEAM_ANGLE),
rotBackLeftBeam2(STARTING_BACK_BEAM_ANGLE),
rotBackLeftBeam3(STARTING_BACK_BEAM_ANGLE),
rotBackRightBeam1(-STARTING_BACK_BEAM_ANGLE),
rotBackRightBeam2(-STARTING_BACK_BEAM_ANGLE),
rotBackRightBeam3(-STARTING_BACK_BEAM_ANGLE),

rotFrontLeftBeam1(STARTING_FRONT_BEAM_ANGLE),
rotFrontLeftBeam2(STARTING_FRONT_BEAM_ANGLE),
rotFrontLeftBeam3(STARTING_FRONT_BEAM_ANGLE),
rotFrontRightBeam1(-STARTING_FRONT_BEAM_ANGLE),
rotFrontRightBeam2(-STARTING_FRONT_BEAM_ANGLE),
rotFrontRightBeam3(-STARTING_FRONT_BEAM_ANGLE)

//rotateEffectorCW(0, 5, ESPParticleRotateEffector::CLOCKWISE),
//rotateEffectorCCW(0, 5, ESPParticleRotateEffector::COUNTER_CLOCKWISE)
{
	// Setup the beam effects
	this->backBeamEffect->SetColour(Colour(1, 1, 1));
	this->backBeamEffect->SetFadeExponent(1.5f);
	this->backBeamEffect->SetScale(0.025f);
	this->backBeamEffect->SetFrequency(3.0f);
	this->backBeamEffect->SetAlphaMultiplier(0.7f);

	this->frontBeamEffect->SetColour(Colour(1, 1, 1));
	this->frontBeamEffect->SetFadeExponent(2.25f);
	this->frontBeamEffect->SetScale(0.05f);
	this->frontBeamEffect->SetFrequency(4.0f);
	this->frontBeamEffect->SetAlphaMultiplier(0.45f);

    this->InitializeTextures();
    this->InitializeEmitters();
    this->InitializeAnimations();
}

FuturismWorldAssets::~FuturismWorldAssets() {
    bool success = true;
    
    // Clean up effects
    delete this->backBeamEffect;
    this->backBeamEffect = NULL;

    delete this->frontBeamEffect;
    this->frontBeamEffect = NULL;

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

    UNUSED_VARIABLE(success);
}


void FuturismWorldAssets::TickSkybeams(double dT) {
    for (int i = 0; i < static_cast<int>(this->beamAnimators.size()); i++) {
        this->beamAnimators[i]->Tick(dT);
    }
}

void FuturismWorldAssets::DrawBackgroundEffects(const Camera& camera) {
	static const float BACK_BEAM_Z_OFFSET = -95.0f;
    static const float BACK_BEAM_X_OFFSET = 5.0f;
    static const float BACK_BEAM_SPACING  = 8.0f;
    
    static const float FRONT_BEAM_Z_OFFSET = -25.0f;
    static const float FRONT_BEAM_X_OFFSET = 8.0f;
    static const float FRONT_BEAM_SPACING  = 8.0f;

    float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();
	if (currBGAlpha == 0) {
		return;
	}
	
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
    
    glPopAttrib();
}

void FuturismWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                              const BasicPointLight& bgFillLight) {

    const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();

    // Draw the model with the current background colour
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void FuturismWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
    GameWorldAssets::FadeBackground(fadeout, fadeTime);
}

void FuturismWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();

}

void FuturismWorldAssets::InitializeTextures() {
   // TODO
}

void FuturismWorldAssets::InitializeEmitters() {
    
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

}