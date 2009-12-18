/**
 * TriangleBlocks.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TriangleBlocks.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameEventManager.h"

#include "EmptySpaceBlock.h"

// Breakable Triangle Block Class Functions ---------------------------------------------------------

BreakableTriangleBlock::BreakableTriangleBlock(char type, TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc) : 
BreakableBlock(type, wLoc, hLoc), orient(orientation) {
}

BreakableTriangleBlock::~BreakableTriangleBlock() {
}

void BreakableTriangleBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
																					const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
																					const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
																					const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	this->bounds = TriangleBlock::CreateTriangleBounds(this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor);
}

Matrix4x4 BreakableTriangleBlock::GetPieceToLevelTransform() const {
	Matrix4x4 translation =  Matrix4x4::translationMatrix(Vector3D(this->center[0], this->center[1], 0.0f));
	Matrix4x4 orient = TriangleBlock::GetOrientationMatrix(this->orient);
	return translation * orient;
}

Matrix4x4 BreakableTriangleBlock::GetPieceToLevelInvTransform() const {
	Matrix4x4 invTranslation =  Matrix4x4::translationMatrix(Vector3D(-this->center[0], -this->center[1], 0.0f));
	Matrix4x4 invOrient = TriangleBlock::GetInvOrientationMatrix(this->orient);
	return invOrient * invTranslation;
}

// Solid Triangle Block Class Functions ---------------------------------------------------------------
SolidTriangleBlock::SolidTriangleBlock(TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc) :
SolidBlock(wLoc, hLoc), orient(orientation) {
}

SolidTriangleBlock::~SolidTriangleBlock() {
}

void SolidTriangleBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
																			const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
																			const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
																			const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	this->bounds = TriangleBlock::CreateTriangleBounds(this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor);
}

Matrix4x4 SolidTriangleBlock::GetPieceToLevelTransform() const {
	Matrix4x4 translation =  Matrix4x4::translationMatrix(Vector3D(this->center[0], this->center[1], 0.0f));
	Matrix4x4 orient = TriangleBlock::GetOrientationMatrix(this->orient);
	return translation * orient;
}

Matrix4x4 SolidTriangleBlock::GetPieceToLevelInvTransform() const {
	Matrix4x4 invTranslation =  Matrix4x4::translationMatrix(Vector3D(-this->center[0], -this->center[1], 0.0f));
	Matrix4x4 invOrient = TriangleBlock::GetInvOrientationMatrix(this->orient);
	return invOrient * invTranslation;
}

PrismTriangleBlock::PrismTriangleBlock(TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc) :
PrismBlock(wLoc, hLoc), orient(orientation) {
}

PrismTriangleBlock::~PrismTriangleBlock() {
}

Matrix4x4 PrismTriangleBlock::GetPieceToLevelTransform() const {
	Matrix4x4 translation =  Matrix4x4::translationMatrix(Vector3D(this->center[0], this->center[1], 0.0f));
	Matrix4x4 orient = TriangleBlock::GetOrientationMatrix(this->orient);
	return translation * orient;
}

Matrix4x4 PrismTriangleBlock::GetPieceToLevelInvTransform() const {
	Matrix4x4 invTranslation =  Matrix4x4::translationMatrix(Vector3D(-this->center[0], -this->center[1], 0.0f));
	Matrix4x4 invOrient = TriangleBlock::GetInvOrientationMatrix(this->orient);
	return invOrient * invTranslation;
}

void PrismTriangleBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
																			const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
																			const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
																			const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
	this->bounds = TriangleBlock::CreateTriangleBounds(this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor);
}

LevelPiece* PrismTriangleBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	if (projectile->GetType() == Projectile::PaddleLaserBulletProjectile) {
	// Based on where the laser bullet hits, we change its direction
		
		// Need to figure out if this laser bullet already collided with this block... if it has then we just ignore it
		if (!projectile->IsLastLevelPieceCollidedWith(this)) {

			// Obtain all the normals of the lines that the projectile is colliding with...
			std::vector<int> collidingIndices = this->bounds.CollisionCheckIndices(projectile->BuildBoundingLines());
			if (collidingIndices.size() > 2 || collidingIndices.size() == 0) {
				// This should never happen...
				assert(false);
			}
			else if (collidingIndices.size() == 2) {
				// Special case - due to the impression of the collision algorithm 2 lines were found to be
				// colliding with the projectile, we need to reconcile this...

				// TODO

			}
			else {
				// Only one collision line (collidingIndices.size() == 1)
			}


			// TODO...


			// The projectile has now officially collided with this prism block, set it into the projectile
			// so that it doesn't keep happening while the projectile is colliding with this block
			projectile->SetLastLevelPieceCollidedWith(this);
		}
	}

	return this;
}

// Triangle Block Namespace Functions ----------------------------------------------------------------

/**
 * Static class for sharing the creation of triangle boundries used by the triangle block classes.
 * Returns: bounding lines for the given triangle.
 */
BoundingLines TriangleBlock::CreateTriangleBounds(Orientation triOrient, const Point2D& center, 
																									const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
																									const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor) {
	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

	Vector2D longSideNorm, shortSideNorm, hypSideNorm;
	Collision::LineSeg2D shortSide, longSide, hypSide;

	bool topNeighborEmpty			= topNeighbor != NULL && topNeighbor->IsNoBoundsPieceType();
	bool bottomNeighborEmpty	= bottomNeighbor != NULL && bottomNeighbor->IsNoBoundsPieceType();
	bool leftNeighborEmpty		= leftNeighbor != NULL && leftNeighbor->IsNoBoundsPieceType();
	bool rightNeighborEmpty		= rightNeighbor != NULL && rightNeighbor->IsNoBoundsPieceType();

	switch (triOrient) {
		case TriangleBlock::UpperLeft:
			// Long side is the top, short side is left and hypotenuse goes from lower left to upper right
			longSideNorm	= Vector2D(0, 1);
			shortSideNorm	= Vector2D(-1, 0);
			
			longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
									 center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			shortSide = Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			
			if (bottomNeighborEmpty || rightNeighborEmpty) {
				hypSideNorm		= Vector2D(1, -1);
				hypSide		= Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
											center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				boundingLines.push_back(hypSide);
				boundingNorms.push_back(hypSideNorm);
			}
			break;

		case TriangleBlock::UpperRight:
			// Long side is the top, short side is the right and hypotenuse goes from lower right to upper left
			longSideNorm	= Vector2D(0, 1);
			shortSideNorm	= Vector2D(1, 0);

			longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
										center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));		
			shortSide = Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
										center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));			
			
			if (bottomNeighborEmpty || leftNeighborEmpty) {
				hypSideNorm		= Vector2D(-1, -1);
				hypSide		= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
											center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				boundingLines.push_back(hypSide);
				boundingNorms.push_back(hypSideNorm);				
			}
			break;

		case TriangleBlock::LowerLeft:
			// Long side is at the bottom, short side is on the left and hypotenuse goes from lower-right to upper-left
			longSideNorm	= Vector2D(0, -1);
			shortSideNorm	= Vector2D(-1, 0);
			
			longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
										center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));		
			shortSide = Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			
			if (topNeighborEmpty || rightNeighborEmpty) {
				hypSideNorm		= Vector2D(1, 1);
				hypSide		= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
											center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				boundingLines.push_back(hypSide);
				boundingNorms.push_back(hypSideNorm);
			}
			break;
		case TriangleBlock::LowerRight:
			// Long side is at the bottom, short side is on the right and hypotenuse goes from lower-left to upper-right
			longSideNorm	= Vector2D(0, -1);
			shortSideNorm	= Vector2D(1, 0);
			
			longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
										center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));		
			shortSide = Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			
			if (topNeighborEmpty || leftNeighborEmpty) {
				hypSideNorm		= Vector2D(-1, 1);
				hypSide		= Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
											center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				boundingLines.push_back(hypSide);
				boundingNorms.push_back(hypSideNorm);			
			}
			break;

		default:
			assert(false);
			break;
	}

	// We only create boundries for breakables in cases where neighbours exist AND they are empty 
	// (i.e., the ball can actually get to them).
	if (topNeighborEmpty && (triOrient == TriangleBlock::UpperRight || triOrient == TriangleBlock::UpperLeft)) {
		boundingLines.push_back(longSide);
		boundingNorms.push_back(longSideNorm);
	}
	else if (bottomNeighborEmpty && (triOrient == TriangleBlock::LowerRight || triOrient == TriangleBlock::LowerLeft)) {
		boundingLines.push_back(longSide);
		boundingNorms.push_back(longSideNorm);
	}

	if (leftNeighborEmpty && (triOrient == TriangleBlock::LowerLeft || triOrient == TriangleBlock::UpperLeft)) {
		boundingLines.push_back(shortSide);
		boundingNorms.push_back(shortSideNorm);		
	}
	else if (rightNeighborEmpty && (triOrient == TriangleBlock::LowerRight || triOrient == TriangleBlock::UpperRight)) {
		boundingLines.push_back(shortSide);
		boundingNorms.push_back(shortSideNorm);		
	}

	return BoundingLines(boundingLines, boundingNorms);
}

Matrix4x4 TriangleBlock::GetOrientationMatrix(TriangleBlock::Orientation orient) {
	Matrix4x4 orientMat;
	switch (orient) {
		case TriangleBlock::UpperLeft:
			// Reflection in y-axis
			orientMat = Matrix4x4::reflectionMatrix('y');
			break;
		case TriangleBlock::UpperRight:
			// Identity matrix.
			break;
		case TriangleBlock::LowerLeft:
			// 180 degree rotation
			orientMat = Matrix4x4::rotationMatrix('z', 180, true);
			break;
		case TriangleBlock::LowerRight:
			orientMat = Matrix4x4::reflectionMatrix('x');
			break;
		default:
			assert(false);
			break;
	}

	return orientMat;
}

Matrix4x4 TriangleBlock::GetInvOrientationMatrix(TriangleBlock::Orientation orient) {
	Matrix4x4 invOrientMat;
	switch (orient) {
		case TriangleBlock::UpperLeft:
			// reflection in y-axis
			invOrientMat = Matrix4x4::reflectionMatrix('y');
			break;
		case TriangleBlock::UpperRight:
			// Identity matrix.
			break;
		case TriangleBlock::LowerLeft:
			// 180 degree rotation
			invOrientMat = Matrix4x4::rotationMatrix('z', -180, true);
			break;
		case TriangleBlock::LowerRight:
			// reflection in x-axis
			invOrientMat = Matrix4x4::reflectionMatrix('x');
			break;
		default:
			assert(false);
			break;
	}

	return invOrientMat;
}