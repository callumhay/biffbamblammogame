/**
 * FragileCannonBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __FRAGILECANNONBLOCKMESH_H__
#define __FRAGILECANNONBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "AbstractCannonBlockMesh.h"

class FragileCannonBlock;


class FragileCannonBlockMesh : public AbstractCannonBlockMesh {
public:
    FragileCannonBlockMesh();
    ~FragileCannonBlockMesh();

    void Flush();

    void AddFragileCannonBlock(const FragileCannonBlock* block);
    void RemoveFragileCannonBlock(const FragileCannonBlock* block);

    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;

    void SetAlphaMultiplier(float alpha);

private:
    Mesh* basePostMesh; 
    Mesh* baseBarMesh;
    Mesh* barrelMesh;

    std::set<const FragileCannonBlock*> cannonBlocks;
    void DrawBlockMesh(float rotDegs, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(FragileCannonBlockMesh);
};

inline void FragileCannonBlockMesh::Flush() {
    this->cannonBlocks.clear();
}

inline void FragileCannonBlockMesh::AddFragileCannonBlock(const FragileCannonBlock* block) {
    assert(block != NULL);
    this->cannonBlocks.insert(block);
}

inline void FragileCannonBlockMesh::RemoveFragileCannonBlock(const FragileCannonBlock* block) {
    assert(block != NULL);
    size_t numRemoved = this->cannonBlocks.erase(block);
    UNUSED_VARIABLE(numRemoved);
    assert(numRemoved == 1);
}

inline void FragileCannonBlockMesh::SetAlphaMultiplier(float alpha) {
    this->basePostMesh->SetAlpha(alpha);
    this->baseBarMesh->SetAlpha(alpha);
    this->barrelMesh->SetAlpha(alpha);
}

#endif // __FRAGILECANNONBLOCKMESH_H__