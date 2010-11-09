/**
 * BlockStatusEffectRenderer.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BlockStatusEffectRenderer.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"

#include "../ESPEngine/ESPPointEmitter.h"
#include "../ESPEngine/ESPVolumeEmitter.h"

#include "../ResourceManager.h"

BlockStatusEffectRenderer::BlockStatusEffectRenderer() :
fireColourEffector(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(0.5f, 0.0f, 0.0f, 0.0f)) {
	this->SetupTextures();
}

BlockStatusEffectRenderer::~BlockStatusEffectRenderer() {
	// Clean up any left over effects...
	for (PieceStatusEffectMap::iterator pseIter = this->pieceStatusEffects.begin(); pseIter != this->pieceStatusEffects.end(); ++pseIter) {
		StatusEffectMap& statusEffectMap = pseIter->second;
		for (StatusEffectMap::iterator seIter = statusEffectMap.begin(); seIter != statusEffectMap.end(); ++seIter) {
			ESPEmitter* currEmitter = seIter->second;
			delete currEmitter;
			currEmitter = NULL;
		}
		statusEffectMap.clear();
	}
	this->pieceStatusEffects.clear();

	// Release all the persistant texture resources
	for (std::vector<Texture2D*>::iterator iter = this->smokePuffTextures.begin();
		iter != this->smokePuffTextures.end(); ++iter) {
		
		bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->smokePuffTextures.clear();

	for (std::vector<Texture2D*>::iterator iter = this->fireGlobTextures.begin();
		iter != this->fireGlobTextures.end(); ++iter) {
		
		bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->fireGlobTextures.clear();
}

/**
 * Add a status effect to a level piece so that it gets rendered.
 */
void BlockStatusEffectRenderer::AddLevelPieceStatus(const LevelPiece& piece, const LevelPiece::PieceStatus& status) {
	// Prepare the piece status effects map to be ready to receive a status effect for the given piece/status
	PieceStatusEffectMap::iterator findPieceIter = this->pieceStatusEffects.find(&piece);
	if (findPieceIter == this->pieceStatusEffects.end()) {
		std::pair<PieceStatusEffectMap::iterator, bool> insertResult = this->pieceStatusEffects.insert(std::make_pair(&piece, StatusEffectMap()));
		assert(insertResult.second);
		findPieceIter = insertResult.first;
	}
	
	// Check to see if the piece already has the status effect being activated - this should never happen so make
	// sure it doesn't
	StatusEffectMap& statusEffectMap = findPieceIter->second;
	StatusEffectMap::iterator findEffectIter = statusEffectMap.find(status);
	if (findEffectIter != statusEffectMap.end()) {
		assert(false);
		return;
	}

	ESPEmitter* emitter = NULL;
	switch (status) {
		case LevelPiece::OnFireStatus:
			// TODO
			break;
		default:
			assert(false);
			return;
	}

	assert(emitter != NULL);
	std::pair<StatusEffectMap::iterator, bool> insertResult = statusEffectMap.insert(std::make_pair(status, emitter));
}

/**
 * Remove a status from a level piece so that it is no longer rendered.
 */
void BlockStatusEffectRenderer::RemoveLevelPieceStatus(const LevelPiece& piece, const LevelPiece::PieceStatus& status) {
	PieceStatusEffectMap::iterator findPieceIter = this->pieceStatusEffects.find(&piece);
	// We should always be able to find the piece (since you MUST add a status before removing one)
	if (findPieceIter == this->pieceStatusEffects.end()) {
		assert(false);
		return;
	}
	
	StatusEffectMap& statusEffectMap = findPieceIter->second;
	StatusEffectMap::iterator findEffectIter = statusEffectMap.find(status);
	// We should also be able to find the status!
	if (findEffectIter == statusEffectMap.end()) {
		assert(false);
		return;
	}

	// Delete the effect and remove the status from the mapping
	ESPEmitter* statusEffect = findEffectIter->second;
	delete statusEffect;
	statusEffect = NULL;
	statusEffectMap.erase(findEffectIter);

	// Also remove the piece from the pieceStatusEffects map, if all statuses for that piece are gone
	if (statusEffectMap.empty()) {
		this->pieceStatusEffects.erase(findPieceIter);
	}
}

/**
 * Remove every status effect for the given piece.
 */
void BlockStatusEffectRenderer::RemoveAllLevelPieceStatus(const LevelPiece& piece) {
	PieceStatusEffectMap::iterator findPieceIter = this->pieceStatusEffects.find(&piece);
	// We should always be able to find the piece (since you MUST add a status before removing one)
	if (findPieceIter == this->pieceStatusEffects.end()) {
		assert(false);
		return;
	}

	StatusEffectMap& statusEffectMap = findPieceIter->second;

	// Go through every status in the status effect map and delete its corresponding effect then clean
	// everything up by getting rid of the piece in the map
	for (StatusEffectMap::iterator iter = statusEffectMap.begin(); iter != statusEffectMap.end(); ++iter) {
		ESPEmitter* currEmitter = iter->second;
		delete currEmitter;
		currEmitter = NULL;
	}
	statusEffectMap.clear();
	this->pieceStatusEffects.erase(findPieceIter);
}

void BlockStatusEffectRenderer::Draw(double dT, const Camera& camera) {

}

void BlockStatusEffectRenderer::SetupTextures() {
	// Initialize all the smoke puff textures...
	assert(this->smokePuffTextures.empty());
	this->smokePuffTextures.reserve(6);
	Texture2D* temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);	

	// Initialize all the fire glob textures
	assert(this->fireGlobTextures.empty());
	this->fireGlobTextures.reserve(3);
	temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_FIRE_GLOB1, Texture::Trilinear));
	assert(temp != NULL);
	this->fireGlobTextures.push_back(temp);
	temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_FIRE_GLOB2, Texture::Trilinear));
	assert(temp != NULL);
	this->fireGlobTextures.push_back(temp);
	temp = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_FIRE_GLOB3, Texture::Trilinear));
	assert(temp != NULL);
	this->fireGlobTextures.push_back(temp);
}

ESPVolumeEmitter* BlockStatusEffectRenderer::BuildBlockOnFireStatusEffect(const LevelPiece& piece) {
	

	return NULL;
}