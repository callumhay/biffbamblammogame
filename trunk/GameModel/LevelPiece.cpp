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

LevelPiece::LevelPiece(unsigned int wLoc, unsigned int hLoc) : colour(1,1,1) {
	this->SetWidthAndHeightIndex(wLoc, hLoc);
}


LevelPiece::~LevelPiece() {
}

void LevelPiece::SetWidthAndHeightIndex(unsigned int wLoc, unsigned int hLoc) {
	this->wIndex = wLoc;
	this->hIndex = hLoc;
	this->center = Point2D(wLoc * PIECE_WIDTH + HALF_PIECE_WIDTH, hLoc * PIECE_HEIGHT + HALF_PIECE_HEIGHT);
}

Collision::AABB2D LevelPiece::GetAABB() const {
	return Collision::AABB2D(this->center - Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
													 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
}

/**
 * Check for a collision of a given circle with this block.
 * Returns: true on collision as well as the normal of the line being collided with
 * and the distance from that line of the given circle; false otherwise.
 */
bool LevelPiece::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const {
	// If there are no bounds to collide with or this level piece was the
	// last one collided with then we can't collide with this piece
	if (this->IsNoBoundsPieceType()) {
		return false;
	}
	if (ball.IsLastPieceCollidedWith(this)) {
		return false;
	}

	return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision);
}

/**
 * Check for a collision of a given AABB with this block.
 * Returns: true on collision, false otherwise.
 */
bool LevelPiece::CollisionCheck(const Collision::AABB2D& aabb) const {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}

	// See if there's a collision between this and the piece using AABBs
	return Collision::IsCollision(aabb, this->GetAABB());	
}

/**
 * Check for a collision of a given ray with this block. Also, on collision, will
 * set the value rayT to the value on the ray where the collision occurred.
 * Returns: true on collision, false otherwise.
 */
bool LevelPiece::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

/**
 * Check for a collision of a given set of bounding lines with this block.
 * Returns: true on collision, false otherwise.
 */
bool LevelPiece::CollisionCheck(const BoundingLines& boundingLines) const {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}

	return this->bounds.CollisionCheck(boundingLines);
}

bool LevelPiece::CollisionCheck(const Collision::Circle2D& c) const {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}
	return Collision::IsCollision(this->GetAABB(), c);
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

	// Clear all the currently existing boundry lines first
	this->bounds.Clear();

	// Set the bounding lines for a rectangular block
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

	this->bounds = BoundingLines(boundingLines, boundingNorms);
}

/**
 * Hit this block with the given beam segment - if this block reflects or refracts
 * the beam segment then this will return any new beam segments created by that reflection/
 * refraction.
 */
void LevelPiece::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const {
	UNUSED_PARAMETER(hitPoint);
	UNUSED_PARAMETER(impactDir);

	// The default behaviour is to just not do any reflection/refraction and return an empty list
	rays.clear();
}

LevelPiece* LevelPiece::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	UNUSED_PARAMETER(dT);

	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	return this;
}

LevelPiece* LevelPiece::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(paddle);

	assert(gameModel != NULL);
	return this;
}

// Draws the boundry lines and normals for this level piece.
void LevelPiece::DebugDraw() const {
	this->bounds.DebugDraw();
}