/**
 * BossLightningBoltProjectile.cpp
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

#include "BossLightningBoltProjectile.h"

const float BossLightningBoltProjectile::HEIGHT_DEFAULT	= 1.8f;  // Height of a bolt in game units
const float BossLightningBoltProjectile::WIDTH_DEFAULT	= 0.9f;  // Width of a bolt in game units 
const float BossLightningBoltProjectile::SPD_DEFAULT    = 14.0f;

BossLightningBoltProjectile::BossLightningBoltProjectile(const Point2D& spawnLoc, const Vector2D& dirVec, float speed) :
Projectile(spawnLoc, WIDTH_DEFAULT, HEIGHT_DEFAULT) {
    this->SetVelocity(dirVec, speed);
}

BossLightningBoltProjectile::BossLightningBoltProjectile(const BossLightningBoltProjectile& copy) : Projectile(copy) {
}

BossLightningBoltProjectile::~BossLightningBoltProjectile() {
}

void BossLightningBoltProjectile::Tick(double seconds, const GameModel& model) {
    this->AugmentDirectionOnPaddleMagnet(seconds, model, 55.0f);

    // Update the bolt's position
    this->position = this->position + (seconds * this->velocityMag * this->velocityDir);
}

BoundingLines BossLightningBoltProjectile::BuildBoundingLines() const {
    const Vector2D& UP_DIR    = this->GetVelocityDirection();
    const Vector2D& RIGHT_DIR = this->GetRightVectorDirection();

    Point2D topRight    = this->GetPosition() + this->GetHalfHeight()*UP_DIR + this->GetHalfWidth()*RIGHT_DIR;
    Point2D bottomRight = topRight - this->GetHeight()*UP_DIR;
    Point2D topLeft     = topRight - this->GetWidth()*RIGHT_DIR;
    Point2D bottomLeft  = topLeft - this->GetHeight()*UP_DIR;

    std::vector<Collision::LineSeg2D> sideBounds;
    sideBounds.reserve(2);
    sideBounds.push_back(Collision::LineSeg2D(topLeft, bottomLeft));
    sideBounds.push_back(Collision::LineSeg2D(topRight, bottomRight));
    std::vector<Vector2D> normBounds;
    normBounds.resize(2);

    return BoundingLines(sideBounds, normBounds);
}
