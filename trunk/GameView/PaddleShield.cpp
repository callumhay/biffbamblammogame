/**
 * PaddleShield.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleShield.h"
#include "CgFxPostRefract.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/GeometryMaker.h"
#include "../ESPEngine/ESPPointEmitter.h"
#include "../GameModel/PlayerPaddle.h"
#include "../ResourceManager.h"

const double PaddleShield::ACTIVATING_TIME		= 0.4f;	// Time it takes for the shield to become fully active/sustained
const double PaddleShield::DEACTIVATING_TIME	= 0.4f; // Time it takes to go from sustained to completely deactivated

PaddleShield::PaddleShield() : SHIELD_COLOUR(0.7f, 0.8f, 1.0f), particleFader(1.0f, 0.0f), particleShrink(1.0f, 0.25f),
state(PaddleShield::Deactivated), sphereNormalsTex(NULL), haloTex(NULL), shieldRefractEffect(NULL), sparkleTex(NULL), 
particleGrowth(1.0f, 1.8f), particleEnergyInAndOut(NULL), alpha(1.0f) {
	this->sphereNormalsTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPHERE_NORMALS, Texture::Trilinear));
	assert(this->sphereNormalsTex != NULL);
	this->sparkleTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
	assert(this->sparkleTex != NULL);
	this->haloTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
	assert(this->haloTex != NULL);

	// TODO: change this to use noise as well...
	this->shieldRefractEffect = new CgFxPostRefract();
	this->shieldRefractEffect->SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_WITH_NOISE_TECHNIQUE_NAME);
	this->shieldRefractEffect->SetWarpAmountParam(30.0f);
	this->shieldRefractEffect->SetIndexOfRefraction(1.33f);
	this->shieldRefractEffect->SetScale(0.02f);
	this->shieldRefractEffect->SetFrequency(0.08f);
	this->shieldRefractEffect->SetNormalTexture(this->sphereNormalsTex);

	// Set up the emitter that will suck in a whole bunch of sparkley particles when activating the shield
	this->particleEnergyInAndOut = new ESPPointEmitter();
	this->particleEnergyInAndOut->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	this->particleEnergyInAndOut->SetInitialSpd(ESPInterval(5.5f, 8.0f));
	this->particleEnergyInAndOut->SetParticleLife(ESPInterval(1.25f, 1.8f));
	this->particleEnergyInAndOut->SetParticleSize(ESPInterval(2.0f, 4.0f));
	this->particleEnergyInAndOut->SetParticleColour(
        ESPInterval(SHIELD_COLOUR.R(), 1.0f),
        ESPInterval(SHIELD_COLOUR.G(), 1.0f), 
        ESPInterval(SHIELD_COLOUR.B(), 1.0f),
        ESPInterval(1.0f));

	this->particleEnergyInAndOut->SetEmitAngleInDegrees(180);
	this->particleEnergyInAndOut->SetParticleAlignment(ESP::ScreenAligned);
	this->particleEnergyInAndOut->SetEmitPosition(Point3D(0, 0, 0));
	this->particleEnergyInAndOut->AddEffector(&this->particleFader);
	this->particleEnergyInAndOut->AddEffector(&this->particleShrink);
	this->particleEnergyInAndOut->SetParticles(35, this->sparkleTex);


    std::vector<double> timeVals;
    timeVals.reserve(3);
    timeVals.push_back(0.0);
    timeVals.push_back(1.0);
    timeVals.push_back(2.0);
    std::vector<float> glowVals;
    glowVals.reserve(timeVals.size());
    glowVals.push_back(0.0f);
    glowVals.push_back(0.3f);
    glowVals.push_back(0.0f);

    this->shieldGlowAnimation.SetInterpolantValue(0.0f);
    this->shieldGlowAnimation.SetLerp(timeVals, glowVals);
    this->shieldGlowAnimation.SetRepeat(true);
}

PaddleShield::~PaddleShield() {
	delete this->shieldRefractEffect;
	this->shieldRefractEffect = NULL;
	delete this->particleEnergyInAndOut;
	this->particleEnergyInAndOut = NULL;

	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sphereNormalsTex);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTex);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTex);
	assert(success);
}

void PaddleShield::ActivateShield(const PlayerPaddle& paddle, const Texture2D& bgTexture) {
	// Reset any of the emitters that will be used to draw / animate the shield
	this->particleEnergyInAndOut->SetRadiusDeviationFromCenter(ESPInterval(0.0f, paddle.GetHalfWidthTotal()));
	this->particleEnergyInAndOut->SetIsReversed(true);
	this->particleEnergyInAndOut->Reset();

	this->shieldSizeAnimation.SetLerp(PaddleShield::ACTIVATING_TIME, 1.0f);
	this->shieldSizeAnimation.SetRepeat(false);
	this->shieldSizeAnimation.SetInterpolantValue(0.0f);

	this->shieldRefractEffect->SetFBOTexture(&bgTexture);

	// Set the state for the next tick/draw call
	this->state = PaddleShield::Activating;
}

void PaddleShield::DeactivateShield() {
	if (this->state == PaddleShield::Deactivated || this->state == PaddleShield::Deactivating) {
		return;
	}

	this->particleEnergyInAndOut->SetIsReversed(false);
	this->particleEnergyInAndOut->Reset();

	this->shieldSizeAnimation.SetLerp(PaddleShield::DEACTIVATING_TIME, 0.0f);
	this->shieldSizeAnimation.SetRepeat(false);
	
	// Set the state to deactivating for the next tick/draw call
	this->state = PaddleShield::Deactivating;
}

void PaddleShield::DrawAndTick(const PlayerPaddle& paddle, const Camera& camera, double dT) {
	switch (this->state) {

		case PaddleShield::Deactivated:
			// Draw/Do nothing
			break;

		case PaddleShield::Activating: {
				// Tick all of the proper timers/animations
				this->particleEnergyInAndOut->Tick(dT);
                this->shieldGlowAnimation.Tick(dT);
				this->shieldRefractEffect->AddToTimer(dT);
				bool isDead = this->particleEnergyInAndOut->IsDead();
				isDead &= this->shieldSizeAnimation.Tick(dT); 
				
				// Do expanding halo and fade-in shield refraction...
				this->DrawRefractionWithAura(paddle, camera, dT);
				this->particleEnergyInAndOut->Draw(camera);

				// Check to see if all timers/animations are done - if so move on to the next state
				if (isDead) {
					this->state = PaddleShield::Sustained;
				}
			}
			break;

		case PaddleShield::Sustained:
			// Just keep drawing the refraction bubble with the aura(s) around it
            this->shieldGlowAnimation.Tick(dT);
			this->shieldRefractEffect->AddToTimer(dT);
			this->DrawRefractionWithAura(paddle, camera, dT);
			break;

		case PaddleShield::Deactivating: {
				this->shieldGlowAnimation.Tick(dT);
				this->shieldRefractEffect->AddToTimer(dT);
				this->particleEnergyInAndOut->Tick(dT);
				bool isDead = this->particleEnergyInAndOut->IsDead(); 
				isDead &= this->shieldSizeAnimation.Tick(dT); 
	
				this->DrawRefractionWithAura(paddle, camera, dT);
				this->particleEnergyInAndOut->Draw(camera);

				// Check to see if we're done all deactivating animations, if we are
				// finish by changing to a fully deactivated state
				if (isDead) {
					this->state = PaddleShield::Deactivated;
				}
			}
			break;

		default:
			assert(false);
			break;
	}
}

// Draw the shield effect in all of its refracty-aura-glowing awesomeness
void PaddleShield::DrawRefractionWithAura(const PlayerPaddle& paddle, const Camera& camera, double dT) {
	UNUSED_PARAMETER(dT);

	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);

	float scaleAmt = 2.25f * paddle.GetHalfWidthTotal() * this->shieldSizeAnimation.GetInterpolantValue();

	Vector3D alignNormalVec = -camera.GetNormalizedViewVector();
	Vector3D alignUpVec	    = camera.GetNormalizedUpVector();
	Vector3D alignRightVec	= Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
	Matrix4x4 screenAlignMatrix(alignRightVec, alignUpVec, alignNormalVec);

	glPushMatrix();
	glMultMatrixf(screenAlignMatrix.begin());
	glScalef(scaleAmt, scaleAmt, scaleAmt);

	glColor4f(1.0f, 1.0f, 1.0f, this->alpha * 0.9f);
	this->shieldRefractEffect->Draw(camera, GeometryMaker::GetInstance()->GetQuadDL());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float colourAdd = this->shieldGlowAnimation.GetInterpolantValue();
    Colour currColour = SHIELD_COLOUR + Colour(colourAdd, colourAdd, colourAdd);
	glColor4f(currColour.R(), currColour.G(), currColour.B(), this->alpha);

	this->haloTex->BindTexture();
	GeometryMaker::GetInstance()->DrawQuad();

	glPopMatrix();
	glPopAttrib();
}