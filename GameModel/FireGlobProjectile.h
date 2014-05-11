/**
 * FireGlobProjectile.h
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

	float xMovementVariation;           // The variation of the x-axis movement as the fire glob falls
	double totalTickTime;               // The total time ticked on this projectile so far
	RelativeSize relativeSize;          // The relative size of this fire glob
    bool hasClearedLastThingCollided;   // Whether or not we've cleared the initial collided with thing or not
                                        // NOTE: Bit of a hack, just makes sure that we can re-collide with the block that spawned this
                                        // and that we don't keep clearing the last thing it collided with so portals still work properly

	DISALLOW_COPY_AND_ASSIGN(FireGlobProjectile);
};

inline const FireGlobProjectile::RelativeSize& FireGlobProjectile::GetRelativeSize() const {
	return this->relativeSize;
}

#endif // __FIREGLOBPROJECTILE_H__