/**
 * FireGlobProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __FIREGLOBPROJECTILE_H__
#define __FIREGLOBPROJECTILE_H__

#include "Projectile.h"

class FireGlobProjectile : public Projectile {
public:
	FireGlobProjectile(const Point2D& spawnLoc, float size, const Vector2D& gravityDir);
	~FireGlobProjectile();

    bool IsRocket() const { return false; }
    bool IsMine() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }

	void Tick(double seconds, const GameModel& model);
	BoundingLines BuildBoundingLines() const;
    ProjectileType GetType() const {
        return Projectile::FireGlobProjectile;
    }
    float GetDamage() const { return 0.0f; /* NEVER CHANGE THIS TO ANYTHING BUT ZERO! */ }

	enum RelativeSize { Small = 1, Medium = 2, Large = 3 };
	const FireGlobProjectile::RelativeSize& GetRelativeSize() const;

private:
	static const float FIRE_GLOB_MIN_VELOCITY;
	static const float FIRE_GLOB_MAX_ADD_VELOCITY;

	float xMovementVariation;		// The variation of the x-axis movement as the fire glob falls
	double totalTickTime;				// The total time ticked on this projectile so far
	RelativeSize relativeSize;  // The relative size of this fire glob

	DISALLOW_COPY_AND_ASSIGN(FireGlobProjectile);
};

inline const FireGlobProjectile::RelativeSize& FireGlobProjectile::GetRelativeSize() const {
	return this->relativeSize;
}

#endif // __FIREGLOBPROJECTILE_H__