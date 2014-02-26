/**
 * ESPTextureShaderParticle.h
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

#ifndef __ESPTEXTURESHADERPARTICLE_H__
#define __ESPTEXTURESHADERPARTICLE_H__

#include "ESPTextureShaderParticle.h"
#include "../BlammoEngine/CgFxEffect.h"

class ESPTextureShaderParticle : public ESPShaderParticle {
public: 
    ESPTextureShaderParticle(CgFxTextureEffectBase* effect, const std::vector<Texture2D*>& textures) : ESPShaderParticle(effect), textures(textures) {
        assert(!textures.empty());
        this->SelectRandomTexture();
    }
    ~ESPTextureShaderParticle() {}

    void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	void Draw(const Camera& camera, const ESP::ESPAlignment& alignment);

private:
    int currSelectedTexIdx;
    std::vector<Texture2D*> textures;
    
    void SelectRandomTexture();

	// Disallow copy and assign
    DISALLOW_COPY_AND_ASSIGN(ESPTextureShaderParticle);
};

inline void ESPTextureShaderParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
    ESPShaderParticle::Revive(pos, vel, size, rot, totalLifespan);
    this->SelectRandomTexture();
}

inline void ESPTextureShaderParticle::Draw(const Camera& camera, const ESP::ESPAlignment& alignment) {
    const Texture2D* currTexture = this->textures[this->currSelectedTexIdx];
    static_cast<CgFxTextureEffectBase*>(this->shaderEffect)->SetTexture(currTexture);
    ESPShaderParticle::Draw(camera, alignment);
}

inline void ESPTextureShaderParticle::SelectRandomTexture() {
    this->currSelectedTexIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->textures.size();
}

#endif //__ESPTEXTURESHADERPARTICLE_H__