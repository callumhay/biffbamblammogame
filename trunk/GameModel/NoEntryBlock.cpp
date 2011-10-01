/**
 * NoEntryBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "NoEntryBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"

NoEntryBlock::NoEntryBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {
}

NoEntryBlock::~NoEntryBlock() {
}

/**
 * Get the number of points when this piece changes to the given piece.
 */
int NoEntryBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return NoEntryBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

LevelPiece* NoEntryBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
	    // EVENT: Ice was shattered
	    GameEventManager::Instance()->ActionBlockIceShattered(*this);
	    bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
	    assert(success);
	}

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

void NoEntryBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

	// Left boundry of the piece
	if (leftNeighbor != NULL) {
        if (leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() != LevelPiece::NoEntry) {
			Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n1(-1, 0);
			boundingLines.push_back(l1);
			boundingNorms.push_back(n1);
		}
	}

	// Bottom boundry of the piece
	if (bottomNeighbor != NULL) {
		if (bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::NoEntry) {
			Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n2(0, -1);
			boundingLines.push_back(l2);
			boundingNorms.push_back(n2);
		}
	}

	// Right boundry of the piece
	if (rightNeighbor != NULL) {
		if (rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::NoEntry) {
			Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n3(1, 0);
			boundingLines.push_back(l3);
			boundingNorms.push_back(n3);
		}
	}

	// Top boundry of the piece
	if (topNeighbor != NULL) {
		if (topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::NoEntry) {
			Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
									 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			Vector2D n4(0, 1);
			boundingLines.push_back(l4);
			boundingNorms.push_back(n4);
		}
	}

	this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
		 						 topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

LevelPiece* NoEntryBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	ball.SetLastPieceCollidedWith(NULL);

	// No Entry blocks can be frozen, but they still can't be destroyed (in most cases)
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		if (!isFireBall) {
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				// EVENT: Ice was shattered
				GameEventManager::Instance()->ActionBlockIceShattered(*this);
			}
		}

		// Unfreeze a frozen block
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
            UNUSED_VARIABLE(success);
			assert(success);
		}
	}

	return this;
}
