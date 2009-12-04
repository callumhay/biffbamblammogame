/**
 * TriangleBlocks.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TRIANGLEBLOCKS_H__
#define __TRIANGLEBLOCKS_H__

#include "BreakableBlock.h"
#include "SolidBlock.h"

#include "../BlammoEngine/Matrix.h"

/** 
 * Namespace for shared triangle block functionality - this exists to avoid
 * multiple inheritance because stupid c++ doesn't have interfaces, le sigh.
 */
namespace TriangleBlock {
		enum Orientation { UpperLeft, UpperRight, LowerLeft, LowerRight };
		
		static BoundingLines CreateTriangleBounds(Orientation triOrient, const Point2D& center,
			const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
			const LevelPiece* rightNeightbor, const LevelPiece* topNeighbor);

		static Matrix4x4 GetOrientationMatrix(Orientation orient);
		static Matrix4x4 GetInvOrientationMatrix(Orientation orient);
};

/**
 * Triangle block that acts like breakable blocks. Only major difference is its shape
 * and orientation.
 */
class BreakableTriangleBlock : public BreakableBlock {
protected:
	TriangleBlock::Orientation orient;	// Orientation of the triangle - i.e., where the apex corner is located

public:
	BreakableTriangleBlock(char type, TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc);
	virtual ~BreakableTriangleBlock();

	virtual LevelPieceType GetType() const {
		return LevelPiece::BreakableTriangle;
	}

	virtual Matrix4x4 GetPieceToLevelTransform() const;
	virtual Matrix4x4 GetPieceToLevelInvTransform() const;

	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
													  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
														const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
														const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

};

/**
 * Triangle block that acts like solid blocks. Only major difference is its shape
 * and orientation.
 */
class SolidTriangleBlock : public SolidBlock {
protected:
	TriangleBlock::Orientation orient;	// Orientation of the triangle - i.e., where the apex corner is located

public:
	SolidTriangleBlock(TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc);
	virtual ~SolidTriangleBlock();

	virtual LevelPieceType GetType() const {
		return LevelPiece::SolidTriangle;
	}

	virtual Matrix4x4 GetPieceToLevelTransform() const;
	virtual Matrix4x4 GetPieceToLevelInvTransform() const;

	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
													  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
														const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
														const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

};
#endif