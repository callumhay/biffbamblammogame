/**
 * BlockStatusEffectRenderer.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BlockStatusEffectRenderer.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"

#include "../ESPEngine/ESPPointEmitter.h"
#include "../ESPEngine/ESPVolumeEmitter.h"

#include "../ResourceManager.h"

BlockStatusEffectRenderer::BlockStatusEffectRenderer() :
fireColourEffector(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(0.5f, 0.0f, 0.0f, 0.0f)),
semiFaderEffector(1.0f, 0.1f), 
fullFaderEffector(1.0f, 0.0f),
particleLargeGrowth(1.0f, 2.3f),
particleMediumGrowth(1.0f, 1.85f),
smokeRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE),
smokeRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
gritTexture(NULL),
cloudTexture(NULL),
rectPrismTexture(NULL),
frostTexture(NULL) {

	this->SetupTextures();

	this->fireEffect.SetTechnique(CgFxFireBallEffect::NO_DEPTH_WITH_MASK_TECHNIQUE_NAME);
	this->fireEffect.SetMaskTexture(this->cloudTexture);

	this->iceBlockEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_WITH_OVERLAY_TECHNIQUE_NAME);
	this->iceBlockEffect.SetWarpAmountParam(35.0f);
	this->iceBlockEffect.SetIndexOfRefraction(1.31f);
	this->iceBlockEffect.SetNormalTexture(this->rectPrismTexture);
	this->iceBlockEffect.SetOverlayTexture(this->frostTexture);
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

    bool removed;
    UNUSED_VARIABLE(removed);

	// Release all the persistant texture resources
	for (std::vector<Texture2D*>::iterator iter = this->smokePuffTextures.begin();
		iter != this->smokePuffTextures.end(); ++iter) {
		
		removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
        UNUSED_VARIABLE(removed);
		assert(removed);	
	}
	this->smokePuffTextures.clear();

	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->cloudTexture);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->gritTexture);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->rectPrismTexture);
	assert(removed);
	removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->frostTexture);
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
			emitterList.push_back(this->BuildBlockOnFireFlameEffect(piece, false));
			emitterList.push_back(this->BuildBlockOnFireFlameEffect(piece, true));
			break;

		case LevelPiece::IceCubeStatus:
			break;

		default:
			assert(false);
			return;
	}

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

void BlockStatusEffectRenderer::Draw(double dT, const Camera& camera, const Texture2D* sceneTexture) {
	this->iceBlockEffect.SetFBOTexture(sceneTexture);

	for (PieceStatusEffectMap::iterator iter = this->pieceStatusEffects.begin(); iter != this->pieceStatusEffects.end(); ++iter) {
		StatusEffectMap& effectMap = iter->second;
		const LevelPiece* currPiece = iter->first;

		glPushAttrib(GL_CURRENT_BIT);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		for (StatusEffectMap::iterator iter2 = effectMap.begin(); iter2 != effectMap.end(); ++iter2) {

			if (iter2->first == LevelPiece::IceCubeStatus) {
				glPushMatrix();
				glTranslatef(currPiece->GetCenter()[0], currPiece->GetCenter()[1], 0.0f);
				glScalef(1.1f * LevelPiece::PIECE_WIDTH, 1.1f * LevelPiece::PIECE_HEIGHT, 1.5f * LevelPiece::PIECE_DEPTH);
				this->iceBlockEffect.Draw(camera, GeometryMaker::GetInstance()->GetCubeDL());
				glPopMatrix();
			}

			EmitterList& emitterList = iter2->second;
			for (EmitterList::iterator iter3 = emitterList.begin(); iter3 != emitterList.end();) {
				ESPEmitter* currEmitter = *iter3;
				currEmitter->Tick(dT);
				currEmitter->Draw(camera);

				if (currEmitter->IsDead()) {
					delete currEmitter;
					currEmitter = NULL;
					iter3 = emitterList.erase(iter3);
				}
				else {
					++iter3;
				}

			}
		}
		glPopAttrib();

	}
}

void BlockStatusEffectRenderer::SetupTextures() {
	// Initialize all the smoke puff textures...
	assert(this->smokePuffTextures.empty());
	this->smokePuffTextures.reserve(6);
	Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);
	temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
	assert(temp != NULL);
	this->smokePuffTextures.push_back(temp);	

	assert(this->cloudTexture == NULL);
	this->cloudTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CLOUD, Texture::Trilinear));
	assert(this->cloudTexture != NULL);

	assert(this->gritTexture == NULL);
	this->gritTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_GRIT, Texture::Trilinear));
	assert(this->gritTexture != NULL);

	assert(this->rectPrismTexture == NULL);
	this->rectPrismTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_RECT_PRISM_NORMALS, Texture::Trilinear));
	assert(this->rectPrismTexture != NULL);

	assert(this->frostTexture == NULL);
	this->frostTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_FROST, Texture::Trilinear));
	assert(this->frostTexture != NULL);
}

ESPPointEmitter* BlockStatusEffectRenderer::BuildBlockOnFireFlameEffect(const LevelPiece& piece, bool spinCW) {
	ESPPointEmitter* pulsingFlame = new ESPPointEmitter();
	pulsingFlame->SetSpawnDelta(ESPInterval(0.225f, 0.31f));
	pulsingFlame->SetInitialSpd(ESPInterval(0.1f, 0.5f));
	pulsingFlame->SetParticleLife(ESPInterval(0.75f, 1.25f));
	pulsingFlame->SetEmitAngleInDegrees(30);
	pulsingFlame->SetEmitDirection(Vector3D(0, 1, 0));
	pulsingFlame->SetRadiusDeviationFromCenter(
        ESPInterval(0.0f, LevelPiece::PIECE_WIDTH/3.1f), 
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
	bool result = pulsingFlame->SetParticles(4, &this->fireEffect);
    UNUSED_VARIABLE(result);
	pulsingFlame->SimulateTicking(Randomizer::GetInstance()->RandomNumZeroToOne() * 10);
	assert(result);

	return pulsingFlame;
}

ESPPointEmitter* BlockStatusEffectRenderer::BuildBlockOnFireSmokeEffect(const LevelPiece& piece) {

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
	smokeClouds->SetRandomTextureParticles(6, this->smokePuffTextures);
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

ESPPointEmitter* BlockStatusEffectRenderer::BuildIceBlockEffect(const LevelPiece& piece) {
	ESPPointEmitter* iceCubeEffect = new ESPPointEmitter();
	iceCubeEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
	iceCubeEffect->SetParticleColour(ESPInterval(0.75f), ESPInterval(0.85f), ESPInterval(1.0f), ESPInterval(1.0f));
	iceCubeEffect->SetInitialSpd(ESPInterval(0.0f));
	iceCubeEffect->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
	iceCubeEffect->SetParticleSize(ESPInterval(1.1f * LevelPiece::PIECE_WIDTH), ESPInterval(1.1f * LevelPiece::PIECE_HEIGHT));
	iceCubeEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	iceCubeEffect->SetParticleAlignment(ESP::ScreenAligned);
	iceCubeEffect->SetEmitPosition(Point3D(piece.GetCenter()));
	iceCubeEffect->SetParticles(1, &this->iceBlockEffect);
	return iceCubeEffect;
}