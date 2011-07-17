/**
 * OneWayBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "OneWayBlock.h"

#include "EmptySpaceBlock.h"
#include "Projectile.h"
#include "GameModel.h"
#include "GameEventManager.h"

OneWayBlock::OneWayBlock(const OneWayDir& dir, unsigned int wLoc, unsigned int hLoc) :
LevelPiece(wLoc, hLoc), dirType(dir) {

    switch (dir) {
        case OneWayBlock::OneWayUp:
            this->oneWayDir = Vector2D(0, 1);
            break;
        case OneWayBlock::OneWayDown:
            this->oneWayDir = Vector2D(0, -1);
            break;
        case OneWayBlock::OneWayLeft:
            this->oneWayDir = Vector2D(-1, 0);
            break;
        case OneWayBlock::OneWayRight:
            this->oneWayDir = Vector2D(1, 0);
            break;
        default:
            assert(false);
            break;
    }
}

OneWayBlock::~OneWayBlock() {
}

bool OneWayBlock::ConvertCharToOneWayDir(const char& oneWayChar, OneWayBlock::OneWayDir& oneWayDirEnum) {
    switch (oneWayChar) {
        case 'u':
            oneWayDirEnum = OneWayBlock::OneWayUp;
            break;
        case 'd':
            oneWayDirEnum = OneWayBlock::OneWayDown;
            break;
        case 'l':
            oneWayDirEnum = OneWayBlock::OneWayLeft;
            break;
        case 'r':
            oneWayDirEnum = OneWayBlock::OneWayRight;
            break;
        default:
            assert(false);
            return false;
    }
    return true;
}

// Determine whether the given projectile will pass through this block...
bool OneWayBlock::ProjectilePassesThrough(Projectile* projectile) const {
	switch (projectile->GetType()) {

		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
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
int OneWayBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return OneWayBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

LevelPiece* OneWayBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			// EVENT: Ice was shattered
			GameEventManager::Instance()->ActionBlockIceShattered(*this);
			bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
			assert(success);
	}

	// When destroying a breakable there is the possiblity of dropping an item...
	gameModel->AddPossibleItemDrop(*this);

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
 * Update the collision boundries of this block.
 */
void OneWayBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                               const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                               const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                               const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	// We ALWAYS create boundries unless the neighbour does not exist (NULL) 
	// or is another solid block.

	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

	// Left boundry of the piece
	if (leftNeighbor != NULL) {
        if (leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() != LevelPiece::OneWay) {
			Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n1(-1, 0);
			boundingLines.push_back(l1);
			boundingNorms.push_back(n1);
		}
	}

	// Bottom boundry of the piece
	if (bottomNeighbor != NULL) {
		if (bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::OneWay) {
			Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n2(0, -1);
			boundingLines.push_back(l2);
			boundingNorms.push_back(n2);
		}
	}

	// Right boundry of the piece
	if (rightNeighbor != NULL) {
		if (rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::OneWay) {
			Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n3(1, 0);
			boundingLines.push_back(l3);
			boundingNorms.push_back(n3);
		}
	}

	// Top boundry of the piece
	if (topNeighbor != NULL) {
		if (topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::OneWay) {
			Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n4(0, 1);
			boundingLines.push_back(l4);
			boundingNorms.push_back(n4);
		}
	}

	this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
		 						 topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

// Doesn't matter if a ball collides with solid block, it does nothing to the block.
LevelPiece* OneWayBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
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
			assert(success);
		}
	}

	return this;
}

/**
 * Called when the solid block is hit by a projectile. Tends to cause the projectile to
 * extinguish, however for the collateral block projectile, it will completely destroy this.
 */
LevelPiece* OneWayBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {
		
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			break;
		
		case Projectile::CollateralBlockProjectile:
            resultingPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);

            if (this->HasStatus(LevelPiece::IceCubeStatus)) {
                GameEventManager::Instance()->ActionBlockIceShattered(*this);
                bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                assert(success);
            }

			break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a solid block, unless it's frozen in an ice cube;
			// in that case, unfreeze a frozen solid block
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
				assert(success);
			}
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}