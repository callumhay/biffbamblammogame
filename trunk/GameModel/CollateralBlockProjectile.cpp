/**
 * CollateralBlockProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CollateralBlockProjectile.h"
#include "CollateralBlock.h"
#include "GameEventManager.h"

CollateralBlockProjectile::CollateralBlockProjectile(CollateralBlock* collateralBlock) : 
Projectile(Projectile::CollateralBlockProjectile, collateralBlock->GetCenter(), LevelPiece::PIECE_WIDTH, LevelPiece::PIECE_HEIGHT),
collateralBlock(collateralBlock) {
	this->SetVelocity(Vector2D(0, -1), CollateralBlock::COLLATERAL_FALL_SPEED);
}

CollateralBlockProjectile::~CollateralBlockProjectile() {

	// EVENT: Collateral block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this->collateralBlock);

	// Clean up the collateral block
	delete this->collateralBlock;
	this->collateralBlock = NULL;
}

void CollateralBlockProjectile::Tick(double seconds) {
	// The collateral block has several states that it goes through, in the case of this projectile
	// it will start in its warning state then go into collateral damage mode and end via death,
	// we tick the block to make this happen
	this->collateralBlock->Tick(seconds, *this);
}

/**
 * Just vertical bounding line(s) - does the job by hitting everything below as it falls down.
 */
BoundingLines CollateralBlockProjectile::BuildBoundingLines() const {
		const Vector2D UP_DIR			= this->GetVelocityDirection();
		const Vector2D RIGHT_DIR	= this->GetRightVectorDirection();

		Point2D centerTop    = this->GetPosition() + this->GetHalfHeight() * UP_DIR;
		Point2D centerBottom = this->GetPosition() - this->GetHalfHeight() * UP_DIR;
		
		// other collision lines??
		//Point2D 

		std::vector<Collision::LineSeg2D> sideBounds;
		sideBounds.reserve(1);
		sideBounds.push_back(Collision::LineSeg2D(centerTop, centerBottom));

		std::vector<Vector2D> normBounds;
		normBounds.resize(1);

		return BoundingLines(sideBounds, normBounds);
}

// This allows things like the portal block to set the position...
void CollateralBlockProjectile::SetPosition(const Point2D& pos) { 
	this->collateralBlock->TeleportToNewPosition(pos);
	this->position = pos; 
} 