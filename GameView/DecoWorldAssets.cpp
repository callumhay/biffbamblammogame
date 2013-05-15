/**
 * DecoWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "DecoWorldAssets.h"
#include "GameViewConstants.h"
#include "CgFxVolumetricEffect.h"
#include "DecoSkybox.h"

#include "../ResourceManager.h"


// Basic constructor: Load all the basic assets for the deco world...
DecoWorldAssets::DecoWorldAssets(GameAssets* assets) : 
GameWorldAssets(assets, new DecoSkybox(),
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
	this->spiralTexSm = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPIRAL_SMALL, Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->spiralTexSm != NULL);
	
	assert(this->spiralTexMed == NULL);
	this->spiralTexMed = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPIRAL_MEDIUM, Texture::Trilinear, GL_TEXTURE_2D));
	assert(this->spiralTexMed != NULL);

	assert(this->spiralTexLg == NULL);
	this->spiralTexLg = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPIRAL_LARGE, Texture::Trilinear, GL_TEXTURE_2D));
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
	this->spiralEmitterSm.SetParticleAlignment(ESP::ScreenAligned);
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
	this->spiralEmitterMed.SetParticleAlignment(ESP::ScreenAligned);
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
	this->spiralEmitterLg.SetParticleAlignment(ESP::ScreenAligned);
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

void DecoWorldAssets::Tick(double dT) {
	// Rotate the background effect (sky beams)
	this->RotateSkybeams(dT);

	// Tick the effects with the appropriate colour and alpha set...
	const Colour& spiralColour = this->currBGMeshColourAnim.GetInterpolantValue();
	float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();

	this->spiralEmitterSm.Tick(dT);
	this->spiralEmitterSm.SetParticleColour(ESPInterval(spiralColour.R()), ESPInterval(spiralColour.G()), ESPInterval(spiralColour.B()), ESPInterval(currBGAlpha));
	this->spiralEmitterMed.Tick(dT);
	this->spiralEmitterMed.SetParticleColour(ESPInterval(spiralColour.R()), ESPInterval(spiralColour.G()), ESPInterval(spiralColour.B()), ESPInterval(currBGAlpha));
	this->spiralEmitterLg.Tick(dT);
	this->spiralEmitterLg.SetParticleColour(ESPInterval(spiralColour.R()), ESPInterval(spiralColour.G()), ESPInterval(spiralColour.B()), ESPInterval(currBGAlpha));
	
	GameWorldAssets::Tick(dT);
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
	glTranslatef(-18.0f, -50.0f, -20.0f);
	glRotated(this->beamRotationfg1, 0, 0, 1);
	this->skybeam->Draw(camera, this->beamEffect);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(18.0f, -50.0f, -20.0f);
	glRotated(this->beamRotationfg2, 0, 0, 1);
	this->skybeam->Draw(camera, this->beamEffect);
	glPopMatrix();

	glPopAttrib();
}
