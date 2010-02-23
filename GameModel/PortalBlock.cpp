
#include "PortalBlock.h"
#include "Projectile.h"

PortalBlock::PortalBlock(unsigned int wLoc, unsigned int hLoc, const PortalBlock* sibling) : LevelPiece(wLoc, hLoc), sibling(sibling) {
	// Change colour...
	// TODO
	this->colour = Colour(0.3f, 0, 0);
}

PortalBlock::~PortalBlock() {
}

LevelPiece* PortalBlock::CollisionOccurred(GameModel* gameModel, const GameBall& ball) {
	//ball.TeleportViaPortal(this->sibling);
	return this;
}

LevelPiece* PortalBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	// Send the projectile to the sibling portal...
	std::list<Collision::Ray2D> newProjectileRay = this->GetReflectionRefractionRays(projectile->GetPosition(), projectile->GetVelocityDirection());
	assert(newProjectileRay.size() == 1);
	
	// Change the projectile so that it comes out of the sibling portal
	const Collision::Ray2D& ray = *newProjectileRay.begin();
	projectile->SetPosition(ray.GetOrigin());
	projectile->SetVelocity(ray.GetUnitDirection(), projectile->GetVelocityMagnitude());

	return this;
}

/**
 * Calculates what happens when a ray goes through the portal and comes out of its sibling portal.
 */
std::list<Collision::Ray2D> PortalBlock::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir) const {
	// Find position difference between the point and center...
	Vector2D toHitVec = hitPoint - this->GetCenter();
	// The new ray is simply the old one coming out of the sibling portal
	Point2D newPosition = this->sibling->GetCenter() + toHitVec;

	std::list<Collision::Ray2D> result;
	result.push_back(Collision::Ray2D(newPosition, impactDir));
	return result;
}