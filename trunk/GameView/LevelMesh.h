/**
 * LevelMesh.h
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

#ifndef __LEVELMESH_H__
#define __LEVELMESH_H__

#include "BlockStatusEffectRenderer.h"
#include "GameItemAssets.h"
#include "ItemDropBlockMesh.h"
#include "RegenBlockMesh.h"
#include "TeslaBlockMesh.h"
#include "LaserTurretBlockMesh.h"
#include "MineTurretBlockMesh.h"
#include "RocketTurretBlockMesh.h"
#include "OneWayBlockMesh.h"

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/ObjReader.h"
#include "../BlammoEngine/Light.h"

#include "../ESPEngine/ESP.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/ItemDropBlock.h"
#include "../GameModel/LaserTurretBlock.h"
#include "../GameModel/MineTurretBlock.h"

class GameWorldAssets;
class Mesh;
class PrismBlockMesh;
class PortalBlockMesh;
class CannonBlockMesh;
class FragileCannonBlockMesh;
class CollateralBlockMesh;
class TeslaBlockMesh;
class SwitchBlockMesh;
class LaserTurretBlockMesh;
class RocketTurretBlockMesh;
class MineTurretBlockMesh;
class AlwaysDropBlockMesh;
class MaterialGroup;
class ESPEmitter;
class BossMesh;
class GameAssets;

class LevelMesh {
public:
	LevelMesh(GameSound* sound, const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level);
	~LevelMesh();
	
	void ChangePiece(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void RemovePiece(const LevelPiece& piece);
	
	void DrawFirstPassPieces(const Vector3D& worldTranslation, double dT, const Camera& camera, const GameModel* gameModel, 
        const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawSecondPassPieces(double dT, const Camera& camera, const GameModel* gameModel, 
        const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight, const Texture2D* sceneTexture);
    void DrawPiecesPostEffects(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawNoBloomPieces(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawTransparentNoBloomPieces(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);

	void DrawStatusEffects(double dT, const Camera& camera, const Texture2D* sceneTexture);
    void DrawBoss(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const GameAssets* assets);

	void LoadNewLevel(GameSound* sound, const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level);

	void LevelPieceStatusAdded(const LevelPiece& piece, const LevelPiece::PieceStatus& status);
	void LevelPieceStatusRemoved(const LevelPiece& piece, const LevelPiece::PieceStatus& status);
	void LevelPieceAllStatusRemoved(const LevelPiece& piece);

    void LevelIsAlmostComplete();

    void SwitchActivated(const SwitchBlock* block, const GameLevel* currLevel);
    void LaserTurretAIStateChanged(const LaserTurretBlock* block, const LaserTurretBlock::TurretAIState& oldState,
        const LaserTurretBlock::TurretAIState& newState);
    void LaserFired(const LaserTurretBlock* block);
    void RocketTurretAIStateChanged(const RocketTurretBlock* block, const RocketTurretBlock::TurretAIState& oldState,
        const RocketTurretBlock::TurretAIState& newState);
    void MineTurretAIStateChanged(const MineTurretBlock* block, const MineTurretBlock::TurretAIState& oldState,
        const MineTurretBlock::TurretAIState& newState);
    void RocketFired(const RocketTurretBlock* block);
    void MineFired(const MineTurretBlock* block);

	void SetLevelAlpha(float alpha);
    float GetLevelAlpha() const;

	void UpdateItemDropBlock(const GameItemAssets& gameItemAssets, const ItemDropBlock& block);
    void UpdateRegenBlock(const RegenBlock* block, bool gotHurt);
    void UpdateNoEntryBlock(bool remoteControlRocketOn);

    BossMesh* GetBossMesh() const { return this->bossMesh; }
    double ActivateBossIntro();
    double ActivateBossExplodingFlashEffects(double delayInSecs, const GameModel* model, const Camera& camera);
    void BossHurt();

private:
	const GameLevel* currLevel;

	// Meshes for all types of level pieces
	Mesh* basicBlock;
	Mesh* bombBlock;
	Mesh* triangleBlockUR;                  // Triangle block (default position in upper-right)
	Mesh* inkBlock;

	PortalBlockMesh* portalBlock;
	PrismBlockMesh* prismBlockDiamond;		// Prism diamond block
	PrismBlockMesh* prismBlockTriangleUR;	// Prism triangle block in upper-right corner position

	CannonBlockMesh* cannonBlock;
    FragileCannonBlockMesh* fragileCannonBlock;
	CollateralBlockMesh* collateralBlock;
	TeslaBlockMesh* teslaBlock;
	ItemDropBlockMesh* itemDropBlock;
    SwitchBlockMesh* switchBlock;
    LaserTurretBlockMesh* laserTurretBlock;
    RocketTurretBlockMesh* rocketTurretBlock;
    MineTurretBlockMesh* mineTurretBlock;
    AlwaysDropBlockMesh* alwaysDropBlock;
    RegenBlockMesh* regenBlock;
    OneWayBlockMesh* oneWayBlock;
    Mesh* noEntryBlock;

	// Style-specific level pieces
	Mesh* styleBlock;

    // Boss (if there is one)
    BossMesh* bossMesh;

    // Misc. effects/texture pointers
    Texture2D* remainingPieceGlowTexture;
    ESPParticleScaleEffector remainingPiecePulser;

	// The unique identifiers of, and associated materials of the level piece meshes
	std::map<std::string, CgFxAbstractMaterialEffect*> levelMaterials;

	// Which display lists correspond to a given material
	std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> > firstPassDisplayListsPerMaterial;
    std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> > secondPassDisplayListsPerMaterial;
	// The display lists associated with each level piece
	std::map<const LevelPiece*, std::map<CgFxAbstractMaterialEffect*, GLuint> > firstPassPieceDisplayLists;
    std::map<const LevelPiece*, std::map<CgFxAbstractMaterialEffect*, GLuint> > secondPassPieceDisplayLists;
	// Special effects always present for specific level pieces
	std::map<const LevelPiece*, std::list<ESPEmitter*> > pieceEmitterEffects;
    // Effects for the last couple of pieces left in the level, to highlight them for the player
    std::map<const LevelPiece*, ESPEmitter*> lastPieceEffects;

	// Block status rendering object
	BlockStatusEffectRenderer* statusEffectRenderer;

    float levelAlpha;
        
	const std::map<std::string, MaterialGroup*>* GetMaterialGrpsForPieceType(const LevelPiece* piece) const;
	void CreateDisplayListsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation);
	void CreateEmitterEffectsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation);
	void CreateDisplayListForBallSafetyNet(float levelWidth);
	void Flush();	
};

inline float LevelMesh::GetLevelAlpha() const {
    return this->levelAlpha;
}

inline void LevelMesh::DrawPiecesPostEffects(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                                             const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    this->teslaBlock->DrawPostEffects(dT, camera, keyLight, fillLight, ballLight);
    this->laserTurretBlock->DrawPostEffects(dT, camera);
    this->mineTurretBlock->DrawPostEffects(dT, camera);
    this->rocketTurretBlock->DrawPostEffects(dT, camera);
}

inline void LevelMesh::LevelPieceAllStatusRemoved(const LevelPiece& piece) {
    this->statusEffectRenderer->RemoveAllLevelPieceStatus(piece);
    this->oneWayBlock->IceCubeStatusRemoved(&piece);
}

inline void LevelMesh::DrawStatusEffects(double dT, const Camera& camera, const Texture2D* sceneTexture) {
	this->statusEffectRenderer->Draw(dT, camera, sceneTexture);
}

/**
 * Call when the item type that the item drop block will drop, changes and the model needs to be
 * updated to display this to the player.
 */
inline void LevelMesh::UpdateItemDropBlock(const GameItemAssets& gameItemAssets, const ItemDropBlock& block) {
	// Figure out what texture is associated with the next item to be dropped from the block
	Texture2D* itemTexture = gameItemAssets.GetItemTexture(block.GetNextItemDropType());
	this->itemDropBlock->UpdateItemDropBlockTexture(&block, itemTexture);
}

inline void LevelMesh::UpdateRegenBlock(const RegenBlock* block, bool gotHurt) {
    this->regenBlock->UpdateRegenBlock(block, gotHurt);
}

#endif