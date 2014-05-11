/**
 * MineMeshManager.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MINEMESHMANAGER_H__
#define __MINEMESHMANAGER_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESP.h"

#include "CgFxPostRefract.h"

class MineProjectile;
class PlayerPaddle;

class MineMeshManager {
public:
    MineMeshManager();
    ~MineMeshManager();

    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const Texture2D* sceneTex);
    void DrawLoadingMine(double dT, const PlayerPaddle& paddle, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const Texture2D* sceneTex);
    
    void AddMineProjectile(const MineProjectile* mine);
    void RemoveMineProjectile(const MineProjectile* mine);

private:
    Mesh* mineMesh;

    // Assets and emitters for each mine
    Texture2D* trailTexture;
    Texture2D* pulseTexture;
    Texture2D* triggeredTexture;

    CgFxPostRefract invisibleEffect;

    double timeSinceLastMineLaunch;

    class MineInstance {
    public:
        MineInstance(MineMeshManager* manager, const MineProjectile* mine, Texture2D* trailTexture,
            Texture2D* pulseTexture, Texture2D* triggeredTexture);
        ~MineInstance();

        void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
            const BasicPointLight& fillLight, const BasicPointLight& ballLight);

    private:
        MineMeshManager* manager;

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
        new MineInstance(this, mine, this->trailTexture, this->pulseTexture, this->triggeredTexture)));
    this->timeSinceLastMineLaunch = 0.0;
}

inline void MineMeshManager::RemoveMineProjectile(const MineProjectile* mine) {
    MineInstanceMapIter findIter = this->mineInstanceMap.find(mine);
    assert(findIter != this->mineInstanceMap.end());
    delete findIter->second;
    this->mineInstanceMap.erase(findIter);
}

#endif // __MINEMESHMANAGER_H__