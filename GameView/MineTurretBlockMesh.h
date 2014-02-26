/**
 * MineTurretBlockMesh.h
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

#ifndef __MINETURRETBLOCKMESH_H__
#define __MINETURRETBLOCKMESH_H__

#include "TurretBlockMesh.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESP.h"

#include "../GameModel/MineTurretBlock.h"

// A holder for the mesh assets of the MineTurretBlock, this class holds all of the current
// level's blocks and draws the interactive parts of them.
class MineTurretBlockMesh : public TurretBlockMesh {

public:
	MineTurretBlockMesh();
	~MineTurretBlockMesh();

	void Flush();
	void AddMineTurretBlock(const MineTurretBlock* mineTurretBlock);
	void RemoveMineTurretBlock(const MineTurretBlock* mineTurretBlock);

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawPostEffects(double dT, const Camera& camera);
	void SetAlphaMultiplier(float alpha);

    void AIStateChanged(const MineTurretBlock* block, const MineTurretBlock::TurretAIState& oldState,
        const MineTurretBlock::TurretAIState& newState);
    void MineShotByBlock(const MineTurretBlock* block);

private:
    class BlockData {
    public:
        BlockData(const MineTurretBlock& block, Texture2D* glowTexture,
            Texture2D* sparkleTexture, std::vector<Texture2D*>& smokeTextures);
        ~BlockData();

        void DrawBlockEffects(double dT, const Camera& camera);
        void DrawBlockPostEffects(double dT, const Camera& camera, float lightPulseAmt);
        float GetFlashIntensity() const;

        void BlockStateChanged(const MineTurretBlock::TurretAIState& oldState,
            const MineTurretBlock::TurretAIState& newState);
        void MineShotByBlock();

        void SetAlpha(float alpha);

    private:
        const MineTurretBlock& block;

        float alpha;

        AnimationMultiLerp<float> redColourMultiplierAnim;
        
        ESPPointEmitter* fireySmokeEmitter;
        ESPPointEmitter* smokeySmokeEmitter;

        Texture2D* glowTexture;
        Texture2D* sparkleTexture;

        TextLabel2D* emoteLabel;
        AnimationMultiLerp<float> emoteScaleAnim;

        ESPParticleScaleEffector  particleLargeGrowth;
        ESPParticleScaleEffector  particleMediumGrowth;
        ESPParticleColourEffector smokeColourFader;
	    ESPParticleRotateEffector rotateEffectorCW;
	    ESPParticleRotateEffector rotateEffectorCCW;
        ESPParticleColourEffector particleFireColourFader;

        void DrawLights(float pulse);

        static void InitTimeValueEmoteAnimVectors(std::vector<double>& times, std::vector<float>& values, float lifeTimeInSecs);
    };

    typedef std::map<const MineTurretBlock*, BlockData*> BlockCollection;
    typedef BlockCollection::iterator BlockCollectionIter;
    typedef BlockCollection::const_iterator BlockCollectionConstIter;
    
    BlockCollection blocks;  // A list of all the laser turret blocks that are currently present in the game

    // Meshes for the mine turret block
    Mesh* barrelMesh;
    Mesh* headMesh;
    Mesh* mineMesh;

    AnimationMultiLerp<float> lightPulseAnim;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(MineTurretBlockMesh);
};

inline float MineTurretBlockMesh::BlockData::GetFlashIntensity() const {
    return this->redColourMultiplierAnim.GetInterpolantValue();
}

#endif // __MINETURRETBLOCKMESH_H__