#ifndef __LEVELPIECE_H__
#define __LEVELPIECE_H__

#include "../BlammoEngine/BlammoEngine.h"
#include "BoundingLines.h"

class Circle2D;
class LineSeg2D;
class Vector2D;
class GameBall;
class GameLevel;
class GameModel;

class LevelPiece {

public:
	// All level pieces must conform to these measurements...
	static const float PIECE_WIDTH;
	static const float PIECE_HEIGHT;
	static const float HALF_PIECE_WIDTH;
	static const float HALF_PIECE_HEIGHT;

	enum LevelPieceType { Breakable, Solid, Empty, Bomb };
	virtual LevelPieceType GetType() const = 0;

protected:
	Colour colour;								// The colour of this level piece
	Point2D center;								// The exact center of this piece in the game model
	BoundingLines bounds;			 		// The bounding box, rep. as lines forming the boundry of this, kept in world space
	unsigned int wIndex, hIndex;	// The width and height index to where this block is in its level
	
	void AddPossibleItemDrop(GameModel *gameModel);

public:
	LevelPiece(unsigned int wLoc, unsigned int hLoc);
	virtual ~LevelPiece();

	Point2D GetCenter() const {	return this->center; }
	unsigned int GetWidthIndex() const { return this->wIndex; }
	unsigned int GetHeightIndex() const { return this->hIndex; }
	Colour GetColour() const { return this->colour; }
	bool CollisionCheck(const Circle2D& c, Vector2D& n, float& d);

	virtual int GetPointValueForCollision() = 0;

	// Collision related stuffs
	virtual LevelPiece* Destroy(GameModel* gameModel) = 0;
	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
		const LevelPiece* rightNeightbor, const LevelPiece* topNeighbor) = 0;
	virtual LevelPiece* BallCollisionOccurred(GameModel* gameModel, const GameBall& ball) = 0;
	

	// Debug Stuffs
	void DebugDraw() const;

	virtual bool IsNoBoundsPieceType() const = 0;
	virtual bool MustBeDestoryedToEndLevel() const = 0;
	virtual bool CanBeDestroyed() const = 0;
	virtual bool UberballBlastsThrough() const = 0;
	virtual bool GhostballPassesThrough() const = 0;
};
#endif