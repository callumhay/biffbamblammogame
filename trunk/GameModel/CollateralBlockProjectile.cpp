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

CollateralBlockProjectile::CollateralBlockProjectile(CollateralBlock* collateralBlock) : 
Projectile(Projectile::CollateralBlockProjectile, collateralBlock->GetCenter(), LevelPiece::PIECE_WIDTH, LevelPiece::PIECE_HEIGHT),
collateralBlock(collateralBlock) {
	this->SetVelocity(Vector2D(0, -1), CollateralBlock::COLLATERAL_FALL_SPEED);
}

CollateralBlockProjectile::~CollateralBlockProjectile() {
	// Clean up the collateral block
	delete this->collateralBlock;
	this->collateralBlock = NULL;
}

void CollateralBlockProjectile::Tick(double seconds) {
	// The collateral block has several states that it goes through, in the case of this projectile
	// it will start in its warning state then go into collateral damage mode and end via death,
	// we tick the block to make this happen
	this->collateralBlock->Tick(seconds);

	switch (this->collateralBlock->GetState()) {

		case CollateralBlock::WarningState:
			break;

		case CollateralBlock::CollateralDamageState: {
				Vector2D moveAmt = static_cast<float>(seconds) * this->velocityMag * this->velocityDir;
				this->SetPosition(this->GetPosition() + moveAmt);
			}
			break;

		default:
			assert(false);
			break;
	}

}

BoundingLines CollateralBlockProjectile::BuildBoundingLines() const {
	// Build the bounds based on the default position of the collateral block's bounds
	// moved to the projectile's current position
	BoundingLines bounds = this->collateralBlock->GetBounds();
	Vector2D diffToPos = this->GetPosition() - this->collateralBlock->GetCenter();
	bounds.TranslateBounds(diffToPos);
	return bounds;
}