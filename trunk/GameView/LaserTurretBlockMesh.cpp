/**
 * LaserTurretBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LaserTurretBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/LaserTurretBlock.h"

#include "../ResourceManager.h"

LaserTurretBlockMesh::LaserTurretBlockMesh() : barrel1Mesh(NULL), barrel2Mesh(NULL),
headMesh(NULL), baseMesh(NULL) {
    this->LoadMesh();
}

LaserTurretBlockMesh::~LaserTurretBlockMesh() {
    bool success = false;
	success = ResourceManager::GetInstance()->ReleaseMeshResource(this->barrel1Mesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->barrel2Mesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->headMesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->baseMesh);
    assert(success);
    UNUSED_VARIABLE(success);
}

void LaserTurretBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    
    if (this->blocks.empty()) {
        return;
    }

	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Go through each block and draw the barrels
    for (std::set<const LaserTurretBlock*>::const_iterator iter = this->blocks.begin();
         iter != this->blocks.end(); ++iter) {

		const LaserTurretBlock* currBlock = *iter;
		const Point2D& blockCenter = currBlock->GetCenter();

		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
        
        // TODO: recoil animation transform...
        this->barrel1Mesh->Draw(camera, keyLight, fillLight, ballLight);
        this->barrel2Mesh->Draw(camera, keyLight, fillLight, ballLight);

        glPopMatrix();
    }

    glPopAttrib();
}

// Loads all the mesh assets for the laser turret block mesh
void LaserTurretBlockMesh::LoadMesh() {
    assert(this->barrel1Mesh == NULL);
	assert(this->barrel2Mesh == NULL);
    assert(this->headMesh == NULL);
	assert(this->baseMesh == NULL);
	assert(this->materialGroups.size() == 0);
	
    // Load the base mesh
    this->baseMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->LASER_TURRET_BASE_MESH);
    assert(this->baseMesh != NULL);
    const std::map<std::string, MaterialGroup*>& baseMatGrps = this->baseMesh->GetMaterialGroups();
    this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());

    // Load the head mesh (holds the turret's barrels)
    this->headMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->LASER_TURRET_HEAD_MESH);
    assert(this->headMesh != NULL);
    const std::map<std::string, MaterialGroup*>& headMatGrps = this->headMesh->GetMaterialGroups();
    this->materialGroups.insert(headMatGrps.begin(), headMatGrps.end());
    
    assert(this->materialGroups.size() > 0);

	// We don't add the turret's barrel material groups, we will draw those manually for each laser turret block
	// so that we can move them around (i.e., for recoil when the turret shoots lasers)
    this->barrel1Mesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->LASER_TURRET_BARREL_1);
    assert(this->barrel1Mesh != NULL);
    this->barrel2Mesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->LASER_TURRET_BARREL_2);
    assert(this->barrel2Mesh != NULL);
}