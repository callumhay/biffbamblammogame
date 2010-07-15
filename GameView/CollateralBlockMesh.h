#ifndef __COLLATERALBLOCKMESH_H__
#define __COLLATERALBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"

class CollateralBlock;

class CollateralBlockMesh {
public:
	CollateralBlockMesh();
	~CollateralBlockMesh();

	void Flush();
	void AddCollateralBlock(const CollateralBlock* collateralBlock);
	void RemoveCollateralBlock(const CollateralBlock* collateralBlock);

	void SetWorldTranslation(const Vector3D& t);
	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;

private:
	Mesh* collateralBlockGeometry;
	Vector3D currWorldTranslation;
	std::set<const CollateralBlock*> collateralBlocks;	// A list of all the collateral blocks that are currently present in the game

	void LoadMesh();
};

inline void CollateralBlockMesh::Flush() {
	this->collateralBlocks.clear();
}

inline void CollateralBlockMesh::AddCollateralBlock(const CollateralBlock* collateralBlock) {
	this->collateralBlocks.insert(collateralBlock);
}

inline void CollateralBlockMesh::RemoveCollateralBlock(const CollateralBlock* collateralBlock) {
	size_t numRemoved = this->collateralBlocks.erase(collateralBlock);
	assert(numRemoved == 1);
}

inline void CollateralBlockMesh::SetWorldTranslation(const Vector3D& t) {
	this->currWorldTranslation = t;
}

#endif // __COLLATERALBLOCKMESH_H__