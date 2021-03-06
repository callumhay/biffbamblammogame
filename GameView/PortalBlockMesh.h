/**
 * PortalBlockMesh.h
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

	void CreatePortalBlockEmitters(const Colour& colour, const Point3D &worldTranslation, std::list<ESPEmitter*>& emitters);
    static void CreatePortalBlockEmitters(const Colour& colour, const Point3D &worldTranslation, 
        ESPParticleScaleEffector& haloExpandPulse, ESPParticleColourEffector& haloFader, Texture2D* haloTexture,
        ESPParticleColourEffector& particleFader, ESPParticleScaleEffector& particleMediumGrowth,
        Texture2D* circleTex, Texture2D* hoopTex, std::list<ESPEmitter*>& emitters);

	void SetAlphaMultiplier(float alpha);

private:
    static const float HALO_FADER_START;
    static const float HALO_FADER_END;
    static const float PARTICLE_FADER_START;
    static const float PARTICLE_FADER_END;

	// Mesh variables
	Mesh* portalBlockGeometry;
	CgFxPortalBlock* portalEffect;

	// Effect variables
	ESPParticleScaleEffector haloExpandPulse;
	ESPParticleColourEffector haloFader;
	Texture2D* haloTexture;

    ESPParticleColourEffector particleFader;
    ESPParticleScaleEffector particleMediumGrowth;
    Texture2D* circleTex;
    Texture2D* hoopTex;

	void LoadMesh();

};

/**
 * Creates emitter effects for a portal block.
 */
inline void PortalBlockMesh::CreatePortalBlockEmitters(const Colour& colour, const Point3D &worldTranslation, 
                                                       std::list<ESPEmitter*>& emitters) {

    PortalBlockMesh::CreatePortalBlockEmitters(colour, worldTranslation, this->haloExpandPulse, this->haloFader,
        this->haloTexture, this->particleFader, this->particleMediumGrowth, this->circleTex, this->hoopTex, emitters);
}

#endif // __PORTAL_BLOCK_MESH_H__