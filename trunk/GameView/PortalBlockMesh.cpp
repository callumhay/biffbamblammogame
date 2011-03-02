/**
 * PortalBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PortalBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/LevelPiece.h"
#include "../ESPEngine/ESPPointEmitter.h"
#include "../ResourceManager.h"

static const float HALO_FADER_START = 1.0f;
static const float HALO_FADER_END   = 0.15f;

PortalBlockMesh::PortalBlockMesh() : portalBlockGeometry(NULL), portalEffect(NULL), haloTexture(NULL),
haloExpandPulse(1.0f, 3.0f), haloFader(HALO_FADER_START, HALO_FADER_END) {
	this->LoadMesh();
	assert(this->portalBlockGeometry != NULL);
	assert(this->portalEffect != NULL);

	assert(this->haloTexture == NULL);
	this->haloTexture = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
	assert(this->haloTexture != NULL);

}

PortalBlockMesh::~PortalBlockMesh() {
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTexture);
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
	haloExpandingPulse->SetParticleSize(ESPInterval(LevelPiece::HALF_PIECE_WIDTH), ESPInterval(LevelPiece::HALF_PIECE_HEIGHT));
	haloExpandingPulse->SetEmitAngleInDegrees(0);
	haloExpandingPulse->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	haloExpandingPulse->SetParticleAlignment(ESP::ScreenAligned);
	haloExpandingPulse->SetEmitPosition(worldTranslation);
	haloExpandingPulse->SetParticleColour(ESPInterval(colour.R()), ESPInterval(colour.G()), ESPInterval(colour.B()), ESPInterval(1.0f));
	haloExpandingPulse->AddEffector(&this->haloExpandPulse);
	haloExpandingPulse->AddEffector(&this->haloFader);
	bool result = haloExpandingPulse->SetParticles(NUM_HALOS, this->haloTexture);
	assert(result);
	portalEmitters.push_back(haloExpandingPulse);

	return portalEmitters;
}

void PortalBlockMesh::SetAlphaMultiplier(float alpha) {
	this->haloFader.SetStartAlpha(HALO_FADER_START * alpha);
	this->haloFader.SetEndAlpha(HALO_FADER_END * alpha);
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
	this->portalEffect = dynamic_cast<CgFxPortalBlock*>(portalMatGrp->GetMaterial());
}