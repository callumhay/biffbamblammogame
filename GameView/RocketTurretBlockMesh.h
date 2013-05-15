/**
 * RocketTurretBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ROCKETTURRETBLOCKMESH_H__
#define __ROCKETTURRETBLOCKMESH_H__

#include "TurretBlockMesh.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESP.h"

#include "../GameModel/RocketTurretBlock.h"

// A holder for the mesh assets of the RocketTurretBlock, this class holds all of the current
// level's blocks and draws the interactive parts of them.
class RocketTurretBlockMesh : public TurretBlockMesh {
public:
	RocketTurretBlockMesh();
	~RocketTurretBlockMesh();

	void Flush();
	void AddRocketTurretBlock(const RocketTurretBlock* rocketTurretBlock);
	void RemoveRocketTurretBlock(const RocketTurretBlock* rocketTurretBlock);

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void SetAlphaMultiplier(float alpha);

    void AIStateChanged(const RocketTurretBlock* block, const RocketTurretBlock::TurretAIState& oldState,
        const RocketTurretBlock::TurretAIState& newState);
    void RocketShotByBlock(const RocketTurretBlock* block);

private:
    class BlockData {
    public:
        BlockData(const RocketTurretBlock& block, Texture2D* glowTexture,
            Texture2D* sparkleTexture, std::vector<Texture2D*>& smokeTextures);
        ~BlockData();

        void DrawBlockEffects(double dT, const Camera& camera, float lightPulseAmt);
        float GetFlashIntensity() const;

        void BlockStateChanged(const RocketTurretBlock::TurretAIState& oldState,
            const RocketTurretBlock::TurretAIState& newState);
        void RocketShotByBlock();

        void SetAlpha(float alpha);

    private:
        const RocketTurretBlock& block;

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

    typedef std::map<const RocketTurretBlock*, BlockData*> BlockCollection;
    typedef BlockCollection::iterator BlockCollectionIter;
    typedef BlockCollection::const_iterator BlockCollectionConstIter;
    
    BlockCollection blocks;  // A list of all the laser turret blocks that are currently present in the game

    // Meshes for the rocket turret block
    Mesh* barrelMesh;
    Mesh* headMesh;
    Mesh* rocketMesh;

    AnimationMultiLerp<float> lightPulseAnim;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(RocketTurretBlockMesh);
};


inline float RocketTurretBlockMesh::BlockData::GetFlashIntensity() const {
    return this->redColourMultiplierAnim.GetInterpolantValue();
}

#endif // __ROCKETTURRETBLOCKMESH_H__