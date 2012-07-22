/**
 * CannonBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CANNONBLOCKMESH_H__
#define __CANNONBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Texture2D.h"

#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"

class CannonBlock;
class ESPPointEmitter;

class CannonBlockMesh {
public:

	CannonBlockMesh();
	~CannonBlockMesh();

	void Flush();
	void AddCannonBlock(const CannonBlock* cannonBlock);
	void RemoveCannonBlock(const CannonBlock* cannonBlock);
	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight, bool lightsAreOff) const;
	
	void SetAlphaMultiplier(float alpha);

private:
	Mesh* cannonBlockBaseGeometry;
	Mesh* cannonBlockBarrelGeometry;
	std::map<std::string, MaterialGroup*> materialGroups;

	std::set<const CannonBlock*> cannonBlocks;	// A list of all the cannon blocks that are currently present in the game

	// Effects for drawing attention to active cannon blocks
	ESPParticleScaleEffector haloExpandPulse;
	ESPParticleColourEffector haloFader;
	Texture2D* haloTexture;
	ESPPointEmitter* activeCannonEffectEmitter;

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
	size_t numRemoved = this->cannonBlocks.erase(cannonBlock);
    UNUSED_VARIABLE(numRemoved);
	assert(numRemoved == 1);
}

inline const std::map<std::string, MaterialGroup*>& CannonBlockMesh::GetMaterialGroups() const {
	return this->materialGroups;
}

#endif // __CANNONBLOCKMESH_H__