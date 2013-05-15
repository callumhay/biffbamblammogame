/**
 * TurretBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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