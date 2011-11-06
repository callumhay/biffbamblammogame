/**
* LaserTurretBlock.cpp
*
* (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
* Callum Hay, 2011
*
* You may not use this work for commercial purposes.
* If you alter, transform, or build upon this work, you may distribute the 
* resulting work only under the same or similar licence to this one.
*/

#include "LaserTurretBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "Beam.h"

const float LaserTurretBlock::BALL_DAMAGE_AMOUNT = LaserTurretBlock::PIECE_STARTING_LIFE_POINTS / 5.0f;

LaserTurretBlock::LaserTurretBlock(unsigned int wLoc, unsigned int hLoc) :
LevelPiece(wLoc, hLoc), currLifePoints(LaserTurretBlock::PIECE_STARTING_LIFE_POINTS) {
}

LaserTurretBlock::~LaserTurretBlock() {
}

// Determine whether the given projectile will pass through this block...
bool LaserTurretBlock::ProjectilePassesThrough(Projectile* projectile) const {
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

        default:
            break;
    }

    return false;
}

/**
 * Get the number of points when this piece changes to the given piece.
 */
int LaserTurretBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return LaserTurretBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

void LaserTurretBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                    const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                    const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                    const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    // Set the bounding lines for a rectangular block
    std::vector<Collision::LineSeg2D> boundingLines;
    std::vector<Vector2D>  boundingNorms;

    // Left boundry of the piece
    if (leftNeighbor != NULL) {
        if (leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() == LevelPiece::LaserTurret &&
            leftNeighbor->GetType() != LevelPiece::Breakable) {
                Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
                    this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n1(-1, 0);
                boundingLines.push_back(l1);
                boundingNorms.push_back(n1);
        }
    }

    // Bottom boundry of the piece
    if (bottomNeighbor != NULL) {
        if (bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() == LevelPiece::LaserTurret &&
            bottomNeighbor->GetType() != LevelPiece::Breakable) {
                Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n2(0, -1);
                boundingLines.push_back(l2);
                boundingNorms.push_back(n2);
        }
    }

    // Right boundry of the piece
    if (rightNeighbor != NULL) {
        if (rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() == LevelPiece::LaserTurret &&
            rightNeighbor->GetType() != LevelPiece::Breakable) {
                Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n3(1, 0);
                boundingLines.push_back(l3);
                boundingNorms.push_back(n3);
        }
    }

    // Top boundry of the piece
    if (topNeighbor != NULL) {
        if (topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() == LevelPiece::LaserTurret &&
            topNeighbor->GetType() != LevelPiece::Breakable) {
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

LevelPiece* LaserTurretBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// Check to see if the block is frozen...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        // EVENT: Ice was shattered
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
        assert(success);
	}

    if (method == LevelPiece::RocketDestruction && !this->IsDead()) {
        return this;
    }
    
    // EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

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

LevelPiece* LaserTurretBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {

	LevelPiece* resultingPiece = this;
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		if (isFireBall) {
			// Unfreeze a frozen block if it gets hit by a fireball
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}
            else {
                resultingPiece = this->DiminishPiece(LaserTurretBlock::BALL_DAMAGE_AMOUNT,
                    gameModel, LevelPiece::RegularDestruction);
            }
		}
        else {
            resultingPiece = this->DiminishPiece(LaserTurretBlock::BALL_DAMAGE_AMOUNT,
                gameModel, LevelPiece::RegularDestruction);
        }
	}

    ball.SetLastPieceCollidedWith(resultingPiece);
    return resultingPiece;
}

LevelPiece* LaserTurretBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	assert(gameModel != NULL);
	assert(projectile != NULL);

	LevelPiece* newPiece = this;
    
	switch (projectile->GetType()) {
	
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			else {	
				// Laser bullets dimish the piece, but don't necessarily obliterated/destroy it
                newPiece = this->DiminishPiece(projectile->GetDamage(), gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;

		case Projectile::CollateralBlockProjectile:
			// Completely destroy the block...
            newPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

        case Projectile::PaddleRocketBulletProjectile: {
            LevelPiece* tempPiece = this->DiminishPiece(projectile->GetDamage(), gameModel, LevelPiece::RocketDestruction);
			newPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);
            assert(tempPiece == newPiece);
            UNUSED_VARIABLE(tempPiece);
            break;
        }

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a laser turret block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}
			break;

		default:
			assert(false);
			break;
	}

	return newPiece;
}

LevelPiece* LaserTurretBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	
	// If the piece is frozen in ice we don't hurt it, instead it will refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}

	LevelPiece* newPiece = this->DiminishPiece(
        static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond())),
        gameModel, LevelPiece::LaserBeamDestruction);
	return newPiece;
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* LaserTurretBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
	
	LevelPiece* newPiece = this->DiminishPiece(
        static_cast<float>(dT * static_cast<double>(paddle.GetShieldDamagePerSecond())),
        gameModel, LevelPiece::PaddleShieldDestruction);
	return newPiece;
}

LevelPiece* LaserTurretBlock::DiminishPiece(float dmgAmount, GameModel* model, const LevelPiece::DestructionMethod& method) {
    currLifePoints -= dmgAmount;
    if (currLifePoints <= 0.0f) {
        return this->Destroy(model, method);
    }
    return this;
}