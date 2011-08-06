/**
 * ItemDropBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
	this->flareTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
	assert(this->flareTex != NULL);
	this->sparkTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
	assert(this->sparkTex != NULL);

	Point3D minPt(-LevelPiece::PIECE_WIDTH / 2.0f, -LevelPiece::PIECE_HEIGHT / 2.0f, -LevelPiece::PIECE_DEPTH / 2.0f);
	Point3D maxPt( LevelPiece::PIECE_WIDTH / 2.0f, -LevelPiece::PIECE_HEIGHT / 2.0f,  LevelPiece::PIECE_DEPTH / 2.0f);

	this->flareParticleEmitter = new ESPVolumeEmitter();
	this->flareParticleEmitter->SetSpawnDelta(ESPInterval(0.07f));
	this->flareParticleEmitter->SetInitialSpd(ESPInterval(2.0f, 4.5f));
	this->flareParticleEmitter->SetParticleLife(ESPInterval(0.65f, 0.85f));
	this->flareParticleEmitter->SetParticleSize(ESPInterval(0.5f, 0.9f));
	this->flareParticleEmitter->SetParticleColour(ESPInterval(0.9f, 1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
	this->flareParticleEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->flareParticleEmitter->SetAsPointSpriteEmitter(true);
	this->flareParticleEmitter->SetEmitDirection(Vector3D(0, -1, 0));
	this->flareParticleEmitter->SetEmitVolume(minPt, maxPt);
	this->flareParticleEmitter->AddEffector(&this->particleFader);
	bool result = this->flareParticleEmitter->SetParticles(7, this->flareTex);
	UNUSED_VARIABLE(result);
	assert(result);

	this->powerUpSparkEmitter      = this->InitSparkEmitter(GameViewConstants::GetInstance()->ITEM_GOOD_COLOUR);
	this->powerNeutralSparkEmitter = this->InitSparkEmitter(GameViewConstants::GetInstance()->ITEM_NEUTRAL_COLOUR);
	this->powerDownSparkEmitter    = this->InitSparkEmitter(GameViewConstants::GetInstance()->ITEM_BAD_COLOUR);

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

	delete this->flareParticleEmitter;
	this->flareParticleEmitter = NULL;
	delete this->powerUpSparkEmitter;
	this->powerUpSparkEmitter = NULL;
	delete this->powerNeutralSparkEmitter;
	this->powerNeutralSparkEmitter = NULL;
	delete this->powerDownSparkEmitter;
	this->powerDownSparkEmitter = NULL;
}


void ItemDropBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
														 const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

	glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	// Go through all the item drop blocks, set the proper materials and draw
	for (std::map<const ItemDropBlock*, Texture*>::const_iterator iter = this->itemDropBlockToItemTexMap.begin(); 
		iter != this->itemDropBlockToItemTexMap.end(); ++iter) {

		// We draw only the material part of the block that indicates the next item that will be dropped...
		const ItemDropBlock* currItemDropBlock = iter->first;
		Texture* currTex = iter->second;

		CgFxMaterialEffect* matEffect = this->itemDropTypeMatGrp->GetMaterial();
		MaterialProperties* matProps  = matEffect->GetProperties();
		matProps->diffuseTexture = currTex;

		const Point2D& blockCenter = currItemDropBlock->GetCenter();
		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

		// Draw
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
		this->itemDropTypeMatGrp->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}

	glPopAttrib();
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

ESPVolumeEmitter* ItemDropBlockMesh::InitSparkEmitter(const Colour& colour) {
	Point3D minPt(-LevelPiece::PIECE_WIDTH / 2.0f, -LevelPiece::PIECE_HEIGHT / 2.0f, -LevelPiece::PIECE_DEPTH / 2.0f);
	Point3D maxPt( LevelPiece::PIECE_WIDTH / 2.0f, -LevelPiece::PIECE_HEIGHT / 2.0f,  LevelPiece::PIECE_DEPTH / 2.0f);

	ESPVolumeEmitter* emitter = new ESPVolumeEmitter();
	emitter->SetSpawnDelta(ESPInterval(0.07f));
	emitter->SetInitialSpd(ESPInterval(2.0f, 4.5f));
	emitter->SetParticleLife(ESPInterval(0.65f, 0.85f));
	emitter->SetParticleSize(ESPInterval(0.1f, 0.3f));
	emitter->SetParticleColour(ESPInterval(colour.R(), std::min<float>(1.0f, colour.R() + 0.5f)), 
														 ESPInterval(colour.G(), std::min<float>(1.0f, colour.G() + 0.5f)), 
														 ESPInterval(colour.B(), std::min<float>(1.0f, colour.B() + 0.5f)), ESPInterval(1.0f));
	emitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	emitter->SetAsPointSpriteEmitter(true);
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