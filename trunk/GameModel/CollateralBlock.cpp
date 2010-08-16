/**
 * CollateralBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CollateralBlock.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "EmptySpaceBlock.h"
#include "CollateralBlockProjectile.h"
#include "Beam.h"

const float CollateralBlock::COLLATERAL_FALL_SPEED	= 3.0f;
const double CollateralBlock::WARNING_TIME_MIN			= 3.0;
const double CollateralBlock::WARNING_TIME_MAX			= 6.0;
const float CollateralBlock::ROTATION_SPEED					= 280.0f;

CollateralBlock::CollateralBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc),
currState(CollateralBlock::InitialState), currTimeElapsedSinceHit(0.0), timeUntilCollateralDmg(0.0),
currentRotation(0.0f), currLifePoints(CollateralBlock::PIECE_STARTING_LIFE_POINTS), 
rotationSgn(Randomizer::GetInstance()->RandomNegativeOrPositive()) {
}

CollateralBlock::~CollateralBlock() {
}

bool CollateralBlock::ProjectilePassesThrough(Projectile* projectile) {
	if (projectile->GetType() == Projectile::CollateralBlockProjectile) {
		// Much like all other blocks, when collateral blocks are hit by other collateral
		// blocks they just get destroyed and the other collateral block passes through
		return true;
	}
	return false;
}

LevelPiece* CollateralBlock::Destroy(GameModel* gameModel) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

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

LevelPiece* CollateralBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	ball.SetLastPieceCollidedWith(NULL);
	// Being hit by the ball causes the collateral block to detonate and
	// go into warning/collateral mode
	return this->Detonate(gameModel);
}

/**
 * Call when the collateral block encounters a projectile - this may either completely obliterate/destroy
 * the block or detonate it.
 */
LevelPiece* CollateralBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* newLevelPiece = this;

	switch (projectile->GetType()) {
		case Projectile::PaddleLaserBulletProjectile: 
			newLevelPiece = this->Detonate(gameModel);
			break;

		case Projectile::CollateralBlockProjectile:
			newLevelPiece = this->Destroy(gameModel);
			break;

		case Projectile::PaddleRocketBulletProjectile:
			newLevelPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, projectile, this);
			break;

		default:
			assert(false);
			break;
	}

	return newLevelPiece;
}

// Beams eat away at the life points of the block over time until it goes into warning/collateral damage mode
LevelPiece* CollateralBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));
	
	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// This piece will now go into warning and collateral damage mode...
		this->currLifePoints = 0.0f;
		newPiece = this->Detonate(gameModel);
	}

	return newPiece;
}

void CollateralBlock::Tick(double dT, CollateralBlockProjectile& collateralProjectile) {
	switch(this->currState) {

		case CollateralBlock::WarningState:
			// Increment the time spent in this state, if we exceed/equal the time until
			// collateral damage then we move into that state
			this->currTimeElapsedSinceHit += dT;
			if (this->currTimeElapsedSinceHit >= this->timeUntilCollateralDmg) {
				this->currState = CollateralBlock::CollateralDamageState;
			}
			break;

		case CollateralBlock::CollateralDamageState: {
				// The collateral block moves down the level, rotating along the way 
				// (for movement see the collateralblockprojectile)...
				
				Vector2D moveAmt = static_cast<float>(dT) * collateralProjectile.GetVelocityMagnitude() * 
																									  collateralProjectile.GetVelocityDirection();
				this->center = this->center + moveAmt;

				// Keep the projectile in the same position as the block (sync em up!)
				collateralProjectile.SetPosition(this->center);

				// Rotate...
				this->currentRotation += rotationSgn * static_cast<float>(dT) * CollateralBlock::ROTATION_SPEED;

				// We don't need to check to see if the collateral block is off the level yet - the game loop
				// in BallInPlayState will do this for us with all projectiles.
			}
			break;

		default:
			assert(false);
			break;
	}
}

/**
 * "Detonates" this collateral block - doesn't actually destroy the block, just removes it from
 * the level and adds it as an active collateral block to the game model allowing it to become
 * a particle that descends down the level destroying all in its path.
 */
LevelPiece* CollateralBlock::Detonate(GameModel* gameModel) {
	assert(this->currState == CollateralBlock::InitialState);

	// The block is destroyed in the game level, however it lives on as a projectile
	// within the game model!
	this->currState = CollateralBlock::WarningState;
	this->timeUntilCollateralDmg = CollateralBlock::WARNING_TIME_MIN +
		Randomizer::GetInstance()->RandomNumZeroToOne() * (CollateralBlock::WARNING_TIME_MAX - CollateralBlock::WARNING_TIME_MIN);
	assert(this->timeUntilCollateralDmg >= CollateralBlock::WARNING_TIME_MIN);
	this->currTimeElapsedSinceHit = 0.0;

	// Tell the level that this piece has changed to empty (we aren't actually destroying the 
	// memory of the piece, we're just changing it to avoid collision detection issues in the level)
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(this, emptyPiece);

	// Add the collateral block to the game model as a special type of soon-to-be-falling projectile...
	/**
	 * A collateral block enters here once it has been 'activated' by a ball hitting it (or
	 * some such thing) - it will count down on a timer (in warning mode) and then activate
	 * itself by going into a collateral damage mode where it will barrel down the level destroying
	 * everything in its path as a bad-ass projectile. This starts the whole process.
	 */
	gameModel->AddProjectile(new CollateralBlockProjectile(this));

	return emptyPiece;
}