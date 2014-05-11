/**
 * TurretBlockMesh.cpp
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

#include "TurretBlockMesh.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Mesh.h"

#include "../ResourceManager.h"

TurretBlockMesh::TurretBlockMesh() : glowTexture(NULL), sparkleTexture(NULL), baseMesh(NULL) {
	
    assert(this->materialGroups.size() == 0);

    // Load the base mesh
    this->baseMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TURRET_BASE_MESH);
    assert(this->baseMesh != NULL);
    const std::map<std::string, MaterialGroup*>& baseMatGrps = this->baseMesh->GetMaterialGroups();
    this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());

    assert(this->materialGroups.size() > 0);

    if (this->smokeTextures.empty()) {
		this->smokeTextures.reserve(6);
		Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);	
	}

    this->glowTexture = static_cast<Texture2D*>(
        ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->glowTexture != NULL);
    this->sparkleTexture = static_cast<Texture2D*>(
        ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
    assert(this->sparkleTexture != NULL);

}

TurretBlockMesh::~TurretBlockMesh() {
    bool success = false;

    for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
		iter != this->smokeTextures.end(); ++iter) {
		
		success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(success);	
	}
	this->smokeTextures.clear();
    
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->glowTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTexture);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->baseMesh);
    assert(success);

    UNUSED_VARIABLE(success);
}