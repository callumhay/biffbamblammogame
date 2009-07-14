#include "BombBlock.h"
#include "EmptySpaceBlock.h"

#include "GameModel.h"
#include "GameEventManager.h"
#include "GameBall.h"
#include "GameLevel.h"

#include <set>

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
		for (std::list<std::set<LevelPiece*>::iterator>::iterator iter = toRemove.begin(); iter != toRemove.end(); iter++) {
			destroyedPieces.erase(*iter);
		}
	}

	// Go through a set of every UNIQUE level piece that was destroyed when the bomb went off
	// and destroy each of those pieces properly
	for (std::set<LevelPiece*>::iterator iter = destroyedPieces.begin(); iter != destroyedPieces.end(); ++iter) {
		LevelPiece* currDestroyedPiece = *iter;
		if (currDestroyedPiece != NULL) {
			assert(currDestroyedPiece->GetType() != LevelPiece::Bomb);
			currDestroyedPiece->Destroy(gameModel);
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

LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, const GameBall& ball) {
	return this->Destroy(gameModel);
}

LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, const Projectile& projectile) {
	if (projectile.GetType() == Projectile::PaddleLaserProjectile) {
		return this->Destroy(gameModel);
	}
	return this;
}