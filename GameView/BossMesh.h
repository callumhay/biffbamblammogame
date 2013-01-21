/**
 * BossMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSSMESH_H__
#define __BOSSMESH_H__

#include "../GameModel/GameWorld.h"
#include "../ESPEngine/ESP.h"

class Camera;
class BasicPointLight;
class Boss;
class Texture2D;

/**
 * The abstract superclass for all meshes/visuals of bosses in the game.
 */
class BossMesh {
public:
    BossMesh();
    virtual ~BossMesh();

    static BossMesh* Build(const GameWorld::WorldStyle& style, Boss* boss);
    
    virtual void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) = 0;

protected:
    // Shared visual effects and textures for bosses
    std::vector<Texture2D*> smokeTextures;
    Texture2D* explosionAnimTex;

    ESPParticleScaleEffector  particleLargeGrowth;
    ESPParticleScaleEffector  particleMediumGrowth;
    ESPParticleColourEffector particleFireColourFader;
    //ESPParticleColourEffector smokeColourFader;
	ESPParticleRotateEffector rotateEffectorCW;

    ESPPointEmitter* BuildFireSmokeEmitter(float width, float height);
    ESPPointEmitter* BuildExplodingEmitter(float width, float height);

private:
    DISALLOW_COPY_AND_ASSIGN(BossMesh);
};

#endif // __BOSSMESH_H__