/**
 * CollateralBlockProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CollateralBlockProjectile.h"
#include "CollateralBlock.h"
#include "GameEventManager.h"

// NOTE: We don't make the collateral block projectile have the exact width of a LevelPiece because we don't want
// numerical precision errors in the collisions with blocks that are directly beside the falling
// collateral block

CollateralBlockProjectile::CollateralBlockProjectile(CollateralBlock* collateralBlock) : 
Projectile(collateralBlock->GetCenter(), LevelPiece::PIECE_WIDTH - 10*EPSILON, 0.9f*LevelPiece::PIECE_HEIGHT),
collateralBlock(collateralBlock) {
	this->SetVelocity(Vector2D(0, -1), 0.0f);
}

CollateralBlockProjectile::~CollateralBlockProjectile() {

	// EVENT: Collateral block is being destroyed
    GameEventManager::Instance()->ActionBlockDestroyed(*this->collateralBlock, LevelPiece::NotApplicableDestruction);

	// Clean up the collateral block
	delete this->collateralBlock;
	this->collateralBlock = NULL;
}

void CollateralBlockProjectile::Tick(double seconds, const GameModel& model) {
	// The collateral block has several states that it goes through, in the case of this projectile
	// it will start in its warning state then go into collateral damage mode and end via death,
	// we tick the block to make this happen
	this->collateralBlock->Tick(seconds, model, *this);
}

/**
 * Just vertical bounding line(s) - does the job by hitting everything below as it falls down.
 */
BoundingLines CollateralBlockProjectile::BuildBoundingLines() const {
    const Vector2D UP_DIR			= this->GetHalfHeight() * this->GetVelocityDirection();
    const Vector2D HALF_RIGHT_DIR	= 0.5f * this->GetHalfWidth() * this->GetRightVectorDirection();
    
    Point2D centerTop    = this->GetPosition() + UP_DIR;
    Point2D centerBottom = this->GetPosition() - UP_DIR;

    Point2D halfLeftCenterTop    = this->GetPosition() - HALF_RIGHT_DIR + UP_DIR;
    Point2D halfLeftCenterBottom = this->GetPosition() - HALF_RIGHT_DIR - UP_DIR;

    Point2D halfRightCenterTop   = 	this->GetPosition() + HALF_RIGHT_DIR + UP_DIR;
    Point2D halfRightCenterBottom = this->GetPosition() + HALF_RIGHT_DIR - UP_DIR;

    Point2D leftCenterTop    = halfLeftCenterTop - HALF_RIGHT_DIR;
    Point2D leftCenterBottom = halfLeftCenterBottom - HALF_RIGHT_DIR;

    Point2D rightCenterTop   = 	halfRightCenterTop + HALF_RIGHT_DIR;
    Point2D rightCenterBottom = halfRightCenterBottom + HALF_RIGHT_DIR;

    std::vector<Collision::LineSeg2D> sideBounds;
    sideBounds.reserve(5);
    sideBounds.push_back(Collision::LineSeg2D(centerTop, centerBottom));
    sideBounds.push_back(Collision::LineSeg2D(halfLeftCenterTop, halfLeftCenterBottom));
    sideBounds.push_back(Collision::LineSeg2D(halfRightCenterTop, halfRightCenterBottom));
    sideBounds.push_back(Collision::LineSeg2D(leftCenterTop, leftCenterBottom));
    sideBounds.push_back(Collision::LineSeg2D(rightCenterTop, rightCenterBottom));

    std::vector<Vector2D> normBounds;
    normBounds.resize(5);

    return BoundingLines(sideBounds, normBounds);
}

// This allows things like the portal block to set the position...
void CollateralBlockProjectile::SetPosition(const Point2D& pos) { 
	this->collateralBlock->TeleportToNewPosition(pos);
	this->position = pos; 
} 