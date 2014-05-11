/**
 * PrismBlockMesh.cpp
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

#include "PrismBlockMesh.h"
#include "GameViewConstants.h"

PrismBlockMesh::PrismBlockMesh(PrismBlockType type) : prismBlockGeometry(NULL), prismEffect(NULL) {
	this->LoadMesh(type);
	assert(this->prismBlockGeometry != NULL);
	assert(this->prismEffect != NULL);
}

PrismBlockMesh::~PrismBlockMesh() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->prismBlockGeometry);
    assert(success);
    UNUSED_VARIABLE(success);
}

/**
 * Private helper for loading the mesh and material for the prism block.
 */
void PrismBlockMesh::LoadMesh(PrismBlockType type) {
	assert(this->prismBlockGeometry == NULL);
	assert(this->prismEffect == NULL);	

	// Grab prism mesh from the resource manager based on the type of prism we're dealing with
	switch (type) {
		case PrismBlockMesh::DiamondPrism:
			this->prismBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PRISM_DIAMOND_BLOCK_MESH);
			break;
		case PrismBlockMesh::TrianglePrism:
			this->prismBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PRISM_TRIANGLE_BLOCK_MESH);
			break;
		default:
			assert(false);
			break;
	}

	const std::map<std::string, MaterialGroup*> prismMaterialGrps = this->prismBlockGeometry->GetMaterialGroups();
	assert(prismMaterialGrps.size() == 1);

	MaterialGroup* prismMatGrp = prismMaterialGrps.begin()->second;
	this->prismEffect = static_cast<CgFxPrism*>(prismMatGrp->GetMaterial());
    assert(this->prismEffect != NULL);
}