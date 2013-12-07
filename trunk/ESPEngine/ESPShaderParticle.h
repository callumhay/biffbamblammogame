/**
 * ESPShaderParticle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPSHADERPARTICLE_H__
#define __ESPSHADERPARTICLE_H__

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

#include "../GameModel/Onomatoplex.h"

#include "ESPUtil.h"
#include "ESPParticle.h"

class TextureFontSet;
class CgFxEffectBase;

class ESPShaderParticle : public ESPParticle {
public: 
	ESPShaderParticle(CgFxEffectBase* effect);
	virtual ~ESPShaderParticle();

	virtual void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	void Tick(const double dT);
	virtual void Draw(const Camera& camera, const ESP::ESPAlignment& alignment);

protected:
	// The shader effect for this particle
	CgFxEffectBase* shaderEffect;

private:
    DISALLOW_COPY_AND_ASSIGN(ESPShaderParticle);
};

/**
 * Revive this particle with the given lifespan length in seconds.
 */
inline void ESPShaderParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
	// Set the members to reflect a 'new life'
	ESPParticle::Revive(pos, vel, size, rot, totalLifespan);
}

/**
 * Called each frame with the delta time for that frame, this will
 * provide a slice of the lifetime of the particle.
 */
inline void ESPShaderParticle::Tick(const double dT) {
	ESPParticle::Tick(dT);
}
#endif