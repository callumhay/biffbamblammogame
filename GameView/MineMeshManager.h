/**
 * MineMeshManager.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MINEMESHMANAGER_H__
#define __MINEMESHMANAGER_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"

class PaddleMineProjectile;

class MineMeshManager {
public:
    MineMeshManager();
    ~MineMeshManager();

    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    
    void AddMineProjectile(const PaddleMineProjectile* mine);
    void RemoveMineProjectile(const PaddleMineProjectile* mine);

private:
    Mesh* mineMesh;
    
    class MineInstance {
    public:
        MineInstance(const PaddleMineProjectile* mine) : mine(mine) { assert(mine != NULL); };
        ~MineInstance() {};

        void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
            const BasicPointLight& fillLight, const BasicPointLight& ballLight, Mesh* mineMesh);

    private:
        const PaddleMineProjectile* mine;
        AnimationMultiLerp<float> glowAnim;
    };
    typedef std::map<const PaddleMineProjectile*, MineInstance*> MineInstanceMap;
    typedef MineInstanceMap::iterator MineInstanceMapIter;
    typedef MineInstanceMap::const_iterator MineInstanceMapConstIter;
    MineInstanceMap mineInstanceMap;


    DISALLOW_COPY_AND_ASSIGN(MineMeshManager);
};

inline void MineMeshManager::AddMineProjectile(const PaddleMineProjectile* mine) {
    assert(this->mineInstanceMap.find(mine) == this->mineInstanceMap.end());
    this->mineInstanceMap.insert(std::make_pair(mine, new MineInstance(mine)));
}

inline void MineMeshManager::RemoveMineProjectile(const PaddleMineProjectile* mine) {
    MineInstanceMapIter findIter = this->mineInstanceMap.find(mine);
    assert(findIter != this->mineInstanceMap.end());
    delete findIter->second;
    this->mineInstanceMap.erase(findIter);
}

#endif // __MINEMESHMANAGER_H__