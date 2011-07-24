/**
 * ItemDropBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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

class ItemDropBlockMesh {
public:
	ItemDropBlockMesh();
	~ItemDropBlockMesh();

	void Flush();

	void AddItemDropBlock(const ItemDropBlock* itemDropBlock, Texture* itemTexture);
	void RemoveItemDropBlock(const ItemDropBlock* itemDropBlock);

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);
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
	
	ESPVolumeEmitter* InitSparkEmitter(const Colour& colour);
	void LoadMesh();
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