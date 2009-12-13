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
#include "Projectile.h"

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

/**
 * The prism blocks reflects and refracts laser projectiles fired at it.
 */
LevelPiece* PrismBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	if (projectile->GetType() == Projectile::PaddleLaserBulletProjectile) {
		// Based on where the laser bullet hits, we change its direction
		
		// Need to figure out if this laser bullet already collided with this block... if it has
		// then we just ignore it, if not then we need to change its trajectory once it goes through
		// the center of this block based on its x-position relative to this block
		if (!projectile->IsLastLevelPieceCollidedWith(this)) {
			
			// Determine how the projectile will move through the prism based on where it hit...
			const float PROJECTILE_VELOCITY_MAG			= projectile->GetVelocityMagnitude();
			const Vector2D PROJECTILE_VELOCITY_DIR	= projectile->GetVelocityDirection();
			const Vector2D OLD_PROJECTILE_DELTA			= projectile->GetPosition() - this->GetCenter();
			const float MIDDLE_HALF_INTERVAL				= projectile->GetHalfWidth()*0.7f;
			
			if (fabs(OLD_PROJECTILE_DELTA[0]) < MIDDLE_HALF_INTERVAL) {
				// Projectile is almost in the very center (reflect in 3 directions with smaller projectiles in each)
				if (OLD_PROJECTILE_DELTA[1] < 0.0f) {
					// Projectile is colliding with the lower-center

				}
				else {
					// Projectile is colliding with the upper-center

				}
			}
			else if (OLD_PROJECTILE_DELTA[0] < 0.0f){
				// Projectile is on the left side of the center 

				if (fabs(OLD_PROJECTILE_DELTA[1]) < MIDDLE_HALF_INTERVAL) {
					// Projectile is colliding with the center-left


				}
				if (OLD_PROJECTILE_DELTA[1] < 0.0f) {
					// Projectile is colliding with the lower-left boundry...

					// Based on the angle of the laser it will either pass through or reflect
					const Vector2D CURRENT_NORMAL = Vector2D(-1.0f, -1.0f) / 1.414213562f;
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser > 10) {
						// Reflect the laser in the normal
						Vector2D newVelDir = Reflect(PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL);
						newVelDir.Normalize();
						projectile->SetVelocity(newVelDir, PROJECTILE_VELOCITY_MAG);
					}
				}
				else {
					// Projectile is colliding with the upper-left boundry...

					// Based on the angle of the laser it will either pass through or reflect
					const Vector2D CURRENT_NORMAL = Vector2D(-1.0f, 1.0f) / 1.414213562f;
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser > 10) {
						// Reflect the laser in the normal
						Vector2D newVelDir = Reflect(PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL);
						newVelDir.Normalize();
						projectile->SetVelocity(newVelDir, PROJECTILE_VELOCITY_MAG);
					}
				}
			}
			else {
				// Projecitle is on the right side of the center

				if (fabs(OLD_PROJECTILE_DELTA[1]) < MIDDLE_HALF_INTERVAL) {
					// Projectile is colliding with the center-right

				}
				if (OLD_PROJECTILE_DELTA[1] < 0.0f) {
					// Projectile is colliding with the lower-right boundry

				}
				else {
					// Projectile is colliding with the upper-right boundry

				}


				Vector2D newVelDir(-LevelPiece::HALF_PIECE_HEIGHT, LevelPiece::HALF_PIECE_WIDTH);
				newVelDir.Normalize();
				projectile->SetVelocity(newVelDir, PROJECTILE_VELOCITY_MAG);

				// Move the projectile to the proper location in the piece - from this location it will move 
				// in its new reflected/refracted direction
				//projectile->SetPosition(this->GetCenter() - Vector2D(LevelPiece::HALF_PIECE_WIDTH*0.5f, 0.0f));
			}

			// The projectile has now officially collided with this prism block, set it into the projectile
			// so that it doesn't keep happening while the projectile is colliding with this block
			projectile->SetLastLevelPieceCollidedWith(this);
		}
	}

	return this;
}