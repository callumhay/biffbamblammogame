/**
 * ItemDropBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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