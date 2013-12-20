/**
 * SafetyNet.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SAFETYNET_H__
#define __SAFETYNET_H__

#include "GameBall.h"
#include "PlayerPaddle.h"
#include "BoundingLines.h"

class GameLevel;
class MineProjectile;

/**
 * Class for representing the safety net formed below the paddle when
 * a safety net item is acquired by the player. This will save the ball from
 * falling past the paddle. It is broken by most objects that will collide with it.
 * Some objects can stick/attach to the net until it is destroyed.
 */
class SafetyNet {
public:
    static const float SAFETY_NET_HEIGHT;
    static const float SAFETY_NET_HALF_HEIGHT;

    SafetyNet(const GameLevel& currLevel);
    ~SafetyNet();

    const BoundingLines& GetBounds() { return this->bounds; }

    bool BallCollisionCheck(const GameBall& b, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& pointOfCollision);
	bool PaddleCollisionCheck(const PlayerPaddle& p);
	bool ProjectileCollisionCheck(const BoundingLines& projectileBoundingLines, double dT, const Vector2D& projectileVel);

    bool BallBlastsThrough(const GameBall& ball) const;

    void AttachProjectile(MineProjectile* p);
    void DetachProjectile(MineProjectile* p);
    
private:
    BoundingLines bounds;
    std::set<MineProjectile*> attachedProjectiles;

    DISALLOW_COPY_AND_ASSIGN(SafetyNet);
};

inline bool SafetyNet::BallCollisionCheck(const GameBall& b, double dT, Vector2D& n,
                                          Collision::LineSeg2D& collisionLine,
                                          double& timeUntilCollision, Point2D& pointOfCollision) {

	return this->bounds.Collide(dT, b.GetBounds(), b.GetVelocity(), n, 
        collisionLine, timeUntilCollision, pointOfCollision);
}

inline bool SafetyNet::PaddleCollisionCheck(const PlayerPaddle& p) {

	// Test for collision, if there was one then we kill the safety net
	return p.CollisionCheck(this->bounds, false);
}

inline bool SafetyNet::ProjectileCollisionCheck(const BoundingLines& projectileBoundingLines, 
                                                double dT, const Vector2D& projectileVel) {
	return this->bounds.CollisionCheck(projectileBoundingLines, dT, projectileVel);
}

inline bool SafetyNet::BallBlastsThrough(const GameBall& ball) const {
    return ball.HasBallType(GameBall::UberBall);
}

inline void SafetyNet::AttachProjectile(MineProjectile* p) {
    assert(p != NULL);
    this->attachedProjectiles.insert(p);
}

inline void SafetyNet::DetachProjectile(MineProjectile* p) {
    assert(p != NULL);
    this->attachedProjectiles.erase(p);
}

#endif // __SAFETYNET_H__