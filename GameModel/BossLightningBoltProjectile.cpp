/**
 * BossLightningBoltProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BossLightningBoltProjectile.h"

const float BossLightningBoltProjectile::HEIGHT_DEFAULT	= 1.8f;  // Height of a bolt in game units
const float BossLightningBoltProjectile::WIDTH_DEFAULT	= 0.9f;  // Width of a bolt in game units 
const float BossLightningBoltProjectile::SPD_DEFAULT    = 14.0f;

BossLightningBoltProjectile::BossLightningBoltProjectile(const Point2D& spawnLoc, const Vector2D& dirVec, float speed) :
Projectile(spawnLoc, WIDTH_DEFAULT, HEIGHT_DEFAULT) {
    this->SetVelocity(dirVec, speed);
}

BossLightningBoltProjectile::BossLightningBoltProjectile(const BossLightningBoltProjectile& copy) : Projectile(copy) {
}

BossLightningBoltProjectile::~BossLightningBoltProjectile() {
}

void BossLightningBoltProjectile::Tick(double seconds, const GameModel& model) {
    this->AugmentDirectionOnPaddleMagnet(seconds, model, 55.0f);

    // Update the bolt's position
    this->position = this->position + (seconds * this->velocityMag * this->velocityDir);
}

BoundingLines BossLightningBoltProjectile::BuildBoundingLines() const {
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