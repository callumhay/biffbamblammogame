/**
 * LevelMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LEVELMESH_H__
#define __LEVELMESH_H__

#include "BlockStatusEffectRenderer.h"
#include "GameItemAssets.h"
#include "ItemDropBlockMesh.h"

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
class CollateralBlockMesh;
class TeslaBlockMesh;
class SwitchBlockMesh;
class LaserTurretBlockMesh;
class RocketTurretBlockMesh;
class MineTurretBlockMesh;
class MaterialGroup;
class ESPEmitter;
class BossMesh;

class LevelMesh {
public:
	LevelMesh(const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level);
	~LevelMesh();
	
	void ChangePiece(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void RemovePiece(const LevelPiece& piece);
	
	void DrawPieces(const Vector3D& worldTranslation, double dT, const Camera& camera, bool lightsAreOut, const BasicPointLight& keyLight, 
	    const BasicPointLight& fillLight, const BasicPointLight& ballLight, const Texture2D* sceneTexture);
	void DrawStatusEffects(double dT, const Camera& camera, const Texture2D* sceneTexture);
    void DrawBoss(const Vector3D& worldTranslation, double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);

	void LoadNewLevel(const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level);

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
	void UpdateItemDropBlock(const GameItemAssets& gameItemAssets, const ItemDropBlock& block);

    double ActivateBossIntro();
    double ActivateBossExplodingFlashEffects(double delayInSecs, const GameModel* model, const Camera& camera);

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
	CollateralBlockMesh* collateralBlock;
	TeslaBlockMesh* teslaBlock;
	ItemDropBlockMesh* itemDropBlock;
    SwitchBlockMesh* switchBlock;
    LaserTurretBlockMesh* laserTurretBlock;
    RocketTurretBlockMesh* rocketTurretBlock;
    MineTurretBlockMesh* mineTurretBlock;
    Mesh* noEntryBlock;
    Mesh* oneWayUpBlock;
    Mesh* oneWayDownBlock;
    Mesh* oneWayLeftBlock;
    Mesh* oneWayRightBlock;

	// Style-specific level pieces
	Mesh* styleBlock;

    // Boss (if there is one)
    BossMesh* bossMesh;

    // Misc. effects/texture pointers
    Texture2D* remainingPieceGlowTexture;
    ESPParticleScaleEffector remainingPiecePulser;

	// The unique identifiers of, and associated materials of the level piece meshes
	std::map<std::string, CgFxMaterialEffect*> levelMaterials;

	// Which display lists correspond to a given material
	std::map<CgFxMaterialEffect*, std::vector<GLuint> > displayListsPerMaterial;
	// The display lists associated with each level piece
	std::map<const LevelPiece*, std::map<CgFxMaterialEffect*, GLuint> > pieceDisplayLists;
	// Special effects always present for specific level pieces
	std::map<const LevelPiece*, std::list<ESPEmitter*> > pieceEmitterEffects;
    // Effects for the last couple of pieces left in the level, to highlight them for the player
    std::map<const LevelPiece*, ESPEmitter*> lastPieceEffects;

	// Block status rendering object
	BlockStatusEffectRenderer* statusEffectRenderer;

	const std::map<std::string, MaterialGroup*>* GetMaterialGrpsForPieceType(const LevelPiece* piece) const;
	void CreateDisplayListsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation);
	void CreateEmitterEffectsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation);
	void CreateDisplayListForBallSafetyNet(float levelWidth);
	void Flush();	
};

inline void LevelMesh::LevelPieceStatusAdded(const LevelPiece& piece, const LevelPiece::PieceStatus& status) {
	this->statusEffectRenderer->AddLevelPieceStatus(piece, status);
}

inline void LevelMesh::LevelPieceStatusRemoved(const LevelPiece& piece, const LevelPiece::PieceStatus& status) {
	this->statusEffectRenderer->RemoveLevelPieceStatus(piece, status);
}

inline void LevelMesh::LevelPieceAllStatusRemoved(const LevelPiece& piece) {
	this->statusEffectRenderer->RemoveAllLevelPieceStatus(piece);
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
	Texture2D* itemTexture = gameItemAssets.GetItemTexture(block.GetNextDropItemType());
	this->itemDropBlock->UpdateItemDropBlockTexture(&block, itemTexture);
}

#endif