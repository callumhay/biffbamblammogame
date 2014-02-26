/**
 * RocketProjectile.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    virtual bool ModifyLevelUpdate(double dT, GameModel& model);

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