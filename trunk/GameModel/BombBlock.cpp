/**
 * BombBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
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
bool BombBlock::ProjectilePassesThrough(Projectile* projectile) {
	if (projectile->GetType() == Projectile::CollateralBlockProjectile) {
		return true;
	}
	
	return false;
}

LevelPiece* BombBlock::Destroy(GameModel* gameModel) {
	// Obtain the level from the model...
	GameLevel* level = gameModel->GetCurrentLevel();
	
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
				currDestroyedPiece->Destroy(gameModel);
			}

		}
	}

	// Go through a set of every UNIQUE bomb that was destroyed (including this bomb) and destroy each bomb properly
	for (std::set<LevelPiece*>::iterator iter = destroyedBombs.begin(); iter != destroyedBombs.end(); ++iter) {
		LevelPiece* currDestroyedBomb = *iter;
		assert(currDestroyedBomb->GetType() == LevelPiece::Bomb);
		if (currDestroyedBomb != this) {
			GameEventManager::Instance()->ActionBlockDestroyed(*currDestroyedBomb);
			level->PieceChanged(currDestroyedBomb, new EmptySpaceBlock(currDestroyedBomb->GetWidthIndex(), currDestroyedBomb->GetHeightIndex()));
			delete currDestroyedBomb;
			currDestroyedBomb = NULL;
		}
	}

	// Obliterate this bomb
	// EVENT: Bomb Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);
	LevelPiece* emptyPieceForBomb = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(this, emptyPieceForBomb);
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

	return emptyPieceForBomb;
}

LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	LevelPiece* resultingPiece = this->Destroy(gameModel);
	return resultingPiece;
}

/**
 * When the bomb block encounters a projectile it tends to go boom...
 * Results in an empty level piece where the bomb block once was.
 */
LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* result = this;

	switch(projectile->GetType()) {
		
		case Projectile::PaddleLaserBulletProjectile:
		case Projectile::CollateralBlockProjectile:
			result = this->Destroy(gameModel);
			break;

		case Projectile::PaddleRocketBulletProjectile:
			result = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, this);
			break;

		default:
			assert(false);
			break;
	}

	return result;
}

/**
 * Tick the a collision with a beam - the beam will eat away at this bomb block until
 * it is destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* BombBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	
	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));

	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... destroy it
		this->currLifePoints = 0;
		newPiece = this->Destroy(gameModel);
	}

	return newPiece;
}