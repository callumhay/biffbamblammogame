/**
 * ESPRandomTextureParticle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPRANDOMTEXTUREPARTICLE_H__
#define __ESPRANDOMTEXTUREPARTICLE_H__

#include "ESPParticle.h"

class ESPRandomTextureParticle : public ESPParticle {
public:
    ESPRandomTextureParticle(const std::vector<Texture2D*>& textures);
	~ESPRandomTextureParticle();

	void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	void Draw(const Camera& camera, const ESP::ESPAlignment alignment);

private:
    int currSelectedTexIdx;
    std::vector<Texture2D*> textures;

    void SelectRandomTexture();

    DISALLOW_COPY_AND_ASSIGN(ESPRandomTextureParticle);
};

#endif // __ESPRANDOMTEXTUREPARTICLE_H__