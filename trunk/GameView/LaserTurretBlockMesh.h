/**
 * LaserTurretBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LASERTURRETBLOCKMESH_H__
#define __LASERTURRETBLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESP.h"

#include "../GameModel/LaserTurretBlock.h"

// A holder for the mesh assets of the LaserTurretBlock, this class holds all of the current
// level's blocks and draws the interactive parts of them.
class LaserTurretBlockMesh {
public:
	LaserTurretBlockMesh();
	~LaserTurretBlockMesh();

	void Flush();
	void AddLaserTurretBlock(const LaserTurretBlock* laserTurretBlock);
	void RemoveLaserTurretBlock(const LaserTurretBlock* laserTurretBlock);
	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void SetAlphaMultiplier(float alpha);

    void AIStateChanged(const LaserTurretBlock* block, const LaserTurretBlock::TurretAIState& oldState,
        const LaserTurretBlock::TurretAIState& newState);

private:
    class BlockData {
    public:
        BlockData(const LaserTurretBlock& block, Texture2D* glowTexture,
            Texture2D* sparkleTexture, std::vector<Texture2D*>& smokeTextures);
        ~BlockData();

        void DrawBlockEffects(double dT, const Camera& camera, float lightPulseAmt);
        float GetFlashIntensity() const;

        void BlockStateChanged(const LaserTurretBlock::TurretAIState& oldState,
            const LaserTurretBlock::TurretAIState& newState);

        void SetAlpha(float alpha);

    private:
        const LaserTurretBlock& block;

        float alpha;

        AnimationMultiLerp<float> redColourMultiplierAnim;
        ESPPointEmitter* fireySmokeEmitter;
        ESPPointEmitter* smokeySmokeEmitter;
        //ESPPointEmitter* fireEmitter;

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

    typedef std::map<const LaserTurretBlock*, BlockData*> BlockCollection;
    typedef BlockCollection::iterator BlockCollectionIter;
    typedef BlockCollection::const_iterator BlockCollectionConstIter;
    
    BlockCollection blocks;  // A list of all the laser turret blocks that are currently present in the game

	std::map<std::string, MaterialGroup*> materialGroups; // Material groups for the static parts of the laser turret block mesh
	
    // Meshes for the two barrels of the laser turret block - these are
    // kept separate for animation purposes
    Mesh* barrel1Mesh;
    Mesh* barrel2Mesh;
    Mesh* headMesh;
    Mesh* baseMesh;

    std::vector<Texture2D*> smokeTextures;
    Texture2D* glowTexture;
    Texture2D* sparkleTexture;

    AnimationMultiLerp<float> lightPulseAnim;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(LaserTurretBlockMesh);
};

inline const std::map<std::string, MaterialGroup*>& LaserTurretBlockMesh::GetMaterialGroups() const {
	return this->materialGroups;
}

inline float LaserTurretBlockMesh::BlockData::GetFlashIntensity() const {
    return this->redColourMultiplierAnim.GetInterpolantValue();
}

#endif // __LASERTURRETBLOCKMESH_H__