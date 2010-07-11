#ifndef __PRISMBLOCKMESH_H__
#define __PRISMBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "CgFxPrism.h"

/**
 * Wraps up the mesh for the prism block and ensures it has the appropriately
 * casted shader so that the scene texture can be set onthefly for it.
 */
class PrismBlockMesh {

public:
	enum PrismBlockType { DiamondPrism, TrianglePrism };

	PrismBlockMesh(PrismBlockType type);
	~PrismBlockMesh();

	/**
	 * Set the scene texture for the material used to draw the sticky goo -
	 * this texture is used to draw the refractive portion of the material.
	 */
	inline void SetSceneTexture(const Texture2D* sceneTex) {
		assert(sceneTex != NULL);
		this->prismEffect->SetSceneTexture(sceneTex);
	}

	inline const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const {
		return this->prismBlockGeometry->GetMaterialGroups();
	}

private:
	Mesh* prismBlockGeometry;
	CgFxPrism* prismEffect;

	void LoadMesh(PrismBlockType type);

};
#endif // __PRISMBLOCKMESH_H__