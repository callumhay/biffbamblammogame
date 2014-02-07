/**
 * OneWayBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ONEWAYBLOCKMESH_H__
#define __ONEWAYBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "../GameModel/OneWayBlock.h"

class OneWayBlockMesh {
public:
    OneWayBlockMesh();
    ~OneWayBlockMesh();

    void Flush();
    void AddOneWayBlock(const OneWayBlock* block);
    void RemoveOneWayBlock(const OneWayBlock* block);

    void IceCubeStatusAdded(const LevelPiece* block);
    void IceCubeStatusRemoved(const LevelPiece* block);

    void DrawRegularPass(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawTransparentNoBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);

    void SetAlphaMultiplier(float alpha);

private:
    Mesh* oneWayUpBlock;
    Mesh* oneWayDownBlock;
    Mesh* oneWayLeftBlock;
    Mesh* oneWayRightBlock;

    std::map<const OneWayBlock*, Mesh*> oneWayBlocks;
    std::map<const LevelPiece*, Mesh*> frozenOneWayBlocks;
    std::map<const LevelPiece*, Mesh*> notFrozenOneWayBlocks;

    void LoadMesh();
    Mesh* GetMeshForOneWayType(OneWayBlock::OneWayDir dir) const;

    DISALLOW_COPY_AND_ASSIGN(OneWayBlockMesh);
};

inline void OneWayBlockMesh::Flush() {
    this->oneWayBlocks.clear();
    this->frozenOneWayBlocks.clear();
    this->notFrozenOneWayBlocks.clear();
}

inline void OneWayBlockMesh::AddOneWayBlock(const OneWayBlock* block) {
    Mesh* oneWayBlockMesh = this->GetMeshForOneWayType(block->GetDirType());
    this->oneWayBlocks.insert(std::make_pair(block, oneWayBlockMesh));

    if (block->HasStatus(LevelPiece::IceCubeStatus)) {
        this->frozenOneWayBlocks.insert(std::make_pair(block, oneWayBlockMesh));
    }
    else {
        this->notFrozenOneWayBlocks.insert(std::make_pair(block, oneWayBlockMesh));
    }
}

inline void OneWayBlockMesh::RemoveOneWayBlock(const OneWayBlock* block) {
    this->oneWayBlocks.erase(block);
    this->frozenOneWayBlocks.erase(block);
    this->notFrozenOneWayBlocks.erase(block);
}

inline void OneWayBlockMesh::IceCubeStatusAdded(const LevelPiece* block) {
    
    std::map<const LevelPiece*, Mesh*>::iterator findIter = this->notFrozenOneWayBlocks.find(block);
    if (findIter != this->notFrozenOneWayBlocks.end()) {
        this->frozenOneWayBlocks.insert(*findIter);
        this->notFrozenOneWayBlocks.erase(findIter);
    }
}

inline void OneWayBlockMesh::IceCubeStatusRemoved(const LevelPiece* block) {

    std::map<const LevelPiece*, Mesh*>::iterator findIter = this->frozenOneWayBlocks.find(block);
    if (findIter != this->frozenOneWayBlocks.end()) {
        this->notFrozenOneWayBlocks.insert(*findIter);
        this->frozenOneWayBlocks.erase(findIter);
    }
}

#endif // __ONEWAYBLOCKMESH_H__