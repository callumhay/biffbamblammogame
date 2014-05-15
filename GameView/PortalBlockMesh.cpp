/**
 * PortalBlockMesh.cpp
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

void PortalBlockMesh::CreatePortalBlockEmitters(const Colour& colour, const Point3D &worldTranslation, 
                                                ESPParticleScaleEffector& haloExpandPulse, ESPParticleColourEffector& haloFader, Texture2D* haloTexture,
                                                ESPParticleColourEffector& particleFader, ESPParticleScaleEffector& particleMediumGrowth,
                                                Texture2D* circleTex, Texture2D* hoopTex, std::list<ESPEmitter*>& emitters) {

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
	haloExpandingPulse->SetParticleAlignment(ESP::GlobalAxisAlignedX);
	haloExpandingPulse->SetEmitPosition(worldTranslation);
	haloExpandingPulse->SetParticleColour(ESPInterval(colour.R()), ESPInterval(colour.G()), ESPInterval(colour.B()), ESPInterval(1.0f));
	haloExpandingPulse->AddEffector(&haloExpandPulse);
	haloExpandingPulse->AddEffector(&haloFader);
	haloExpandingPulse->SetParticles(NUM_HALOS, haloTexture);

    std::vector<Texture2D*> particleTextures(2, NULL);
    particleTextures[0] = circleTex;
    particleTextures[1] = hoopTex;

    ESPInterval rColour(colour.R(), std::min<float>(1.0f, 1.75f*(0.25f+colour.R())));
    ESPInterval gColour(colour.G(), std::min<float>(1.0f, 1.75f*(0.25f+colour.G())));
    ESPInterval bColour(colour.B(), std::min<float>(1.0f, 1.75f*(0.25f+colour.B())));

    ESPPointEmitter* particlesComing = new ESPPointEmitter();
    particlesComing->SetSpawnDelta(ESPInterval(0.03f, 0.06f));
    particlesComing->SetInitialSpd(ESPInterval(1.0f, 2.4f));
    particlesComing->SetParticleLife(ESPInterval(0.7f, 1.2f));
    particlesComing->SetParticleSize(ESPInterval(0.1f*LevelPiece::PIECE_HEIGHT, 0.33f*LevelPiece::PIECE_HEIGHT));
    particlesComing->SetParticleColour(rColour, gColour, bColour, ESPInterval(1.0f));
    particlesComing->SetEmitAngleInDegrees(180);
    particlesComing->SetParticleAlignment(ESP::ScreenPlaneAligned);
    particlesComing->SetRadiusDeviationFromCenter(
        ESPInterval(0.0f, 0.4f*LevelPiece::HALF_PIECE_WIDTH), 
        ESPInterval(0.0f, 0.4f*LevelPiece::HALF_PIECE_HEIGHT), 
        ESPInterval(0.0f, 0.4f*LevelPiece::HALF_PIECE_DEPTH));
    particlesComing->SetIsReversed(true);
    particlesComing->SetEmitPosition(worldTranslation);
    particlesComing->AddEffector(&particleFader);
    particlesComing->AddEffector(&particleMediumGrowth);
    particlesComing->SetRandomTextureParticles(9, particleTextures);

    ESPPointEmitter* particlesGoing  = new ESPPointEmitter();
    particlesGoing->SetSpawnDelta(ESPInterval(0.03f, 0.06f));
    particlesGoing->SetInitialSpd(ESPInterval(1.0f, 2.4f));
    particlesGoing->SetParticleLife(ESPInterval(0.8f, 1.2f));
    particlesGoing->SetParticleSize(ESPInterval(0.1f*LevelPiece::PIECE_HEIGHT, 0.33f*LevelPiece::PIECE_HEIGHT));
    particlesGoing->SetParticleColour(rColour, gColour, bColour, ESPInterval(1.0f));
    particlesGoing->SetEmitAngleInDegrees(180);
    particlesGoing->SetParticleAlignment(ESP::ScreenPlaneAligned);
    particlesGoing->SetRadiusDeviationFromCenter(
        ESPInterval(0.0f, 0.5f*LevelPiece::HALF_PIECE_WIDTH), 
        ESPInterval(0.0f, 0.5f*LevelPiece::HALF_PIECE_HEIGHT), 
        ESPInterval(0.0f, 0.5f*LevelPiece::HALF_PIECE_DEPTH));
    particlesGoing->SetEmitPosition(worldTranslation);
    particlesGoing->AddEffector(&particleFader);
    particlesGoing->AddEffector(&particleMediumGrowth);
    particlesGoing->SetRandomTextureParticles(9, particleTextures);

	emitters.push_back(haloExpandingPulse);
    emitters.push_back(particlesComing);
    emitters.push_back(particlesGoing);
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