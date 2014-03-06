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
#include "PortalBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"

const double CollateralBlockProjectile::PORTAL_COLLISION_RESET_TIME = LevelPiece::PIECE_HEIGHT / CollateralBlock::COLLATERAL_FALL_SPEED;

// NOTE: We don't make the collateral block projectile have the exact width of a LevelPiece because we don't want
// numerical precision errors in the collisions with blocks that are directly beside the falling
// collateral block

CollateralBlockProjectile::CollateralBlockProjectile(CollateralBlock* collateralBlock) : 
Projectile(collateralBlock->GetCenter(), LevelPiece::PIECE_WIDTH - 10*EPSILON, 0.9f*LevelPiece::PIECE_HEIGHT),
collateralBlock(collateralBlock), resetPortalRecollisionCountdown(-1), lastPortalCollidedWith(NULL) {
	this->SetVelocity(Vector2D(0, -1), 0.0f);
}

CollateralBlockProjectile::~CollateralBlockProjectile() {

	// EVENT: Collateral block is being destroyed
    GameEventManager::Instance()->ActionBlockDestroyed(*this->collateralBlock, LevelPiece::NotApplicableDestruction);

	// Clean up the collateral block
	delete this->collateralBlock;
	this->collateralBlock = NULL;
}

void CollateralBlockProjectile::LevelPieceCollisionOccurred(LevelPiece* block) {
    // If we collide with a portal block then we'll need to set a timer to get rid of the last thing this
    // rocket collided with since the player could redirect the rocket back through the portal block and we want
    // to allow this...
    if (block->GetType() == LevelPiece::Portal) {
        this->resetPortalRecollisionCountdown = PORTAL_COLLISION_RESET_TIME;
        this->lastPortalCollidedWith = static_cast<PortalBlock*>(block)->GetSiblingPortal();
    }
    else {
        this->resetPortalRecollisionCountdown = -1;
        this->lastPortalCollidedWith = NULL;
    }
}

void CollateralBlockProjectile::Tick(double seconds, const GameModel& model) {
    
    // Check the gravity direction and set the collateral block's general direction based on it...
    Vector3D gravityDir = model.GetGravityDir();
    this->velocityDir[1] = NumberFuncs::SignOf(gravityDir[1]) * fabs(this->velocityDir[1]);

	// The collateral block has several states that it goes through, in the case of this projectile
	// it will start in its warning state then go into collateral damage mode and end via death,
	// we tick the block to make this happen
	this->collateralBlock->Tick(seconds, model, *this);

    // Check to see if we should reset the last portal collision 
    if (this->resetPortalRecollisionCountdown != -1) {
        if (this->resetPortalRecollisionCountdown <= 0.0) {

            assert(this->lastPortalCollidedWith != NULL);
            if (this->lastThingCollidedWith == this->lastPortalCollidedWith ||
                this->lastThingCollidedWith == this->lastPortalCollidedWith->GetSiblingPortal()) {
                    this->SetLastThingCollidedWith(NULL);
            }

            this->lastPortalCollidedWith = NULL;
            this->resetPortalRecollisionCountdown = -1;
        }
        else {
            this->resetPortalRecollisionCountdown -= seconds;
        }
    }
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