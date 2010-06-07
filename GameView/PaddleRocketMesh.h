/**
 * PaddleRocketMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLEROCKETMESH_H__
#define __PADDLEROCKETMESH_H__

#include "../BlammoEngine/BasicIncludes.h"

class Mesh;
class PaddleRocketProjectile;
class Camera;
class PointLight;
class PlayerPaddle;

/**
 * Class that holds the paddle rocket bullet mesh - this will be drawn
 * only when there is a paddle rocket projectile in play.
 */
class PaddleRocketMesh {
public:
	PaddleRocketMesh();
	~PaddleRocketMesh();

	void Activate(const PaddleRocketProjectile* rocketProjectile);
	void Deactivate();

	void Draw(const PlayerPaddle& paddle, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight);

private:
	Mesh* rocketMesh;
	const PaddleRocketProjectile* rocketProjectile;
	
	void LoadMesh();
};

// Activates the paddle rocket mesh so that it gets drawn based on the given projectile
inline void PaddleRocketMesh::Activate(const PaddleRocketProjectile* rocketProjectile) {
	this->rocketProjectile = rocketProjectile;
}

// Deactivates the rocket projectile - stops it from being drawn until next time it is activated
inline void PaddleRocketMesh::Deactivate() {
	this->rocketProjectile = NULL;
}

#endif // __PADDLEROCKETMESH_H__