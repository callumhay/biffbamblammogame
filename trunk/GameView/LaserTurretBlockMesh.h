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

#include "../ESPEngine/ESPParticleScaleEffector.h"

class LaserTurretBlock;

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

private:
    std::set<const LaserTurretBlock*> blocks;  // A list of all the laser turret blocks that are currently present in the game

	std::map<std::string, MaterialGroup*> materialGroups; // Material groups for the static parts of the laser turret block mesh
	
    // Meshes for the two barrels of the laser turret block - these are
    // kept separate for animation purposes
    Mesh* barrel1Mesh;
    Mesh* barrel2Mesh;
    Mesh* headMesh;
    Mesh* baseMesh;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(LaserTurretBlockMesh);
};

inline void LaserTurretBlockMesh::Flush() {
	this->blocks.clear();
}

inline void LaserTurretBlockMesh::AddLaserTurretBlock(const LaserTurretBlock* laserTurretBlock) {
	assert(laserTurretBlock != NULL);
	std::pair<std::set<const LaserTurretBlock*>::iterator, bool> insertResult =
        this->blocks.insert(laserTurretBlock);
	assert(insertResult.second);
}

inline void LaserTurretBlockMesh::RemoveLaserTurretBlock(const LaserTurretBlock* laserTurretBlock) {
	size_t numRemoved = this->blocks.erase(laserTurretBlock);
    UNUSED_VARIABLE(numRemoved);
	assert(numRemoved == 1);
}

inline const std::map<std::string, MaterialGroup*>& LaserTurretBlockMesh::GetMaterialGroups() const {
	return this->materialGroups;
}

inline void LaserTurretBlockMesh::SetAlphaMultiplier(float alpha) {
    this->barrel1Mesh->SetAlpha(alpha);
    this->barrel2Mesh->SetAlpha(alpha);
}

#endif // __LASERTURRETBLOCKMESH_H__