/**
 * ESPTextureShaderParticle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
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