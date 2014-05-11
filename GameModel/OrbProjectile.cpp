/**
 * OrbProjectile.cpp
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

#include "OrbProjectile.h"
#include "GameModel.h"

OrbProjectile::OrbProjectile(const Point2D& spawnLoc, float radius,
                             float velocityMag, const Vector2D& velocityDir) :
Projectile(spawnLoc, 2*radius, 2*radius) {
    this->SetVelocity(velocityDir, velocityMag);
}
OrbProjectile::OrbProjectile(const OrbProjectile& copy) : Projectile(copy) {
}

OrbProjectile::~OrbProjectile() {
}

/**
 * Tick function - executed every frame with given delta seconds. This will
 * update the position and other relevant information for this paddle laser projectile.
 */
void OrbProjectile::Tick(double seconds, const GameModel& model) {
    this->AugmentDirectionOnPaddleMagnet(seconds, model, 60.0f);
	// Update the orb's position
	this->position = this->position + (seconds * this->velocityMag * this->velocityDir);
}

/**
 * Obtain the bounding lines for this projectile for use in various collision detection tests.
 * Returns: Bounding lines for this projectile.
 */
BoundingLines OrbProjectile::BuildBoundingLines() const {
    // Radius provides for both the width and height...
    assert(this->GetWidth() == this->GetHeight());
    
    float radius = this->GetWidth() / 2.0f;
    
    // Build a polygon around the circle using lines...
    static const int NUM_SIDES = 6;
    static const float BETWEEN_EXTENT_DEGS = 360.0f / static_cast<float>(NUM_SIDES);
    static const Vector2D EXTENT_VECS[NUM_SIDES] = {
        Vector2D(0, 1),
        Vector2D::Rotate(BETWEEN_EXTENT_DEGS,   Vector2D(0, 1)),
        Vector2D::Rotate(2*BETWEEN_EXTENT_DEGS, Vector2D(0, 1)),
        Vector2D::Rotate(3*BETWEEN_EXTENT_DEGS, Vector2D(0, 1)),
        Vector2D::Rotate(4*BETWEEN_EXTENT_DEGS, Vector2D(0, 1)),
        Vector2D::Rotate(5*BETWEEN_EXTENT_DEGS, Vector2D(0, 1))
    };

    std::vector<Collision::LineSeg2D> sideBounds;
    sideBounds.reserve(NUM_SIDES);
    for (int i = 1; i < NUM_SIDES; i++) {
        sideBounds.push_back(Collision::LineSeg2D(this->GetPosition() + radius*EXTENT_VECS[i-1], 
            this->GetPosition() + radius*EXTENT_VECS[i]));
    }
    sideBounds.push_back(Collision::LineSeg2D(this->GetPosition() + radius*EXTENT_VECS[NUM_SIDES-1], 
        this->GetPosition() + radius*EXTENT_VECS[0]));

    // No normals are needed, just make them all zero vectors
    std::vector<Vector2D> normBounds;
    normBounds.resize(NUM_SIDES);

    return BoundingLines(sideBounds, normBounds);
}