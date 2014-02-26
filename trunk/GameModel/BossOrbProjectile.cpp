/**
 * BossOrbProjectile.cpp
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

#include "BossOrbProjectile.h"

const float BossOrbProjectile::RADIUS_DEFAULT = 0.45f;
const float BossOrbProjectile::SPD_DEFAULT    = 13.0f;

BossOrbProjectile::BossOrbProjectile(const Point2D& spawnLoc) :
OrbProjectile(spawnLoc, RADIUS_DEFAULT, SPD_DEFAULT, Vector2D(0, -1)) {
}

BossOrbProjectile::BossOrbProjectile(const Point2D& spawnLoc, const Vector2D& dirVec) :
OrbProjectile(spawnLoc, RADIUS_DEFAULT, SPD_DEFAULT, dirVec) {
}

BossOrbProjectile::BossOrbProjectile(const BossOrbProjectile& copy) : OrbProjectile(copy) {
}

BossOrbProjectile::~BossOrbProjectile() {
}