#include "BreakableBlock.h"
#include "EmptySpaceBlock.h"

#include "GameModel.h"
#include "GameLevel.h"
#include "GameBall.h"
#include "GameEventManager.h"

BreakableBlock::BreakableBlock(char type, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), pieceType(static_cast<BreakablePieceType>(type)) {
	assert(IsValidBreakablePieceType(type));

	this->colour = BreakableBlock::GetColourOfBreakableType(this->pieceType);
}


BreakableBlock::~BreakableBlock() {
}

/**
 * Inherited private function for destroying this breakable block.
 */
LevelPiece* BreakableBlock::Destroy(GameModel* gameModel) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this);

	// When destroying a breakable there is the possiblity of dropping an item...
	this->AddPossibleItemDrop(gameModel);

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(this, emptyPiece);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

	return emptyPiece;
}

/**
 * Private static helper function used to move the colour of this breakable
 * down to the next level (thus decrementing it appropriately). For example
 * when a red block is decremented it goes to orange, then yellow, and then to green.
 */
BreakableBlock::BreakablePieceType BreakableBlock::GetDecrementedPieceType(BreakablePieceType breakableType) {
	switch(breakableType) {
		case YellowBreakable:
			return GreenBreakable;
		case OrangeBreakable:
			return YellowBreakable;
		case RedBreakable:
			return OrangeBreakable;
		default:
			break;
	}
	assert(false);
	return GreenBreakable;
}

Colour BreakableBlock::GetColourOfBreakableType(BreakableBlock::BreakablePieceType breakableType) {
	switch(breakableType) {
		case GreenBreakable:
			return Colour(0.0f, 1.0f, 0.0f);
		case YellowBreakable:
			return Colour(1.0f, 1.0f, 0.0f);
		case OrangeBreakable:
			return Colour(1.0f, 0.5f, 0.0f);
		case RedBreakable:
			return Colour(1.0f, 0.0f, 0.0f);
		default:
			assert(false);
			break;
	}
	
	return Colour(1,1,1);
}

/**
 * Update the bounds of this block based on the given neighbours.
 */
void BreakableBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor, 
																	const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor) {

		// Clear all the currently existing boundry lines first
		this->bounds.Clear();

		// Set the bounding lines for a rectangular block
		std::vector<Collision::LineSeg2D> boundingLines;
		std::vector<Vector2D>  boundingNorms;

		// We only create boundries for breakables in cases where neighbours exist AND they are empty 
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
 * Call this when a collision has actually occured with the ball and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, const GameBall& ball) {

	// If the ball is an 'uber' ball then we decrement the piece type twice when it is
	// not the lowest kind of breakable block (i.e., green)
	if (((ball.GetBallType() & GameBall::UberBall) == GameBall::UberBall) && this->pieceType != GreenBreakable) {
		this->DecrementPieceType();
	}
	
	LevelPiece* newPiece = NULL;

	switch(this->pieceType) {
		case GreenBreakable:
			newPiece = this->Destroy(gameModel);
			break;
		default:
			// By default any other type of block is simply decremented
			this->DecrementPieceType();
			{
				GameLevel* level = gameModel->GetCurrentLevel();
				level->PieceChanged(this, this);
			}
			newPiece = this;
			break;
	}
	assert(newPiece != NULL);
	
	return newPiece;
}

/**
 * Call this when a collision has actually occured with a projectile and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* BreakableBlock::CollisionOccurred(GameModel* gameModel, const Projectile& projectile) {
	LevelPiece* newPiece = this;

	// For destructive projectile types...
	if (projectile.GetType() == Projectile::PaddleLaserProjectile) {
		switch(this->pieceType) {
			case GreenBreakable:
				newPiece = this->Destroy(gameModel);
				break;

			default:
				this->DecrementPieceType();
				{
					GameLevel* level = gameModel->GetCurrentLevel();
					level->PieceChanged(this, this);
				}
				newPiece = this;
				break;				
		}
	}
	return newPiece;
}