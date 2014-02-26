/**
 * PrismBlockMesh.h
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