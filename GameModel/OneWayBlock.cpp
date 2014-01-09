/**
 * OneWayBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "OneWayBlock.h"

#include "EmptySpaceBlock.h"
#include "Projectile.h"
#include "GameModel.h"
#include "GameEventManager.h"

const float OneWayBlock::ACCEPTIBLE_MAX_ANGLE_TO_ONE_WAY_IN_RADS = M_PI / 180.0f * 82.0f; // (82 degrees)

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
bool OneWayBlock::ProjectilePassesThrough(const Projectile* projectile) const {
    if (this->IsGoingTheOneWay(projectile->GetVelocityDirection())) {
        return true;
    }

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
        case Projectile::PaddleFlameBlastProjectile:
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

	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
	    // EVENT: Ice was shattered
	    GameEventManager::Instance()->ActionBlockIceShattered(*this);
	    bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
	    assert(success);
	}

    // Only collateral blocks, tesla lightning and disintegration can destroy a one-way block
    if (method != LevelPiece::CollateralDestruction && method != LevelPiece::TeslaDestruction &&
        method != LevelPiece::DisintegrationDestruction) {
        return this;
    }

	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

    // Only drop an item if the block wasn't disintegrated
    if (method != LevelPiece::DisintegrationDestruction) {
	    gameModel->AddPossibleItemDrop(*this);
    }

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

    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D  boundingNorms[MAX_NUM_LINES];
    bool onInside[MAX_NUM_LINES];

    int lineCount = 0;
    bool hasBound, isOnInside;

	// Left boundary of the piece
	if (leftNeighbor != NULL) {
        if (leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() != LevelPiece::Solid &&
            leftNeighbor->GetType() != LevelPiece::LaserTurret && leftNeighbor->GetType() != LevelPiece::RocketTurret &&
            leftNeighbor->GetType() != LevelPiece::MineTurret && leftNeighbor->GetType() != LevelPiece::Breakable &&
            leftNeighbor->GetType() != LevelPiece::AlwaysDrop && leftNeighbor->GetType() != LevelPiece::Regen) {

            hasBound = true;
            isOnInside = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
                leftNeighbor->GetType() == LevelPiece::NoEntry);

            if (leftNeighbor->GetType() == LevelPiece::OneWay && !leftNeighbor->HasStatus(LevelPiece::IceCubeStatus)) {
                if (static_cast<const OneWayBlock*>(leftNeighbor)->GetDirType() != this->GetDirType()) {
                    isOnInside = true;
                }
                else {
                    hasBound = false;
                }
            }

            if (hasBound) {
                boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
                    this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			    boundingNorms[lineCount] = Vector2D(-1, 0);
                onInside[lineCount] = isOnInside;
                lineCount++;
            }
		}
	}

	// Bottom boundary of the piece
	if (bottomNeighbor != NULL) {
		if (bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            bottomNeighbor->GetType() != LevelPiece::Solid &&
            bottomNeighbor->GetType() != LevelPiece::LaserTurret && bottomNeighbor->GetType() != LevelPiece::RocketTurret &&
            bottomNeighbor->GetType() != LevelPiece::MineTurret && bottomNeighbor->GetType() != LevelPiece::Breakable &&
            bottomNeighbor->GetType() != LevelPiece::AlwaysDrop && bottomNeighbor->GetType() != LevelPiece::Regen) {

            hasBound = true;
            isOnInside = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
                bottomNeighbor->GetType() == LevelPiece::NoEntry);

            if (bottomNeighbor->GetType() == LevelPiece::OneWay && !bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus)) {
                if (static_cast<const OneWayBlock*>(bottomNeighbor)->GetDirType() != this->GetDirType()) {
                    isOnInside = true;
                }
                else {
                    hasBound = false;
                }
            }

            if (hasBound) {
                boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			    boundingNorms[lineCount] = Vector2D(0, -1);
                onInside[lineCount] = isOnInside;
                lineCount++;
            }
		}
	}

	// Right boundary of the piece
	if (rightNeighbor != NULL) {
		if (rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() != LevelPiece::Solid && 
            rightNeighbor->GetType() != LevelPiece::LaserTurret && rightNeighbor->GetType() != LevelPiece::RocketTurret &&
            rightNeighbor->GetType() != LevelPiece::MineTurret && rightNeighbor->GetType() != LevelPiece::Breakable &&
            rightNeighbor->GetType() != LevelPiece::AlwaysDrop && rightNeighbor->GetType() != LevelPiece::Regen) {

            hasBound = true;
            isOnInside = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
                rightNeighbor->GetType() == LevelPiece::NoEntry);

            if (rightNeighbor->GetType() == LevelPiece::OneWay && !rightNeighbor->HasStatus(LevelPiece::IceCubeStatus)) {
                if (static_cast<const OneWayBlock*>(rightNeighbor)->GetDirType() != this->GetDirType()) {
                    isOnInside = true;
                }
                else {
                    hasBound = false;
                }
            }

            if (hasBound) {
                boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			    boundingNorms[lineCount] = Vector2D(1, 0);
                onInside[lineCount] = isOnInside;
                lineCount++;
            }
		}
	}

	// Top boundary of the piece
	if (topNeighbor != NULL) {
		if (topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
            topNeighbor->GetType() != LevelPiece::Solid && 
            topNeighbor->GetType() != LevelPiece::LaserTurret && topNeighbor->GetType() != LevelPiece::RocketTurret &&
            topNeighbor->GetType() != LevelPiece::MineTurret && topNeighbor->GetType() != LevelPiece::Breakable &&
            topNeighbor->GetType() != LevelPiece::AlwaysDrop && topNeighbor->GetType() != LevelPiece::Regen) {

            hasBound = true;
            isOnInside = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
                topNeighbor->GetType() == LevelPiece::NoEntry);

            if (topNeighbor->GetType() == LevelPiece::OneWay && !topNeighbor->HasStatus(LevelPiece::IceCubeStatus)) {
                if (static_cast<const OneWayBlock*>(topNeighbor)->GetDirType() != this->GetDirType()) {
                    isOnInside = true;
                }
                else {
                    hasBound = false;
                }
            }

            if (hasBound) {
                boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			    boundingNorms[lineCount] = Vector2D(0, 1);
                onInside[lineCount] = isOnInside;
                lineCount++;
            }
		}
	}

	this->SetBounds(BoundingLines(lineCount, boundingLines, boundingNorms, onInside),
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

LevelPiece* OneWayBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	ball.SetLastPieceCollidedWith(NULL);

	// One-way blocks can be frozen, but they still can't be destroyed (in most cases)
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

		// Unfreeze a frozen block
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
 * Called when the block is hit by a projectile. Tends to cause the projectile to
 * extinguish, however for the collateral block projectile, it will completely destroy this.
 */
LevelPiece* OneWayBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
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
		
        case Projectile::BossLightningBoltBulletProjectile:
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
			// Fire glob just extinguishes on a one-way block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
            this->LightPieceOnFire(gameModel, false);
			break;

        case Projectile::PaddleFlameBlastProjectile:
            this->LightPieceOnFire(gameModel, false);
            break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}


//int OneWayBlock::GetBlockingBoundIndex() const {
//    switch (this->dirType) {
//        case OneWayBlock::OneWayUp:
//            return OneWayBlock::TOP_BOUNDRY_IDX;
//        case OneWayBlock::OneWayDown:
//            return OneWayBlock::BOTTOM_BOUNDRY_IDX;
//        case OneWayBlock::OneWayLeft:
//            return OneWayBlock::LEFT_BOUNDRY_IDX;
//        case OneWayBlock::OneWayRight:
//            return OneWayBlock::RIGHT_BOUNDRY_IDX;
//        default:
//            assert(false);
//            break;
//    }
//
//    assert(false);
//    return -1;
//}