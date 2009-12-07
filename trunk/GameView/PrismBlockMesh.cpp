#include "PrismBlockMesh.h"
#include "GameViewConstants.h"

PrismBlockMesh::PrismBlockMesh() : prismBlockGeometry(NULL), prismEffect(NULL) {
	this->LoadMesh();
	assert(this->prismBlockGeometry != NULL);
	assert(this->prismEffect != NULL);
}

PrismBlockMesh::~PrismBlockMesh() {
}

/**
 * Private helper for loading the mesh and material for the prism block.
 */
void PrismBlockMesh::LoadMesh() {
	assert(this->prismBlockGeometry == NULL);
	assert(this->prismEffect == NULL);	

	// Grab prism mesh from the resource manager
	this->prismBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PRISM_BLOCK_MESH);
	const std::map<std::string, MaterialGroup*> prismMaterialGrps = this->prismBlockGeometry->GetMaterialGroups();
	assert(prismMaterialGrps.size() == 1);

	// 
	MaterialGroup* prismMatGrp = prismMaterialGrps.begin()->second;
	this->prismEffect = dynamic_cast<CgFxPrism*>(prismMatGrp->GetMaterial());
}