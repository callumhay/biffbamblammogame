/**
 * PortalBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PortalBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/LevelPiece.h"
#include "../ESPEngine/ESPPointEmitter.h"
#include "../ResourceManager.h"

const float PortalBlockMesh::HALO_FADER_START = 1.0f;
const float PortalBlockMesh::HALO_FADER_END   = 0.0f;

const float PortalBlockMesh::PARTICLE_FADER_START = 1.0f;
const float PortalBlockMesh::PARTICLE_FADER_END   = 0.0f;

PortalBlockMesh::PortalBlockMesh() : portalBlockGeometry(NULL), portalEffect(NULL), haloTexture(NULL),
haloExpandPulse(1.0f, 1.5f), haloFader(HALO_FADER_START, HALO_FADER_END), 
particleFader(PARTICLE_FADER_START, PARTICLE_FADER_END), 
particleMediumGrowth(1.0f, 2.0f), circleTex(NULL), hoopTex(NULL) {

	this->LoadMesh();
	assert(this->portalBlockGeometry != NULL);
	assert(this->portalEffect != NULL);

	assert(this->haloTexture == NULL);
	this->haloTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
	assert(this->haloTexture != NULL);

    assert(this->circleTex == NULL);
    this->circleTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE, Texture::Trilinear));
    assert(this->circleTex != NULL);
    
    assert(this->hoopTex == NULL);
    this->hoopTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_OUTLINED_HOOP, Texture::Trilinear));
    assert(this->hoopTex != NULL);
}

PortalBlockMesh::~PortalBlockMesh() {
	bool success = false;
    
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTexture);
    UNUSED_VARIABLE(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->circleTex);
    UNUSED_VARIABLE(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->hoopTex);
    UNUSED_VARIABLE(success);
	assert(success);
}

/**
 * Creates emitter effects for a portal block.
 */
std::list<ESPEmitter*> PortalBlockMesh::CreatePortalBlockEmitters(const Colour& colour, const Point3D &worldTranslation) {
	std::list<ESPEmitter*> portalEmitters;

	// Halo effect that pulses outwards
	const float HALO_LIFETIME = 2.3f;
	const int NUM_HALOS = 3;
	ESPPointEmitter* haloExpandingPulse = new ESPPointEmitter();
	haloExpandingPulse->SetSpawnDelta(ESPInterval(HALO_LIFETIME / static_cast<float>(NUM_HALOS)));
	haloExpandingPulse->SetInitialSpd(ESPInterval(0));
	haloExpandingPulse->SetParticleLife(ESPInterval(HALO_LIFETIME));
	haloExpandingPulse->SetParticleSize(ESPInterval(0.9f*LevelPiece::PIECE_WIDTH), ESPInterval(0.9f*LevelPiece::PIECE_HEIGHT));
	haloExpandingPulse->SetEmitAngleInDegrees(0);
	haloExpandingPulse->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	haloExpandingPulse->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	haloExpandingPulse->SetEmitPosition(worldTranslation);
	haloExpandingPulse->SetParticleColour(ESPInterval(colour.R()), ESPInterval(colour.G()), ESPInterval(colour.B()), ESPInterval(1.0f));
	haloExpandingPulse->AddEffector(&this->haloExpandPulse);
	haloExpandingPulse->AddEffector(&this->haloFader);
	haloExpandingPulse->SetParticles(NUM_HALOS, this->haloTexture);

    std::vector<Texture2D*> particleTextures(2, NULL);
    particleTextures[0] = this->circleTex;
    particleTextures[1] = this->hoopTex;

    ESPInterval rColour(colour.R(), std::min<float>(1.0f, 1.75f*(0.25f+colour.R())));
    ESPInterval gColour(colour.G(), std::min<float>(1.0f, 1.75f*(0.25f+colour.G())));
    ESPInterval bColour(colour.B(), std::min<float>(1.0f, 1.75f*(0.25f+colour.B())));

    ESPPointEmitter* particlesComing = new ESPPointEmitter();
    particlesComing->SetSpawnDelta(ESPInterval(0.02f, 0.05f));
    particlesComing->SetInitialSpd(ESPInterval(1.0f, 2.4f));
    particlesComing->SetParticleLife(ESPInterval(0.7f, 1.2f));
    particlesComing->SetParticleSize(ESPInterval(0.1f*LevelPiece::PIECE_HEIGHT, 0.33f*LevelPiece::PIECE_HEIGHT));
    particlesComing->SetParticleColour(rColour, gColour, bColour, ESPInterval(1.0f));
    particlesComing->SetEmitAngleInDegrees(180);
    particlesComing->SetRadiusDeviationFromCenter(
        ESPInterval(0.0f, 0.4f*LevelPiece::HALF_PIECE_WIDTH), 
        ESPInterval(0.0f, 0.4f*LevelPiece::HALF_PIECE_HEIGHT), 
        ESPInterval(0.0f, 0.4f*LevelPiece::HALF_PIECE_DEPTH));
    particlesComing->SetIsReversed(true);
    particlesComing->SetEmitPosition(worldTranslation);
    particlesComing->AddEffector(&this->particleFader);
    particlesComing->AddEffector(&this->particleMediumGrowth);
    particlesComing->SetRandomTextureParticles(12, particleTextures);

    ESPPointEmitter* particlesGoing  = new ESPPointEmitter();
    particlesGoing->SetSpawnDelta(ESPInterval(0.02f, 0.05f));
    particlesGoing->SetInitialSpd(ESPInterval(1.0f, 2.4f));
    particlesGoing->SetParticleLife(ESPInterval(0.8f, 1.3f));
    particlesGoing->SetParticleSize(ESPInterval(0.1f*LevelPiece::PIECE_HEIGHT, 0.33f*LevelPiece::PIECE_HEIGHT));
    particlesGoing->SetParticleColour(rColour, gColour, bColour, ESPInterval(1.0f));
    particlesGoing->SetEmitAngleInDegrees(180);
    particlesGoing->SetRadiusDeviationFromCenter(
        ESPInterval(0.0f, 0.5f*LevelPiece::HALF_PIECE_WIDTH), 
        ESPInterval(0.0f, 0.5f*LevelPiece::HALF_PIECE_HEIGHT), 
        ESPInterval(0.0f, 0.5f*LevelPiece::HALF_PIECE_DEPTH));
    particlesGoing->SetEmitPosition(worldTranslation);
    particlesGoing->AddEffector(&this->particleFader);
    particlesGoing->AddEffector(&this->particleMediumGrowth);
    particlesGoing->SetRandomTextureParticles(12, particleTextures);

	portalEmitters.push_back(haloExpandingPulse);
    portalEmitters.push_back(particlesComing);
    portalEmitters.push_back(particlesGoing);

	return portalEmitters;
}

void PortalBlockMesh::SetAlphaMultiplier(float alpha) {
	this->haloFader.SetStartAlpha(HALO_FADER_START * alpha);
	this->haloFader.SetEndAlpha(HALO_FADER_END * alpha);

    this->particleFader.SetStartAlpha(PARTICLE_FADER_START * alpha);
    this->particleFader.SetEndAlpha(PARTICLE_FADER_END * alpha);
}

/**
 * Private helper for loading the mesh and material for the prism block.
 */
void PortalBlockMesh::LoadMesh() {
	assert(this->portalBlockGeometry == NULL);
	assert(this->portalEffect == NULL);	

	this->portalBlockGeometry = ResourceManager::GetInstance()->GetPortalBlockMeshResource();
	const std::map<std::string, MaterialGroup*>& portalMaterialGrps = this->portalBlockGeometry->GetMaterialGroups();
	assert(portalMaterialGrps.size() == 1);

	MaterialGroup* portalMatGrp = portalMaterialGrps.begin()->second;
	this->portalEffect = static_cast<CgFxPortalBlock*>(portalMatGrp->GetMaterial());
    assert(this->portalEffect != NULL);
}