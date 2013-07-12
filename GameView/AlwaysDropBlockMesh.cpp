/**
 * AlwaysDropBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "AlwaysDropBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/AlwaysDropBlock.h"

#include "../BlammoEngine/Texture2D.h"
#include "../ResourceManager.h"

AlwaysDropBlockMesh::AlwaysDropBlockMesh() :
blockMesh(NULL), dropTypeMatGrp(NULL), dropColourMatGrp(NULL) {
	this->LoadMesh();
}

AlwaysDropBlockMesh::~AlwaysDropBlockMesh() {
	// Make sure we get rid of the textures attached to the material group that shows the item type and colour
	// or we'll end up trying to delete the textures twice
	this->blockMesh->SetTextureForMaterial(GameViewConstants::GetInstance()->ALWAYS_DROP_BLOCK_ITEMTYPE_MATGRP, NULL);

	bool success = ResourceManager::GetInstance()->ReleaseMeshResource(this->blockMesh);
	assert(success);
    UNUSED_VARIABLE(success);
}

void AlwaysDropBlockMesh::AddAlwaysDropBlock(const AlwaysDropBlock* block, Texture* itemTexture) {
	assert(block != NULL);
	assert(itemTexture != NULL);
	std::pair<std::map<const AlwaysDropBlock*, Texture*>::iterator, bool> insertResult =
		this->blockToItemTexMap.insert(std::make_pair(block, itemTexture));
	assert(insertResult.second);
}

void AlwaysDropBlockMesh::RemoveAlwaysDropBlock(const AlwaysDropBlock* block) {
	assert(block != NULL);
	std::map<const AlwaysDropBlock*, Texture*>::iterator findIter = this->blockToItemTexMap.find(block);
	assert(findIter != this->blockToItemTexMap.end());
	this->blockToItemTexMap.erase(findIter);
}

void AlwaysDropBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                               const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    UNUSED_PARAMETER(dT);

	glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
	
    glColor3f(1.0f, 1.0f, 1.0f);

	// Go through all the always drop blocks, set the proper materials and draw
	for (std::map<const AlwaysDropBlock*, Texture*>::const_iterator iter = this->blockToItemTexMap.begin(); 
		iter != this->blockToItemTexMap.end(); ++iter) {
        
		const AlwaysDropBlock* currBlock = iter->first;
		Texture* currTex = iter->second;

		CgFxMaterialEffect* itemTexMatEffect = this->dropTypeMatGrp->GetMaterial();
		MaterialProperties* itemTexMatProps  = itemTexMatEffect->GetProperties();
		itemTexMatProps->diffuseTexture = currTex;

        CgFxMaterialEffect* itemColourMatEffect = this->dropColourMatGrp->GetMaterial();
        MaterialProperties* itemColourMatProps  = itemColourMatEffect->GetProperties();
        itemColourMatProps->diffuse = GameViewConstants::GetInstance()->GetItemColourFromDisposition(currBlock->GetNextDropItemDisposition());

		const Point2D& blockCenter = currBlock->GetCenter();
		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
        this->blockMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

	glPopAttrib();
}

void AlwaysDropBlockMesh::SetAlphaMultiplier(float alpha) {
	this->dropTypeMatGrp->GetMaterial()->GetProperties()->alphaMultiplier   = alpha;
    this->dropColourMatGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
}

void AlwaysDropBlockMesh::LoadMesh() {
	assert(this->blockMesh == NULL);
	assert(this->dropTypeMatGrp == NULL);
	assert(this->dropColourMatGrp == NULL);

	this->blockMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ALWAYS_DROP_BLOCK_MESH);

	// Set the base material group map (which never changes its texture) and also set the item drop type material group; 
	// which changes its texture on a per-block basis depending on what the next item type it will drop will be...
	const std::map<std::string, MaterialGroup*>& matGrpMap = this->blockMesh->GetMaterialGroups();
	
    std::map<std::string, MaterialGroup*>::const_iterator findIter;
    
    findIter = matGrpMap.find(GameViewConstants::GetInstance()->ALWAYS_DROP_BLOCK_ITEMTYPE_MATGRP);
	assert(findIter != matGrpMap.end());
	this->dropTypeMatGrp = findIter->second;

    findIter = matGrpMap.find(GameViewConstants::GetInstance()->ALWAYS_DROP_BLOCK_COLOUR_MATGRP);
	assert(findIter != matGrpMap.end());
	this->dropColourMatGrp = findIter->second;

	assert(this->blockMesh != NULL);
	assert(this->dropTypeMatGrp != NULL);
	assert(this->dropColourMatGrp != NULL);
}