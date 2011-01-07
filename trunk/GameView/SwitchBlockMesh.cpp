/**
 * SwitchBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "SwitchBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/SwitchBlock.h"
#include "../ResourceManager.h"

SwitchBlockMesh::SwitchBlockMesh() : switchBlockGeometry(NULL), 
switchOnMaterialGrp(NULL), switchOffMaterialGrp(NULL), switchCurrentMaterialGrp(NULL),
greenOnSwitchTexture(NULL), redOnSwitchTexture(NULL), offSwitchTexture(NULL) {
    this->LoadMesh();

    this->greenOnSwitchTexture  = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_GREEN_ON_SWITCH, Texture::Trilinear));
    this->redOnSwitchTexture    = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_RED_ON_SWITCH, Texture::Trilinear));
    this->offSwitchTexture      = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_OFF_SWITCH, Texture::Trilinear));
    assert(this->greenOnSwitchTexture != NULL);
    assert(this->redOnSwitchTexture != NULL);
    assert(this->offSwitchTexture != NULL);

}

SwitchBlockMesh::~SwitchBlockMesh() {

    bool success = ResourceManager::GetInstance()->ReleaseMeshResource(this->switchBlockGeometry);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->greenOnSwitchTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->redOnSwitchTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->offSwitchTexture);
    assert(success);
}

void SwitchBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                           const BasicPointLight& fillLight, const BasicPointLight& ballLight, 
                           bool lightsAreOff) const {
    

    MaterialProperties* switchOnMatProperties       = this->switchOnMaterialGrp->GetMaterial()->GetProperties();
    MaterialProperties* switchOffMatProperties      = this->switchOffMaterialGrp->GetMaterial()->GetProperties();
    MaterialProperties* switchCurrentMatProperties  = this->switchCurrentMaterialGrp->GetMaterial()->GetProperties();
    
	glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

    // Go through each switch block and draw it...
    for (std::set<const SwitchBlock*>::const_iterator iter = this->switchBlocks.begin(); iter != this->switchBlocks.end(); ++iter) {
        const SwitchBlock* switchBlock = *iter;

        const Point2D& blockCenter = switchBlock->GetCenter();
        
		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

        // Set the texture based on whether the block is currently switched on or not
        if (switchBlock->GetIsSwitchOn()) {
            switchOnMatProperties->diffuseTexture      = this->greenOnSwitchTexture;
            switchOffMatProperties->diffuseTexture     = this->offSwitchTexture;
            switchCurrentMatProperties->diffuseTexture = this->greenOnSwitchTexture;
        }
        else {
            switchOffMatProperties->diffuseTexture     = this->redOnSwitchTexture;
            switchOnMatProperties->diffuseTexture      = this->offSwitchTexture;
            switchCurrentMatProperties->diffuseTexture = this->redOnSwitchTexture;
        }
        
        this->switchOnMaterialGrp->Draw(camera, keyLight, fillLight, ballLight);
        this->switchOffMaterialGrp->Draw(camera, keyLight, fillLight, ballLight);
        this->switchCurrentMaterialGrp->Draw(camera, keyLight, fillLight, ballLight);

        glPopMatrix();
    }
    glPopAttrib();
}

void SwitchBlockMesh::SetAlphaMultiplier(float alpha) {
	const std::map<std::string, MaterialGroup*>&  matGrps = this->switchBlockGeometry->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = matGrps.begin(); iter != matGrps.end(); ++iter) {
		MaterialGroup* matGrp = iter->second;
		matGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
	}
}

void SwitchBlockMesh::LoadMesh() {
    assert(this->switchBlockGeometry == NULL);
    assert(this->switchOnMaterialGrp == NULL);
    assert(this->switchOffMaterialGrp == NULL);
    assert(this->switchCurrentMaterialGrp == NULL);
    assert(this->baseMaterialGrp.empty());

    this->switchBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SWITCH_BLOCK_MESH);
    assert(this->switchBlockGeometry != NULL);

	// Set the base material group map (which never changes its texture) and also set the on and off material groups; 
	// which changes its texture on a per-block basis depending on whether a particular switch is on/off
	this->baseMaterialGrp = this->switchBlockGeometry->GetMaterialGroups();

    // On switch material group...
    std::map<std::string, MaterialGroup*>::iterator findIter = this->baseMaterialGrp.find(GameViewConstants::GetInstance()->SWITCH_ON_MATGRP);
    assert(findIter != this->baseMaterialGrp.end());
    this->switchOnMaterialGrp = findIter->second;
    assert(this->switchOnMaterialGrp != NULL);
	this->baseMaterialGrp.erase(findIter);

    // Off switch material group...
    findIter = this->baseMaterialGrp.find(GameViewConstants::GetInstance()->SWITCH_OFF_MATGRP);
    assert(findIter != this->baseMaterialGrp.end());
    this->switchOffMaterialGrp = findIter->second;
    assert(this->switchOffMaterialGrp != NULL);
	this->baseMaterialGrp.erase(findIter);

    // Current switch mode material group...
    findIter = this->baseMaterialGrp.find(GameViewConstants::GetInstance()->SWITCH_CURRENT_MATGRP);
    assert(findIter != this->baseMaterialGrp.end());
    this->switchCurrentMaterialGrp = findIter->second;
    assert(this->switchCurrentMaterialGrp != NULL);
	this->baseMaterialGrp.erase(findIter);

    assert(this->baseMaterialGrp.size() == 1);
}