/**
 * RocketMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ROCKETMESH_H__
#define __ROCKETMESH_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "../ESPEngine/ESPParticleScaleEffector.h"

class Mesh;
class RocketProjectile;
class Camera;
class BasicPointLight;
class PlayerPaddle;
class ESPPointEmitter;
class Texture2D;

/**
 * Class that holds the paddle rocket bullet mesh - this will be drawn
 * only when there is a paddle rocket projectile in play.
 */
class RocketMesh {
public:
	RocketMesh();
	~RocketMesh();

	void ActivateRocket(const RocketProjectile* rocketProjectile);
	void DeactivateRocket(const RocketProjectile* rocketProjectile);
    bool NoActiveRockets() const { return this->rocketProjectiles.empty(); }

	void Draw(double dT, const PlayerPaddle& paddle, const Camera& camera, const BasicPointLight& keyLight, 
	          const BasicPointLight& fillLight, const BasicPointLight& ballLight);

private:
	ESPParticleScaleEffector pulseEffector;
	Texture2D* glowTex;
	ESPPointEmitter* rocketGlowEmitter;

	Mesh* paddleRocketMesh;
    Mesh* turretRocketMesh;
	std::set<const RocketProjectile*> rocketProjectiles;
	
	void LoadMesh();
};

// Activates the paddle rocket mesh so that it gets drawn based on the given projectile
inline void RocketMesh::ActivateRocket(const RocketProjectile* rocketProjectile) {
	std::pair<std::set<const RocketProjectile*>::iterator, bool> insertResult = this->rocketProjectiles.insert(rocketProjectile);
	assert(insertResult.second);
}

// Deactivates the rocket projectile - stops it from being drawn until next time it is activated
inline void RocketMesh::DeactivateRocket(const RocketProjectile* rocketProjectile) {
	size_t numErased = this->rocketProjectiles.erase(rocketProjectile);
    UNUSED_VARIABLE(numErased);
	assert(numErased == 1);
}

#endif // __ROCKETMESH_H__