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

const float TeslaBlock::LIGHTNING_ARC_RADIUS = LevelPiece::PIECE_HEIGHT * 0.25f;

TeslaBlock::TeslaBlock(bool isActive, bool isChangable, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), electricityIsActive(isActive), isChangable(isChangable) {
}

TeslaBlock::~TeslaBlock() {
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
		static const float HALF_TESLA_HEIGHT_BOUND = 0.8f * LevelPiece::HALF_PIECE_HEIGHT;
		static const float HALF_TESLA_WIDTH_BOUND  = 0.6f * LevelPiece::HALF_PIECE_WIDTH;

		// Set the bounding lines for a rectangular block
		std::vector<Collision::LineSeg2D> boundingLines;
		std::vector<Vector2D>  boundingNorms;

		// Left boundry of the piece
		if (leftNeighbor != NULL) {
			if (leftNeighbor->GetType() != LevelPiece::Solid) {
				Collision::LineSeg2D l1(this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND), 
										 this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND));
				Vector2D n1(-1, 0);
				boundingLines.push_back(l1);
				boundingNorms.push_back(n1);
			}
		}

		// Bottom boundry of the piece
		if (bottomNeighbor != NULL) {
			if (bottomNeighbor->GetType() != LevelPiece::Solid) {
				Collision::LineSeg2D l2(this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND),
										 this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND));
				Vector2D n2(0, -1);
				boundingLines.push_back(l2);
				boundingNorms.push_back(n2);
			}
		}

		// Right boundry of the piece
		if (rightNeighbor != NULL) {
			if (rightNeighbor->GetType() != LevelPiece::Solid) {
				Collision::LineSeg2D l3(this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND),
										 this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND));
				Vector2D n3(1, 0);
				boundingLines.push_back(l3);
				boundingNorms.push_back(n3);
			}
		}

		// Top boundry of the piece
		if (topNeighbor != NULL) {
			if (topNeighbor->GetType() != LevelPiece::Solid) {
				Collision::LineSeg2D l4(this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND),
										 this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND));
				Vector2D n4(0, 1);
				boundingLines.push_back(l4);
				boundingNorms.push_back(n4);
			}
		}

		this->bounds = BoundingLines(boundingLines, boundingNorms);
}

bool TeslaBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

LevelPiece* TeslaBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	if (ball.IsLastPieceCollidedWith(this)) {
		return this;
	}

	// Tell the ball what the last piece it collided with was...
	ball.SetLastPieceCollidedWith(this);
	
	// Toggle the electricity to the tesla block
	GameLevel* currLevel = gameModel->GetCurrentLevel();
	this->ToggleElectricity(*gameModel, *currLevel);

	return this;
}

/**
 * Called when the tesla block is hit by a projectile. Tends to cause the projectile to
 * extinguish - all projectiles also tend to activate/deactivate the tesla block.
 */
LevelPiece* TeslaBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	// TODO: Tell the game model to recalculate it's tesla lightning arcs...

	GameLevel* currLevel = gameModel->GetCurrentLevel();
	switch (projectile->GetType()) {
		
		case Projectile::PaddleLaserBulletProjectile:
			this->ToggleElectricity(*gameModel, *currLevel);
			break;
		
		case Projectile::CollateralBlockProjectile:
			this->ToggleElectricity(*gameModel, *currLevel);
			break;

		case Projectile::PaddleRocketBulletProjectile: {
				// The rocket should not destroy this block, however it certainly
				// is allowed to destroy blocks around it!
				LevelPiece* resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);
				assert(resultingPiece == this);
				this->ToggleElectricity(*gameModel, *currLevel);
			}
			break;

		default:
			assert(false);
			break;
	}

	return this;
}

void TeslaBlock::ToggleElectricity(GameModel& gameModel, GameLevel& level) {
	if (!this->GetIsChangable()) {
		return;
	}

	// Get the list of active connected tesla blocks
	std::list<TeslaBlock*> activeNeighbourTeslaBlocks = this->GetActiveConnectedTeslaBlocks();

	// Toggle the electricity...
	this->electricityIsActive = !this->electricityIsActive;

	// Based on whether the electricity is now active or not, signal the proper events and add/remove
	// lightning arcs from the current game level
	if (this->electricityIsActive) {
		for (std::list<TeslaBlock*>::const_iterator iter = activeNeighbourTeslaBlocks.begin(); iter != activeNeighbourTeslaBlocks.end(); ++iter) {
			const TeslaBlock* activeNeighbour = *iter;
			assert(activeNeighbour != NULL);
			level.AddTeslaLightningBarrier(&gameModel, this, activeNeighbour);
		}
	}
	else {
		for (std::list<TeslaBlock*>::const_iterator iter = activeNeighbourTeslaBlocks.begin(); iter != activeNeighbourTeslaBlocks.end(); ++iter) {
			const TeslaBlock* activeNeighbour = *iter;
			assert(activeNeighbour != NULL);
			level.RemoveTeslaLightningBarrier(this, activeNeighbour);
		}
	}
}