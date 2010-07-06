/**
 * PaddleRocketProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLEROCKETPROJECTILE_H__
#define __PADDLEROCKETPROJECTILE_H__

#include "Projectile.h"

class PlayerPaddle;

class PaddleRocketProjectile : public Projectile {
public:
	static const float PADDLEROCKET_HEIGHT_DEFAULT;
	static const float PADDLEROCKET_WIDTH_DEFAULT;

	PaddleRocketProjectile(const Point2D& spawnLoc, float width, float height);
	~PaddleRocketProjectile();

	void Tick(double seconds);
	BoundingLines BuildBoundingLines() const;

	float GetYRotation() const;

private:

	static const Vector2D PADDLEROCKET_VELOCITYDIR;
	static const Vector2D PADDLEROCKET_RIGHTDIR;

	static const float ACCELERATION_MAG;
	static const float ROTATION_ACCELERATION_MAG;

	static const float MAX_VELOCITY_MAG;
	static const float MAX_ROTATION_VELOCITY_MAG;

	// Spiral rotation of the rocket as it moves up
	float currYRotationSpd;
	float currYRotation;

};

inline float PaddleRocketProjectile::GetYRotation() const {
	return this->currYRotation;
}

// Obtain the size relative to the the normal size of the rocket (1.0) this rocket
// is currently - used to determine how much it will destroy.


#endif // __PADDLEROCKETPROJECTILE_H__