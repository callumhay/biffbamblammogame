
#include "CannonBlockMesh.h"
#include "GameViewConstants.h"

CannonBlockMesh::CannonBlockMesh() : cannonBlockBaseGeometry(NULL),
cannonBlockBarrelGeometry(NULL) {
	this->LoadMesh();
}

CannonBlockMesh::~CannonBlockMesh() {
}

void CannonBlockMesh::LoadMesh() {
	assert(this->cannonBlockBaseGeometry == NULL);
	assert(this->cannonBlockBarrelGeometry == NULL);
	assert(this->materialGroups.size() == 0);
	
	this->cannonBlockBaseGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CANNON_BLOCK_BASE_MESH);
	assert(this->cannonBlockBaseGeometry != NULL);
	const std::map<std::string, MaterialGroup*>&  baseMatGrps = this->cannonBlockBaseGeometry->GetMaterialGroups();

	this->cannonBlockBarrelGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CANNON_BLOCK_BARREL_MESH);
	assert(this->cannonBlockBarrelGeometry != NULL);
	const std::map<std::string, MaterialGroup*>&  barrelMatGrps = this->cannonBlockBarrelGeometry->GetMaterialGroups();
	
	this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());
	this->materialGroups.insert(barrelMatGrps.begin(), barrelMatGrps.end());
	
	assert(this->materialGroups.size() > 0);
}