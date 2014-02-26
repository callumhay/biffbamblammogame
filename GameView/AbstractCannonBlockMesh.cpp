/**
 * AbstractCannonBlockMesh.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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
