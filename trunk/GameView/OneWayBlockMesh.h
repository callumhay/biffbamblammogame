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

    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);

    void SetAlphaMultiplier(float alpha);

private:
    Mesh* oneWayUpBlock;
    Mesh* oneWayDownBlock;
    Mesh* oneWayLeftBlock;
    Mesh* oneWayRightBlock;

    std::map<const OneWayBlock*, Mesh*> oneWayBlocks;

    void LoadMesh();
    Mesh* GetMeshForOneWayType(OneWayBlock::OneWayDir dir) const;

    DISALLOW_COPY_AND_ASSIGN(OneWayBlockMesh);
};

inline void OneWayBlockMesh::Flush() {
    this->oneWayBlocks.clear();
}

inline void OneWayBlockMesh::AddOneWayBlock(const OneWayBlock* block) {
    this->oneWayBlocks.insert(std::make_pair(block, GetMeshForOneWayType(block->GetDirType())));
}

inline void OneWayBlockMesh::RemoveOneWayBlock(const OneWayBlock* block) {
    this->oneWayBlocks.erase(block);
}

#endif // __ONEWAYBLOCKMESH_H__