/**
 * PrismBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PrismBlock.h"
#include "EmptySpaceBlock.h"
#include "PaddleLaserProjectile.h"
#include "GameModel.h"

const float PrismBlock::REFLECTION_REFRACTION_SPLIT_ANGLE	= 25.0f;

PrismBlock::PrismBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {
}

PrismBlock::~PrismBlock() {
}

LevelPiece* PrismBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {

    // Only collateral blocks and tesla lightning can destroy a prism block
    if (method != LevelPiece::CollateralDestruction && method != LevelPiece::TeslaDestruction) {
        return this;
    }

	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPiece, method);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

	return emptyPiece;
}

/**
 * Update the boundary lines of the prism block. The prism block is shaped like a diamond, meaning
 * we care about its corner neighbors as well. If any of the 3 neighbors making up a full corner and
 * its two sides are eliminated, we need to form a boundary.
 */
void PrismBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                              const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                              const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                              const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	// Set the bounding lines for a diamond-shaped block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;
    std::vector<bool> onInside;

	//std::vector<Collision::LineSeg2D> reflectRefractBoundingLines;
    //std::vector<Vector2D>  reflectRefractBoundingNorms;

    bool shouldGenBounds = false;

    // Bottom-left diagonal boundary
    if ((leftNeighbor != NULL && leftNeighbor->IsNoBoundsPieceType()) ||
        (bottomLeftNeighbor != NULL && bottomLeftNeighbor->IsNoBoundsPieceType()) ||
        (bottomNeighbor != NULL && bottomNeighbor->IsNoBoundsPieceType())) {

        shouldGenBounds = true;
    }
    else if (leftNeighbor == NULL || bottomLeftNeighbor == NULL || bottomNeighbor == NULL) {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        Collision::LineSeg2D boundary(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, 0), 
            this->center + Vector2D(0, -LevelPiece::HALF_PIECE_HEIGHT));
        Vector2D n1(-LevelPiece::HALF_PIECE_HEIGHT, -LevelPiece::HALF_PIECE_WIDTH);
        n1.Normalize();

        boundingLines.push_back(boundary);
        boundingNorms.push_back(n1);
        onInside.push_back(
            ((leftNeighbor == NULL || !leftNeighbor->IsNoBoundsPieceType()) && (bottomNeighbor == NULL || !bottomNeighbor->IsNoBoundsPieceType())) ||
            ((leftNeighbor == NULL || !leftNeighbor->IsNoBoundsPieceType()) && (bottomLeftNeighbor == NULL || !bottomLeftNeighbor->IsNoBoundsPieceType())) ||
            ((bottomNeighbor == NULL || !bottomNeighbor->IsNoBoundsPieceType()) && (bottomLeftNeighbor == NULL || !bottomLeftNeighbor->IsNoBoundsPieceType()))
        );
                            
        shouldGenBounds = false;
    }
    
    // Bottom-right diagonal boundary
    if ((rightNeighbor != NULL && rightNeighbor->IsNoBoundsPieceType()) ||
        (bottomRightNeighbor != NULL && bottomRightNeighbor->IsNoBoundsPieceType()) ||
        (bottomNeighbor != NULL && bottomNeighbor->IsNoBoundsPieceType())) {
            
        shouldGenBounds = true;
    }
    else if (rightNeighbor == NULL || bottomRightNeighbor == NULL || bottomNeighbor == NULL) {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        Collision::LineSeg2D boundary(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, 0), 
            this->center + Vector2D(0, -LevelPiece::HALF_PIECE_HEIGHT));
        Vector2D n1(LevelPiece::HALF_PIECE_HEIGHT, -LevelPiece::HALF_PIECE_WIDTH);
        n1.Normalize();

        boundingLines.push_back(boundary);
        boundingNorms.push_back(n1);
        onInside.push_back(
            ((rightNeighbor == NULL || !rightNeighbor->IsNoBoundsPieceType()) && (bottomNeighbor == NULL || !bottomNeighbor->IsNoBoundsPieceType())) ||
            ((rightNeighbor == NULL || !rightNeighbor->IsNoBoundsPieceType()) && (bottomRightNeighbor == NULL || !bottomRightNeighbor->IsNoBoundsPieceType())) ||
            ((bottomNeighbor == NULL || !bottomNeighbor->IsNoBoundsPieceType()) && (bottomRightNeighbor == NULL || !bottomRightNeighbor->IsNoBoundsPieceType()))
        );

        shouldGenBounds = false;
    }
    
    // Top-left diagonal boundary
    if ((leftNeighbor != NULL && leftNeighbor->IsNoBoundsPieceType()) ||
        (topLeftNeighbor != NULL && topLeftNeighbor->IsNoBoundsPieceType()) ||
        (topNeighbor != NULL && topNeighbor->IsNoBoundsPieceType())) {

        shouldGenBounds = true;
    }
    else if (leftNeighbor == NULL || topLeftNeighbor == NULL || topNeighbor == NULL) {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        Collision::LineSeg2D boundary(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, 0), 
            this->center + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT));
        Vector2D n1(-LevelPiece::HALF_PIECE_HEIGHT, LevelPiece::HALF_PIECE_WIDTH);
        n1.Normalize();

        boundingLines.push_back(boundary);
        boundingNorms.push_back(n1);
        onInside.push_back(
            ((leftNeighbor == NULL || !leftNeighbor->IsNoBoundsPieceType()) && (topNeighbor == NULL || !topNeighbor->IsNoBoundsPieceType())) ||
            ((leftNeighbor == NULL || !leftNeighbor->IsNoBoundsPieceType()) && (topLeftNeighbor == NULL || !topLeftNeighbor->IsNoBoundsPieceType())) ||
            ((topNeighbor == NULL || !topNeighbor->IsNoBoundsPieceType()) && (topLeftNeighbor == NULL || !topLeftNeighbor->IsNoBoundsPieceType()))
        );

        shouldGenBounds = false;
    }

    // Top-right diagonal boundary
    if ((rightNeighbor != NULL && rightNeighbor->IsNoBoundsPieceType()) ||
        (topRightNeighbor != NULL && topRightNeighbor->IsNoBoundsPieceType()) ||
        (topNeighbor != NULL && topNeighbor->IsNoBoundsPieceType())) {

        shouldGenBounds = true;
    }
    else if (rightNeighbor == NULL || topRightNeighbor == NULL || topNeighbor == NULL) {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        Collision::LineSeg2D boundary(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, 0), 
            this->center + Vector2D(0, LevelPiece::HALF_PIECE_HEIGHT));
        Vector2D n1(LevelPiece::HALF_PIECE_HEIGHT, LevelPiece::HALF_PIECE_WIDTH);
        n1.Normalize();

        boundingLines.push_back(boundary);
        boundingNorms.push_back(n1);
        onInside.push_back(
            ((rightNeighbor == NULL || !rightNeighbor->IsNoBoundsPieceType()) && (topNeighbor == NULL      || !topNeighbor->IsNoBoundsPieceType())) ||
            ((rightNeighbor == NULL || !rightNeighbor->IsNoBoundsPieceType()) && (topRightNeighbor == NULL || !topRightNeighbor->IsNoBoundsPieceType())) ||
            ((topNeighbor == NULL   || !topNeighbor->IsNoBoundsPieceType())   && (topRightNeighbor == NULL || !topRightNeighbor->IsNoBoundsPieceType()))
        );

    }

    this->SetBounds(BoundingLines(boundingLines, boundingNorms, onInside),
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

/**
 * Called when the prism block is hit by a projectile.
 * The prism block tends to reflect and refract laser projectiles fired at it.
 */
LevelPiece* PrismBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile: {
		    // Based on where the laser bullet hits, we change its direction	
			// Need to figure out if this laser bullet already collided with this block... if it has then we just ignore it
			if (!projectile->IsLastThingCollidedWith(this)) {
				
				const float PROJECTILE_VELOCITY_MAG    = projectile->GetVelocityMagnitude();
				const Vector2D PROJECTILE_VELOCITY_DIR = projectile->GetVelocityDirection();
				const Point2D IMPACT_POINT = projectile->GetPosition() + projectile->GetHalfHeight()*PROJECTILE_VELOCITY_DIR;

				std::list<Collision::Ray2D> rays;
				this->GetReflectionRefractionRays(IMPACT_POINT, PROJECTILE_VELOCITY_DIR, rays);
				assert(rays.size() >= 1);
				
				std::list<Collision::Ray2D>::iterator rayIter = rays.begin();
				// The first ray is how the current projectile gets transmitted through this block...
				projectile->SetPosition(rayIter->GetOrigin());
				projectile->SetVelocity(rayIter->GetUnitDirection(), PROJECTILE_VELOCITY_MAG);
				projectile->SetLastThingCollidedWith(this);

                // Make refracted rays smaller based on how many there are
                float scaleFactor = Projectile::GetProjectileSplitScaleFactor(rays.size());

				// All the other rays were created via refraction or some such thing, so spawn new particles for them
				++rayIter;
				for (; rayIter != rays.end(); ++rayIter) {
                    Projectile* newProjectile = Projectile::CreateProjectileFromCopy(projectile, true);

                    newProjectile->SetWidth(scaleFactor * newProjectile->GetWidth());
                    newProjectile->SetHeight(scaleFactor * newProjectile->GetHeight());

					newProjectile->SetPosition(rayIter->GetOrigin() + newProjectile->GetHalfHeight() * rayIter->GetUnitDirection());
					newProjectile->SetVelocity(rayIter->GetUnitDirection(), PROJECTILE_VELOCITY_MAG);
					newProjectile->SetLastThingCollidedWith(this); // If we don't do this then it will cause recursive doom

					gameModel->AddProjectile(newProjectile);
				}
			}
		}
		break;

        case Projectile::BossLightningBoltBulletProjectile:
            break;

		case Projectile::CollateralBlockProjectile:
			// The collateral block projectile will actually completely destroy the prism block
            resultingPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			// Fire glob will just extinguish
			break;

		default:
			assert(false);
			break;

	}

	return resultingPiece;
}

/**
 * Obtains all reflected/refracted rays that can result from an impacting object at a given point on this
 * prism block with the given impact velocity unit direction.
 */
void PrismBlock::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const {
	Collision::Ray2D defaultRay(hitPoint, impactDir);	// This is what happens to the original ray

	// Determine how the ray will move through the prism based on where it hit...
	const Vector2D OLD_PROJECTILE_DELTA     = hitPoint - this->GetCenter();
	const float MIDDLE_HALF_INTERVAL_X      = LevelPiece::PIECE_WIDTH / 5.2f;
	const float MIDDLE_HALF_INTERVAL_Y      = LevelPiece::PIECE_HEIGHT / 3.5f;
	
	
	if (fabs(OLD_PROJECTILE_DELTA[0]) <= MIDDLE_HALF_INTERVAL_X) {

		// Ray is almost in the very center (reflect in 3 directions with smaller projectiles in each)
		if (OLD_PROJECTILE_DELTA[1] <= EPSILON) {

			// Colliding with the lower-center - we only do special refraction stuff if the angle is
			// almost perfectly perpendicular to the bottom
			const Vector2D CURRENT_NORMAL = Vector2D(0.0f, -1.0f);
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-impactDir, CURRENT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= REFLECTION_REFRACTION_SPLIT_ANGLE) {
				// We spawn two other rays to go out of the top right and left
				const Vector2D TOP_RIGHT_NORMAL = Vector2D(1.0f, 1.0f) / SQRT_2;
				const Vector2D TOP_LEFT_NORMAL = Vector2D(-1.0f, 1.0f) / SQRT_2;
				rays.push_back(Collision::Ray2D(hitPoint, TOP_RIGHT_NORMAL));
				rays.push_back(Collision::Ray2D(hitPoint, TOP_LEFT_NORMAL));
			}
		}
		else {
			// Colliding with the upper-center - we only do special refraction stuff if the angle is
			// almost perfectly perpendicular to the top
			const Vector2D CURRENT_NORMAL = Vector2D(0.0f, 1.0f);
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-impactDir, CURRENT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= REFLECTION_REFRACTION_SPLIT_ANGLE) {
				// We spawn two other rays to go out of the bottom right and left
				const Vector2D BOTTOM_RIGHT_NORMAL = Vector2D(1.0f, -1.0f) / SQRT_2;
				const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(-1.0f, -1.0f) / SQRT_2;
				rays.push_back(Collision::Ray2D(hitPoint, BOTTOM_RIGHT_NORMAL));
				rays.push_back(Collision::Ray2D(hitPoint, BOTTOM_LEFT_NORMAL));
			}
		}
	}
	else if (OLD_PROJECTILE_DELTA[0] <= EPSILON){
		// Ray is on the left side of the center 

		if (fabs(OLD_PROJECTILE_DELTA[1]) <= MIDDLE_HALF_INTERVAL_Y) {
			// Colliding with the center-left - we only do special refraction stuff if the angle is
			// almost perfectly perpendicular to the left
			const Vector2D CURRENT_NORMAL = Vector2D(-1.0f, 0.0f);
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-impactDir, CURRENT_NORMAL)))));
			if (angleBetweenNormalAndLaser < REFLECTION_REFRACTION_SPLIT_ANGLE) {
				// We spawn two other projectiles to go out the top-right and bottom-right
				const Vector2D BOTTOM_RIGHT_NORMAL = Vector2D(1.0f, -1.0f) / SQRT_2;
				const Vector2D TOP_RIGHT_NORMAL = Vector2D(1.0f, 1.0f) / SQRT_2;
				rays.push_back(Collision::Ray2D(this->GetCenter(), BOTTOM_RIGHT_NORMAL));
				rays.push_back(Collision::Ray2D(this->GetCenter(), TOP_RIGHT_NORMAL));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else if (OLD_PROJECTILE_DELTA[1] <= EPSILON) {
			// Ray is colliding with the lower-left boundary...

			// Based on the angle of the ray it will either pass through or reflect
			const Vector2D CURRENT_NORMAL = Vector2D(-1.0f, -1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-impactDir, CURRENT_NORMAL)))));
			if (angleBetweenNormalAndLaser >= REFLECTION_REFRACTION_SPLIT_ANGLE) {
				// Reflect the laser in the normal
				Vector2D newVelDir = Reflect(impactDir, CURRENT_NORMAL);
				newVelDir.Normalize();
				defaultRay.SetUnitDirection(newVelDir);
			}
		}
		else {
			// Ray is colliding with the upper-left boundary...

			// Based on the angle of the ray it will either pass through or reflect
			const Vector2D CURRENT_NORMAL = Vector2D(-1.0f, 1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-impactDir, CURRENT_NORMAL)))));
			if (angleBetweenNormalAndLaser >= REFLECTION_REFRACTION_SPLIT_ANGLE) {
				// Reflect the ray in the normal
				Vector2D newVelDir = Reflect(impactDir, CURRENT_NORMAL);
				newVelDir.Normalize();
				defaultRay.SetUnitDirection(newVelDir);
			}
		}
	}
	else {
		// Ray is on the right side of the center

		if (fabs(OLD_PROJECTILE_DELTA[1]) <= MIDDLE_HALF_INTERVAL_Y) {
			// Colliding with the center-right - we only do special refraction stuff if the angle is
			// almost perfectly perpendicular to the left
			const Vector2D CURRENT_NORMAL = Vector2D(1.0f, 0.0f);
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-impactDir, CURRENT_NORMAL)))));
			if (angleBetweenNormalAndLaser < REFLECTION_REFRACTION_SPLIT_ANGLE) {
				// We spawn two other rays to go out the top-right and bottom-right
				const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(-1.0f, -1.0f) / SQRT_2;
				const Vector2D TOP_LEFT_NORMAL = Vector2D(-1.0f, 1.0f) / SQRT_2;
				rays.push_back(Collision::Ray2D(this->GetCenter(), BOTTOM_LEFT_NORMAL));
				rays.push_back(Collision::Ray2D(this->GetCenter(), TOP_LEFT_NORMAL));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else if (OLD_PROJECTILE_DELTA[1] <= EPSILON) {
			// Ray is colliding with the lower-right boundary...

			// Based on the angle of the ray it will either pass through or reflect
			const Vector2D CURRENT_NORMAL = Vector2D(1.0f, -1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-impactDir, CURRENT_NORMAL)))));
			if (angleBetweenNormalAndLaser >= REFLECTION_REFRACTION_SPLIT_ANGLE) {
				// Reflect the ray in the normal
				Vector2D newVelDir = Reflect(impactDir, CURRENT_NORMAL);
				newVelDir.Normalize();
				defaultRay.SetUnitDirection(newVelDir);
			}
		}
		else {
			// Ray is colliding with the upper-right boundary...

			// Based on the angle of the ray it will either pass through or reflect
			const Vector2D CURRENT_NORMAL = Vector2D(1.0f, 1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-impactDir, CURRENT_NORMAL)))));
			if (angleBetweenNormalAndLaser >= REFLECTION_REFRACTION_SPLIT_ANGLE) {
				// Reflect the ray in the normal
				Vector2D newVelDir = Reflect(impactDir, CURRENT_NORMAL);
				newVelDir.Normalize();
				defaultRay.SetUnitDirection(newVelDir);
			}
		}
	}

	rays.push_front(defaultRay);
}