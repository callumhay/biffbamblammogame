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
auraEnergyOut(NULL), particleGrowth(1.0f, 1.8f), particleEnergyInAndOut(NULL) {
	this->sphereNormalsTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPHERE_NORMALS, Texture::Trilinear));
	assert(this->sphereNormalsTex != NULL);
	this->sparkleTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
	assert(this->sparkleTex != NULL);
	this->haloTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
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
	this->particleEnergyInAndOut->SetParticleColour(ESPInterval(SHIELD_COLOUR.R(), 1.0f), 
																						 ESPInterval(SHIELD_COLOUR.G(), 1.0f), 
																						 ESPInterval(SHIELD_COLOUR.B(), 1.0f), ESPInterval(1.0f));
	this->particleEnergyInAndOut->SetEmitAngleInDegrees(180);
	this->particleEnergyInAndOut->SetAsPointSpriteEmitter(true);
	this->particleEnergyInAndOut->SetParticleAlignment(ESP::ScreenAligned);
	
	this->particleEnergyInAndOut->SetEmitPosition(Point3D(0, 0, 0));
	this->particleEnergyInAndOut->AddEffector(&this->particleFader);
	this->particleEnergyInAndOut->AddEffector(&this->particleShrink);
	bool result = this->particleEnergyInAndOut->SetParticles(35, this->sparkleTex);
	assert(result);

	const float HALO_LIFETIME = 2.3f;
	const int NUM_HALOS = 3;
	this->auraEnergyOut = new ESPPointEmitter();
	this->auraEnergyOut->SetSpawnDelta(ESPInterval(HALO_LIFETIME / static_cast<float>(NUM_HALOS)));
	this->auraEnergyOut->SetInitialSpd(ESPInterval(0));
	this->auraEnergyOut->SetParticleLife(ESPInterval(HALO_LIFETIME));
	this->auraEnergyOut->SetEmitAngleInDegrees(0);
	this->auraEnergyOut->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->auraEnergyOut->SetParticleAlignment(ESP::ScreenAligned);
	this->auraEnergyOut->SetEmitPosition(Point3D(0, 0, 0));
	this->auraEnergyOut->SetParticleColour(ESPInterval(0.5f, 0.7f), ESPInterval(0.75f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->auraEnergyOut->AddEffector(&this->particleGrowth);
	this->auraEnergyOut->AddEffector(&this->particleFader);
	result = this->auraEnergyOut->SetParticles(NUM_HALOS, this->haloTex);
	assert(result);
}

PaddleShield::~PaddleShield() {
	delete this->shieldRefractEffect;
	this->shieldRefractEffect = NULL;
	delete this->particleEnergyInAndOut;
	this->particleEnergyInAndOut = NULL;
	delete this->auraEnergyOut;
	this->auraEnergyOut = NULL;

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
				this->auraEnergyOut->Tick(dT);
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
			this->auraEnergyOut->Tick(dT);
			this->shieldRefractEffect->AddToTimer(dT);
			this->DrawRefractionWithAura(paddle, camera, dT);
			break;

		case PaddleShield::Deactivating: {
				this->auraEnergyOut->Tick(dT);
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
	this->auraEnergyOut->SetParticleSize(ESPInterval(scaleAmt));


	Vector3D alignNormalVec = -camera.GetNormalizedViewVector();
	Vector3D alignUpVec		  = camera.GetNormalizedUpVector();
	Vector3D alignRightVec	= Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
	Matrix4x4 screenAlignMatrix(alignRightVec, alignUpVec, alignNormalVec);

	glPushMatrix();
	glMultMatrixf(screenAlignMatrix.begin());
	glScalef(scaleAmt, scaleAmt, scaleAmt);

	glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	this->shieldRefractEffect->Draw(camera, GeometryMaker::GetInstance()->GetQuadDL());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(SHIELD_COLOUR.R(), SHIELD_COLOUR.G(), SHIELD_COLOUR.B(), 1.0f);

	this->haloTex->BindTexture();
	GeometryMaker::GetInstance()->DrawQuad();

	glPopMatrix();

	this->auraEnergyOut->Draw(camera);
	glPopAttrib();
}