#include "DecoWorldAssets.h"
#include "GameViewConstants.h"
#include "DecoSkybox.h"
#include "CgFxVolumetricEffect.h"

#include "../BlammoEngine/Skybox.h"
#include "../BlammoEngine/ResourceManager.h"

// Basic constructor: Load all the basic assets for the deco world...
DecoWorldAssets::DecoWorldAssets() : 
GameWorldAssets(DecoSkybox::CreateDecoSkybox(GameViewConstants::GetInstance()->SKYBOX_MESH),
								ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BACKGROUND_MESH),
								ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_PADDLE_MESH),
								ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_BLOCK_MESH_PATH)),
skybeam(ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->DECO_SKYBEAM_MESH)),
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
rotationStatebg2(RotateCCW)
{
	// Setup the beam effect
	this->beamEffect->SetColour(Colour(1, 1, 1));
	this->beamEffect->SetConstantFactor(0.0f);
	this->beamEffect->SetFadeExponent(2.0f);
	this->beamEffect->SetScale(0.05f);
	this->beamEffect->SetFrequency(5.0f);
	this->beamEffect->SetAlphaMultiplier(0.60f);
}

DecoWorldAssets::~DecoWorldAssets() {
	// Clean up deco specific background assets:
	// Beam effect
	delete this->beamEffect;
	this->beamEffect = NULL;
}

void DecoWorldAssets::Tick(double dT) {
	// Add to the timer in the beam shader
	this->beamEffect->AddToTimer(dT);
	// Rotate the background effect (sky beams)
	this->RotateSkybeams(dT);

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

void DecoWorldAssets::DrawBackgroundModel(const Camera& camera) {
	Colour currSkyboxColour = this->skybox->GetCurrentColour();
	Colour currBGModelColour = currSkyboxColour.GetComplementaryColour();
	
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), 1.0f);
	this->background->Draw(camera, this->bgKeyLight, this->bgFillLight);
}

void DecoWorldAssets::DrawBackgroundEffects(const Camera& camera) {
	// Draw deco background beams:
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
}