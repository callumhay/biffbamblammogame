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
#include "PrismBlock.h"

#include "../BlammoEngine/Matrix.h"

/** 
 * Namespace for shared triangle block functionality - this exists to avoid
 * multiple inheritance because stupid c++ doesn't have interfaces, le sigh.
 */
namespace TriangleBlock {
	enum SideType { ShortSide, LongSide, HypotenuseSide };
	enum Orientation { UpperLeft, UpperRight, LowerLeft, LowerRight };
		
	BoundingLines CreateTriangleBounds(Orientation triOrient, const Point2D& center,
		const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
		const LevelPiece* rightNeightbor, const LevelPiece* topNeighbor);

	Matrix4x4 GetOrientationMatrix(Orientation orient);
	Matrix4x4 GetInvOrientationMatrix(Orientation orient);

	Vector2D GetSideNormal(SideType side, Orientation orient);
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
	~BreakableTriangleBlock();

	LevelPieceType GetType() const {
		return LevelPiece::BreakableTriangle;
	}

	Matrix4x4 GetPieceToLevelTransform() const;
	Matrix4x4 GetPieceToLevelInvTransform() const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
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
	~SolidTriangleBlock();

	LevelPieceType GetType() const {
		return LevelPiece::SolidTriangle;
	}

	Matrix4x4 GetPieceToLevelTransform() const;
	Matrix4x4 GetPieceToLevelInvTransform() const;

	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	
	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
										const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
										const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

};

/**
 * Triangle block that acts like prism blocks - only it can reflect lasers based on its
 * different shape and orientation.
 */
class PrismTriangleBlock : public PrismBlock {
protected:
	TriangleBlock::Orientation orient;	// Orientation of the triangle - i.e., where the apex corner is located

public:
	PrismTriangleBlock(TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc);
	~PrismTriangleBlock();

	LevelPieceType GetType() const {
		return LevelPiece::PrismTriangle;
	}

	Matrix4x4 GetPieceToLevelTransform() const;
	Matrix4x4 GetPieceToLevelInvTransform() const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
										const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
										const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;
};

#endif