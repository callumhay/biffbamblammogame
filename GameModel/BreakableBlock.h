#ifndef __BREAKABLEBLOCK_H__
#define __BREAKABLEBLOCK_H__

#include "LevelPiece.h"

class BreakableBlock : public LevelPiece {
private:
	static const int POINTS_ON_BLOCK_HIT				= 5;		// Points obtained when you just hit a breakable block
	static const int POINTS_ON_BLOCK_DESTROYED = 10;		// Points obtained when you destory a breakable block

	enum BreakablePieceType { GreenBreakable = 'G', YellowBreakable = 'Y', OrangeBreakable = 'O', RedBreakable = 'R' }; 
	BreakablePieceType pieceType;

	static BreakablePieceType GetDecrementedPieceType(BreakablePieceType breakableType);
	static Colour GetColourOfBreakableType(BreakablePieceType breakableType);

	// Decrements this piecetype to the next lesser breakable piece type.
	void DecrementPieceType() {
		this->pieceType = BreakableBlock::GetDecrementedPieceType(this->pieceType);
		this->colour    = BreakableBlock::GetColourOfBreakableType(this->pieceType);
	}

public:
	BreakableBlock(char type, unsigned int wLoc, unsigned int hLoc);
	virtual ~BreakableBlock();

	virtual LevelPieceType GetType() const { 
		return LevelPiece::Breakable;
	}

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	virtual bool IsNoBoundsPieceType() const {
		return false;
	}

	// Check to see if a given character is a type of breakable block
	// Returns: true if c is a breakable block type, false otherwise.
	static bool IsValidBreakablePieceType(char c) {
		return (c == GreenBreakable || c == YellowBreakable || c == OrangeBreakable || c == RedBreakable);
	}
	
	// All breakable blocks must be destroyed to end a level...
	virtual bool MustBeDestoryedToEndLevel() const {
		return true;
	}
	virtual bool CanBeDestroyed() const {
		return true;
	}

	// Whether or not the uber ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	virtual bool UberballBlastsThrough() const {
		return this->pieceType == GreenBreakable;
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	virtual bool GhostballPassesThrough() const {
		return true;
	}

	// Obtain the point value for a collision with this breakable block.
	// Returns: point value.
	virtual int GetPointValueForCollision() {
		switch (this->pieceType) {
			case GreenBreakable:
				return POINTS_ON_BLOCK_DESTROYED;
			case YellowBreakable:
			case OrangeBreakable:
			case RedBreakable:
				return POINTS_ON_BLOCK_HIT;
			default:
				break;
		}
		return 0;
	}

	// Collision related stuffs
	virtual LevelPiece* Destroy(GameModel* gameModel);	
	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
													  const LevelPiece* rightNeightbor, const LevelPiece* topNeighbor);
	virtual LevelPiece* BallCollisionOccurred(GameModel* gameModel, const GameBall& ball);

};
#endif