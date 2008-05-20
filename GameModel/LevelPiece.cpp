#include "LevelPiece.h"
#include "GameEventManager.h"

#include "../Utils/Vector.h"
#include "../Utils/Shape2D.h"

const float LevelPiece::PIECE_WIDTH = 2.5f;
const float LevelPiece::PIECE_HEIGHT = 1.0f;
const float LevelPiece::HALF_PIECE_WIDTH = PIECE_WIDTH / 2.0f;
const float LevelPiece::HALF_PIECE_HEIGHT = PIECE_HEIGHT / 2.0f;


LevelPiece::LevelPiece(unsigned int wLoc, unsigned int hLoc, LevelPieceType type): pieceType(type),
center(wLoc * PIECE_WIDTH + HALF_PIECE_WIDTH, hLoc * PIECE_HEIGHT + HALF_PIECE_HEIGHT),
currHalfWidth(HALF_PIECE_WIDTH), currHalfHeight(HALF_PIECE_HEIGHT), wIndex(wLoc), hIndex(hLoc) {
	this->UpdateBounds(NULL, NULL, NULL, NULL);
}


LevelPiece::~LevelPiece() {
}
/*
 * Determines if a given character is in the level piece enum.
 * Precondition: true.
 * Return: true if c is in the enum, false otherwise.
 */
bool LevelPiece::IsValidBlockEnum(char c) {
	bool isValid = true;
	
	switch(c) {
		case LevelPiece::RedBreakable:
		case LevelPiece::OrangeBreakable:
		case LevelPiece::YellowBreakable:
		case LevelPiece::GreenBreakable:
		case LevelPiece::Bomb:
		case LevelPiece::Solid:
		case LevelPiece::Empty:
			break;
		default:
			isValid = false;
	}

	return isValid;
}

/**
 * Decrements the piece type (e.g., if this was a yellow block it would now be green).
 */
void LevelPiece::DecrementPieceType() {
	switch(this->pieceType) {
		case GreenBreakable:
			this->pieceType = Empty;
			break;
		case YellowBreakable:
			this->pieceType = GreenBreakable;
			break;
		case OrangeBreakable:
			this->pieceType = YellowBreakable;
			break;
		case RedBreakable:
			this->pieceType = OrangeBreakable;
			break;
		case Bomb:
			this->pieceType = Empty;
			break;
		default:
			break;
	}
}

/**
 * Update the boundry lines of this levelpiece by eliminating lines 
 * that are next to neighbors.
 */
void LevelPiece::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
									const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor) {

		// Clear all the currently existing boundry lines first
		this->bounds.Clear();

		// We don't build a boundry outline if this piece is non-existant
		if (this->pieceType == Empty) {
			return;
		}

		// Set the bounding lines for a rectangular block
		std::vector<LineSeg2D> boundingLines;
		std::vector<Vector2D>  boundingNorms;

		// Left boundry of the piece
		if (leftNeighbor != NULL && leftNeighbor->IsNoBoundsPieceType()) {
			LineSeg2D l1(this->center + Vector2D(-this->currHalfWidth, this->currHalfHeight), 
									 this->center + Vector2D(-this->currHalfWidth, -this->currHalfHeight));
			Vector2D n1(-1, 0);
			boundingLines.push_back(l1);
			boundingNorms.push_back(n1);
		}

		// Bottom boundry of the piece
		if (bottomNeighbor != NULL && bottomNeighbor->IsNoBoundsPieceType()) {
			LineSeg2D l2(this->center + Vector2D(-this->currHalfWidth, -this->currHalfHeight),
									 this->center + Vector2D(this->currHalfWidth, -this->currHalfHeight));
			Vector2D n2(0, -1);
			boundingLines.push_back(l2);
			boundingNorms.push_back(n2);
		}

		// Right boundry of the piece
		if (rightNeighbor != NULL && rightNeighbor->IsNoBoundsPieceType()) {
			LineSeg2D l3(this->center + Vector2D(this->currHalfWidth, -this->currHalfHeight),
									 this->center + Vector2D(this->currHalfWidth, this->currHalfHeight));
			Vector2D n3(1, 0);
			boundingLines.push_back(l3);
			boundingNorms.push_back(n3);
		}

		// Top boundry of the piece
		if (topNeighbor != NULL && topNeighbor->IsNoBoundsPieceType()) {
			LineSeg2D l4(this->center + Vector2D(this->currHalfWidth, this->currHalfHeight),
									 this->center + Vector2D(-this->currHalfWidth, this->currHalfHeight));
			Vector2D n4(0, 1);
			boundingLines.push_back(l4);
			boundingNorms.push_back(n4);
		}

		this->bounds = BoundingLines(boundingLines, boundingNorms);
}

bool LevelPiece::CollisionCheck(const Circle2D& c, Vector2D& n, float &d) {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}

	return this->bounds.Collide(c, n, d);
}

void LevelPiece::BallCollisionOccurred() {
	// Nothing happens if this is just an empty space or solid block
	switch(this->pieceType) {
		case Empty:
		case Solid:
			return;
		case Bomb:
			return;
		case GreenBreakable:
			// EVENT: Block is being destoryed
			GameEventManager::Instance()->ActionBlockDestroyed(*this);

			// Make empty, eliminate bounding lines
			this->DecrementPieceType();
			this->UpdateBounds(NULL, NULL, NULL, NULL);

			break;
		default:
			this->DecrementPieceType();
			break;
	}
	assert(LevelPiece::IsValidBlockEnum(this->pieceType));
}

void LevelPiece::DebugDraw() const {
	this->bounds.DebugDraw();
}