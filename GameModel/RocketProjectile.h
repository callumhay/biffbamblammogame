/**
 * RocketProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ROCKETPROJECTILE_H__
#define __ROCKETPROJECTILE_H__

#include "Projectile.h"

class CannonBlock;

class RocketProjectile : public Projectile {
public:
    static const float DEFAULT_VISUAL_WIDTH;
    static const float DEFAULT_VISUAL_HEIGHT;

	RocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir, float width, float height);
    RocketProjectile(const RocketProjectile& copy);
	virtual ~RocketProjectile();

    BoundingLines BuildBoundingLines() const;

    bool IsRocket() const { return true; }
    bool IsMine() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }

    float GetYRotation() const;

	virtual void Tick(double seconds, const GameModel& model);

	void LoadIntoCannonBlock(CannonBlock* cannonBlock);
	bool IsLoadedInCannonBlock() const;

    bool GetIsActive() const;

    virtual float GetForcePercentageFactor() const {
        return this->GetWidth() / DEFAULT_VISUAL_WIDTH;
    }

    virtual float GetVisualScaleFactor() const = 0;

    virtual float GetAccelerationMagnitude() const = 0;
    virtual float GetRotationAccelerationMagnitude() const = 0;

    virtual float GetMaxVelocityMagnitude() const = 0;
    virtual float GetMaxRotationVelocityMagnitude() const = 0;

    virtual float GetDefaultHeight() const = 0;
    virtual float GetDefaultWidth() const  = 0;

protected:
	static const Vector2D ROCKET_DEFAULT_VELOCITYDIR;
	static const Vector2D ROCKET_DEFAULT_RIGHTDIR;

	// When the rocket is loaded into a cannon block this will not be NULL
	CannonBlock* cannonBlock;

	// Spiral rotation of the rocket as it moves
	float currYRotationSpd;
	float currYRotation;

};

inline float RocketProjectile::GetYRotation() const {
	return this->currYRotation;
}

inline bool RocketProjectile::IsLoadedInCannonBlock() const {
	return (this->cannonBlock != NULL);
}

inline bool RocketProjectile::GetIsActive() const {
	return !this->IsLoadedInCannonBlock();
}


#endif // __ROCKETPROJECTILE_H__