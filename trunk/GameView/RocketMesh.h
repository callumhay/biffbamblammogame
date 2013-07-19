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
#include "../ESPEngine/ESP.h"

#include "CgFxVolumetricEffect.h"

class Mesh;
class RocketProjectile;
class Camera;
class BasicPointLight;
class PlayerPaddle;
class ESPPointEmitter;
class Texture2D;
class PaddleRemoteControlRocketProjectile;

/**
 * Class that holds and draws the various rocket bullet meshes.
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

    void ApplyRocketThrust(const PaddleRemoteControlRocketProjectile& rocket);
    void ResetRemoteControlRocketEmitters();

private:
	ESPParticleScaleEffector pulseEffector;
	Texture2D* glowTex;
	ESPPointEmitter* rocketGlowEmitter;

    std::vector<Texture2D*> smokeTextures;
    Texture2D* sparkTex;
    ESPParticleColourEffector particleFader;
    ESPParticleColourEffector particleBurstColourEffector;
    ESPParticleScaleEffector particleLargeGrowth;
    ESPParticleRotateEffector loopRotateEffectorCW;
    ESPParticleRotateEffector loopRotateEffectorCCW;
    ESPPointEmitter* rocketThrustBurstEmitter;
    ESPPointEmitter* rocketThrustingSparksEmitter;

    CgFxVolumetricEffect fireEffect;
    ESPParticleColourEffector hyperBurnColourFader;
    ESPPointEmitter* rocketThrustHyperBurnEmitter;


	Mesh* paddleRocketMesh;
    Mesh* paddleRemoteControlRocketMesh;
    Mesh* turretRocketMesh;
	std::set<const RocketProjectile*> rocketProjectiles;
	
    void DrawBasicRocket(const RocketProjectile* rocket, Mesh* rocketMesh, const Camera& camera, 
        const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawRemoteControlRocket(const PaddleRemoteControlRocketProjectile* rocket, double dT, const Camera& camera, 
        const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);

	void LoadMeshes();
};

#endif // __ROCKETMESH_H__