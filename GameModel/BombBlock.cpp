/**
 * BombBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BombBlock.h"
#include "EmptySpaceBlock.h"

#include "GameModel.h"
#include "GameEventManager.h"
#include "GameBall.h"
#include "GameLevel.h"
#include "Beam.h"

#include <set>

BombBlock::BombBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc),
currLifePoints(BombBlock::PIECE_STARTING_LIFE_POINTS) {
}

BombBlock::~BombBlock() {
}

// Whether or not the given projectile passes through this block...
bool BombBlock::ProjectilePassesThrough(const Projectile* projectile) const {
	switch (projectile->GetType()) {

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

		default:
			break;
	}
	
	return false;
}

/**
 * Get the number of points when this piece changes to the given piece.
 */
int BombBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return BombBlock::BOMB_DESTRUCTION_POINTS;
    }
    return 0;
}

LevelPiece* BombBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// Obtain the level from the model...
	GameLevel* level = gameModel->GetCurrentLevel();
	
	// If the bomb is incased in ice then we don't blow up, we just shatter
	if (!this->HasStatus(LevelPiece::IceCubeStatus)) {
		// Keep track of the bombs we need to destroy and the pieces we need to destroy.
		std::set<LevelPiece*> destroyedBombs;
		destroyedBombs.insert(this);

		std::set<LevelPiece*> destroyedPieces;
		destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(this->hIndex+1, this->wIndex));
		destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(this->hIndex+1, this->wIndex-1));
		destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(this->hIndex+1, this->wIndex+1));
		destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(this->hIndex-1, this->wIndex));
		destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(this->hIndex-1, this->wIndex-1));
		destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(this->hIndex-1, this->wIndex+1));
		destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(this->hIndex, this->wIndex-1));
		destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(this->hIndex, this->wIndex+1));

		// When you destroy a bomb block, everything around it goes with it...BOOM!
		// The idea here is to watch out for chain reactions!!! - We go through all the pieces 
		// that could possibly be destroyed and when we find bombs that we haven't already found we
		// add them to the list of bombs, meanwhile keeping a seperate list of all the other pieces we find.
		bool otherBombsFound = true;
		while (otherBombsFound) {
			otherBombsFound = false;

			std::list<std::set<LevelPiece*>::iterator> toRemove;

			for (std::set<LevelPiece*>::iterator iter = destroyedPieces.begin(); iter != destroyedPieces.end(); ++iter) {
				LevelPiece* currDestroyedPiece = *iter;
			
				// Avoid destroying non-existant pieces (including this one since we destroy it above).
				if (currDestroyedPiece != NULL) {

					// Check to see if the piece is a bomb 
					if (currDestroyedPiece->GetType() == LevelPiece::Bomb) {

						// Make sure the bomb hasn't already been accounted for...
						if (destroyedBombs.find(currDestroyedPiece) == destroyedBombs.end()) {
							// Add all the pieces that would be destroyed by the found bomb (currDestroyedPiece) as well
							destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(currDestroyedPiece->GetHeightIndex()+1, currDestroyedPiece->GetWidthIndex()));
							destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(currDestroyedPiece->GetHeightIndex()+1, currDestroyedPiece->GetWidthIndex()-1));
							destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(currDestroyedPiece->GetHeightIndex()+1, currDestroyedPiece->GetWidthIndex()+1));
							destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(currDestroyedPiece->GetHeightIndex()-1, currDestroyedPiece->GetWidthIndex()));
							destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(currDestroyedPiece->GetHeightIndex()-1, currDestroyedPiece->GetWidthIndex()-1));
							destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(currDestroyedPiece->GetHeightIndex()-1, currDestroyedPiece->GetWidthIndex()+1));
							destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(currDestroyedPiece->GetHeightIndex(), currDestroyedPiece->GetWidthIndex()-1));
							destroyedPieces.insert(level->GetLevelPieceFromCurrentLayout(currDestroyedPiece->GetHeightIndex(), currDestroyedPiece->GetWidthIndex()+1));
							destroyedBombs.insert(currDestroyedPiece);
							otherBombsFound = true;
						}
						// Add the bomb to list of pieces to remove...
						toRemove.push_back(iter);
					}
				}
			}
			
			// Remove any unwanted pieces...
			for (std::list<std::set<LevelPiece*>::iterator>::iterator iter = toRemove.begin(); iter != toRemove.end(); ++iter) {
				destroyedPieces.erase(*iter);
			}
		}

		// Go through a set of every UNIQUE level piece that was destroyed when the bomb went off
		// and destroy each of those pieces properly
		for (std::set<LevelPiece*>::iterator iter = destroyedPieces.begin(); iter != destroyedPieces.end(); ++iter) {
			LevelPiece* currDestroyedPiece = *iter;
			if (currDestroyedPiece != NULL) {
				assert(currDestroyedPiece->GetType() != LevelPiece::Bomb);

				// Only allow the piece to be destroyed if the ball can destroy it as well...
				if (currDestroyedPiece->CanBeDestroyedByBall()) {
                    currDestroyedPiece->Destroy(gameModel, LevelPiece::BombDestruction);
				}

			}
		}

		// Go through a set of every UNIQUE bomb that was destroyed and destroy each properly
		for (std::set<LevelPiece*>::iterator iter = destroyedBombs.begin(); iter != destroyedBombs.end(); ++iter) {
			LevelPiece* currDestroyedBomb = *iter;
			assert(currDestroyedBomb->GetType() == LevelPiece::Bomb);
			if (currDestroyedBomb != this) {
                GameEventManager::Instance()->ActionBlockDestroyed(*currDestroyedBomb, LevelPiece::BombDestruction);
				
                level->PieceChanged(gameModel, currDestroyedBomb, 
                    new EmptySpaceBlock(currDestroyedBomb->GetWidthIndex(), currDestroyedBomb->GetHeightIndex()),
                    LevelPiece::BombDestruction);

				delete currDestroyedBomb;
				currDestroyedBomb = NULL;
			}
		}
	}
	else {
		// EVENT: Ice was shattered
		GameEventManager::Instance()->ActionBlockIceShattered(*this);
	}

	// Obliterate this bomb
	// EVENT: Bomb Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);
	LevelPiece* emptyPieceForBomb = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPieceForBomb, method);
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;
    
    // Charge up the boost meter a little bit
    gameModel->AddPercentageToBoostMeter(0.05);

	return emptyPieceForBomb;
}

LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	LevelPiece* resultingPiece = this;

	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	
	// Bombs may be encased in ice, when they are they don't explode if they get destroyed...
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		if (isFireBall) {
			// Unfreeze a frozen bomb block if it gets hit by a fireball
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}
		}
		else {
            resultingPiece = this->Destroy(gameModel, 
                this->HasStatus(LevelPiece::IceCubeStatus) ? LevelPiece::IceShatterDestruction : LevelPiece::RegularDestruction);
		}
	}

    ball.SetLastPieceCollidedWith(resultingPiece);   
	return resultingPiece;
}

/**
 * When the bomb block encounters a projectile it tends to go boom...
 * Results in an empty level piece where the bomb block once was.
 */
LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch(projectile->GetType()) {
		
        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			else {
                resultingPiece = this->Destroy(gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;

		case Projectile::CollateralBlockProjectile:
            resultingPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a bomb block, unless it's frozen in an ice cube;
			// in that case, unfreeze a frozen bomb block
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

	return resultingPiece;
}

/**
 * Tick the a collision with a beam - the beam will eat away at this bomb block until
 * it is destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* BombBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	
	// If the piece is frozen in ice we don't hurt it, instead it will refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}

	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));

	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... destroy it
		this->currLifePoints = 0;
        newPiece = this->Destroy(gameModel, LevelPiece::LaserBeamDestruction);
	}

	return newPiece;
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* BombBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
	
	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(paddle.GetShieldDamagePerSecond()));

	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... destroy it
		this->currLifePoints = 0;
        newPiece = this->Destroy(gameModel, LevelPiece::PaddleShieldDestruction);
	}

	return newPiece;
}