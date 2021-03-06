/**
 * ItemDropBlockMesh.cpp
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

#include "ItemDropBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/ItemDropBlock.h"

#include "../BlammoEngine/Texture2D.h"

#include "../ESPEngine/ESPVolumeEmitter.h"

#include "../ResourceManager.h"

ItemDropBlockMesh::ItemDropBlockMesh() : blockMesh(NULL), itemDropTypeMatGrp(NULL),
flareParticleEmitter(NULL), flareTex(NULL), particleFader(1.0f, 0.1f), powerUpSparkEmitter(NULL), 
powerNeutralSparkEmitter(NULL), powerDownSparkEmitter(NULL) {
	this->LoadMesh();

	// Load emitters that come out of the block to indicate that it drops items
	this->flareTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
	assert(this->flareTex != NULL);
	this->sparkTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
	assert(this->sparkTex != NULL);

	Point3D minPt(-LevelPiece::PIECE_WIDTH / 2.0f, -LevelPiece::PIECE_HEIGHT / 2.0f, -LevelPiece::PIECE_DEPTH / 2.0f);
	Point3D maxPt( LevelPiece::PIECE_WIDTH / 2.0f, -LevelPiece::PIECE_HEIGHT / 2.0f,  LevelPiece::PIECE_DEPTH / 2.0f);

    const ESPInterval particleLifeTime(0.45f, 0.65f);
    const ESPInterval particleSpd(1.33f, 3.0f);

	this->flareParticleEmitter = new ESPVolumeEmitter();
	this->flareParticleEmitter->SetSpawnDelta(ESPInterval(0.07f));
	this->flareParticleEmitter->SetInitialSpd(particleSpd);
	this->flareParticleEmitter->SetParticleLife(particleLifeTime);
	this->flareParticleEmitter->SetParticleSize(ESPInterval(0.5f, 0.9f));
	this->flareParticleEmitter->SetParticleColour(ESPInterval(0.9f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->flareParticleEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->flareParticleEmitter->SetEmitDirection(Vector3D(0, -1, 0));
	this->flareParticleEmitter->SetEmitVolume(minPt, maxPt);
	this->flareParticleEmitter->AddEffector(&this->particleFader);
	bool result = this->flareParticleEmitter->SetParticles(7, this->flareTex);
	UNUSED_VARIABLE(result);
	assert(result);

	this->powerUpSparkEmitter      = this->InitSparkEmitter(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR, particleLifeTime, particleSpd);
	this->powerNeutralSparkEmitter = this->InitSparkEmitter(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR, particleLifeTime, particleSpd);
	this->powerDownSparkEmitter    = this->InitSparkEmitter(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR, particleLifeTime, particleSpd);
}

ItemDropBlockMesh::~ItemDropBlockMesh() {
	// Make sure we get rid of the texture attached to the material group that shows the item type
	// or we'll end up trying to delete the texture twice
	this->blockMesh->SetTextureForMaterial(GameViewConstants::GetInstance()->ITEM_DROP_BLOCK_ITEMTYPE_MATGRP, NULL);

	bool success = ResourceManager::GetInstance()->ReleaseMeshResource(this->blockMesh);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->flareTex);
	assert(success);
	success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkTex);
	assert(success);

    UNUSED_VARIABLE(success);

	delete this->flareParticleEmitter;
	this->flareParticleEmitter = NULL;
	delete this->powerUpSparkEmitter;
	this->powerUpSparkEmitter = NULL;
	delete this->powerNeutralSparkEmitter;
	this->powerNeutralSparkEmitter = NULL;
	delete this->powerDownSparkEmitter;
	this->powerDownSparkEmitter = NULL;
}

void ItemDropBlockMesh::DrawBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                                      const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    UNUSED_PARAMETER(dT);

    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT);
    glDepthMask(GL_FALSE);

    // Go through all the item drop blocks, set the proper materials and draw
    for (std::map<const ItemDropBlock*, Texture*>::const_iterator iter = this->itemDropBlockToItemTexMap.begin(); 
      iter != this->itemDropBlockToItemTexMap.end(); ++iter) {

          // We draw only the material part of the block that indicates the next item that will be dropped...
          const ItemDropBlock* currItemDropBlock = iter->first;
          if (!currItemDropBlock->HasStatus(LevelPiece::IceCubeStatus)) {
              continue;
          }

          Texture* currTex = iter->second;
          this->Draw(currItemDropBlock, currTex, camera, keyLight, fillLight, ballLight);
    }

    glPopAttrib();
}

void ItemDropBlockMesh::DrawNoBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                                        const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    UNUSED_PARAMETER(dT);

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT);
    glDepthMask(GL_FALSE);

	// Go through all the item drop blocks, set the proper materials and draw
	for (std::map<const ItemDropBlock*, Texture*>::const_iterator iter = this->itemDropBlockToItemTexMap.begin(); 
		iter != this->itemDropBlockToItemTexMap.end(); ++iter) {

		// We draw only the material part of the block that indicates the next item that will be dropped...
		const ItemDropBlock* currItemDropBlock = iter->first;
        if (currItemDropBlock->HasStatus(LevelPiece::IceCubeStatus)) {
            continue;
        }

		Texture* currTex = iter->second;
        this->Draw(currItemDropBlock, currTex, camera, keyLight, fillLight, ballLight);
	}

	glPopAttrib();
}

void ItemDropBlockMesh::DrawEffects(const Vector3D& worldTranslation, double dT, const Camera& camera) {
    if (this->itemDropBlockToItemTexMap.empty()) {
        return;
    }

    glPushMatrix();
    glTranslatef(worldTranslation[0], worldTranslation[1], worldTranslation[2]);
    for (std::map<const ItemDropBlock*, Texture*>::const_iterator iter = this->itemDropBlockToItemTexMap.begin(); 
         iter != this->itemDropBlockToItemTexMap.end(); ++iter) {

        const ItemDropBlock* currItemDropBlock = iter->first;
        if (!currItemDropBlock->GetHasSparkleEffect() || currItemDropBlock->HasStatus(LevelPiece::IceCubeStatus)) {
            continue;
        }

        const Point2D& blockCenter = currItemDropBlock->GetCenter();
        glPushMatrix();
        glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

        switch (currItemDropBlock->GetNextDropItemDisposition()) {
            case GameItem::Good:
                this->powerUpSparkEmitter->Draw(camera);
                break;

            case GameItem::Neutral:
                this->powerNeutralSparkEmitter->Draw(camera);
                break;

            case GameItem::Bad:
                this->powerDownSparkEmitter->Draw(camera);
                break;

            default:
                assert(false);
                break;
        }
        this->flareParticleEmitter->Draw(camera);

        glPopMatrix();
    }
    glPopMatrix();

    this->flareParticleEmitter->Tick(dT);
    this->powerUpSparkEmitter->Tick(dT);
    this->powerNeutralSparkEmitter->Tick(dT);
    this->powerDownSparkEmitter->Tick(dT);
}

void ItemDropBlockMesh::SetAlphaMultiplier(float alpha) {
	const std::map<std::string, MaterialGroup*>&  matGrps = this->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = matGrps.begin(); iter != matGrps.end(); ++iter) {
		MaterialGroup* matGrp = iter->second;
		matGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
	}
	
	this->itemDropTypeMatGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
	this->flareParticleEmitter->SetParticleAlpha(ESPInterval(alpha));
	this->powerUpSparkEmitter->SetParticleAlpha(ESPInterval(alpha));
	this->powerNeutralSparkEmitter->SetParticleAlpha(ESPInterval(alpha));
	this->powerDownSparkEmitter->SetParticleAlpha(ESPInterval(alpha));
}

void ItemDropBlockMesh::Draw(const ItemDropBlock* currItemDropBlock, Texture* itemTexture,
                             const Camera& camera, const BasicPointLight& keyLight, 
                             const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

     CgFxAbstractMaterialEffect* matEffect = this->itemDropTypeMatGrp->GetMaterial();
     matEffect->SetDiffuseTexture(itemTexture);

     const Point2D& blockCenter = currItemDropBlock->GetCenter();
     glPushMatrix();
     glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
     glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
     this->itemDropTypeMatGrp->Draw(camera, keyLight, fillLight, ballLight);
     glPopMatrix();
}

ESPVolumeEmitter* ItemDropBlockMesh::InitSparkEmitter(const Colour& colour, const ESPInterval& lifeTime, 
                                                      const ESPInterval& particleSpd) {

	Point3D minPt(-LevelPiece::PIECE_WIDTH / 2.0f, -LevelPiece::PIECE_HEIGHT / 2.0f, -LevelPiece::PIECE_DEPTH / 2.0f);
	Point3D maxPt( LevelPiece::PIECE_WIDTH / 2.0f, -LevelPiece::PIECE_HEIGHT / 2.0f,  LevelPiece::PIECE_DEPTH / 2.0f);

	ESPVolumeEmitter* emitter = new ESPVolumeEmitter();
	emitter->SetSpawnDelta(ESPInterval(0.07f));
	emitter->SetInitialSpd(particleSpd);
	emitter->SetParticleLife(lifeTime);
	emitter->SetParticleSize(ESPInterval(0.1f, 0.3f));
	emitter->SetParticleColour(
        ESPInterval(colour.R(), std::min<float>(1.0f, colour.R() + 0.5f)), 
        ESPInterval(colour.G(), std::min<float>(1.0f, colour.G() + 0.5f)), 
        ESPInterval(colour.B(), std::min<float>(1.0f, colour.B() + 0.5f)), ESPInterval(1.0f));
	emitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	emitter->SetEmitDirection(Vector3D(0, -1, 0));
	emitter->SetEmitVolume(minPt, maxPt);
	emitter->AddEffector(&this->particleFader);
	bool result = emitter->SetParticles(10, this->sparkTex);
    UNUSED_VARIABLE(result);
	assert(result);

	return emitter;
}

void ItemDropBlockMesh::LoadMesh() {
	assert(this->blockMesh == NULL);
	assert(this->itemDropTypeMatGrp == NULL);
	assert(baseMaterialGrp.empty());

	this->blockMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ITEM_DROP_BLOCK_MESH);

	// Set the base material group map (which never changes its texture) and also set the item drop type material group; 
	// which changes its texture on a per-block basis depending on what the next item type it will drop will be...
	this->baseMaterialGrp = this->blockMesh->GetMaterialGroups();
	std::map<std::string, MaterialGroup*>::iterator findIter = this->baseMaterialGrp.find(GameViewConstants::GetInstance()->ITEM_DROP_BLOCK_ITEMTYPE_MATGRP);
	assert(findIter != this->baseMaterialGrp.end());
	
	this->itemDropTypeMatGrp = findIter->second;
	this->baseMaterialGrp.erase(findIter);

	assert(this->blockMesh != NULL);
	assert(this->itemDropTypeMatGrp != NULL);
	assert(!baseMaterialGrp.empty());
}