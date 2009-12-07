/**
 * PrismBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PrismBlock.h"

/**
 * Update the boundry lines of the prism block. The prism block is shaped like a diamond, meaning
 * we care about its corner neighbors as well. If any of the 3 neighbors making up a full corner and
 * its two sides are eliminated, we need to form a boundry.
 */
void PrismBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
															const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
															const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
															const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

		// Clear all the currently existing boundry lines first
		this->bounds.Clear();

		// Set the bounding lines for a diamond-shaped block
		std::vector<Collision::LineSeg2D> boundingLines;
		std::vector<Vector2D>  boundingNorms;

		// Bottom-left diagonal boundry
		if ((leftNeighbor != NULL && leftNeighbor->IsNoBoundsPieceType()) ||
			  (bottomLeftNeighbor != NULL && bottomLeftNeighbor->IsNoBoundsPieceType()) ||
				(bottomNeighbor != NULL && bottomNeighbor->IsNoBoundsPieceType())) {
			
			Collision::LineSeg2D boundry(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, 0), 
															this->center + Vector2D(0, -LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n1(-LevelPiece::HALF_PIECE_HEIGHT, -LevelPiece::HALF_PIECE_WIDTH);
			n1.Normalize();

			boundingLines.push_back(boundry);
			boundingNorms.push_back(n1);
		}

		// Bottom-right diagonal boundry
		if ((rightNeighbor != NULL && rightNeighbor->IsNoBoundsPieceType()) ||
			  (bottomRightNeighbor != NULL && bottomRightNeighbor->IsNoBoundsPieceType()) ||
				(bottomNeighbor != NULL && bottomNeighbor->IsNoBoundsPieceType())) {
			Collision::LineSeg2D boundry(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, 0), 
																	 this->center + Vector2D(0, -LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n1(LevelPiece::HALF_PIECE_HEIGHT, -LevelPiece::HALF_PIECE_WIDTH);
			n1.Normalize();

			boundingLines.push_back(boundry);
			boundingNorms.push_back(n1);
		}

		// Top-left diagonal boundry
		if ((leftNeighbor != NULL && leftNeighbor->IsNoBoundsPieceType()) ||
			  (topLeftNeighbor != NULL && topLeftNeighbor->IsNoBoundsPieceType()) ||
				(topNeighbor != NULL && topNeighbor->IsNoBoundsPieceType())) {

			Collision::LineSeg2D boundry(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, 0), 
															this->center + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n1(-LevelPiece::HALF_PIECE_HEIGHT, LevelPiece::HALF_PIECE_WIDTH);
			n1.Normalize();

			boundingLines.push_back(boundry);
			boundingNorms.push_back(n1);
		}

		// Top-right diagonal boundry
		if ((rightNeighbor != NULL && rightNeighbor->IsNoBoundsPieceType()) ||
			  (topRightNeighbor != NULL && topRightNeighbor->IsNoBoundsPieceType()) ||
				(topNeighbor != NULL && topNeighbor->IsNoBoundsPieceType())) {

			Collision::LineSeg2D boundry(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, 0), 
																	 this->center + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n1(LevelPiece::HALF_PIECE_HEIGHT, LevelPiece::HALF_PIECE_WIDTH);
			n1.Normalize();

			boundingLines.push_back(boundry);
			boundingNorms.push_back(n1);
		}

		this->bounds = BoundingLines(boundingLines, boundingNorms);
}

LevelPiece* PrismBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {


	// TODO...


	return this;
}