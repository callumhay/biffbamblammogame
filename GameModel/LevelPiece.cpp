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

	// Make a 2D AABB for this piece
	Collision::AABB2D pieceBounds(this->center - Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
																this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));

	// See if there's a collision between this and the piece using AABBs
	return Collision::IsCollision(pieceBounds, aabb);	
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

	return this->bounds.CollisionCheck(ray, rayT);
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


/**
 * Function for adding a possible item drop for the given level piece.
 */
void LevelPiece::AddPossibleItemDrop(GameModel *gameModel) {
	assert(gameModel != NULL);

	std::list<GameItem*>& currentItemsDropping = gameModel->GetLiveItems();
	// Make sure we don't drop more items than the max allowable...
	if (currentItemsDropping.size() >= GameModelConstants::GetInstance()->MAX_LIVE_ITEMS) {
		return;
	}

	// We will drop an item based on probablility
	// TODO: Add probability based off multiplier and score stuff...
	double itemDropProb = GameModelConstants::GetInstance()->PROB_OF_ITEM_DROP;
	double randomNum    = Randomizer::GetInstance()->RandomNumZeroToOne();
	
	//debug_output("Probability of drop: " << itemDropProb << " Number for deciding: " << randomNum);

	if (randomNum <= itemDropProb) {
		// Drop an item - create a random item and add it to the list...
		GameItem* newGameItem = GameItemFactory::CreateRandomItem(this->GetCenter(), gameModel);
		currentItemsDropping.push_back(newGameItem);
		// EVENT: Item has been created and added to the game
		GameEventManager::Instance()->ActionItemSpawned(*newGameItem);
	}
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
	// The default behaviour is to just not do any reflection/refraction and return an empty list
	rays.clear();
}

LevelPiece* LevelPiece::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	return this;
}

// Draws the boundry lines and normals for this level piece.
void LevelPiece::DebugDraw() const {
	this->bounds.DebugDraw();
}