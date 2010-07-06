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
#include "Beam.h"

InkBlock::InkBlock(unsigned int wLoc, unsigned int hLoc) :
LevelPiece(wLoc, hLoc), currLifePoints(InkBlock::PIECE_STARTING_LIFE_POINTS) {
}

InkBlock::~InkBlock() {
}

// Determine whether the given projectile will pass through this block...
bool InkBlock::ProjectilePassesThrough(Projectile* projectile) {
	if (projectile->GetType() == Projectile::CollateralBlockProjectile) {
		return true;
	}
	return false;
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

LevelPiece* InkBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	return this->Destroy(gameModel);
}

LevelPiece* InkBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {

		case Projectile::PaddleLaserBulletProjectile:
		case Projectile::CollateralBlockProjectile:
			resultingPiece = this->Destroy(gameModel);
			break;

		case Projectile::PaddleRocketBulletProjectile:
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

LevelPiece* InkBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	
	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));

	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... destroy it
		this->currLifePoints = 0;
		newPiece = this->Destroy(gameModel);
	}

	return newPiece;
}