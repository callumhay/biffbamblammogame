/**
 * CollateralBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;

	void SetAlphaMultiplier(float alpha);

private:
	Mesh* collateralBlockGeometry;
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
	this->collateralBlocks.erase(collateralBlock);
}

#endif // __COLLATERALBLOCKMESH_H__