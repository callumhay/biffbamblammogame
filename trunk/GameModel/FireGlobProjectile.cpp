/**
 * FireGlobProjectile.cpp
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

#include "FireGlobProjectile.h"
#include "LevelPiece.h"
#include "GameModel.h"

const float FireGlobProjectile::FIRE_GLOB_MIN_VELOCITY			= 2.5f;
const float FireGlobProjectile::FIRE_GLOB_MAX_ADD_VELOCITY	= 1.25f;

FireGlobProjectile::FireGlobProjectile(const Point2D& spawnLoc, float size, const Vector2D& gravityDir) : 
Projectile(spawnLoc, size, size), totalTickTime(0.0), hasClearedLastThingCollided(false) {

	this->SetVelocity(gravityDir, FireGlobProjectile::FIRE_GLOB_MIN_VELOCITY);
	this->xMovementVariation = 0.8f * size;

	if (size >= LevelPiece::HALF_PIECE_WIDTH) {
		this->relativeSize = Large;
	}
	else if (size >= 0.66f * LevelPiece::HALF_PIECE_WIDTH) {
		this->relativeSize = Medium;
	}
	else {
		this->relativeSize = Small;
	}
}

FireGlobProjectile::~FireGlobProjectile() {
}

void FireGlobProjectile::Tick(double seconds, const GameModel& model) {
    if (!this->AugmentDirectionOnPaddleMagnet(seconds, model, 65.0f)) {

	    // The glob just keeps falling down with gravity...
        this->velocityDir[1] = model.GetGravityDir()[1];
        this->velocityDir.Normalize();
    }

	Vector2D dDist = seconds * this->velocityMag * this->velocityDir;
	this->SetPosition(this->GetPosition() + dDist);
	this->totalTickTime += seconds;

    // Keep clearing the last thing this collided with once it's fallen from the block that spawned it
    if (!this->hasClearedLastThingCollided && this->totalTickTime > 1.0) {
        this->SetLastThingCollidedWith(NULL);
        this->hasClearedLastThingCollided = true;
    }
}

BoundingLines FireGlobProjectile::BuildBoundingLines() const {
	const Vector2D& UP_DIR	  = this->GetVelocityDirection();
	const Vector2D& RIGHT_DIR = this->GetRightVectorDirection();

	Point2D topRight = this->GetPosition() + this->GetHalfHeight()*UP_DIR + 0.85f*this->GetHalfWidth()*RIGHT_DIR;
	Point2D bottomRight = topRight - this->GetHeight()*UP_DIR;
	Point2D topLeft = topRight - this->GetWidth()*RIGHT_DIR;
	Point2D bottomLeft = topLeft - this->GetHeight()*UP_DIR;

	std::vector<Collision::LineSeg2D> sideBounds;
	sideBounds.reserve(4);
	sideBounds.push_back(Collision::LineSeg2D(topLeft, bottomLeft));
	sideBounds.push_back(Collision::LineSeg2D(topRight, bottomRight));
	sideBounds.push_back(Collision::LineSeg2D(topLeft, topRight));
	sideBounds.push_back(Collision::LineSeg2D(bottomLeft, bottomRight));
	std::vector<Vector2D> normBounds;
	normBounds.resize(4);

	return BoundingLines(sideBounds, normBounds);
}
