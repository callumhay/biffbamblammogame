#ifndef __LEVELPIECE_H__
#define __LEVELPIECE_H__

#include <vector>

#include "../Utils/Point.h"
#include "BoundingLines.h"

class Circle2D;
class LineSeg2D;
class Vector2D;

class LevelPiece {

public:
	// These categorize the various types of blocks that make up a level
	enum LevelPieceType { GreenBreakable = 'G', YellowBreakable = 'Y', OrangeBreakable = 'O', RedBreakable = 'R', 
								        Solid = 'S', Bomb = 'B', Empty = 'E'}; 
	static bool IsValidBlockEnum(char c);

private:
	static const int POINTS_PER_BLOCK_HIT = 1;

	static const float PIECE_WIDTH;
	static const float PIECE_HEIGHT;
	static const float HALF_PIECE_WIDTH;
	static const float HALF_PIECE_HEIGHT;

	LevelPieceType pieceType;		 // The type of level piece this is... TODO: polymorphism?
	Point2D center;							 // The exact center of this piece in the game model
	BoundingLines bounds;			 	 // The bounding box, rep. as lines forming the boundry of this, kept in world space
	unsigned int wIndex, hIndex; // The width and height index to where this block is in its level

	float currHalfWidth, currHalfHeight;

	bool IsNoBoundsPieceType() const {
		return this->pieceType == Empty;
	}

	void DecrementPieceType();

public:
	LevelPiece(unsigned int wLoc, unsigned int hLoc, LevelPieceType type);
	~LevelPiece();

	// Returns whether this is a piece that is vital to destroy in order
	// to end the level.
	bool MustBeDestoryedToEndLevel() const {
		return this->pieceType == GreenBreakable || this->pieceType == YellowBreakable ||
					 this->pieceType == OrangeBreakable || this->pieceType == RedBreakable;
	}

	// Obtain the type of level piece this is.
	LevelPieceType GetType() const {
		return this->pieceType;
	}
	
	static LevelPieceType GetDecrementedPieceType(LevelPieceType pieceType);

	// Obtain the center point of this piece.
	Point2D GetCenter() const {
		return this->center;
	}
	
	// Obtain width/height index
	unsigned int GetWidthIndex() const {
		return this->wIndex;
	}
	unsigned int GetHeightIndex() const {
		return this->hIndex;
	}

	// Obtain half-width/height
	float GetHalfWidth() const {
		return this->currHalfWidth;
	}
	float GetHalfHeight() const {
		return this->currHalfHeight;
	}

	int GetPointValueForCollision();

	// Collision related stuffs
	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeightbor, const LevelPiece* topNeighbor);
	bool CollisionCheck(const Circle2D& c, Vector2D& n, float& d);
	void BallCollisionOccurred();


	// Debug Stuffs
	void DebugDraw() const;

};
#endif