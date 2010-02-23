#ifndef __PORTAL_BLOCK_MESH_H__
#define __PORTAL_BLOCK_MESH_H__

#include "../BlammoEngine/Mesh.h"
#include "CgFxPortalBlock.h"

/**
 * Wraps up the mesh for the portal block and ensures it has the appropriately
 * casted shader so that the scene texture can be set onthefly for it.
 */
class PortalBlockMesh {

public:
	PortalBlockMesh();
	~PortalBlockMesh();

	/**
	 * Set the scene texture for the material used to draw the portal.
	 */
	inline void SetSceneTexture(const Texture2D* sceneTex) {
		assert(sceneTex != NULL);
		this->portalEffect->SetFBOTexture(sceneTex);
	}

	inline const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const {
		return this->portalBlockGeometry->GetMaterialGroups();
	}

	inline void Tick(double dT) {
		this->portalEffect->AddToTimer(dT);
	}

private:
	Mesh* portalBlockGeometry;
	CgFxPortalBlock* portalEffect;

	void LoadMesh();

};
#endif // __PORTAL_BLOCK_MESH_H__