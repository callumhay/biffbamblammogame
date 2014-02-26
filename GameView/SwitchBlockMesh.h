/**
 * SwitchBlockMesh.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __SWITCHBLOCKMESH_H__
#define __SWITCHBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Texture2D.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"

class SwitchBlock;
class LevelPiece;
class ESPPointEmitter;

class SwitchBlockMesh {
public:
    SwitchBlockMesh();
    ~SwitchBlockMesh();

    void Flush();

	void AddSwitchBlock(const SwitchBlock* switchBlock);
	void RemoveSwitchBlock(const SwitchBlock* switchBlock);

    void SwitchBlockActivated(const SwitchBlock* switchBlock, const LevelPiece* triggeredPiece);

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void SetAlphaMultiplier(float alpha);

private:
    class SwitchConnection {
    public:
        SwitchConnection(const SwitchBlock* switchBlock, const LevelPiece* triggeredPiece);
        ~SwitchConnection();
        bool Draw(double dT, const Camera& camera);

    private:
        std::vector<Point2D> points;
        AnimationLerp<float> alphaAnim;

        Texture2D* glowBitTexture;
        Texture2D* sparkleTexture;

        ESPPointEmitter* glowEmitter1;
        ESPPointEmitter* glowEmitter2;

        ESPParticleColourEffector particleFader;
        ESPParticleScaleEffector particleGrower;

        ESPPointEmitter* BuildGlowEmitter(const Point2D& position, Texture2D* texture);

        DISALLOW_COPY_AND_ASSIGN(SwitchConnection);
    };


    Mesh* switchBlockGeometry;
    std::map<std::string, MaterialGroup*> baseMaterialGrp;
    std::set<const SwitchBlock*> switchBlocks;
    std::list<SwitchConnection*> activeConnections;

    MaterialGroup* switchCurrentMaterialGrp;
    MaterialGroup* switchOnMaterialGrp;
    MaterialGroup* switchOffMaterialGrp;

    Texture2D* greenOnSwitchTexture;
    Texture2D* redOnSwitchTexture;
    Texture2D* offSwitchTexture;

	// Effect variables
    ESPPointEmitter* onEmitter;
	ESPParticleScaleEffector haloExpandPulse;
	ESPParticleColourEffector haloFader;
	Texture2D* haloTexture;

    ESPPointEmitter* idlePulseEmitter;
    ESPParticleScaleEffector pulser;
    Texture2D* pulseGlowTexture;

    void LoadMesh();
    void InitEmitters();

    DISALLOW_COPY_AND_ASSIGN(SwitchBlockMesh);
};

inline void SwitchBlockMesh::AddSwitchBlock(const SwitchBlock* switchBlock) {
    assert(switchBlock != NULL);
    std::pair<std::set<const SwitchBlock*>::iterator, bool> insertResult = 
        this->switchBlocks.insert(switchBlock);
    assert(insertResult.second);
}

inline void SwitchBlockMesh::RemoveSwitchBlock(const SwitchBlock* switchBlock) {
    assert(switchBlock != NULL);
    std::set<const SwitchBlock*>::iterator findIter = this->switchBlocks.find(switchBlock);
    assert(findIter != this->switchBlocks.end());
    this->switchBlocks.erase(findIter);
}

inline void SwitchBlockMesh::SwitchBlockActivated(const SwitchBlock* switchBlock, const LevelPiece* triggeredPiece) {
    this->activeConnections.push_back(new SwitchBlockMesh::SwitchConnection(switchBlock, triggeredPiece));
}

inline const std::map<std::string, MaterialGroup*>& SwitchBlockMesh::GetMaterialGroups() const {
    return this->baseMaterialGrp;
}

#endif //__SWITCHBLOCKMESH_H__