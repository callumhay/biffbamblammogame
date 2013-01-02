/**
 * BossBodyPart.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BossBodyPart.h"

BossBodyPart::BossBodyPart(const BoundingLines& localBounds) :
AbstractBossBodyPart(), localBounds(localBounds) {
}

BossBodyPart::~BossBodyPart() {

}

void BossBodyPart::Tick(double dT, GameModel* gameModel) {
    UNUSED_PARAMETER(gameModel);

    // Move the body part by whatever velocity is currently set by its movement animation
    this->velocityMagAnim.Tick(dT);
    this->movementDirAnim.Tick(dT);
    Vector3D dMovement = (dT * this->velocityMagAnim.GetInterpolantValue()) * this->movementDirAnim.GetInterpolantValue();
    this->Translate(dMovement);
}

void BossBodyPart::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
    assert(gameModel != NULL);

    UNUSED_PARAMETER(gameModel);

    if (this->GetIsDestroyed()) {
        return;
    }

    // TODO...

    //ball.SetLastThingCollidedWith(this);
}

void BossBodyPart::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
    assert(gameModel != NULL);
	assert(projectile != NULL);

    UNUSED_PARAMETER(gameModel);

    if (this->GetIsDestroyed()) {
        return;
    }

    // TODO...

    //projectile->SetLastThingCollidedWith(this);
}

void BossBodyPart::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    assert(gameModel != NULL);

    if (this->GetIsDestroyed()) {
        return;
    }

    //paddle.HitByBoss(this);
}

void BossBodyPart::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const {
	/*
    // TODO:

    // If this body part is frozen then there are reflection/refraction rays...
    if (this->HasStatus(BossBodyPart::FrozenStatus)) {
		this->GetFrozenReflectionRefractionRays(hitPoint, impactDir, rays);
	}
	else {
		// The default behaviour is to just not do any reflection/refraction and return an empty list
		rays.clear();
	}
    */
    rays.clear();
}

void BossBodyPart::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(beamSegment);

	assert(beamSegment != NULL);
	assert(gameModel != NULL);
}

//void BossBodyPart::TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses) {  
//}


void BossBodyPart::GetFrozenReflectionRefractionRays(const Point2D& impactPt, const Vector2D& currDir,
                                                     std::list<Collision::Ray2D>& rays) const {

    // TODO.. do this based on the bounds of the body part...
    
}

#ifdef _DEBUG
void BossBodyPart::DebugDraw() const {
    this->GetWorldBounds().DebugDraw();
}
#endif