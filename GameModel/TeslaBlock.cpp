/**
 * TeslaBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TeslaBlock.h"
#include "Projectile.h"
#include "GameModel.h"

TeslaBlock::TeslaBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc), electricityIsActive(false) {
}

TeslaBlock::~TeslaBlock() {
}

// Determine whether the given projectile will pass through this block...
bool TeslaBlock::ProjectilePassesThrough(Projectile* projectile) {
	return false;
}

/**
 * Update the collision boundries of this tesla block, tesla blocks are special in that they will
 * ALWAYS have all possible collision boundries enabled and created.
 */
void TeslaBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
															const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
															const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
															const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

		// Clear all the currently existing boundry lines first
		this->bounds.Clear();

		// We ALWAYS create boundries unless the neighbour does not exist (NULL) 
		// or is a solid/tesla block.

		// Set the bounding lines for a rectangular block
		std::vector<Collision::LineSeg2D> boundingLines;
		std::vector<Vector2D>  boundingNorms;

		// Left boundry of the piece
		if (leftNeighbor != NULL) {
			if (leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() != LevelPiece::Tesla) {
				Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
										 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
				Vector2D n1(-1, 0);
				boundingLines.push_back(l1);
				boundingNorms.push_back(n1);
			}
		}

		// Bottom boundry of the piece
		if (bottomNeighbor != NULL) {
			if (bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::Tesla) {
				Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
										 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
				Vector2D n2(0, -1);
				boundingLines.push_back(l2);
				boundingNorms.push_back(n2);
			}
		}

		// Right boundry of the piece
		if (rightNeighbor != NULL) {
			if (rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::Tesla) {
				Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
										 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				Vector2D n3(1, 0);
				boundingLines.push_back(l3);
				boundingNorms.push_back(n3);
			}
		}

		// Top boundry of the piece
		if (topNeighbor != NULL) {
			if (topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::Tesla) {
				Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
										 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				Vector2D n4(0, 1);
				boundingLines.push_back(l4);
				boundingNorms.push_back(n4);
			}
		}

		this->bounds = BoundingLines(boundingLines, boundingNorms);
}

bool TeslaBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return Collision::IsCollision(ray, Collision::AABB2D(this->GetCenter() + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
																this->GetCenter() +  Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT)), rayT);
}

/**
 * Called when the tesla block is hit by a projectile. Tends to cause the projectile to
 * extinguish - all projectiles also tend to activate/deactivate the tesla block.
 */
LevelPiece* TeslaBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	// TODO: Tell the game model to recalculate it's tesla lightning arcs...

	switch (projectile->GetType()) {
		
		case Projectile::PaddleLaserBulletProjectile:
			this->electricityIsActive = !this->electricityIsActive;
			break;
		
		case Projectile::CollateralBlockProjectile:
			this->electricityIsActive = !this->electricityIsActive;
			break;

		case Projectile::PaddleRocketBulletProjectile: {
				// The rocket should not destroy this block, however it certainly
				// is allowed to destroy blocks around it!
				LevelPiece* resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);
				assert(resultingPiece == this);
				this->electricityIsActive = !this->electricityIsActive;
			}
			break;

		default:
			assert(false);
			break;
	}

	return this;
}

