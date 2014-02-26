/**
 * ItemDropBlockMesh.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __ITEMDROPBLOCKMESH_H__
#define __ITEMDROPBLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESPParticleColourEffector.h"

class ItemDropBlock;
class ESPVolumeEmitter;
class Texture2D;
struct ESPInterval;

class ItemDropBlockMesh {
public:
	ItemDropBlockMesh();
	~ItemDropBlockMesh();

	void Flush();

	void AddItemDropBlock(const ItemDropBlock* itemDropBlock, Texture* itemTexture);
	void RemoveItemDropBlock(const ItemDropBlock* itemDropBlock);

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

    void DrawBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void DrawNoBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawEffects(const Vector3D& worldTranslation, double dT, const Camera& camera);

	void SetAlphaMultiplier(float alpha);

	void UpdateItemDropBlockTexture(const ItemDropBlock* itemDropBlock, Texture* itemTexture);

private:
	Mesh* blockMesh;
	std::map<std::string, MaterialGroup*> baseMaterialGrp;

	ESPVolumeEmitter* flareParticleEmitter;
	ESPVolumeEmitter* powerUpSparkEmitter;
	ESPVolumeEmitter* powerNeutralSparkEmitter;
	ESPVolumeEmitter* powerDownSparkEmitter;
	Texture2D* flareTex;
	Texture2D* sparkTex;
	ESPParticleColourEffector particleFader;

	std::map<const ItemDropBlock*, Texture*> itemDropBlockToItemTexMap;
	MaterialGroup* itemDropTypeMatGrp;
	
    void Draw(const ItemDropBlock* currItemDropBlock, Texture* itemTexture, const Camera& camera, 
        const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	ESPVolumeEmitter* InitSparkEmitter(const Colour& colour, const ESPInterval& lifeTime, const ESPInterval& particleSpd);
	void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(ItemDropBlockMesh);
};

inline void ItemDropBlockMesh::Flush() {
	this->itemDropBlockToItemTexMap.clear();
}

inline void ItemDropBlockMesh::AddItemDropBlock(const ItemDropBlock* itemDropBlock, Texture* itemTexture) {
	assert(itemDropBlock != NULL);
	assert(itemTexture != NULL);
	std::pair<std::map<const ItemDropBlock*, Texture*>::iterator, bool> insertResult =
		this->itemDropBlockToItemTexMap.insert(std::make_pair(itemDropBlock, itemTexture));
	assert(insertResult.second);
}

inline void ItemDropBlockMesh::RemoveItemDropBlock(const ItemDropBlock* itemDropBlock) {
	assert(itemDropBlock != NULL);
	std::map<const ItemDropBlock*, Texture*>::iterator findIter = this->itemDropBlockToItemTexMap.find(itemDropBlock);
	assert(findIter != this->itemDropBlockToItemTexMap.end());
	this->itemDropBlockToItemTexMap.erase(findIter);
}

inline const std::map<std::string, MaterialGroup*>& ItemDropBlockMesh::GetMaterialGroups() const {
	return this->baseMaterialGrp;
}

inline void ItemDropBlockMesh::UpdateItemDropBlockTexture(const ItemDropBlock* itemDropBlock, Texture* itemTexture) {
	assert(itemDropBlock != NULL);
	assert(itemTexture != NULL);
	std::map<const ItemDropBlock*, Texture*>::iterator findIter = this->itemDropBlockToItemTexMap.find(itemDropBlock);
	assert(findIter != this->itemDropBlockToItemTexMap.end());
	findIter->second = itemTexture;
}

#endif // __ITEMDROPBLOCKMESH_H__