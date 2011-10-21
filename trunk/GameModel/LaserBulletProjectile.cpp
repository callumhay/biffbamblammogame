/**
 * LaserBulletProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LaserBulletProjectile.h"
#include "GameModel.h"

LaserBulletProjectile::LaserBulletProjectile(const Point2D& spawnLoc, float width, float height, 
                                             float velocityMag, const Vector2D& velocityDir) :
Projectile(spawnLoc, width, height) {
    this->SetVelocity(velocityMag * velocityDir);
}
LaserBulletProjectile::LaserBulletProjectile(const LaserBulletProjectile& copy) : Projectile(copy) {
}

LaserBulletProjectile::~LaserBulletProjectile() {
}

/**
 * Tick function - executed every frame with given delta seconds. This will
 * update the position and other relevant information for this paddle laser projectile.
 */
void LaserBulletProjectile::Tick(double seconds, const GameModel& model) {
    this->AugmentDirectionOnPaddleMagnet(seconds, model, 70.0f);

	// Update the laser's position
	this->position = this->position + (seconds * this->velocityMag * this->velocityDir);
}

/**
 * Obtain the bounding lines for this projectile for use in various collision detection tests.
 * Returns: Bounding lines for this projectile.
 */
BoundingLines LaserBulletProjectile::BuildBoundingLines() const {
    const Vector2D UP_DIR       = this->GetVelocityDirection();
    const Vector2D RIGHT_DIR	= this->GetRightVectorDirection();

    Point2D topRight    = this->GetPosition() + this->GetHalfHeight()*UP_DIR + this->GetHalfWidth()*RIGHT_DIR;
    Point2D bottomRight = topRight - this->GetHeight()*UP_DIR;
    Point2D topLeft     = topRight - this->GetWidth()*RIGHT_DIR;
    Point2D bottomLeft  = topLeft - this->GetHeight()*UP_DIR;

    std::vector<Collision::LineSeg2D> sideBounds;
    sideBounds.reserve(2);
    sideBounds.push_back(Collision::LineSeg2D(topLeft, bottomLeft));
    sideBounds.push_back(Collision::LineSeg2D(topRight, bottomRight));
    std::vector<Vector2D> normBounds;
    normBounds.resize(2);

    return BoundingLines(sideBounds, normBounds);
}