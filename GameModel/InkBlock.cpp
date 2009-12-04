/**
 * InkBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "InkBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"

InkBlock::InkBlock(unsigned int wLoc, unsigned int hLoc) :
LevelPiece(wLoc, hLoc) {
}

InkBlock::~InkBlock() {
}

/**
 * The ink block is destroyed and replaced by an empty space.
 * Returns: A new empty space block.
 */
LevelPiece* InkBlock::Destroy(GameModel* gameModel) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(this, emptyPiece);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

	return emptyPiece;
}

LevelPiece* InkBlock::CollisionOccurred(GameModel* gameModel, const GameBall& ball) {
	return this->Destroy(gameModel);
}

LevelPiece* InkBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	if (projectile->GetType() == Projectile::PaddleLaserProjectile) {
		return this->Destroy(gameModel);
	}
	return this;
}