/**
 * BreakableBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
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
#include "Beam.h"

BreakableBlock::BreakableBlock(char type, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), pieceType(static_cast<BreakablePieceType>(type)), currLifePoints(PIECE_STARTING_LIFE_POINTS) {
	assert(IsValidBreakablePieceType(type));

	this->colour = BreakableBlock::GetColourOfBreakableType(this->pieceType);
}


BreakableBlock::~BreakableBlock() {
}

// Determine whether the given projectile will pass through this block...
bool BreakableBlock::ProjectilePassesThrough(Projectile* projectile) const {
	if (projectile->GetType() == Projectile::CollateralBlockProjectile) {
		return true;
	}
	return false;
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
	level->PieceChanged(this, emptyPiece);

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
				level->PieceChanged(this, this);
			}
			newPiece = this;
			break;
	}
	assert(newPiece != NULL);
	return newPiece;
}

/**
 * Call this when a collision has actually occured with the ball and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	assert(gameModel != NULL);

	// If the ball is an 'uber' ball then we decrement the piece type twice when it is
	// not the lowest kind of breakable block (i.e., green)
	if (((ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall) && this->pieceType != GreenBreakable) {
		this->DecrementPieceType();
	}
	
	LevelPiece* newPiece = this->DiminishPiece(gameModel);

	// Tell the ball what the last piece it collided with was (which will be the diminished
	// piece if it hasn't become an empty/no bounds block yet...
	//if (!newPiece->IsNoBoundsPieceType()) {
	//	ball.SetLastPieceCollidedWith(newPiece);
	//}
	//else {
	ball.SetLastPieceCollidedWith(NULL);
	//}

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

		default:
			assert(false);
			break;
	}

	return newPiece;
}

/**
 * Called as this piece is being hit by the given beam over the given amount of time in seconds.
 */
LevelPiece* BreakableBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));
	
	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... spawn the next one in sequence
		this->currLifePoints = BreakableBlock::PIECE_STARTING_LIFE_POINTS;
		newPiece = this->DiminishPiece(gameModel);
	}

	return newPiece;
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* BreakableBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(paddle.GetShieldDamagePerSecond()));
	
	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... spawn the next one in sequence
		this->currLifePoints = BreakableBlock::PIECE_STARTING_LIFE_POINTS;
		newPiece = this->DiminishPiece(gameModel);
	}

	return newPiece;
}