/**
 * BossRocketProjectile.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __BOSSROCKETPROJECTILE__H__
#define __BOSSROCKETPROJECTILE__H__

#include "RocketProjectile.h"
#include "RocketTurretProjectile.h"

class BossRocketProjectile : public RocketProjectile {
public:
	static const float BOSS_ROCKET_HEIGHT_DEFAULT;
	static const float BOSS_ROCKET_WIDTH_DEFAULT;

	BossRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir, float zOffset);
	~BossRocketProjectile();
    
    ProjectileType GetType() const { return Projectile::BossRocketBulletProjectile; }

    float GetDamage() const { return 100.0f; }

    float GetZOffset() const { return this->zOffset; }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }

    float GetAccelerationMagnitude() const { return 13.0f; }
    float GetRotationAccelerationMagnitude() const { return 300.0f; }

    float GetMaxVelocityMagnitude() const { return 18.0f; }
    float GetMaxRotationVelocityMagnitude() const { return 400.0f; }

    float GetDefaultHeight() const { return RocketTurretProjectile::TURRETROCKET_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return RocketTurretProjectile::TURRETROCKET_WIDTH_DEFAULT;  }

private:
    float zOffset;
};

#endif // __BOSSROCKETPROJECTILE__H__