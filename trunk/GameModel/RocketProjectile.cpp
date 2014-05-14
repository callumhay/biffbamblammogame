/**
 * RocketProjectile.cpp
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

#include "RocketProjectile.h"
#include "CannonBlock.h"
#include "GameEventManager.h"

const Vector2D RocketProjectile::ROCKET_DEFAULT_VELOCITYDIR = Vector2D(0, 1);
const Vector2D RocketProjectile::ROCKET_DEFAULT_RIGHTDIR    = Vector2D(1, 0);

const float RocketProjectile::DEFAULT_VISUAL_WIDTH  = 0.8f;
const float RocketProjectile::DEFAULT_VISUAL_HEIGHT = 1.5f;

RocketProjectile::RocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir,
                                   float width, float height) :
Projectile(spawnLoc, width, height), cannonBlock(NULL) {

    if (rocketVelDir.IsZero()) {
        assert(false);
        this->velocityDir = ROCKET_DEFAULT_VELOCITYDIR;
        this->rightVec    = ROCKET_DEFAULT_RIGHTDIR;
    }
    else {
        this->velocityDir = rocketVelDir;
        this->rightVec    = Vector2D::Rotate(-90, rocketVelDir);
        this->velocityDir.Normalize();
        this->rightVec.Normalize();
    }

	this->velocityMag      = 0.0f;
	this->currYRotation    = 0.0f;
	this->currYRotationSpd = 0.0f;
}

RocketProjectile::RocketProjectile(const RocketProjectile& copy) : Projectile(copy),
cannonBlock(copy.cannonBlock), currYRotationSpd(copy.currYRotationSpd),
currYRotation(copy.currYRotation) {
}

RocketProjectile::~RocketProjectile() {
}

BoundingLines RocketProjectile::BuildBoundingLines() const {
	// If the rocket is inside a cannon block it has no bounding lines...
	if (this->cannonBlock != NULL) {
		return BoundingLines();
	}

    static const float HALF_WIDTH_COEFF = 0.75f;
    static const float HALF_HEIGHT_FRACT_TO_HAT = 0.5f;
    static const float HEIGHT_COEFF = 0.9f;

	const Vector2D& UP_DIR    = this->GetVelocityDirection();
	const Vector2D& RIGHT_DIR = this->GetRightVectorDirection();

    Point2D topCenter    = this->GetPosition() + this->GetHalfHeight()*UP_DIR;
    Point2D bottomCenter = topCenter - HEIGHT_COEFF*this->GetHeight()*UP_DIR;

	Point2D topRight = this->GetPosition() + HALF_HEIGHT_FRACT_TO_HAT*this->GetHalfHeight()*UP_DIR + 
        HALF_WIDTH_COEFF*this->GetHalfWidth()*RIGHT_DIR;
	Point2D bottomRight = topRight - 0.75f*HEIGHT_COEFF*this->GetHeight()*UP_DIR;

	Point2D topLeft = this->GetPosition() + HALF_HEIGHT_FRACT_TO_HAT*this->GetHalfHeight()*UP_DIR - 
        HALF_WIDTH_COEFF*this->GetHalfWidth()*RIGHT_DIR;
	Point2D bottomLeft = topLeft - 0.75f*HEIGHT_COEFF*this->GetHeight()*UP_DIR;

	std::vector<Collision::LineSeg2D> sideBounds;
	sideBounds.reserve(3);
	sideBounds.push_back(Collision::LineSeg2D(topLeft, bottomLeft));
    sideBounds.push_back(Collision::LineSeg2D(topCenter, bottomCenter));
	sideBounds.push_back(Collision::LineSeg2D(topRight, bottomRight));
	std::vector<Vector2D> normBounds;
	normBounds.resize(3);

	return BoundingLines(sideBounds, normBounds);
}

/**
 * Executed every game tick, updates the velocities, 
 * position and rotation of the rocket projectile.
 */
void RocketProjectile::Tick(double seconds, const GameModel& model) {

	if (this->cannonBlock == NULL) {
        this->AugmentDirectionOnPaddleMagnet(seconds, model, 55.0f);

		// Update the rocket's velocity and position
        float dA = seconds * this->GetAccelerationMagnitude();
        this->velocityMag = std::min<float>(this->GetMaxVelocityMagnitude(), this->velocityMag + dA);
		float dV = seconds * this->velocityMag;
		this->SetPosition(this->GetPosition() + dV * this->velocityDir);

		// Update the rocket's rotation
        dA = seconds * this->GetRotationAccelerationMagnitude();
        this->currYRotationSpd = std::min<float>(this->GetMaxRotationVelocityMagnitude(), this->currYRotationSpd + dA);
		dV = static_cast<float>(seconds * this->currYRotationSpd);
		this->currYRotation += dV;
        this->currYRotation = fmod(this->currYRotation, 360.0f);
	}
}

bool RocketProjectile::ModifyLevelUpdate(double dT, GameModel& model) {
    if (this->cannonBlock != NULL) {

        // Start by doing a test to see if the cannon will actually fire...
        bool willCannonFire = this->cannonBlock->TestRotateAndFire(dT, false, this->velocityDir);
        if (willCannonFire) {
            // Set the remaining kinematics parameters of the projectile so that it is fired
            this->position = this->position + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->velocityDir;
            this->velocityMag = 0.25f * this->GetMaxVelocityMagnitude();

            // EVENT: Rocket has officially been fired from the cannon
            GameEventManager::Instance()->ActionProjectileFiredFromCannon(*this, *this->cannonBlock);
        }

        // 'Tick' the cannon to spin the rocket around inside it... eventually the function will say
        // it has fired the rocket
        // WARNING: The cannon block may no longer exist after this function call!
        std::pair<LevelPiece*, bool> cannonFiredPair = this->cannonBlock->RotateAndFire(dT, &model, false);
        if (willCannonFire) {
            assert(cannonFiredPair.second);
            this->cannonBlock = NULL;
        }
    }

    return false;
}

void RocketProjectile::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);

	// When loaded into a cannon block the rocket stops moving and centers on the block...
	this->velocityMag = 0.0f;
	this->position    = cannonBlock->GetCenter();
	this->cannonBlock = cannonBlock;

	// EVENT: The rocket is officially loaded into the cannon block
	GameEventManager::Instance()->ActionProjectileEnteredCannon(*this, *cannonBlock);
}