/**
 * RegenBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "RegenBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "GameLevel.h"
#include "GameBall.h"
#include "FireGlobProjectile.h"

const float RegenBlock::REGEN_LIFE_POINTS_PER_SECOND = 6.0f;

RegenBlock::RegenBlock(bool hasInfiniteLife, unsigned int wLoc, unsigned int hLoc) :
LevelPiece(wLoc, hLoc), currLifePoints(0.0f), fireGlobDropCountdown(GameModelConstants::GetInstance()->GenerateFireGlobDropTime()) {
    if (hasInfiniteLife) {
        this->currLifePoints = RegenBlock::INFINITE_LIFE_POINTS;
    }
    else {
        this->currLifePoints = RegenBlock::MAX_LIFE_POINTS;
    }
}

RegenBlock::~RegenBlock() {
}

// Whether or not the ball can just blast right through this block.
// Returns: true if it can, false otherwise.
bool RegenBlock::BallBlastsThrough(const GameBall& b) const {
    // The may blast through this if it's uber and not firey/icy
	return ((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall) &&
           ((b.GetBallType() & GameBall::IceBall) != GameBall::IceBall) && 
           ((b.GetBallType() & GameBall::FireBall) != GameBall::FireBall) &&
           (!this->HasInfiniteLife() && this->currLifePoints <= b.GetCollisionDamage());
}

// Determine whether the given projectile will pass through this block...
bool RegenBlock::ProjectilePassesThrough(const Projectile* projectile) const {
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
			// Let fire globs pass through if they spawned on this block
			if (projectile->IsLastThingCollidedWith(this)) {
				return true;
			}
			break;

		default:
			break;
	}
	return false;
}

/**
 * Get the number of points when this piece changes to the given piece.
 */
int RegenBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return RegenBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

void RegenBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                              const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                              const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                              const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	UNUSED_PARAMETER(bottomLeftNeighbor);
	UNUSED_PARAMETER(bottomRightNeighbor);
	UNUSED_PARAMETER(topRightNeighbor);
	UNUSED_PARAMETER(topLeftNeighbor);

	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;
    std::vector<bool> onInside;

    bool shouldGenBounds = false;

	// Left boundry of the piece
    shouldGenBounds = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        (leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() != LevelPiece::Breakable &&
        leftNeighbor->GetType() != LevelPiece::AlwaysDrop && leftNeighbor->GetType() != LevelPiece::Regen));
    if (shouldGenBounds) {
		Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
								this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n1(-1, 0);
		boundingLines.push_back(l1);
		boundingNorms.push_back(n1);
        onInside.push_back(leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() == LevelPiece::OneWay);
	}

	// Bottom boundry of the piece
    shouldGenBounds = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        (bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::Breakable &&
         bottomNeighbor->GetType() != LevelPiece::AlwaysDrop && bottomNeighbor->GetType() != LevelPiece::Regen));
    if (shouldGenBounds) {
		Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
								this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n2(0, -1);
		boundingLines.push_back(l2);
		boundingNorms.push_back(n2);
        onInside.push_back(bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            bottomNeighbor->GetType() == LevelPiece::OneWay);
	}

	// Right boundry of the piece
    shouldGenBounds = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        (rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::Breakable &&
         rightNeighbor->GetType() != LevelPiece::AlwaysDrop && rightNeighbor->GetType() != LevelPiece::Regen));
    if (shouldGenBounds) {
		Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
								this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n3(1, 0);
		boundingLines.push_back(l3);
		boundingNorms.push_back(n3);
        onInside.push_back(rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() == LevelPiece::OneWay);
	}

	// Top boundry of the piece
    shouldGenBounds = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        (topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::Breakable &&
         topNeighbor->GetType() != LevelPiece::AlwaysDrop && topNeighbor->GetType() != LevelPiece::Regen));
    if (shouldGenBounds) {
		Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
								this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n4(0, 1);
		boundingLines.push_back(l4);
		boundingNorms.push_back(n4);
        onInside.push_back(topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            topNeighbor->GetType() == LevelPiece::OneWay);
	}

	this->SetBounds(BoundingLines(boundingLines, boundingNorms, onInside),
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

LevelPiece* RegenBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
    // If this block has inifinite life then it can only be destroyed in very particular ways...
    if (this->HasInfiniteLife()) {
        // Infinite life regen blocks may only be destroyed by being shattered or via a collateral block clobbering them
        if (method != LevelPiece::IceShatterDestruction && method != LevelPiece::CollateralDestruction &&
            method != LevelPiece::TeslaDestruction && method != LevelPiece::RocketDestruction) {
            // EVENT: The block has been 'preturbed'
            GameEventManager::Instance()->ActionRegenBlockPreturbed(*this);
            return this;
        }
    }

	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

	// When destroying a non-infinite regen block there is the possiblity of dropping an item...
    if (!this->HasInfiniteLife()) {
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

    // Add to the ball boost meter...
    if (this->HasInfiniteLife()) {
        // Infinite-life regen blocks tend to have a very particular way of being destroyed that doesn't
        // rely heavily on boosting, don't give back quite as much boost juice as finite
        gameModel->AddPercentageToBoostMeter(0.2);
    }
    else {
        // Finite-life regen blocks need lots of boosts to bring down, give the player back some of that boost juice
        gameModel->AddPercentageToBoostMeter(0.3);
    }

	return emptyPiece;
}

LevelPiece* RegenBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	LevelPiece* newPiece = this;
	
	// If the ball is an uber ball with fire or a ball without fire, then we dimish the piece,
	// otherwise we apply the "on fire" status to the piece and leave it at that
	bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);

	if (!isFireBall && !isIceBall) {
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			// EVENT: Ice was shattered
			GameEventManager::Instance()->ActionBlockIceShattered(*this);
			// If the piece is frozen it shatters and is immediately destroyed on ball impact
            newPiece = this->Destroy(gameModel, LevelPiece::IceShatterDestruction);
		}
		else {
            newPiece = this->HurtPiece(ball.GetCollisionDamage(), gameModel, LevelPiece::RegularDestruction);
		}
	}
	else if (isFireBall) {
		this->LightPieceOnFire(gameModel);
	}
	else if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}

	ball.SetLastPieceCollidedWith(newPiece);
	return newPiece;
}

/**
 * Call this when a collision has actually occured with a projectile and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* RegenBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	assert(gameModel != NULL);
	assert(projectile != NULL);
	LevelPiece* newPiece = this;

	switch (projectile->GetType()) {
	
        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			else {	
				// Laser bullets dimish the piece, but don't necessarily obliterate/destroy it
                newPiece = this->HurtPiece(projectile->GetDamage(), gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;

		case Projectile::CollateralBlockProjectile:
			// Completely destroy the block...
            newPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			newPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			if (!projectile->IsLastThingCollidedWith(this)) {
				// This piece may catch on fire...
				this->LightPieceOnFire(gameModel);
			}
			break;

		default:
			assert(false);
			break;
	}

	return newPiece;
}

/**
 * When the piece has a per-frame effect on it (e.g., it's on fire) then this will be called
 * by the game model. See LevelPiece for more information.
 */
bool RegenBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	assert(gameModel != NULL);
	
	LevelPiece* resultingPiece = this;
	int32_t currPieceStatus = this->pieceStatus;

	// If this piece is on fire then we slowly eat away at it with fire...
	if (this->HasStatus(LevelPiece::OnFireStatus)) {
		assert(!this->HasStatus(LevelPiece::IceCubeStatus));

		// Blocks on fire have a chance of dropping a glob of flame over some time...
		if (this->fireGlobDropCountdown <= 0.0) {
			this->DoPossibleFireGlobDrop(gameModel);
            this->fireGlobDropCountdown = GameModelConstants::GetInstance()->GenerateFireGlobDropTime();
		}
        else {
            this->fireGlobDropCountdown -= dT;
        }

		// Fire will continue to diminish the piece... 
		// NOTE: This can destroy this piece resulting in a new level piece to replace it (i.e., an empty piece)
        resultingPiece = this->HurtPiece(dT * GameModelConstants::GetInstance()->FIRE_DAMAGE_PER_SECOND, gameModel,
            LevelPiece::FireDestruction);

		// Technically if the above destroys the block then the block automatically loses all of its status
		// this is done in the destructor of LevelPiece
	}

	// If this piece has been destroyed during this tick then we need to return all the
	// status effects that were previously making it tick
	if (resultingPiece != this) {
		removedStatuses = currPieceStatus;
		return true;
	}

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

/**
 * Regenerates the life of this piece over small increments of time.
 */
void RegenBlock::Regen(double dT) {
    // If the piece has infinite life then this doesn't matter
    if (this->HasInfiniteLife()) {
        return;
    }

    // No regeneration if the piece is frozen
    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        return;
    }
    // NOTE: if the piece is on fire it will be losing life to the fire faster then it
    // will be regenerating it
    
    // Regenerate...
    if (this->currLifePoints < MAX_LIFE_POINTS && this->currLifePoints > 0.0) {
        float lifePointsBefore = this->currLifePoints;
        this->currLifePoints = std::min<float>(MAX_LIFE_POINTS, this->currLifePoints + dT * REGEN_LIFE_POINTS_PER_SECOND);

        if (lifePointsBefore != this->currLifePoints) {
            // EVENT: Life points on a regen block have changed
            GameEventManager::Instance()->ActionRegenBlockLifeChanged(*this, lifePointsBefore);
        }
    }
}

/**
 * Does damage to this piece if this piece doesn't have infinite life.
 */
LevelPiece* RegenBlock::HurtPiece(float damage, GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
    // EVENT: The block has been 'preturbed'
    GameEventManager::Instance()->ActionRegenBlockPreturbed(*this);

    // You can't hurt a block with infinite life, silly
    if (this->HasInfiniteLife()) {
        return this;
    }

    LevelPiece* resultingPiece = this;

    float lifePointsBefore = this->currLifePoints;
    this->currLifePoints = std::max<float>(0.0, this->currLifePoints - damage);
    
    // EVENT: Life points on a regen block have changed
    GameEventManager::Instance()->ActionRegenBlockLifeChanged(*this, lifePointsBefore);
    
    if (this->currLifePoints <= 0.0f) {
        resultingPiece = this->Destroy(gameModel, method);
    }

    return resultingPiece;
}