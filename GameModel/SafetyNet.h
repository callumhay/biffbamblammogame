/**
 * SafetyNet.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SAFETYNET_H__
#define __SAFETYNET_H__

#include "GameBall.h"
#include "PlayerPaddle.h"
#include "BoundingLines.h"

class GameLevel;
class PaddleMineProjectile;

/**
 * Class for representing the safety net formed below the paddle when
 * a safety net item is acquired by the player. This will save the ball from
 * falling past the paddle. It is broken by most objects that will collide with it.
 * Some objects can stick/attach to the net until it is destroyed.
 */
class SafetyNet {
public:
    SafetyNet(const GameLevel& currLevel);
    ~SafetyNet();

    const BoundingLines& GetBounds() { return this->bounds; }

    bool BallCollisionCheck(const GameBall& b, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision);
	bool PaddleCollisionCheck(const PlayerPaddle& p);
	bool ProjectileCollisionCheck(const BoundingLines& projectileBoundingLines);

    void AttachProjectile(PaddleMineProjectile* p);
    void DetachProjectile(PaddleMineProjectile* p);
    
private:
    BoundingLines bounds;
    std::set<PaddleMineProjectile*> attachedProjectiles;

    DISALLOW_COPY_AND_ASSIGN(SafetyNet);
};

inline bool SafetyNet::BallCollisionCheck(const GameBall& b, double dT, Vector2D& n,
                                          Collision::LineSeg2D& collisionLine,
                                          double& timeSinceCollision) {
	return this->bounds.Collide(dT, b.GetBounds(), b.GetVelocity(), n, collisionLine, timeSinceCollision);
}

inline bool SafetyNet::PaddleCollisionCheck(const PlayerPaddle& p) {

	// Test for collision, if there was one then we kill the safety net
	return p.CollisionCheck(this->bounds, false);
}

inline bool SafetyNet::ProjectileCollisionCheck(const BoundingLines& projectileBoundingLines) {
	return projectileBoundingLines.CollisionCheck(this->bounds);
}

inline void SafetyNet::AttachProjectile(PaddleMineProjectile* p) {
    assert(p != NULL);
    this->attachedProjectiles.insert(p);
}

inline void SafetyNet::DetachProjectile(PaddleMineProjectile* p) {
    assert(p != NULL);
    this->attachedProjectiles.erase(p);
}

#endif // __SAFETYNET_H__