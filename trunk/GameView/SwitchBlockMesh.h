/**
 * SwitchBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, bool lightsAreOff);
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