#include "PortalBlockMesh.h"
#include "GameViewConstants.h"

PortalBlockMesh::PortalBlockMesh() : portalBlockGeometry(NULL), portalEffect(NULL) {
	this->LoadMesh();
	assert(this->portalBlockGeometry != NULL);
	assert(this->portalEffect != NULL);
}

PortalBlockMesh::~PortalBlockMesh() {
}

/**
 * Private helper for loading the mesh and material for the prism block.
 */
void PortalBlockMesh::LoadMesh() {
	assert(this->portalBlockGeometry == NULL);
	assert(this->portalEffect == NULL);	

	this->portalBlockGeometry = ResourceManager::GetInstance()->GetPortalBlockMeshResource();
	const std::map<std::string, MaterialGroup*> portalMaterialGrps = this->portalBlockGeometry->GetMaterialGroups();
	assert(portalMaterialGrps.size() == 1);

	MaterialGroup* portalMatGrp = portalMaterialGrps.begin()->second;
	this->portalEffect = dynamic_cast<CgFxPortalBlock*>(portalMatGrp->GetMaterial());
}