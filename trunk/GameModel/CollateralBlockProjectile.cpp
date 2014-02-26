/**
 * CollateralBlockProjectile.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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