#ifndef __ITEMDROPBLOCKMESH_H__
#define __ITEMDROPBLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

class ItemDropBlock;

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

	std::map<const ItemDropBlock*, Texture*> itemDropBlockToItemTexMap;
	MaterialGroup* itemDropTypeMatGrp;
	
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