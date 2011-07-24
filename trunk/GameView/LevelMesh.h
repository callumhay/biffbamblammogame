/**
 * LevelMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LEVELMESH_H__
#define __LEVELMESH_H__

#include "BlockStatusEffectRenderer.h"
#include "BallSafetyNetMesh.h"
#include "GameItemAssets.h"
#include "ItemDropBlockMesh.h"

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/ObjReader.h"
#include "../BlammoEngine/Light.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/ItemDropBlock.h"

class GameWorldAssets;
class BallSafetyNetMesh;
class Mesh;
class PrismBlockMesh;
class PortalBlockMesh;
class CannonBlockMesh;
class CollateralBlockMesh;
class TeslaBlockMesh;
class SwitchBlockMesh;
class MaterialGroup;
class ESPEmitter;

class LevelMesh {
public:
	LevelMesh(const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level);
	~LevelMesh();
	
	void ChangePiece(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void RemovePiece(const LevelPiece& piece);
	
	void DrawPieces(const Vector3D& worldTranslation, double dT, const Camera& camera, bool lightsAreOut, const BasicPointLight& keyLight, 
									const BasicPointLight& fillLight, const BasicPointLight& ballLight, const Texture2D* sceneTexture);
	void DrawSafetyNet(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;
	void DrawStatusEffects(double dT, const Camera& camera, const Texture2D* sceneTexture);

	void LoadNewLevel(const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level);

	void BallSafetyNetCreated();
	void BallSafetyNetDestroyed(const Point2D& pos);

	void LevelPieceStatusAdded(const LevelPiece& piece, const LevelPiece::PieceStatus& status);
	void LevelPieceStatusRemoved(const LevelPiece& piece, const LevelPiece::PieceStatus& status);
	void LevelPieceAllStatusRemoved(const LevelPiece& piece);

    void SwitchActivated(const SwitchBlock* block, const GameLevel* currLevel);
	void PaddleCameraActiveToggle(bool isActive);
	void SetLevelAlpha(float alpha);
	void UpdateItemDropBlock(const GameItemAssets& gameItemAssets, const ItemDropBlock& block);

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
    Mesh* oneWayUpBlock;
    Mesh* oneWayDownBlock;
    Mesh* oneWayLeftBlock;
    Mesh* oneWayRightBlock;

	// Style-specific level pieces
	Mesh* styleBlock;

	// Misc. display lists and meshes for other geometry activated by items and such
	BallSafetyNetMesh* ballSafetyNet;

	// The unique identifiers of, and associated materials of the level piece meshes
	std::map<std::string, CgFxMaterialEffect*> levelMaterials;

	// Which display lists correspond to a given material
	std::map<CgFxMaterialEffect*, std::vector<GLuint> > displayListsPerMaterial;
	// The display lists associated with each level piece
	std::map<const LevelPiece*, std::map<CgFxMaterialEffect*, GLuint> > pieceDisplayLists;
	// Special effects always present for specific level pieces
	std::map<const LevelPiece*, std::list<ESPEmitter*> > pieceEmitterEffects;

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

/**
 * Draw the ball safety net if it is currently active in the game (this
 * appears at the bottom of the level).
 */
inline void LevelMesh::DrawSafetyNet(double dT, const Camera& camera, const BasicPointLight& keyLight, 
																		 const BasicPointLight& fillLight, const BasicPointLight& ballLight) const {
	// If the ball safety net is active then we draw it
	assert(this->currLevel != NULL);
	if (this->currLevel->IsBallSafetyNetActive() || this->ballSafetyNet->IsPlayingAnimation()) {
		glPushMatrix();
		glTranslatef(-this->currLevel->GetLevelUnitWidth() / 2.0f, -(this->currLevel->GetLevelUnitHeight() / 2.0f + LevelPiece::HALF_PIECE_HEIGHT), 0.0f);
		this->ballSafetyNet->Draw(dT, camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}
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