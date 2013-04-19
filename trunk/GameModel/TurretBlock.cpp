/**
 * TurretBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TurretBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "Beam.h"

TurretBlock::TurretBlock(unsigned int wLoc, unsigned int hLoc, float life) :
LevelPiece(wLoc, hLoc), currLifePoints(life), startingLifePoints(life) {
}

TurretBlock::~TurretBlock() {
}

bool TurretBlock::ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
    if (((b.GetBallType() & GameBall::IceBall) == GameBall::IceBall)) {
        return false;
    }

    return b.GetCollisionDamage() < this->currLifePoints ;
}

bool TurretBlock::ProjectileIsDestroyedOnCollision(const Projectile* projectile) const {
    switch (projectile->GetType()) {

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            if (projectile->IsLastThingCollidedWith(this)) {
                break;
            }

            // Mines are destroyed by collisions with turrets.
            return true;

        default:
            break;
    }

    return !this->ProjectilePassesThrough(projectile);
}

void TurretBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                               const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                               const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                               const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    // Set the bounding lines for a rectangular block
    std::vector<Collision::LineSeg2D> boundingLines;
    std::vector<Vector2D>  boundingNorms;
    std::vector<bool> onInside;

    // Left boundry of the piece
    if (leftNeighbor != NULL) {
        if (leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() != LevelPiece::LaserTurret &&
            leftNeighbor->GetType() != LevelPiece::Breakable && leftNeighbor->GetType() != LevelPiece::RocketTurret &&
            leftNeighbor->GetType() != LevelPiece::MineTurret && leftNeighbor->GetType() != LevelPiece::AlwaysDrop &&
            leftNeighbor->GetType() != LevelPiece::Regen) {

                Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
                    this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n1(-1, 0);
                boundingLines.push_back(l1);
                boundingNorms.push_back(n1);
                onInside.push_back(leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
                    leftNeighbor->GetType() == LevelPiece::OneWay);
        }
    }

    // Bottom boundry of the piece
    if (bottomNeighbor != NULL) {
        if (bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::LaserTurret &&
            bottomNeighbor->GetType() != LevelPiece::Breakable && bottomNeighbor->GetType() != LevelPiece::RocketTurret &&
            bottomNeighbor->GetType() != LevelPiece::MineTurret && bottomNeighbor->GetType() != LevelPiece::AlwaysDrop &&
            bottomNeighbor->GetType() != LevelPiece::Regen) {

                Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n2(0, -1);
                boundingLines.push_back(l2);
                boundingNorms.push_back(n2);
                onInside.push_back(bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
                    bottomNeighbor->GetType() == LevelPiece::OneWay);
        }
    }

    // Right boundry of the piece
    if (rightNeighbor != NULL) {
        if (rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::LaserTurret &&
            rightNeighbor->GetType() != LevelPiece::Breakable && rightNeighbor->GetType() != LevelPiece::RocketTurret &&
            rightNeighbor->GetType() != LevelPiece::MineTurret && rightNeighbor->GetType() != LevelPiece::AlwaysDrop &&
            rightNeighbor->GetType() != LevelPiece::Regen) {

                Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n3(1, 0);
                boundingLines.push_back(l3);
                boundingNorms.push_back(n3);
                onInside.push_back(rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
                    rightNeighbor->GetType() == LevelPiece::OneWay);
        }
    }

    // Top boundry of the piece
    if (topNeighbor != NULL) {
        if (topNeighbor->HasStatus(LevelPiece::IceCubeStatus) || topNeighbor->HasStatus(LevelPiece::OnFireStatus) ||
            topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::LaserTurret &&
            topNeighbor->GetType() != LevelPiece::Breakable && topNeighbor->GetType() != LevelPiece::RocketTurret &&
            topNeighbor->GetType() != LevelPiece::MineTurret && topNeighbor->GetType() != LevelPiece::AlwaysDrop &&
            topNeighbor->GetType() != LevelPiece::Regen) {

                Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n4(0, 1);
                boundingLines.push_back(l4);
                boundingNorms.push_back(n4);
                onInside.push_back(topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
                    topNeighbor->HasStatus(LevelPiece::OnFireStatus) || topNeighbor->GetType() == LevelPiece::OneWay);
        }
    }

    this->SetBounds(BoundingLines(boundingLines, boundingNorms, onInside),
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

LevelPiece* TurretBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {

	// Check to see if the block is frozen...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        // EVENT: Ice was shattered
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
        assert(success);
	}

    if ((method == LevelPiece::MineDestruction || method == LevelPiece::RocketDestruction) && !this->IsDead()) {
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

    // Add to the boost meter
    gameModel->AddPercentageToBoostMeter(0.25);

	return emptyPiece;
}

LevelPiece* TurretBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {

	LevelPiece* resultingPiece = this;
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		// Unfreeze a frozen turret if it gets hit by a fireball
        if (isFireBall && this->HasStatus(LevelPiece::IceCubeStatus)) {
			bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
            UNUSED_VARIABLE(success);
			assert(success);
            
            // EVENT: Frozen block cancelled-out by fire
            GameEventManager::Instance()->ActionBlockIceCancelledWithFire(*this);
		}
        else {
            // TODO: Set turrets on fire? Have them say "ouch" when they are...?
            // If I change this then I need to change the ProducesBounceEffectsWithBallWhenHit method as well!

            resultingPiece = this->DiminishPiece(ball.GetCollisionDamage(), gameModel, LevelPiece::RegularDestruction);
        }
	}

    ball.SetLastPieceCollidedWith(resultingPiece);
    return resultingPiece;
}

LevelPiece* TurretBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
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
LevelPiece* TurretBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
	
	LevelPiece* newPiece = this->DiminishPiece(
        static_cast<float>(dT * static_cast<double>(paddle.GetShieldDamagePerSecond())),
        gameModel, LevelPiece::PaddleShieldDestruction);
	return newPiece;
}
