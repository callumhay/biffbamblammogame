/**
 * PaddleRocketProjectile.h
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

#ifndef __PADDLEROCKETPROJECTILE_H__
#define __PADDLEROCKETPROJECTILE_H__

#include "RocketProjectile.h"

/**
 * Projectile for the rocket shot from the player paddle when the player acquires a rocket paddle item.
 */
class PaddleRocketProjectile : public RocketProjectile {
public:
	static const float PADDLEROCKET_HEIGHT_DEFAULT;
	static const float PADDLEROCKET_WIDTH_DEFAULT;

	PaddleRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir, float width, float height);
	~PaddleRocketProjectile();
    
    ProjectileType GetType() const {
        return Projectile::PaddleRocketBulletProjectile;
    }

    float GetDamage() const {
        // Obtain the size relative to the the normal size of the rocket (1.0) this rocket
        // is currently - used to determine how much it will destroy.
        return (this->GetWidth() / PADDLEROCKET_WIDTH_DEFAULT) * 400.0f;
    }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }
    
    float GetAccelerationMagnitude() const { return 2.0f; }
    float GetRotationAccelerationMagnitude() const { return 60.0f; }

    float GetMaxVelocityMagnitude() const { return 20.0f; }
    float GetMaxRotationVelocityMagnitude() const { return 400.0f; }

    float GetDefaultHeight() const { return PADDLEROCKET_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return PADDLEROCKET_WIDTH_DEFAULT;  }

};

#endif // __PADDLEROCKETPROJECTILE_H__