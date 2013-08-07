/**
 * SolidBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "SolidBlock.h"
#include "EmptySpaceBlock.h"
#include "Projectile.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "TriangleBlocks.h"

SolidBlock::SolidBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {
}

SolidBlock::~SolidBlock() {
}

// Determine whether the given projectile will pass through this block...
bool SolidBlock::ProjectilePassesThrough(const Projectile* projectile) const {
	switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			// When frozen, projectiles can pass through
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				return true;
			}
			break;		

		case Projectile::CollateralBlockProjectile:
			return true;
		case Projectile::FireGlobProjectile:
            return false;

		default:
			break;
	}

	return false;
}

/**
 * Get the number of points when this piece changes to the given piece.
 */
int SolidBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return SolidBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

LevelPiece* SolidBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        // EVENT: Ice was shattered
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
        assert(success);
	}

    // Only collateral blocks and tesla lightning can destroy a solid block
    if (method != LevelPiece::CollateralDestruction && method != LevelPiece::TeslaDestruction) {
        return this;
    }

	// Currently, solid blocks don't drop items
	//gameModel->AddPossibleItemDrop(*this);

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

void SolidBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                              const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                              const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                              const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	// We ALWAYS create boundries unless the neighbour does not exist (NULL) 
	// or is another solid block.

	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;
    std::vector<bool> onInside;

    bool shouldGenBounds = false;

	// Left boundary of the piece
	if (leftNeighbor != NULL) {
		if (leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() != LevelPiece::Solid &&
            leftNeighbor->GetType() != LevelPiece::LaserTurret && 
            leftNeighbor->GetType() != LevelPiece::RocketTurret &&
            leftNeighbor->GetType() != LevelPiece::Tesla && 
            leftNeighbor->GetType() != LevelPiece::Switch &&
            leftNeighbor->GetType() != LevelPiece::MineTurret) {

            shouldGenBounds = true;
            TriangleBlock::Orientation triOrientation;
            if (TriangleBlock::GetOrientation(leftNeighbor, triOrientation)) {
                shouldGenBounds = !(triOrientation == TriangleBlock::UpperRight || triOrientation == TriangleBlock::LowerRight);
            }
		}
	}
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
	    Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
							    this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
	    Vector2D n1(-1, 0);
	    boundingLines.push_back(l1);
	    boundingNorms.push_back(n1);
        onInside.push_back(leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() == LevelPiece::OneWay || leftNeighbor->GetType() == LevelPiece::NoEntry);
    }
    shouldGenBounds = false;


	// Bottom boundary of the piece
	if (bottomNeighbor != NULL) {
		if (bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::LaserTurret && 
            bottomNeighbor->GetType() != LevelPiece::RocketTurret &&
            bottomNeighbor->GetType() != LevelPiece::Tesla && bottomNeighbor->GetType() != LevelPiece::Switch &&
            bottomNeighbor->GetType() != LevelPiece::MineTurret) {

            shouldGenBounds = true;
            TriangleBlock::Orientation triOrientation;
            if (TriangleBlock::GetOrientation(bottomNeighbor, triOrientation)) {
                shouldGenBounds = !(triOrientation == TriangleBlock::UpperRight || triOrientation == TriangleBlock::UpperLeft);
            }
		}
	}
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
	    Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
							    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
	    Vector2D n2(0, -1);
	    boundingLines.push_back(l2);
	    boundingNorms.push_back(n2);
        onInside.push_back(bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            bottomNeighbor->GetType() == LevelPiece::OneWay || bottomNeighbor->GetType() == LevelPiece::NoEntry);
    }
    shouldGenBounds = false;


	// Right boundary of the piece
	if (rightNeighbor != NULL) {
		if (rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::LaserTurret && 
            rightNeighbor->GetType() != LevelPiece::RocketTurret &&
            rightNeighbor->GetType() != LevelPiece::Tesla && rightNeighbor->GetType() != LevelPiece::Switch &&
            rightNeighbor->GetType() != LevelPiece::MineTurret) {

            shouldGenBounds = true;
            TriangleBlock::Orientation triOrientation;
            if (TriangleBlock::GetOrientation(rightNeighbor, triOrientation)) {
                shouldGenBounds = !(triOrientation == TriangleBlock::UpperLeft || triOrientation == TriangleBlock::LowerLeft);
            }
		}
	}
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
	    Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
							    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
	    Vector2D n3(1, 0);
	    boundingLines.push_back(l3);
	    boundingNorms.push_back(n3);
        onInside.push_back(rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() == LevelPiece::OneWay || rightNeighbor->GetType() == LevelPiece::NoEntry);
    }
    shouldGenBounds = false;

	// Top boundary of the piece
	if (topNeighbor != NULL) {
		if (topNeighbor->HasStatus(LevelPiece::IceCubeStatus) || topNeighbor->HasStatus(LevelPiece::OnFireStatus) ||
            topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::LaserTurret && 
            topNeighbor->GetType() != LevelPiece::RocketTurret &&
            topNeighbor->GetType() != LevelPiece::Tesla && topNeighbor->GetType() != LevelPiece::Switch &&
            topNeighbor->GetType() != LevelPiece::MineTurret) {


            shouldGenBounds = true;
            TriangleBlock::Orientation triOrientation;
            if (TriangleBlock::GetOrientation(topNeighbor, triOrientation)) {
                shouldGenBounds = !(triOrientation == TriangleBlock::LowerRight || triOrientation == TriangleBlock::LowerLeft);
            }
		}
	}
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
	    Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
							     this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
	    Vector2D n4(0, 1);
	    boundingLines.push_back(l4);
	    boundingNorms.push_back(n4);
        onInside.push_back(topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            topNeighbor->HasStatus(LevelPiece::OnFireStatus) || topNeighbor->GetType() == LevelPiece::OneWay ||
            topNeighbor->GetType() == LevelPiece::NoEntry);
    }

	this->SetBounds(BoundingLines(boundingLines, boundingNorms, onInside),
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

bool SolidBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

// Doesn't matter if a ball collides with solid block, it does nothing to the block.
LevelPiece* SolidBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	ball.SetLastPieceCollidedWith(NULL);

	// Solid blocks can be frozen, but they still can't be destroyed (in most cases)
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		if (!isFireBall) {
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				// EVENT: Ice was shattered
				GameEventManager::Instance()->ActionBlockIceShattered(*this);
			}
		}

		// Unfreeze a frozen solid block
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
            UNUSED_VARIABLE(success);
			assert(success);

            if (isFireBall) {
                // EVENT: Frozen block cancelled-out by fire
                GameEventManager::Instance()->ActionBlockIceCancelledWithFire(*this);
            }
		}
	}

	return this;
}

/**
 * Called when the solid block is hit by a projectile. Tends to cause the projectile to
 * extinguish, however for the collateral block projectile, it will completely destroy this.
 */
LevelPiece* SolidBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {
		
        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			break;
		
		case Projectile::CollateralBlockProjectile:
            resultingPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:

            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);

            if (this->HasStatus(LevelPiece::IceCubeStatus)) {
                GameEventManager::Instance()->ActionBlockIceShattered(*this);
                bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
                assert(success);
            }

			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a solid block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
			this->LightPieceOnFire(gameModel, false);
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}