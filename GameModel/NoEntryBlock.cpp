/**
 * NoEntryBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "NoEntryBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"

NoEntryBlock::NoEntryBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {
}

NoEntryBlock::~NoEntryBlock() {
}

bool NoEntryBlock::ProjectilePassesThrough(const Projectile* projectile) const {
    return projectile->IsRefractableOrReflectable() || !this->HasStatus(LevelPiece::IceCubeStatus);
}

/**
 * Get the number of points when this piece changes to the given piece.
 */
int NoEntryBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return NoEntryBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

LevelPiece* NoEntryBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {

	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
	    // EVENT: Ice was shattered
	    GameEventManager::Instance()->ActionBlockIceShattered(*this);
	    bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
	    assert(success);
	}

    // Only collateral blocks, tesla lightning and disintegration can destroy a no-entry block
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

void NoEntryBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	// Set the bounding lines for a rectangular block
    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D boundingNorms[MAX_NUM_LINES];
    bool onInside[MAX_NUM_LINES];

    bool shouldGenBounds = false;
    int lineCount = 0;

	// Left boundary of the piece
	if (leftNeighbor != NULL) {
        if (leftNeighbor->GetType() != LevelPiece::Solid &&
            leftNeighbor->GetType() != LevelPiece::NoEntry &&
            leftNeighbor->GetType() != LevelPiece::LaserTurret &&
            leftNeighbor->GetType() != LevelPiece::RocketTurret &&
            leftNeighbor->GetType() != LevelPiece::MineTurret &&
            leftNeighbor->GetType() != LevelPiece::Cannon &&
            leftNeighbor->GetType() != LevelPiece::Regen) {

            shouldGenBounds = true;
		}
	}
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
            this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));;
		boundingNorms[lineCount] = Vector2D(-1, 0);
        onInside[lineCount] = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) || 
            leftNeighbor->GetType() == LevelPiece::Breakable || leftNeighbor->GetType() == LevelPiece::OneWay);
        lineCount++;
    }
    shouldGenBounds = false;

	// Bottom boundary of the piece
	if (bottomNeighbor != NULL) {
		if (bottomNeighbor->GetType() != LevelPiece::Solid &&
            bottomNeighbor->GetType() != LevelPiece::NoEntry &&
            bottomNeighbor->GetType() != LevelPiece::LaserTurret &&
            bottomNeighbor->GetType() != LevelPiece::RocketTurret &&
            bottomNeighbor->GetType() != LevelPiece::MineTurret &&
            bottomNeighbor->GetType() != LevelPiece::Cannon &&
            bottomNeighbor->GetType() != LevelPiece::Regen) {
            
            shouldGenBounds = true;
		}
	}
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
            this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		boundingNorms[lineCount] = Vector2D(0, -1);
        onInside[lineCount] = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) || 
            bottomNeighbor->GetType() == LevelPiece::Breakable || bottomNeighbor->GetType() == LevelPiece::OneWay);
        lineCount++;
    }
    shouldGenBounds = false;

	// Right boundary of the piece
	if (rightNeighbor != NULL) {
		if (rightNeighbor->GetType() != LevelPiece::Solid &&
            rightNeighbor->GetType() != LevelPiece::NoEntry &&
            rightNeighbor->GetType() != LevelPiece::LaserTurret &&
            rightNeighbor->GetType() != LevelPiece::RocketTurret &&
            rightNeighbor->GetType() != LevelPiece::MineTurret &&
            rightNeighbor->GetType() != LevelPiece::Cannon &&
            rightNeighbor->GetType() != LevelPiece::Regen) {

            shouldGenBounds = true;
		}
	}
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
            this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));;
		boundingNorms[lineCount] = Vector2D(1, 0);
        onInside[lineCount] = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) || 
            rightNeighbor->GetType() == LevelPiece::Breakable || rightNeighbor->GetType() == LevelPiece::OneWay);
        lineCount++;
    }
    shouldGenBounds = false;

	// Top boundary of the piece
	if (topNeighbor != NULL) {
		if (topNeighbor->GetType() != LevelPiece::Solid &&
            topNeighbor->GetType() != LevelPiece::NoEntry &&
            topNeighbor->GetType() != LevelPiece::LaserTurret &&
            topNeighbor->GetType() != LevelPiece::RocketTurret &&
            topNeighbor->GetType() != LevelPiece::MineTurret &&
            topNeighbor->GetType() != LevelPiece::Cannon &&
            topNeighbor->GetType() != LevelPiece::Regen) {

            shouldGenBounds = true;
		}
	}
    else {
        shouldGenBounds = true;
    }
    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
            this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		boundingNorms[lineCount] = Vector2D(0, 1);
        onInside[lineCount] = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) || 
            topNeighbor->GetType() == LevelPiece::Breakable || topNeighbor->GetType() == LevelPiece::OneWay);
        lineCount++;
    }

	this->SetBounds(BoundingLines(lineCount, boundingLines, boundingNorms, onInside), 
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
		topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

LevelPiece* NoEntryBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	ball.SetLastPieceCollidedWith(NULL);

	// No Entry blocks can be frozen, but they still can't be destroyed (in most cases)
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
 * Called when the block is hit by a projectile. Projectiles tend to be able to pass right
 * through this block type, however for the collateral block projectile, it will completely destroy this.
 */
LevelPiece* NoEntryBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
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
			
            if (this->HasStatus(LevelPiece::IceCubeStatus)) {

                assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			    resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);

                if (this->HasStatus(LevelPiece::IceCubeStatus)) {
                    GameEventManager::Instance()->ActionBlockIceShattered(*this);
                    bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                    UNUSED_VARIABLE(success);
                    assert(success);
                }
            }

			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a no-entry block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
			this->LightPieceOnFire(gameModel, false);
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

void NoEntryBlock::RemoveStatus(GameLevel* level, const PieceStatus& status) {
    LevelPiece::RemoveStatus(level, status);

    // If the status removed was ice, then detach all attached projectiles...
    if (status == LevelPiece::IceCubeStatus) {
        this->DetachAllProjectiles();
    }
}