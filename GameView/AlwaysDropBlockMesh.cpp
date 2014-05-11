/**
 * AlwaysDropBlockMesh.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

void AlwaysDropBlockMesh::DrawBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                        const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    UNUSED_PARAMETER(dT);

    glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Go through all the always drop blocks, set the proper materials and draw
    for (std::map<const AlwaysDropBlock*, Texture*>::const_iterator iter = this->blockToItemTexMap.begin(); 
        iter != this->blockToItemTexMap.end(); ++iter) {

            const AlwaysDropBlock* currBlock = iter->first;
            if (!currBlock->HasStatus(LevelPiece::IceCubeStatus) && !currBlock->HasStatus(LevelPiece::OnFireStatus)) {
                continue;
            }

            Texture* currTex = iter->second;
            this->Draw(currBlock, currTex, camera, keyLight, fillLight, ballLight);
    }

    glPopAttrib();
}

void AlwaysDropBlockMesh::DrawNoBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                                          const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    UNUSED_PARAMETER(dT);

	glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Go through all the always drop blocks, set the proper materials and draw
	for (std::map<const AlwaysDropBlock*, Texture*>::const_iterator iter = this->blockToItemTexMap.begin(); 
		 iter != this->blockToItemTexMap.end(); ++iter) {
        
		const AlwaysDropBlock* currBlock = iter->first;
        if (currBlock->HasStatus(LevelPiece::IceCubeStatus) || currBlock->HasStatus(LevelPiece::OnFireStatus)) {
            continue;
        }

		Texture* currTex = iter->second;
        this->Draw(currBlock, currTex, camera, keyLight, fillLight, ballLight);
    }

	glPopAttrib();
}

void AlwaysDropBlockMesh::SetAlphaMultiplier(float alpha) {
	this->dropTypeMatGrp->GetMaterial()->GetProperties()->alphaMultiplier   = alpha;
    this->dropColourMatGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
}

void AlwaysDropBlockMesh::Draw(const AlwaysDropBlock* currBlock, Texture* currTex, const Camera& camera, const BasicPointLight& keyLight,
                               const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    CgFxAbstractMaterialEffect* itemTexMatEffect = this->dropTypeMatGrp->GetMaterial();
    itemTexMatEffect->SetDiffuseTexture(currTex);

    CgFxAbstractMaterialEffect* itemColourMatEffect = this->dropColourMatGrp->GetMaterial();
    itemColourMatEffect->SetDiffuseColour(GameViewConstants::GetInstance()->GetItemColourFromDisposition(currBlock->GetNextDropItemDisposition()));

    const Point2D& blockCenter = currBlock->GetCenter();
    glPushMatrix();
    glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
    this->blockMesh->Draw(camera, keyLight, fillLight, ballLight);
    glPopMatrix();
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