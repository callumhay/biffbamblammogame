/**
 * BreakableBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009-2010
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

BreakableBlock::BreakableBlock(char type, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), pieceType(static_cast<BreakablePieceType>(type)), currLifePoints(PIECE_STARTING_LIFE_POINTS),
fireGlobTimeCounter(0.0) {
	assert(IsValidBreakablePieceType(type));

	this->colour = BreakableBlock::GetColourOfBreakableType(this->pieceType);
}


BreakableBlock::~BreakableBlock() {
}

/**
 * Inherited private function for destroying this breakable block.
 */
LevelPiece* BreakableBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

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

	// Left boundry of the piece
    if (leftNeighbor != NULL && (leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() != LevelPiece::Breakable)) {
		Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
								 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n1(-1, 0);
		boundingLines.push_back(l1);
		boundingNorms.push_back(n1);
	}

	// Bottom boundry of the piece
	if (bottomNeighbor != NULL && (bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::Breakable)) {
		Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n2(0, -1);
		boundingLines.push_back(l2);
		boundingNorms.push_back(n2);
	}

	// Right boundry of the piece
	if (rightNeighbor != NULL && (rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::Breakable)) {
		Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n3(1, 0);
		boundingLines.push_back(l3);
		boundingNorms.push_back(n3);
	}

	// Top boundry of the piece
	if (topNeighbor != NULL && (topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::Breakable)) {
		Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n4(0, 1);
		boundingLines.push_back(l4);
		boundingNorms.push_back(n4);
	}

	this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor,
                                  rightNeighbor, topNeighbor, topRightNeighbor, topLeftNeighbor, 
                                  bottomRightNeighbor, bottomLeftNeighbor);
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
    if (ball.IsLastPieceCollidedWith(this)) {
        return this;
    }

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
            newPiece = this->Destroy(gameModel, LevelPiece::RegularDestruction);
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
	
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			else {	
				// Laser bullets dimish the piece, but don't necessarily obliterated/destroy it
                newPiece = this->DiminishPiece(gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;

		case Projectile::CollateralBlockProjectile:
			// Completely destroy the block...
            newPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
			newPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);
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
		// Blocks on fire have a (very small) chance of dropping a glob of flame over some time...
		this->fireGlobTimeCounter += dT;
		if (this->fireGlobTimeCounter >= GameModelConstants::GetInstance()->FIRE_GLOB_DROP_CHANCE_INTERVAL) {
			this->fireGlobTimeCounter = 0.0;

			int fireGlobDropRandomNum = Randomizer::GetInstance()->RandomUnsignedInt() % GameModelConstants::GetInstance()->FIRE_GLOB_CHANCE_MOD;
			if (fireGlobDropRandomNum == 0) {

				// Do the same for the width and height...
				float globSize  = 0.4f * LevelPiece::HALF_PIECE_WIDTH + Randomizer::GetInstance()->RandomNumZeroToOne() * LevelPiece::HALF_PIECE_WIDTH;
				float edgeDist  = ((LevelPiece::PIECE_WIDTH - globSize) / 2.0f) - 0.01f;
				assert(edgeDist >= 0.0f);

				// Calculate a place on the block to drop the glob from...
				Point2D dropPos = this->center - Vector2D(Randomizer::GetInstance()->RandomNumNegOneToOne() * edgeDist, globSize / 2.0f);

				// Drop a glob of fire downwards from the block...
				Projectile* fireGlobProjectile = new FireGlobProjectile(dropPos, globSize);
				fireGlobProjectile->SetLastThingCollidedWith(this);
				gameModel->AddProjectile(fireGlobProjectile);
			}
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
bool BreakableBlock::ProjectilePassesThrough(Projectile* projectile) const {
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
    int totalPts = 0;
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        totalPts += BreakableBlock::POINTS_ON_BLOCK_DESTROYED;
        switch (this->pieceType) {
            case RedBreakable:
                totalPts += BreakableBlock::POINTS_ON_RED_TO_ORANGE_HIT;
            case OrangeBreakable:
                totalPts += BreakableBlock::POINTS_ON_ORANGE_TO_YELLOW_HIT;
            case YellowBreakable:
                totalPts += BreakableBlock::POINTS_ON_YELLOW_TO_GREEN_HIT;
            case GreenBreakable:
                break;
            default:
                assert(false);
                break;
        }
    }
    
    if (changeToPiece.GetType() == LevelPiece::Breakable || 
        changeToPiece.GetType() == LevelPiece::BreakableTriangle) {
        
        const BreakableBlock* breakableChangeToPiece = static_cast<const BreakableBlock*>(&changeToPiece);
        assert(breakableChangeToPiece != NULL);
        switch (breakableChangeToPiece->pieceType) {
            case RedBreakable:
                break;

            case OrangeBreakable:
                if (this->pieceType == RedBreakable) {
                    totalPts += BreakableBlock::POINTS_ON_RED_TO_ORANGE_HIT;
                }
                break;

            case YellowBreakable:
                switch (this->pieceType) {
                    case RedBreakable:
                        totalPts += BreakableBlock::POINTS_ON_RED_TO_ORANGE_HIT;
                    case OrangeBreakable:
                        totalPts += BreakableBlock::POINTS_ON_ORANGE_TO_YELLOW_HIT;
                        break;
                    default:
                        break;
                }
                break;

            case GreenBreakable:
                switch (this->pieceType) {
                    case RedBreakable:
                        totalPts += BreakableBlock::POINTS_ON_RED_TO_ORANGE_HIT;
                    case OrangeBreakable:
                        totalPts += BreakableBlock::POINTS_ON_ORANGE_TO_YELLOW_HIT;
                    case YellowBreakable:
                        totalPts += BreakableBlock::POINTS_ON_YELLOW_TO_GREEN_HIT;
                        break;
                    default:
                        break;
                }
                break;

            default:
                assert(false);
                break;
        }
    }

    return totalPts;
}