/**
 * CannonBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CANNONBLOCKMESH_H__
#define __CANNONBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "AbstractCannonBlockMesh.h"

class CannonBlock;
class ESPPointEmitter;

class CannonBlockMesh : public AbstractCannonBlockMesh {
public:
	CannonBlockMesh();
	~CannonBlockMesh();

	void Flush();
	
    void AddCannonBlock(const CannonBlock* cannonBlock);
	void RemoveCannonBlock(const CannonBlock* cannonBlock);

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;
	
	void SetAlphaMultiplier(float alpha);

private:
	Mesh* cannonBlockBaseGeometry;
	Mesh* cannonBlockBarrelGeometry;

	std::set<const CannonBlock*> cannonBlocks;	// A list of all the cannon blocks that are currently present in the game

	void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(CannonBlockMesh);
};

inline void CannonBlockMesh::Flush() {
	this->cannonBlocks.clear();
}

inline void CannonBlockMesh::AddCannonBlock(const CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);
	this->cannonBlocks.insert(cannonBlock);
}

inline void CannonBlockMesh::RemoveCannonBlock(const CannonBlock* cannonBlock) {
    assert(cannonBlock != NULL);
	size_t numRemoved = this->cannonBlocks.erase(cannonBlock);
    UNUSED_VARIABLE(numRemoved);
	assert(numRemoved == 1);
}

inline void CannonBlockMesh::SetAlphaMultiplier(float alpha) {
    this->cannonBlockBarrelGeometry->SetAlpha(alpha);
    this->cannonBlockBaseGeometry->SetAlpha(alpha);
}

#endif // __CANNONBLOCKMESH_H__