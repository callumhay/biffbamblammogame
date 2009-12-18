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
#include "GameModel.h"

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
		
		// Need to figure out if this laser bullet already collided with this block... if it has then we just ignore it
		if (!projectile->IsLastLevelPieceCollidedWith(this)) {
			
			// Determine how the projectile will move through the prism based on where it hit...
			const float PROJECTILE_VELOCITY_MAG			= projectile->GetVelocityMagnitude();
			const Vector2D PROJECTILE_VELOCITY_DIR	= projectile->GetVelocityDirection();
			const Vector2D OLD_PROJECTILE_DELTA			= projectile->GetPosition() - this->GetCenter();

			const float MIDDLE_HALF_INTERVAL_X			= projectile->GetHalfWidth();
			const float MIDDLE_HALF_INTERVAL_Y			= projectile->GetHalfWidth() * 0.5f;
			const float REFLECTION_REFRACTION_ANGLE	= 15.0f;
			
			if (fabs(OLD_PROJECTILE_DELTA[0]) <= MIDDLE_HALF_INTERVAL_X) {

				// Projectile is almost in the very center (reflect in 3 directions with smaller projectiles in each)
				if (OLD_PROJECTILE_DELTA[1] < 0.0f) {
					// Projectile is colliding with the lower-center - we only do special refraction stuff if the angle is
					// almost perfectly perpendicular to the bottom
					const Vector2D CURRENT_NORMAL = Vector2D(0.0f, -1.0f);
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser < REFLECTION_REFRACTION_ANGLE) {
						// The current projectile will just fly through the block, we spawn two other projectiles to go out
						// of the top right and left
						const Vector2D TOP_RIGHT_NORMAL = Vector2D(1.0f, 1.0f) / SQRT_2;
						Projectile* newTopRightProjectile = gameModel->AddProjectile(Projectile::PaddleLaserBulletProjectile, projectile->GetPosition());
						newTopRightProjectile->SetVelocity(TOP_RIGHT_NORMAL, projectile->GetVelocityMagnitude());
						newTopRightProjectile->SetLastLevelPieceCollidedWith(this);	// If we don't do this then it will cause recursive doom
						
						const Vector2D TOP_LEFT_NORMAL = Vector2D(-1.0f, 1.0f) / SQRT_2;
						Projectile* newTopLeftProjectile  = gameModel->AddProjectile(Projectile::PaddleLaserBulletProjectile, projectile->GetPosition());
						newTopLeftProjectile->SetVelocity(TOP_LEFT_NORMAL, projectile->GetVelocityMagnitude());
						newTopLeftProjectile->SetLastLevelPieceCollidedWith(this); // If we don't do this then it will cause recursive doom
					}
				}
				else {
					// Projectile is colliding with the upper-center - we only do special refraction stuff if the angle is
					// almost perfectly perpendicular to the top
					const Vector2D CURRENT_NORMAL = Vector2D(0.0f, 1.0f);
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser < REFLECTION_REFRACTION_ANGLE) {
						// The current projectile will just fly through the block, we spawn two other projectiles to go out
						// of the bottom right and left
						const Vector2D BOTTOM_RIGHT_NORMAL = Vector2D(1.0f, -1.0f) / SQRT_2;
						Projectile* newBottomRightProjectile = gameModel->AddProjectile(Projectile::PaddleLaserBulletProjectile, projectile->GetPosition());
						newBottomRightProjectile->SetVelocity(BOTTOM_RIGHT_NORMAL, projectile->GetVelocityMagnitude());
						newBottomRightProjectile->SetLastLevelPieceCollidedWith(this);	// If we don't do this then it will cause recursive doom
						
						const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(-1.0f, -1.0f) / SQRT_2;
						Projectile* newBottomLeftProjectile  = gameModel->AddProjectile(Projectile::PaddleLaserBulletProjectile, projectile->GetPosition());
						newBottomLeftProjectile->SetVelocity(BOTTOM_LEFT_NORMAL, projectile->GetVelocityMagnitude());
						newBottomLeftProjectile->SetLastLevelPieceCollidedWith(this); // If we don't do this then it will cause recursive doom
					}
				}

			}
			else if (OLD_PROJECTILE_DELTA[0] < 0.0f){
				// Projectile is on the left side of the center 

				if (fabs(OLD_PROJECTILE_DELTA[1]) <= MIDDLE_HALF_INTERVAL_Y) {
					// Projectile is colliding with the center-left - we only do special refraction stuff if the angle is
					// almost perfectly perpendicular to the left
					const Vector2D CURRENT_NORMAL = Vector2D(-1.0f, 0.0f);
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser < REFLECTION_REFRACTION_ANGLE) {
						// The current projectile will fly right through, we spawn two other projectiles to go out the
						// top-right and bottom-right
						const Vector2D BOTTOM_RIGHT_NORMAL = Vector2D(1.0f, -1.0f) / SQRT_2;
						Projectile* newBottomRightProjectile = gameModel->AddProjectile(Projectile::PaddleLaserBulletProjectile, this->GetCenter());
						newBottomRightProjectile->SetVelocity(BOTTOM_RIGHT_NORMAL, projectile->GetVelocityMagnitude());
						newBottomRightProjectile->SetLastLevelPieceCollidedWith(this);	// If we don't do this then it will cause recursive doom
						
						const Vector2D TOP_RIGHT_NORMAL = Vector2D(1.0f, 1.0f) / SQRT_2;
						Projectile* newTopRightProjectile  = gameModel->AddProjectile(Projectile::PaddleLaserBulletProjectile, this->GetCenter());
						newTopRightProjectile->SetVelocity(TOP_RIGHT_NORMAL, projectile->GetVelocityMagnitude());
						newTopRightProjectile->SetLastLevelPieceCollidedWith(this); // If we don't do this then it will cause recursive doom						

						// Since the piece is wider than it is tall we place all the particles at the center for this
						projectile->SetPosition(this->GetCenter());
					}
				}
				else if (OLD_PROJECTILE_DELTA[1] < 0.0f) {
					// Projectile is colliding with the lower-left boundry...

					// Based on the angle of the laser it will either pass through or reflect
					const Vector2D CURRENT_NORMAL = Vector2D(-1.0f, -1.0f) / SQRT_2;
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser > REFLECTION_REFRACTION_ANGLE) {
						// Reflect the laser in the normal
						Vector2D newVelDir = Reflect(PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL);
						newVelDir.Normalize();

						// Move the projectile to where it collided and change its velocity to reflect
						projectile->SetPosition(projectile->GetPosition() + projectile->GetHalfHeight()*PROJECTILE_VELOCITY_DIR);
						projectile->SetVelocity(newVelDir, PROJECTILE_VELOCITY_MAG);
					}
				}
				else {
					// Projectile is colliding with the upper-left boundry...

					// Based on the angle of the laser it will either pass through or reflect
					const Vector2D CURRENT_NORMAL = Vector2D(-1.0f, 1.0f) / SQRT_2;
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser > REFLECTION_REFRACTION_ANGLE) {
						// Reflect the laser in the normal
						Vector2D newVelDir = Reflect(PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL);
						newVelDir.Normalize();

						// Move the projectile to where it collided and change its velocity to reflect
						projectile->SetPosition(projectile->GetPosition() + projectile->GetHalfHeight()*PROJECTILE_VELOCITY_DIR);
						projectile->SetVelocity(newVelDir, PROJECTILE_VELOCITY_MAG);
					}
				}
			}
			else {
				// Projecitle is on the right side of the center

				if (fabs(OLD_PROJECTILE_DELTA[1]) <= MIDDLE_HALF_INTERVAL_Y) {
					// Projectile is colliding with the center-right - we only do special refraction stuff if the angle is
					// almost perfectly perpendicular to the left
					const Vector2D CURRENT_NORMAL = Vector2D(1.0f, 0.0f);
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser < REFLECTION_REFRACTION_ANGLE) {
						// The current projectile will fly right through, we spawn two other projectiles to go out the
						// top-right and bottom-right
						const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(-1.0f, -1.0f) / SQRT_2;
						Projectile* newBottomLeftProjectile = gameModel->AddProjectile(Projectile::PaddleLaserBulletProjectile, this->GetCenter());
						newBottomLeftProjectile->SetVelocity(BOTTOM_LEFT_NORMAL, projectile->GetVelocityMagnitude());
						newBottomLeftProjectile->SetLastLevelPieceCollidedWith(this);	// If we don't do this then it will cause recursive doom
						
						const Vector2D TOP_LEFT_NORMAL = Vector2D(-1.0f, 1.0f) / SQRT_2;
						Projectile* newTopLeftProjectile  = gameModel->AddProjectile(Projectile::PaddleLaserBulletProjectile, this->GetCenter());
						newTopLeftProjectile->SetVelocity(TOP_LEFT_NORMAL, projectile->GetVelocityMagnitude());
						newTopLeftProjectile->SetLastLevelPieceCollidedWith(this); // If we don't do this then it will cause recursive doom						

						// Since the piece is wider than it is tall we place all the particles at the center for this
						projectile->SetPosition(this->GetCenter());
					}

				}
				else if (OLD_PROJECTILE_DELTA[1] < 0.0f) {
					// Projectile is colliding with the lower-right boundry...

					// Based on the angle of the laser it will either pass through or reflect
					const Vector2D CURRENT_NORMAL = Vector2D(1.0f, -1.0f) / SQRT_2;
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser > REFLECTION_REFRACTION_ANGLE) {
						// Reflect the laser in the normal
						Vector2D newVelDir = Reflect(PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL);
						newVelDir.Normalize();

						// Move the projectile to where it collided and change its velocity to reflect
						projectile->SetPosition(projectile->GetPosition() + projectile->GetHalfHeight()*PROJECTILE_VELOCITY_DIR);
						projectile->SetVelocity(newVelDir, PROJECTILE_VELOCITY_MAG);
					}
				}
				else {
					// Projectile is colliding with the upper-right boundry...

					// Based on the angle of the laser it will either pass through or reflect
					const Vector2D CURRENT_NORMAL = Vector2D(1.0f, 1.0f) / SQRT_2;
					float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(Vector2D::Dot(-PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL)));
					if (angleBetweenNormalAndLaser > REFLECTION_REFRACTION_ANGLE) {
						// Reflect the laser in the normal
						Vector2D newVelDir = Reflect(PROJECTILE_VELOCITY_DIR, CURRENT_NORMAL);
						newVelDir.Normalize();

						// Move the projectile to where it collided and change its velocity to reflect
						projectile->SetPosition(projectile->GetPosition() + projectile->GetHalfHeight()*PROJECTILE_VELOCITY_DIR);
						projectile->SetVelocity(newVelDir, PROJECTILE_VELOCITY_MAG);	
					}
				}
			}

			// The projectile has now officially collided with this prism block, set it into the projectile
			// so that it doesn't keep happening while the projectile is colliding with this block
			projectile->SetLastLevelPieceCollidedWith(this);
		}
	}

	return this;
}