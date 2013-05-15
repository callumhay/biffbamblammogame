/**
 * MineMeshManager.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __MINEMESHMANAGER_H__
#define __MINEMESHMANAGER_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESP.h"

class MineProjectile;
class PlayerPaddle;

class MineMeshManager {
public:
    MineMeshManager();
    ~MineMeshManager();

    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawLoadingMine(double dT, const PlayerPaddle& paddle, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    
    void AddMineProjectile(const MineProjectile* mine);
    void RemoveMineProjectile(const MineProjectile* mine);

private:
    Mesh* mineMesh;

    // Assets and emitters for each mine
    Texture2D* trailTexture;
    Texture2D* pulseTexture;
    Texture2D* triggeredTexture;
    
    double timeSinceLastMineLaunch;

    class MineInstance {
    public:
        MineInstance(const MineProjectile* mine, Texture2D* trailTexture,
            Texture2D* pulseTexture, Texture2D* triggeredTexture);
        ~MineInstance();

        void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
            const BasicPointLight& fillLight, const BasicPointLight& ballLight, Mesh* mineMesh);

    private:
        const MineProjectile* mine;
        AnimationMultiLerp<float> glowAnim;
        
        ESPParticleColourEffector trailFader;
        ESPParticleScaleEffector  particleShrinkToNothing;
        
        ESPParticleColourEffector pulseFader;
        ESPParticleScaleEffector pulseGrower;

        ScaleEffect fastPulseSettings;
        ESPParticleScaleEffector fastPulser;

        ESPPointEmitter trailEmitter;
        ESPPointEmitter armedPulseEmitter;
        ESPPointEmitter triggeredPulseEmitter;

    };
    typedef std::map<const MineProjectile*, MineInstance*> MineInstanceMap;
    typedef MineInstanceMap::iterator MineInstanceMapIter;
    typedef MineInstanceMap::const_iterator MineInstanceMapConstIter;
    MineInstanceMap mineInstanceMap;

    void ClearMines();

    DISALLOW_COPY_AND_ASSIGN(MineMeshManager);
};

inline void MineMeshManager::AddMineProjectile(const MineProjectile* mine) {
    assert(this->mineInstanceMap.find(mine) == this->mineInstanceMap.end());
    this->mineInstanceMap.insert(std::make_pair(mine,
        new MineInstance(mine, this->trailTexture, this->pulseTexture, this->triggeredTexture)));
    this->timeSinceLastMineLaunch = 0.0;
}

inline void MineMeshManager::RemoveMineProjectile(const MineProjectile* mine) {
    MineInstanceMapIter findIter = this->mineInstanceMap.find(mine);
    assert(findIter != this->mineInstanceMap.end());
    delete findIter->second;
    this->mineInstanceMap.erase(findIter);
}

#endif // __MINEMESHMANAGER_H__