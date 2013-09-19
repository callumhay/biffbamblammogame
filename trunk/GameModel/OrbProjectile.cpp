/**
 * LaserBulletProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "OrbProjectile.h"
#include "GameModel.h"

OrbProjectile::OrbProjectile(const Point2D& spawnLoc, float radius,
                             float velocityMag, const Vector2D& velocityDir) :
Projectile(spawnLoc, 2*radius, 2*radius) {
    this->SetVelocity(velocityDir, velocityMag);
}
OrbProjectile::OrbProjectile(const OrbProjectile& copy) : Projectile(copy) {
}

OrbProjectile::~OrbProjectile() {
}

/**
 * Tick function - executed every frame with given delta seconds. This will
 * update the position and other relevant information for this paddle laser projectile.
 */
void OrbProjectile::Tick(double seconds, const GameModel& model) {
    this->AugmentDirectionOnPaddleMagnet(seconds, model, 60.0f);
	// Update the orb's position
	this->position = this->position + (seconds * this->velocityMag * this->velocityDir);
}

/**
 * Obtain the bounding lines for this projectile for use in various collision detection tests.
 * Returns: Bounding lines for this projectile.
 */
BoundingLines OrbProjectile::BuildBoundingLines() const {
    // Radius provides for both the width and height...
    assert(this->GetWidth() == this->GetHeight());
    
    float radius = this->GetWidth() / 2.0f;
    
    // Build a polygon around the circle using lines...
    static const int NUM_SIDES = 6;
    static const float BETWEEN_EXTENT_DEGS = 360.0f / static_cast<float>(NUM_SIDES);
    static const Vector2D EXTENT_VECS[NUM_SIDES] = {
        Vector2D(0, 1),
        Vector2D::Rotate(BETWEEN_EXTENT_DEGS,   Vector2D(0, 1)),
        Vector2D::Rotate(2*BETWEEN_EXTENT_DEGS, Vector2D(0, 1)),
        Vector2D::Rotate(3*BETWEEN_EXTENT_DEGS, Vector2D(0, 1)),
        Vector2D::Rotate(4*BETWEEN_EXTENT_DEGS, Vector2D(0, 1)),
        Vector2D::Rotate(5*BETWEEN_EXTENT_DEGS, Vector2D(0, 1))
    };

    std::vector<Collision::LineSeg2D> sideBounds;
    sideBounds.reserve(NUM_SIDES);
    for (int i = 1; i < NUM_SIDES; i++) {
        sideBounds.push_back(Collision::LineSeg2D(this->GetPosition() + radius*EXTENT_VECS[i-1], 
            this->GetPosition() + radius*EXTENT_VECS[i]));
    }
    sideBounds.push_back(Collision::LineSeg2D(this->GetPosition() + radius*EXTENT_VECS[NUM_SIDES-1], 
        this->GetPosition() + radius*EXTENT_VECS[0]));

    // No normals are needed, just make them all zero vectors
    std::vector<Vector2D> normBounds;
    normBounds.resize(NUM_SIDES);

    return BoundingLines(sideBounds, normBounds);
}