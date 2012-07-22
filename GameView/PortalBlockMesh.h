/**
 * PortalBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PORTAL_BLOCK_MESH_H__
#define __PORTAL_BLOCK_MESH_H__

#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Texture2D.h"

#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"

#include "CgFxPortalBlock.h"

class ESPEmitter;

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

	std::list<ESPEmitter*> CreatePortalBlockEmitters(const Colour& colour, const Point3D &worldTranslation);

	void SetAlphaMultiplier(float alpha);

private:
	// Mesh variables
	Mesh* portalBlockGeometry;
	CgFxPortalBlock* portalEffect;

	// Effect variables
	ESPParticleScaleEffector haloExpandPulse;
	ESPParticleColourEffector haloFader;
	Texture2D* haloTexture;

	void LoadMesh();

};
#endif // __PORTAL_BLOCK_MESH_H__