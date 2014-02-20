/**
 * AbstractCannonBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "AbstractCannonBlockMesh.h"
#include "GameViewConstants.h"

#include "../ESPEngine/ESPPointEmitter.h"
#include "../GameModel/CannonBlock.h"
#include "../ResourceManager.h"

AbstractCannonBlockMesh::AbstractCannonBlockMesh() : haloTexture(NULL),
haloExpandPulse(1.0f, 2.75f), attentionExpandPulse(1.0f, 2.0f), 
haloFader(1.0f, 0.10f), attentionFader(1.0f, 0.0f),
activeCannonEffectEmitter(NULL) {

    assert(this->haloTexture == NULL);
    this->haloTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
    assert(this->haloTexture != NULL);

    // Halo effect that pulses outwards
    this->activeCannonEffectEmitter = new ESPPointEmitter();
    this->activeCannonEffectEmitter->SetSpawnDelta(ESPInterval(0.5f));
    this->activeCannonEffectEmitter->SetInitialSpd(ESPInterval(0));
    this->activeCannonEffectEmitter->SetParticleLife(ESPInterval(0.5f));
    this->activeCannonEffectEmitter->SetParticleSize(ESPInterval(CannonBlock::CANNON_BARREL_LENGTH));
    this->activeCannonEffectEmitter->SetEmitAngleInDegrees(0);
    this->activeCannonEffectEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->activeCannonEffectEmitter->SetParticleAlignment(ESP::ScreenAligned);
    this->activeCannonEffectEmitter->SetEmitPosition(Point3D(0,0,0));
    this->activeCannonEffectEmitter->SetParticleColour(
        ESPInterval(0.75f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
    this->activeCannonEffectEmitter->AddEffector(&this->haloExpandPulse);
    this->activeCannonEffectEmitter->AddEffector(&this->haloFader);
    this->activeCannonEffectEmitter->SetParticles(1, this->haloTexture);

    this->ballCamAttentionEffectEmitter = new ESPPointEmitter();
    this->ballCamAttentionEffectEmitter->SetSpawnDelta(ESPInterval(1.25f));
    this->ballCamAttentionEffectEmitter->SetInitialSpd(ESPInterval(0));
    this->ballCamAttentionEffectEmitter->SetParticleLife(ESPInterval(1.25f));
    this->ballCamAttentionEffectEmitter->SetParticleSize(ESPInterval(1.5f*CannonBlock::CANNON_BARREL_LENGTH));
    this->ballCamAttentionEffectEmitter->SetEmitAngleInDegrees(0);
    this->ballCamAttentionEffectEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->ballCamAttentionEffectEmitter->SetParticleAlignment(ESP::ScreenAligned);
    this->ballCamAttentionEffectEmitter->SetEmitPosition(Point3D(0,0,0));
    this->ballCamAttentionEffectEmitter->SetParticleColour(
        ESPInterval(0.75f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
    this->ballCamAttentionEffectEmitter->AddEffector(&this->attentionExpandPulse);
    this->ballCamAttentionEffectEmitter->AddEffector(&this->attentionFader);
    this->ballCamAttentionEffectEmitter->SetParticles(1, this->haloTexture);
}

AbstractCannonBlockMesh::~AbstractCannonBlockMesh() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTexture);
    assert(success);
    UNUSED_VARIABLE(success);

    delete this->activeCannonEffectEmitter;
    this->activeCannonEffectEmitter = NULL;
    delete this->ballCamAttentionEffectEmitter;
    this->ballCamAttentionEffectEmitter = NULL;
}
