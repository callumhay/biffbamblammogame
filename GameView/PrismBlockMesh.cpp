/**
 * PrismBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PrismBlockMesh.h"
#include "GameViewConstants.h"

PrismBlockMesh::PrismBlockMesh(PrismBlockType type) : prismBlockGeometry(NULL), prismEffect(NULL) {
	this->LoadMesh(type);
	assert(this->prismBlockGeometry != NULL);
	assert(this->prismEffect != NULL);
}

PrismBlockMesh::~PrismBlockMesh() {
}

/**
 * Private helper for loading the mesh and material for the prism block.
 */
void PrismBlockMesh::LoadMesh(PrismBlockType type) {
	assert(this->prismBlockGeometry == NULL);
	assert(this->prismEffect == NULL);	

	// Grab prism mesh from the resource manager based on the type of prism we're dealing with
	switch (type) {
		case PrismBlockMesh::DiamondPrism:
			this->prismBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PRISM_DIAMOND_BLOCK_MESH);
			break;
		case PrismBlockMesh::TrianglePrism:
			this->prismBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PRISM_TRIANGLE_BLOCK_MESH);
			break;
		default:
			assert(false);
			break;
	}

	const std::map<std::string, MaterialGroup*> prismMaterialGrps = this->prismBlockGeometry->GetMaterialGroups();
	assert(prismMaterialGrps.size() == 1);

	MaterialGroup* prismMatGrp = prismMaterialGrps.begin()->second;
	this->prismEffect = static_cast<CgFxPrism*>(prismMatGrp->GetMaterial());
    assert(this->prismEffect != NULL);
}