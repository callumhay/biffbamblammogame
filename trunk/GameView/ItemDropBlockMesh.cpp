
#include "ItemDropBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/ItemDropBlock.h"

#include "../ResourceManager.h"

ItemDropBlockMesh::ItemDropBlockMesh() : blockMesh(NULL), itemDropTypeMatGrp(NULL) {
	this->LoadMesh();
}

ItemDropBlockMesh::~ItemDropBlockMesh() {
	// Make sure we get rid of the texture attached to the material group that shows the item type
	// or we'll end up trying to delete the texture twice
	this->blockMesh->SetTextureForMaterial(GameViewConstants::GetInstance()->ITEM_DROP_BLOCK_ITEMTYPE_MATGRP, NULL);

	bool success = ResourceManager::GetInstance()->ReleaseMeshResource(this->blockMesh);
	assert(success);
}


void ItemDropBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
														 const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

	glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	// Go through all the item drop blocks, set the proper materials and draw
	for (std::map<const ItemDropBlock*, Texture*>::const_iterator iter = this->itemDropBlockToItemTexMap.begin(); 
		iter != this->itemDropBlockToItemTexMap.end(); ++iter) {

		// We draw only the material part of the block that indicates the next item that will be dropped...
		const ItemDropBlock* currItemDropBlock = iter->first;
		Texture* currTex = iter->second;

		CgFxMaterialEffect* matEffect = this->itemDropTypeMatGrp->GetMaterial();
		MaterialProperties* matProps  = matEffect->GetProperties();
		matProps->diffuseTexture = currTex;

		const Point2D& blockCenter = currItemDropBlock->GetCenter();
		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
		this->itemDropTypeMatGrp->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}

	glPopAttrib();
}

void ItemDropBlockMesh::SetAlphaMultiplier(float alpha) {
	const std::map<std::string, MaterialGroup*>&  matGrps = this->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = matGrps.begin(); iter != matGrps.end(); ++iter) {
		MaterialGroup* matGrp = iter->second;
		matGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
	}
	
	this->itemDropTypeMatGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
}

void ItemDropBlockMesh::LoadMesh() {
	assert(this->blockMesh == NULL);
	assert(this->itemDropTypeMatGrp == NULL);
	assert(baseMaterialGrp.empty());

	this->blockMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ITEM_DROP_BLOCK_MESH);

	// Set the base material group map (which never changes its texture) and also set the item drop type material group; 
	// which changes its texture on a per-block basis depending on what the next item type it will drop will be...
	this->baseMaterialGrp = this->blockMesh->GetMaterialGroups();
	std::map<std::string, MaterialGroup*>::iterator findIter = this->baseMaterialGrp.find(GameViewConstants::GetInstance()->ITEM_DROP_BLOCK_ITEMTYPE_MATGRP);
	assert(findIter != this->baseMaterialGrp.end());
	
	this->itemDropTypeMatGrp = findIter->second;
	this->baseMaterialGrp.erase(findIter);

	assert(this->blockMesh != NULL);
	assert(this->itemDropTypeMatGrp != NULL);
	assert(!baseMaterialGrp.empty());
}