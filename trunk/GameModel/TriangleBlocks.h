/**
 * TriangleBlocks.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
		
	BoundingLines CreateTriangleBounds(bool generateReflectRefractNormals, Orientation triOrient, const Point2D& center,
		const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
		const LevelPiece* rightNeightbor, const LevelPiece* topNeighbor);

	Matrix4x4 GetOrientationMatrix(Orientation orient);
	Matrix4x4 GetInvOrientationMatrix(Orientation orient);

	Vector2D GetSideNormal(bool generateReflectRefractNormals, SideType side, Orientation orient);
    bool GetOrientation(const LevelPiece* piece, Orientation& orient);

    inline bool IsTriangleType(const LevelPiece& piece) {
        return (piece.GetType() == LevelPiece::BreakableTriangle || piece.GetType() == LevelPiece::SolidTriangle ||
               piece.GetType() == LevelPiece::PrismTriangle);
    }
};

class TriangleBlockInterface {
public:
    virtual TriangleBlock::Orientation GetOrientation() const = 0;
};

/**
 * Triangle block that acts like breakable blocks. Only major difference is its shape
 * and orientation.
 */
class BreakableTriangleBlock : public BreakableBlock, public TriangleBlockInterface {
protected:
	TriangleBlock::Orientation orient;	// Orientation of the triangle - i.e., where the apex corner is located

public:
	BreakableTriangleBlock(char type, TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc);
	~BreakableTriangleBlock();

    TriangleBlock::Orientation GetOrientation() const {
        return this->orient;
    }

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
class SolidTriangleBlock : public SolidBlock, public TriangleBlockInterface {
protected:
	TriangleBlock::Orientation orient;	// Orientation of the triangle - i.e., where the apex corner is located

public:
	SolidTriangleBlock(TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc);
	~SolidTriangleBlock();

    TriangleBlock::Orientation GetOrientation() const {
        return this->orient;
    }

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
class PrismTriangleBlock : public PrismBlock, public TriangleBlockInterface {
protected:
	TriangleBlock::Orientation orient;	// Orientation of the triangle - i.e., where the apex corner is located
	BoundingLines reflectRefractBounds;	// Bounds used for reflection/refraction of beams
public:
	PrismTriangleBlock(TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc);
	~PrismTriangleBlock();

    TriangleBlock::Orientation GetOrientation() const {
        return this->orient;
    }

	LevelPieceType GetType() const {
		return LevelPiece::PrismTriangle;
	}

	const BoundingLines& GetReflectRefractBounds() const;


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