/**
 * PaddleRocketMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLEROCKETMESH_H__
#define __PADDLEROCKETMESH_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "../ESPEngine/ESPParticleScaleEffector.h"

class Mesh;
class PaddleRocketProjectile;
class Camera;
class BasicPointLight;
class PlayerPaddle;
class ESPPointEmitter;
class Texture2D;

/**
 * Class that holds the paddle rocket bullet mesh - this will be drawn
 * only when there is a paddle rocket projectile in play.
 */
class PaddleRocketMesh {
public:
	PaddleRocketMesh();
	~PaddleRocketMesh();

	void Activate(const PaddleRocketProjectile* rocketProjectile);
	void Deactivate(const PaddleRocketProjectile* rocketProjectile);

	void Draw(double dT, const PlayerPaddle& paddle, const Camera& camera, const BasicPointLight& keyLight, 
						const BasicPointLight& fillLight, const BasicPointLight& ballLight);

private:
	static const float GLOW_X_SIZE;
	static const float GLOW_Y_SIZE;

	ESPParticleScaleEffector pulseEffector;
	Texture2D* glowTex;
	ESPPointEmitter* rocketGlowEmitter;

	Mesh* rocketMesh;
	std::set<const PaddleRocketProjectile*> rocketProjectiles;
	
	void LoadMesh();
};

// Activates the paddle rocket mesh so that it gets drawn based on the given projectile
inline void PaddleRocketMesh::Activate(const PaddleRocketProjectile* rocketProjectile) {
	std::pair<std::set<const PaddleRocketProjectile*>::iterator, bool> insertResult = this->rocketProjectiles.insert(rocketProjectile);
	assert(insertResult.second);
}

// Deactivates the rocket projectile - stops it from being drawn until next time it is activated
inline void PaddleRocketMesh::Deactivate(const PaddleRocketProjectile* rocketProjectile) {
	size_t numErased = this->rocketProjectiles.erase(rocketProjectile);
    UNUSED_VARIABLE(numErased);
	assert(numErased == 1);
}

#endif // __PADDLEROCKETMESH_H__