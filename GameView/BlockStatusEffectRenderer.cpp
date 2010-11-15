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
fireMiasma(NULL),
fireColourEffector(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(0.5f, 0.0f, 0.0f, 0.0f)),
semiFaderEffector(1.0f, 0.1f), 
fullFaderEffector(1.0f, 0.0f),
particleLargeGrowth(1.0f, 2.3f),
particleMediumGrowth(1.0f, 1.85f),
smokeRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE),
smokeRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE) {
	this->SetupTextures();
	this->fireEffect.SetTechnique(CgFxFireBallEffect::NO_DEPTH_WITH_MASK_TECHNIQUE_NAME);
	this->fireEffect.SetMaskTexture(this->fireMiasma);
}

BlockStatusEffectRenderer::~BlockStatusEffectRenderer() {
	// Clean up any left over effects...
	for (PieceStatusEffectMap::iterator pseIter = this->pieceStatusEffects.begin(); pseIter != this->pieceStatusEffects.end(); ++pseIter) {
		StatusEffectMap& statusEffectMap = pseIter->second;
		for (StatusEffectMap::iterator seIter = statusEffectMap.begin(); seIter != statusEffectMap.end(); ++seIter) {

			EmitterList& statusEffectList = seIter->second;
			for (EmitterList::iterator iter = statusEffectList.begin(); iter != statusEffectList.end(); ++iter) {
				ESPEmitter* emitter = *iter;
				delete emitter;
				emitter = NULL;
			}
			statusEffectList.clear();
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

	/*
	for (std::vector<Texture2D*>::iterator iter = this->fireGlobTextures.begin();
		iter != this->fireGlobTextures.end(); ++iter) {
		
		bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(removed);	
	}
	this->fireGlobTextures.clear();
	*/

	bool removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->fireMiasma);
	assert(removed);
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

	EmitterList emitterList;
	switch (status) {
		case LevelPiece::OnFireStatus:
			emitterList.push_back(this->BuildBlockOnFireScortchEffect(piece));
			emitterList.push_back(this->BuildBlockOnFireSmokeEffect(piece));
			emitterList.push_back(this->BuildBlockOnFireSmokeEffect(piece));
			emitterList.push_back(this->BuildBlockOnFireFlameEffect(piece, false));
			emitterList.push_back(this->BuildBlockOnFireFlameEffect(piece, true));
			break;
		default:
			assert(false);
			return;
	}

	assert(!emitterList.empty());
	std::pair<StatusEffectMap::iterator, bool> insertResult = statusEffectMap.insert(std::make_pair(status, emitterList));
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
	EmitterList& statusEffectList = findEffectIter->second;
	for (EmitterList::iterator iter = statusEffectList.begin(); iter != statusEffectList.end(); ++iter) {
		ESPEmitter* emitter = *iter;
		delete emitter;
		emitter = NULL;
	}
	statusEffectList.clear();
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
		EmitterList& statusEffectList = iter->second;
		for (EmitterList::iterator iter = statusEffectList.begin(); iter != statusEffectList.end(); ++iter) {
			ESPEmitter* emitter = *iter;
			delete emitter;
			emitter = NULL;
		}
		statusEffectList.clear();
	}
	statusEffectMap.clear();
	this->pieceStatusEffects.erase(findPieceIter);
}

void BlockStatusEffectRenderer::Draw(double dT, const Camera& camera) {

	for (PieceStatusEffectMap::iterator iter = this->pieceStatusEffects.begin(); iter != this->pieceStatusEffects.end(); ++iter) {
		StatusEffectMap& effectMap = iter->second;
		for (StatusEffectMap::iterator iter2 = effectMap.begin(); iter2 != effectMap.end(); ++iter2) {
			EmitterList& emitterList = iter2->second;
			for (EmitterList::iterator iter3 = emitterList.begin(); iter3 != emitterList.end(); ++iter3) {
				ESPEmitter* currEmitter = *iter3;
				currEmitter->Tick(dT);
				currEmitter->Draw(camera);
			}
		}
	}

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

	/*
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
	*/

	this->fireMiasma = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CLOUD, Texture::Trilinear));
	assert(this->fireMiasma != NULL);

	this->gritTexture = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_GRIT, Texture::Trilinear));
	assert(this->gritTexture != NULL);
}

ESPPointEmitter* BlockStatusEffectRenderer::BuildBlockOnFireFlameEffect(const LevelPiece& piece, bool spinCW) {
	ESPPointEmitter* pulsingFlame = new ESPPointEmitter();
	pulsingFlame->SetSpawnDelta(ESPInterval(0.075f, 0.125f));
	pulsingFlame->SetInitialSpd(ESPInterval(0.1f, 0.5f));
	pulsingFlame->SetParticleLife(ESPInterval(0.75f, 1.25f));
	pulsingFlame->SetEmitAngleInDegrees(30);
	pulsingFlame->SetEmitDirection(Vector3D(0, 1, 0));
	pulsingFlame->SetRadiusDeviationFromCenter(ESPInterval(0.0f, LevelPiece::PIECE_WIDTH/3.1f), 
																					   ESPInterval(0.0f, LevelPiece::PIECE_HEIGHT/3.2f), ESPInterval(0.0f));
	pulsingFlame->SetParticleAlignment(ESP::ScreenAligned);
	pulsingFlame->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH / 3.4f, LevelPiece::PIECE_WIDTH / 2.15f));
	pulsingFlame->SetParticleRotation(ESPInterval(0, 359.99));
	pulsingFlame->SetEmitPosition(Point3D(piece.GetCenter(), 0.0f));
	pulsingFlame->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(1));
	pulsingFlame->AddEffector(&this->fullFaderEffector);
	pulsingFlame->AddEffector(&this->particleMediumGrowth);
	if (spinCW) {
		pulsingFlame->AddEffector(&this->smokeRotatorCW);
	}
	else {
		pulsingFlame->AddEffector(&this->smokeRotatorCCW);
	}
	bool result = pulsingFlame->SetParticles(10, &this->fireEffect);
	pulsingFlame->SimulateTicking(Randomizer::GetInstance()->RandomNumZeroToOne() * 10);
	assert(result);

	return pulsingFlame;
}

ESPPointEmitter* BlockStatusEffectRenderer::BuildBlockOnFireSmokeEffect(const LevelPiece& piece) {
	size_t randomTexIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->smokePuffTextures.size();

	ESPPointEmitter* smokeClouds = new ESPPointEmitter();
	smokeClouds->SetSpawnDelta(ESPInterval(0.1f, 0.20f));
	smokeClouds->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
	smokeClouds->SetInitialSpd(ESPInterval(0.75f, 2.5f));
	smokeClouds->SetParticleLife(ESPInterval(0.5f, 1.20f));
	smokeClouds->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH / 3.8f, LevelPiece::PIECE_WIDTH / 2.4f));
	smokeClouds->SetRadiusDeviationFromCenter(ESPInterval(0.0f, LevelPiece::PIECE_WIDTH/3.0f), 
																					  ESPInterval(0.0f, LevelPiece::PIECE_HEIGHT/3.0f), ESPInterval(0.0f));
	smokeClouds->SetParticleAlignment(ESP::ScreenAligned);
	smokeClouds->SetEmitPosition(Point3D(piece.GetCenter()));
	smokeClouds->SetEmitDirection(Vector3D(0, 1, 0));
	smokeClouds->SetEmitAngleInDegrees(30.0f);
	smokeClouds->SetParticles(5, this->smokePuffTextures[randomTexIdx]);
	smokeClouds->AddEffector(&this->particleLargeGrowth);
	smokeClouds->AddEffector(&this->fireColourEffector);

	ESPParticleRotateEffector* rotateEffector;
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		rotateEffector = &this->smokeRotatorCW;
	}
	else {
		rotateEffector = &this->smokeRotatorCCW;
	}
	smokeClouds->AddEffector(rotateEffector);

	return smokeClouds;
}

ESPPointEmitter* BlockStatusEffectRenderer::BuildBlockOnFireScortchEffect(const LevelPiece& piece) {
	ESPPointEmitter* scortchEffect = new ESPPointEmitter();
	scortchEffect->SetSpawnDelta(ESPInterval(0.5f));
	scortchEffect->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
	scortchEffect->SetInitialSpd(ESPInterval(0.0f));
	scortchEffect->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
	scortchEffect->SetParticleSize(ESPInterval(LevelPiece::PIECE_WIDTH), ESPInterval(LevelPiece::PIECE_HEIGHT * 0.9f));
	scortchEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	scortchEffect->SetParticleAlignment(ESP::ScreenAligned);
	scortchEffect->SetEmitPosition(Point3D(piece.GetCenter()));
	scortchEffect->SetParticles(1, this->gritTexture);
	return scortchEffect;
}