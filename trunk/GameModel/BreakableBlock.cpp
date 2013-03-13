/**
 * BreakableBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BreakableBlock.h"
#include "EmptySpaceBlock.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameBall.h"
#include "GameEventManager.h"
#include "FireGlobProjectile.h"

const double BreakableBlock::ALLOWABLE_TIME_BETWEEN_BALL_COLLISIONS_IN_MS = 67;

BreakableBlock::BreakableBlock(char type, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), pieceType(static_cast<BreakablePieceType>(type)), currLifePoints(PIECE_STARTING_LIFE_POINTS),
fireGlobDropCountdown(GameModelConstants::GetInstance()->GenerateFireGlobDropTime()), timeOfLastBallCollision() {
	assert(IsValidBreakablePieceType(type));

	this->colour = ColourRGBA(BreakableBlock::GetColourOfBreakableType(this->pieceType), 1.0f);
}


BreakableBlock::~BreakableBlock() {
}

LevelPiece* BreakableBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

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

    // Add to the ball boost meter...
    gameModel->AddPercentageToBoostMeter(0.025);

	return emptyPiece;
}

void BreakableBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                  const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                  const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	UNUSED_PARAMETER(bottomLeftNeighbor);
	UNUSED_PARAMETER(bottomRightNeighbor);
	UNUSED_PARAMETER(topRightNeighbor);
	UNUSED_PARAMETER(topLeftNeighbor);

	// Set the bounding lines for a rectangular block - these are also used when any block is frozen in an ice cube
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

/**
 * Private static helper function used to move the colour of this breakable
 * down to the next level (thus decrementing it appropriately). For example
 * when a red block is decremented it goes to orange, then yellow, and then to green.
 */
BreakableBlock::BreakablePieceType BreakableBlock::GetDecrementedPieceType(BreakablePieceType breakableType) {
	switch(breakableType) {
		case YellowBreakable:
			return GreenBreakable;
		case OrangeBreakable:
			return YellowBreakable;
		case RedBreakable:
			return OrangeBreakable;
		default:
			break;
	}
	assert(false);
	return GreenBreakable;
}

Colour BreakableBlock::GetColourOfBreakableType(BreakableBlock::BreakablePieceType breakableType) {
	switch(breakableType) {
		case GreenBreakable:
			return Colour(0.0f, 1.0f, 0.0f);
		case YellowBreakable:
			return Colour(1.0f, 1.0f, 0.0f);
		case OrangeBreakable:
			return Colour(1.0f, 0.5f, 0.0f);
		case RedBreakable:
			return Colour(1.0f, 0.0f, 0.0f);
		default:
			assert(false);
			break;
	}
	
	return Colour(1,1,1);
}

/** 
 * This piece is officially dead in its current form, diminish it down
 * to its next form (or officially destroy it if it's in its final form i.e., green).
 * Returns: The resulting piece in its next form after being diminished from the
 * current one.
 */
LevelPiece* BreakableBlock::DiminishPiece(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	assert(gameModel != NULL);
	LevelPiece* newPiece = this;

	switch(this->pieceType) {
		case GreenBreakable:
			newPiece = this->Destroy(gameModel, method);
			break;

		default:
			// By default any other type of block is simply decremented
			this->DecrementPieceType();
			{
				GameLevel* level = gameModel->GetCurrentLevel();
				level->PieceChanged(gameModel, this, this, method);
			}
			newPiece = this;

            gameModel->AddPercentageToBoostMeter(0.01);
			
            break;

	}
	assert(newPiece != NULL);
	return newPiece;
}

// Eat away at this block over the given dT time at the given damage per second...
// Returns: The resulting level piece that this becomes after exposed to the damage
LevelPiece* BreakableBlock::EatAwayAtPiece(double dT, int dmgPerSec, GameModel* gameModel,
                                           const LevelPiece::DestructionMethod& method) {
	this->currLifePoints -= static_cast<float>(dT * dmgPerSec);
	
	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... spawn the next one in sequence
		this->currLifePoints = BreakableBlock::PIECE_STARTING_LIFE_POINTS;
		newPiece = this->DiminishPiece(gameModel, method);
	}

	return newPiece;
}

/**
 * Call this when a collision has actually occured with the ball and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	assert(gameModel != NULL);
    
    long currSystemTime = BlammoTime::GetSystemTimeInMillisecs();

    // Make sure we don't do a double collision - check to make sure the ball hasn't already
    // collided with this block and also that we're not in the same game time tick since the last
    // collision of the ball
    if (ball.IsLastPieceCollidedWith(this) && ball.GetTimeSinceLastCollision() < (BreakableBlock::ALLOWABLE_TIME_BETWEEN_BALL_COLLISIONS_IN_MS/1000.0)) {
        this->timeOfLastBallCollision = currSystemTime;
        return this;
    }
    
    if ((currSystemTime - this->timeOfLastBallCollision) < BreakableBlock::ALLOWABLE_TIME_BETWEEN_BALL_COLLISIONS_IN_MS) {
        this->timeOfLastBallCollision = currSystemTime;
        return this;
    }

    this->timeOfLastBallCollision = currSystemTime;

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
			bool isUberBall = ((ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall);

			// If the ball is an 'uber' ball then we decrement the piece type twice when it is
			// not the lowest kind of breakable block (i.e., green)
			if (isUberBall && this->pieceType != GreenBreakable) {
				this->DecrementPieceType();
			}

            newPiece = this->DiminishPiece(gameModel, LevelPiece::RegularDestruction);
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
LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
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
                newPiece = this->DiminishPiece(gameModel, LevelPiece::LaserProjectileDestruction);
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
bool BreakableBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
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
		resultingPiece = this->EatAwayAtPiece(dT, GameModelConstants::GetInstance()->FIRE_DAMAGE_PER_SECOND, gameModel,
            LevelPiece::FireDestruction);

		// Technically if the above destroys the block then the block automatically loses all of its status
		// this is done in the destructor of LevelPiece
	}
	//else if (this->HasStatus(LevelPiece::IceCubeStatus)) {
	//	assert(!this->HasStatus(LevelPiece::OnFireStatus));
	//  // ... There's no tick activity for ice cubes
	//}

	// If this piece has been destroyed during this tick then we need to return all the
	// status effects that were previously making it tick
	if (resultingPiece != this) {
		removedStatuses = currPieceStatus;
		return true;
	}

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

// Determine whether the given projectile will pass through this block...
bool BreakableBlock::ProjectilePassesThrough(const Projectile* projectile) const {
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
int BreakableBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return BreakableBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}