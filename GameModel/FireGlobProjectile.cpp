/**
 * FireGlobProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "FireGlobProjectile.h"
#include "LevelPiece.h"
#include "GameModel.h"

const float FireGlobProjectile::FIRE_GLOB_MIN_VELOCITY			= 2.5f;
const float FireGlobProjectile::FIRE_GLOB_MAX_ADD_VELOCITY	= 1.25f;

FireGlobProjectile::FireGlobProjectile(const Point2D& spawnLoc, float size, const Vector2D& gravityDir) : 
Projectile(spawnLoc, size, size), totalTickTime(0.0) {

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
    if (this->totalTickTime > 1.0) {
        this->SetLastThingCollidedWith(NULL);
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
