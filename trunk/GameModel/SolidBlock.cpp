/**
 * SolidBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "SolidBlock.h"
#include "EmptySpaceBlock.h"
#include "Projectile.h"
#include "GameModel.h"
#include "GameEventManager.h"

SolidBlock::SolidBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {
}

SolidBlock::~SolidBlock() {
}

// Determine whether the given projectile will pass through this block...
bool SolidBlock::ProjectilePassesThrough(Projectile* projectile) const {
	if (projectile->GetType() == Projectile::CollateralBlockProjectile) {
		return true;
	}
	return false;
}

LevelPiece* SolidBlock::Destroy(GameModel* gameModel) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

	// When destroying a breakable there is the possiblity of dropping an item...
	gameModel->AddPossibleItemDrop(*this);

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

/**
 * Update the collision boundries of this solid block, solid blocks are special in that they will
 * ALWAYS have all possible collision boundries enabled and created.
 */
void SolidBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
															const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
															const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
															const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	UNUSED_PARAMETER(topRightNeighbor);
	UNUSED_PARAMETER(topLeftNeighbor);
	UNUSED_PARAMETER(bottomRightNeighbor);
	UNUSED_PARAMETER(bottomLeftNeighbor);

	// Clear all the currently existing boundry lines first
	this->bounds.Clear();

	// We ALWAYS create boundries unless the neighbour does not exist (NULL) 
	// or is another solid block.

	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

	// Left boundry of the piece
	if (leftNeighbor != NULL) {
		if (leftNeighbor->GetType() != LevelPiece::Solid) {
			Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n1(-1, 0);
			boundingLines.push_back(l1);
			boundingNorms.push_back(n1);
		}
	}

	// Bottom boundry of the piece
	if (bottomNeighbor != NULL) {
		if (bottomNeighbor->GetType() != LevelPiece::Solid) {
			Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n2(0, -1);
			boundingLines.push_back(l2);
			boundingNorms.push_back(n2);
		}
	}

	// Right boundry of the piece
	if (rightNeighbor != NULL) {
		if (rightNeighbor->GetType() != LevelPiece::Solid) {
			Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n3(1, 0);
			boundingLines.push_back(l3);
			boundingNorms.push_back(n3);
		}
	}

	// Top boundry of the piece
	if (topNeighbor != NULL) {
		if (topNeighbor->GetType() != LevelPiece::Solid) {
			Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n4(0, 1);
			boundingLines.push_back(l4);
			boundingNorms.push_back(n4);
		}
	}

	this->bounds = BoundingLines(boundingLines, boundingNorms);
}

bool SolidBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}

	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

/**
 * Called when the solid block is hit by a projectile. Tends to cause the projectile to
 * extinguish, however for the collateral block projectile, it will completely destroy this.
 */
LevelPiece* SolidBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {
		
		case Projectile::PaddleLaserBulletProjectile:
			break;
		
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