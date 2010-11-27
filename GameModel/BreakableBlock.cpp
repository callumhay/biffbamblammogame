/**
 * BreakableBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
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
LevelPiece* BreakableBlock::Destroy(GameModel* gameModel) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

	// When destroying a breakable there is the possiblity of dropping an item...
	gameModel->AddPossibleItemDrop(*this);

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPiece);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

	return emptyPiece;
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
LevelPiece* BreakableBlock::DiminishPiece(GameModel* gameModel) {
	assert(gameModel != NULL);
	LevelPiece* newPiece = this;

	switch(this->pieceType) {
		case GreenBreakable:
			newPiece = this->Destroy(gameModel);
			break;
		default:
			// By default any other type of block is simply decremented
			this->DecrementPieceType();
			{
				GameLevel* level = gameModel->GetCurrentLevel();
				level->PieceChanged(gameModel, this, this);
			}
			newPiece = this;
			break;
	}
	assert(newPiece != NULL);
	return newPiece;
}

// Eat away at this block over the given dT time at the given damage per second...
// Returns: The resulting level piece that this becomes after exposed to the damage
LevelPiece* BreakableBlock::EatAwayAtPiece(double dT, int dmgPerSec, GameModel* gameModel) {
	this->currLifePoints -= static_cast<float>(dT * dmgPerSec);
	
	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... spawn the next one in sequence
		this->currLifePoints = BreakableBlock::PIECE_STARTING_LIFE_POINTS;
		newPiece = this->DiminishPiece(gameModel);
	}

	return newPiece;
}

/**
 * Call this when a collision has actually occured with the ball and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	assert(gameModel != NULL);

	LevelPiece* newPiece = this;
	
	// If the ball is an uber ball with fire or a ball without fire, then we dimish the piece,
	// otherwise we apply the "on fire" status to the piece and leave it at that
	bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (!isFireBall && !isIceBall) {
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			// If the piece is frozen it shatters and is immediately destroyed on ball impact
			newPiece = this->Destroy(gameModel);
		}
		else {
			bool isUberBall = ((ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall);

			// If the ball is an 'uber' ball then we decrement the piece type twice when it is
			// not the lowest kind of breakable block (i.e., green)
			if (isUberBall && this->pieceType != GreenBreakable) {
				this->DecrementPieceType();
			}

			newPiece = this->DiminishPiece(gameModel);
		}
	}
	else if (isFireBall) {
		this->LightPieceOnFire(gameModel);
	}
	else if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}

	ball.SetLastPieceCollidedWith(NULL);
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
			// TODO: Deal with case where the piece is frozen in an ice cube...
				
			// Laser bullets just dimish the piece, but don't necessarily obliterated/destroy it
			newPiece = this->DiminishPiece(gameModel);
			break;

		case Projectile::CollateralBlockProjectile:
			// Completely destroy the block...
			newPiece = this->Destroy(gameModel);
			break;

		case Projectile::PaddleRocketBulletProjectile:
			newPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);
			break;

		case Projectile::FireGlobProjectile:
			if (!projectile->IsLastLevelPieceCollidedWith(this)) {
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
				fireGlobProjectile->SetLastLevelPieceCollidedWith(this);
				gameModel->AddProjectile(fireGlobProjectile);
			}
		}

		// Fire will continue to diminish the piece... 
		// NOTE: This can destroy this piece resulting in a new level piece to replace it (i.e., an empty piece)
		resultingPiece = this->EatAwayAtPiece(dT, GameModelConstants::GetInstance()->FIRE_DAMAGE_PER_SECOND, gameModel);
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
		
		case Projectile::CollateralBlockProjectile:
			return true;

		case Projectile::FireGlobProjectile:
			// Let fire globs pass through if they spawned on this block
			if (projectile->IsLastLevelPieceCollidedWith(this)) {
				return true;
			}
			break;

		default:
			break;
	}
	return false;
}