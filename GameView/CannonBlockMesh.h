#ifndef __CANNONBLOCKMESH_H__
#define __CANNONBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"

class CannonBlock;

class CannonBlockMesh {
public:

	CannonBlockMesh();
	~CannonBlockMesh();

	void Flush();
	void AddCannonBlock(const CannonBlock* cannonBlock);
	void RemoveCannonBlock(const CannonBlock* cannonBlock);
	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void SetWorldTranslation(const Vector3D& t);
	void Draw(const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;
	
private:
	Vector3D currWorldTranslation;

	Mesh* cannonBlockBaseGeometry;
	Mesh* cannonBlockBarrelGeometry;
	std::map<std::string, MaterialGroup*> materialGroups;

	std::set<const CannonBlock*> cannonBlocks;	// A list of all the cannon blocks that are currently present in the game

	void LoadMesh();
};

inline void CannonBlockMesh::Flush() {
	this->cannonBlocks.clear();
}

inline void CannonBlockMesh::AddCannonBlock(const CannonBlock* cannonBlock) {
	this->cannonBlocks.insert(cannonBlock);
}

inline void CannonBlockMesh::RemoveCannonBlock(const CannonBlock* cannonBlock) {
	size_t numRemoved = this->cannonBlocks.erase(cannonBlock);
	assert(numRemoved == 1);
}

inline const std::map<std::string, MaterialGroup*>& CannonBlockMesh::GetMaterialGroups() const {
	return this->materialGroups;
}

inline void CannonBlockMesh::SetWorldTranslation(const Vector3D& t) {
	this->currWorldTranslation = t;
}

#endif // __CANNONBLOCKMESH_H__