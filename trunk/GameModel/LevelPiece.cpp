/**
 * LevelPiece.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LevelPiece.h"
#include "GameEventManager.h"
#include "GameBall.h"
#include "GameModel.h"
#include "GameItem.h"
#include "GameItemFactory.h"
#include "GameModelConstants.h"
#include "Beam.h"

const float LevelPiece::PIECE_WIDTH = 2.5f;
const float LevelPiece::PIECE_HEIGHT = 1.0f;
const float LevelPiece::PIECE_DEPTH  = 1.30f;
const float LevelPiece::HALF_PIECE_WIDTH = PIECE_WIDTH / 2.0f;
const float LevelPiece::HALF_PIECE_HEIGHT = PIECE_HEIGHT / 2.0f;
const float LevelPiece::HALF_PIECE_DEPTH  = PIECE_DEPTH / 2.0f;

LevelPiece::LevelPiece(unsigned int wLoc, unsigned int hLoc) : 
colour(1,1,1), pieceStatus(LevelPiece::NormalStatus), leftNeighbor(NULL),
bottomNeighbor(NULL), rightNeighbor(NULL), topNeighbor(NULL), topRightNeighbor(NULL),
topLeftNeighbor(NULL), bottomRightNeighbor(NULL), bottomLeftNeighbor(NULL) {
	this->SetWidthAndHeightIndex(wLoc, hLoc);
}

LevelPiece::~LevelPiece() {
	// We need to be sure to obliterate any status effects that might be linguring on the level piece -
	// this emits an important event to any model listeners to remove those effects as well!
	this->RemoveAllStatus();
}

void LevelPiece::SetWidthAndHeightIndex(unsigned int wLoc, unsigned int hLoc) {
	this->wIndex = wLoc;
	this->hIndex = hLoc;
	this->center = Point2D(wLoc * PIECE_WIDTH + HALF_PIECE_WIDTH, hLoc * PIECE_HEIGHT + HALF_PIECE_HEIGHT);
}

/** 
 * The default update bounds method used by subclasses - this will update the boundries
 * based on a generalized idea that if there is a boundry piece type on a certain side
 * of this piece then there is no need for another boundry on that side. In all other cases
 * the boundry will be placed there.
 */
void LevelPiece::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
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

	// We only create boundries in cases where neighbours exist AND they are empty 
	// (i.e., the ball can actually get to them).

	// Left boundry of the piece
	if (leftNeighbor != NULL && leftNeighbor->IsNoBoundsPieceType()) {
		Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
								 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n1(-1, 0);
		boundingLines.push_back(l1);
		boundingNorms.push_back(n1);
	}

	// Bottom boundry of the piece
	if (bottomNeighbor != NULL && bottomNeighbor->IsNoBoundsPieceType()) {
		Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n2(0, -1);
		boundingLines.push_back(l2);
		boundingNorms.push_back(n2);
	}

	// Right boundry of the piece
	if (rightNeighbor != NULL && rightNeighbor->IsNoBoundsPieceType()) {
		Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n3(1, 0);
		boundingLines.push_back(l3);
		boundingNorms.push_back(n3);
	}

	// Top boundry of the piece
	if (topNeighbor != NULL && topNeighbor->IsNoBoundsPieceType()) {
		Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n4(0, 1);
		boundingLines.push_back(l4);
		boundingNorms.push_back(n4);
	}

	this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor,
									rightNeighbor, topNeighbor, topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

void LevelPiece::AddStatus(const PieceStatus& status) {
	// If the piece already has the status then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if ((this->pieceStatus & status) == status) {
		return;
	}

	this->pieceStatus = (this->pieceStatus | status);
	if (status == LevelPiece::IceCubeStatus) {
		// We update the bounds since ice cubes change the bounds of a block - this must be done
		// AFTER the change to the status 
		this->UpdateBounds(this->leftNeighbor, this->bottomNeighbor, this->rightNeighbor, this->topNeighbor,
											 this->topRightNeighbor, this->topLeftNeighbor, this->bottomRightNeighbor, this->bottomLeftNeighbor);
	}

	// EVENT: A status has been added to this piece...
	GameEventManager::Instance()->ActionLevelPieceStatusAdded(*this, status);
}

void LevelPiece::RemoveStatus(const PieceStatus& status) {
	// If the piece doesn't have the status then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if ((this->pieceStatus & status) != status) {
		return;
	}

	this->pieceStatus = (this->pieceStatus & ~status);
	
	if (status == LevelPiece::IceCubeStatus) {
		// We update the bounds since ice cubes change the bounds of a block - this must be done
		// AFTER the change to the status 
		this->UpdateBounds(this->leftNeighbor, this->bottomNeighbor, this->rightNeighbor, this->topNeighbor,
											 this->topRightNeighbor, this->topLeftNeighbor, this->bottomRightNeighbor, this->bottomLeftNeighbor);
	}

	// EVENT: A status has been removed from this piece...
	GameEventManager::Instance()->ActionLevelPieceStatusRemoved(*this, status);
}

void LevelPiece::RemoveStatuses(int32_t statusMask) {
	bool statusFound = false;
	// Go through each status and remove it...
	if ((statusMask & LevelPiece::OnFireStatus) == LevelPiece::OnFireStatus) {
		this->RemoveStatus(LevelPiece::OnFireStatus);
		statusFound = true;
	}
	if ((statusMask & LevelPiece::IceCubeStatus) == LevelPiece::IceCubeStatus) {
		this->RemoveStatus(LevelPiece::IceCubeStatus);
		statusFound = true;
	}
	//... TODO for each other status

	assert(statusFound);
}

void LevelPiece::RemoveAllStatus() {
	// If the piece has no status effects then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if (this->pieceStatus == LevelPiece::NormalStatus) {
		return;
	}

	bool hadIceCubeStatus = this->HasStatus(LevelPiece::IceCubeStatus);
	this->pieceStatus = LevelPiece::NormalStatus;
	if (hadIceCubeStatus) {
		// We update the bounds since ice cubes change the bounds of a block - this must be done
		// AFTER the change to the status 
		this->UpdateBounds(this->leftNeighbor, this->bottomNeighbor, this->rightNeighbor, this->topNeighbor,
											 this->topRightNeighbor, this->topLeftNeighbor, this->bottomRightNeighbor, this->bottomLeftNeighbor);
	}

	// EVENT: All status effects removed from this piece...
	GameEventManager::Instance()->ActionLevelPieceAllStatusRemoved(*this);
}

// Helper function used to light this piece on fire - ONLY CALL THIS IF YOU CAN ACTUALLY
// PLACE AN ONFIRE STATUS ON THIS BLOCK!!!
void LevelPiece::LightPieceOnFire(GameModel* gameModel) {
		// If this piece is currently in an ice cube then just melt the ice cube but don't
		// make the block catch on fire...
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
			assert(success);
		}
		else {
			// The ball is on fire, and so we'll make this piece catch fire too...
			// The fire will eat away at the block over time
			this->AddStatus(LevelPiece::OnFireStatus);
			gameModel->AddStatusUpdateLevelPiece(this, LevelPiece::OnFireStatus);
		}
}

// Helper function used to freeze this piece in an ice block - ONLY CALL THIS IF YOU CAN ACTUALLY
// PLACE AN ICECUBE STATUS ON THIS BLOCK!!!
void LevelPiece::FreezePieceInIce(GameModel* gameModel) {
		// If this piece is currently on fire then the ice will cancel with the fire and
		// the piece will no longer be on fire
		if (this->HasStatus(LevelPiece::OnFireStatus)) {
			bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::OnFireStatus);
			assert(success);
		}
		else {
			// The ball is icy, encapsulate this piece in an ice cube, this will make the ball take
			// have to hit this piece once more in order to destroy it...
			this->AddStatus(LevelPiece::IceCubeStatus);
			gameModel->AddStatusUpdateLevelPiece(this, LevelPiece::IceCubeStatus);
		}
}