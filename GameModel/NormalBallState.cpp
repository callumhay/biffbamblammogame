/**
 * NormalBallState.cpp
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

#include "NormalBallState.h"
#include "Projectile.h"
#include "GameModel.h"
#include "BallLaserProjectile.h"

#include "../BlammoEngine/Algebra.h"

const double NormalBallState::OMNI_BULLET_WAIT_TIME_IN_SECS = 0.6;
const int NormalBallState::MAX_BULLETS_AT_A_TIME            = 3;

NormalBallState::NormalBallState(GameBall* ball) : BallState(ball), timeSinceLastOmniBullets(0.0) {
}

NormalBallState::~NormalBallState() {
}

// Clone this object and return it as a new object, the caller takes ownership
BallState* NormalBallState::Clone(GameBall* newBall) const {
	return new NormalBallState(newBall);
}

void NormalBallState::Tick(bool simulateMovement, double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel) {
	// Update based on any active paddle magnet
    this->gameBall->AugmentDirectionOnPaddleMagnet(seconds, *gameModel, 80.0f);
    
    // Update the position of the ball based on its velocity (and if applicable, acceleration)
	Vector2D currVelocity;
    bool crazyBallVelChangeAllowed = true;

    if (this->gameBall->IsBallBoosting()) {
        // Figure out how much loss of speed there is for the ball
        float currBoostDecceleration = seconds * GameBall::BOOST_DECCELERATION;
        this->gameBall->boostSpdDecreaseCounter += currBoostDecceleration;

        if (this->gameBall->boostSpdDecreaseCounter > GameBall::BOOST_TEMP_SPD_INCREASE_AMT) {
            float subtractAmt = this->gameBall->boostSpdDecreaseCounter - GameBall::BOOST_TEMP_SPD_INCREASE_AMT;
            this->gameBall->boostSpdDecreaseCounter = GameBall::BOOST_TEMP_SPD_INCREASE_AMT;
            currBoostDecceleration -= subtractAmt;
            assert(currBoostDecceleration > 0);
        }

        // IMPORTANT: Gather the current velocity BEFORE we change it based on the current impulse acceleration/deceleration
        currVelocity = this->gameBall->currSpeed * this->gameBall->currDir;
        
        this->gameBall->SetSpeed(std::max<float>(GameBall::GetSlowestAllowableSpeed(), this->gameBall->GetSpeed() - currBoostDecceleration));
        this->gameBall->gravitySpeed = this->gameBall->currSpeed;
        crazyBallVelChangeAllowed = false;
    }
    // The impulse applies a sudden acceleration that diminishes
    else if (this->gameBall->impulseSpdDecreaseCounter < this->gameBall->impulseAmount) {

        float currImpulseDeceleration = this->gameBall->impulseDeceleration * seconds;
        this->gameBall->impulseSpdDecreaseCounter += currImpulseDeceleration;

        if (this->gameBall->impulseSpdDecreaseCounter > this->gameBall->impulseAmount) {
            float subtractAmt = this->gameBall->impulseSpdDecreaseCounter - this->gameBall->impulseAmount;
            this->gameBall->impulseSpdDecreaseCounter = this->gameBall->impulseAmount;
            currImpulseDeceleration -= subtractAmt;
            assert(currImpulseDeceleration > 0);
        }

        // IMPORTANT: Gather the current velocity BEFORE we change it based on the current impulse acceleration/deceleration
        if (this->gameBall->HasBallType(GameBall::GraviBall) &&
            gameModel->GetPlayerPaddle()->GetAttachedBall() != this->gameBall) {
            this->ApplyGravityBallVelocityChange(seconds, gameModel, worldSpaceGravityDir, currVelocity);
        }
        else {
            currVelocity = this->gameBall->currSpeed * this->gameBall->currDir;
        }

        this->gameBall->SetSpeed(std::max<float>(GameBall::GetSlowestAllowableSpeed(), this->gameBall->GetSpeed() - currImpulseDeceleration));
        this->gameBall->gravitySpeed = this->gameBall->currSpeed;
   
        crazyBallVelChangeAllowed = false;
    }
    else {

        // When the ball has gravity augment the velocity to constantly be accelerating downwards,
        // in the case of when it's attached to the paddle, don't do this
	    if ((this->gameBall->GetBallType() & GameBall::GraviBall) == GameBall::GraviBall &&
            gameModel->GetPlayerPaddle()->GetAttachedBall() != this->gameBall) {

            this->ApplyGravityBallVelocityChange(seconds, gameModel, worldSpaceGravityDir, currVelocity);
	    }
	    else {
		    currVelocity = this->gameBall->currSpeed * this->gameBall->currDir;
		    this->gameBall->gravitySpeed = this->gameBall->currSpeed;
	    }
    }
    
    if (simulateMovement) {
    
        // Update the position of the ball based on the current velocity of it
	    this->gameBall->bounds.SetCenter(this->gameBall->bounds.Center() + (static_cast<float>(seconds) * currVelocity));
    
        // Crazy ball manipulates the direction and acceleration of the ball... 
        // This can be dangerous for the collision framework (which I admit is a hacky piece of awful):
        // We can't change the direction of the ball if movement is not allowed to be simulated (i.e., there was a
        // change to the position/velocity of the ball via collisions at some point during this frame/tick)
        // IMPORTANT: We do this AFTER changing the position of the ball based on the current velocity
        if (this->gameBall->HasBallType(GameBall::CrazyBall) && crazyBallVelChangeAllowed) {
            if (this->ApplyCrazyBallVelocityChange(seconds, currVelocity, gameModel)) {
                this->gameBall->SetLastThingCollidedWith(NULL);
            }
        }
    }

	// Update the rotation of the ball
	float dRotSpd = GameBall::MAX_ROATATION_SPEED * static_cast<float>(seconds);
	this->gameBall->rotationInDegs = this->gameBall->rotationInDegs + Vector3D(dRotSpd, dRotSpd, dRotSpd);

	// Change the size gradually (lerp based on some constant time) if need be...
	int diffFromNormalSize = static_cast<int>(this->gameBall->currSize) - static_cast<int>(GameBall::NormalSize);
	float targetScaleFactor = (GameBall::DEFAULT_BALL_RADIUS + diffFromNormalSize * GameBall::RADIUS_DIFF_PER_SIZE) / GameBall::DEFAULT_BALL_RADIUS;
	float scaleFactorDiff = targetScaleFactor - this->gameBall->currScaleFactor;
	if (scaleFactorDiff != 0.0f) {
		this->gameBall->SetDimensions(this->gameBall->currScaleFactor + ((scaleFactorDiff * seconds) / GameBall::SECONDS_TO_CHANGE_SIZE));
	}

	// Decrement the collision disabled timers if necessary
	if (this->gameBall->ballballCollisionsDisabledTimer >= 0.0) {
		this->gameBall->ballballCollisionsDisabledTimer -= seconds;
	}
	else {
		this->gameBall->ballballCollisionsDisabledTimer = 0.0;
	}

    // Omni Laser ball has a chance of shooting laser bullets in some random direction...
    if ((this->gameBall->GetBallType() & GameBall::OmniLaserBulletBall) == GameBall::OmniLaserBulletBall) {
        this->AttemptFireOfOmniBullets(seconds, gameModel);
    }
}

// Apply the crazy ball item's effect to the velocity of the ball by changing it somewhat randomly
// and strangely to make it difficult to track where the ball will go
bool NormalBallState::ApplyCrazyBallVelocityChange(double dT, Vector2D& currVelocity, GameModel* gameModel) {
	static double TIME_TRACKER = 0.0;
	static double NEXT_TIME    = 0.0;
	static const double WAIT_TIME_BETWEEN_COLLISIONS = 0.75;

	// If the ball has no velocity then just exit, we're not going to be able to change it...
    if (currVelocity.IsZero()) {
        // We reset the time tracker and make sure the next time is fairly large so that
        // if the ball is stuck to a sticky paddle or some such thing, that it doesn't go crazy (i.e., out of play)
        // immediately after the player lets the ball go
        TIME_TRACKER = 0.0;
        NEXT_TIME    = 1.5;
		return false;
	}

	TIME_TRACKER += dT;

	// After some interval of time, do a random velocity change
	if (TIME_TRACKER >= NEXT_TIME && this->gameBall->GetTimeSinceLastCollision() > WAIT_TIME_BETWEEN_COLLISIONS) {

		TIME_TRACKER = 0.0;
		NEXT_TIME = 1.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 2.0;

        // Special case: if the crazy ball is outside of the tight bounds around the level (other than the
        // bottom) then it won't try to go back in
        const GameLevel* currLevel = gameModel->GetCurrentLevel();
        assert(currLevel != NULL);

        float levelWidth = currLevel->GetLevelUnitWidth();
        float levelHeight = currLevel->GetLevelUnitHeight();
        const Point2D& ballCenter = this->gameBall->GetBounds().Center();

        Vector2D newVelocityDir;
        if (ballCenter[1] > levelHeight) {
            float randomRotation = Randomizer::GetInstance()->RandomNegativeOrPositive() * (45.0f * Randomizer::GetInstance()->RandomNumZeroToOne());
            newVelocityDir = Vector2D::Normalize(Vector2D::Rotate(randomRotation, Vector2D(0, 1)));
        }
        else if (ballCenter[0] < 0.0) {
            float randomRotation = Randomizer::GetInstance()->RandomNegativeOrPositive() * (45.0f * Randomizer::GetInstance()->RandomNumZeroToOne());
            newVelocityDir = Vector2D::Normalize(Vector2D::Rotate(randomRotation, Vector2D(-1, 0)));
        }
        else if (ballCenter[0] > levelWidth) {
            float randomRotation = Randomizer::GetInstance()->RandomNegativeOrPositive() * (45.0f * Randomizer::GetInstance()->RandomNumZeroToOne());
            newVelocityDir = Vector2D::Normalize(Vector2D::Rotate(randomRotation, Vector2D(1, 0)));
        }
        else {
		    float randomRotation  = Randomizer::GetInstance()->RandomNegativeOrPositive() * (45 + Randomizer::GetInstance()->RandomNumZeroToOne() * 115);
		    newVelocityDir = Vector2D::Normalize(Vector2D::Rotate(randomRotation, currVelocity));
        }

        this->gameBall->SetVelocity(this->gameBall->currSpeed, newVelocityDir);
        return true;
    }

    return false;
}

void NormalBallState::ApplyGravityBallVelocityChange(double dT, const GameModel* gameModel, 
                                                     const Vector2D& worldSpaceGravityDir, Vector2D& currVelocity) {

    // Keep track of the last gravity speed calculated based on the gravity pulling the ball down
    // The gravitySpeed variable is basically a mirror of currSpeed but it tracks 
    // the speed of the ball as it gets pulled down by gravity - currSpeed does not do this
    Vector2D currGravityVelocity = (this->gameBall->gravitySpeed * this->gameBall->currDir);
    currVelocity = currGravityVelocity + (dT * GameBall::GRAVITY_ACCELERATION * worldSpaceGravityDir);

    float projectedGravitySpeed  = Vector2D::Dot(currVelocity, worldSpaceGravityDir);
    Vector2D projectedGravityVec = projectedGravitySpeed * worldSpaceGravityDir;

    if (projectedGravitySpeed > this->gameBall->currSpeed) {
        currVelocity = currGravityVelocity - projectedGravityVec + (this->gameBall->currSpeed * worldSpaceGravityDir) + 
            (dT * GameBall::GRAVITY_ACCELERATION * worldSpaceGravityDir);
    }

    float newVelocityMag = Vector2D::Magnitude(currVelocity);
    if (newVelocityMag > 0.0) {
        this->gameBall->SetGravityVelocity(newVelocityMag, currVelocity / newVelocityMag);
    }
    else {
        assert(false);
    }

    // Deal with a special case where the ball gets thrown up by the paddle, but then comes right back down on the paddle --
    // we don't want the ball to go through the paddle in this case...
    if (this->gameBall->IsLastThingCollidedWith(gameModel->GetPlayerPaddle())) {
        this->gameBall->SetLastThingCollidedWith(NULL);
    }
}

// When the ball has the omni laser effect we try to fire lasers whenever possible outwards
// in all directions from the ball - this method does that.
void NormalBallState::AttemptFireOfOmniBullets(double dT, GameModel* gameModel) {
    // If we're still waiting to fire the bullets then don't try to fire any
    if (this->timeSinceLastOmniBullets < OMNI_BULLET_WAIT_TIME_IN_SECS) {
        this->timeSinceLastOmniBullets += dT;
        return;
    }

    // Attempt to fire some bullet(s)...
    int numBullets = Randomizer::GetInstance()->RandomUnsignedInt() % (MAX_BULLETS_AT_A_TIME+1);
    for (int i = 0; i < numBullets; i++) {
        // Determine a random angle to fire the bullet at and fire it into the level from the ball
        float randomAngle = (Randomizer::GetInstance()->RandomUnsignedInt() % 360);
        Vector2D randomVector(0, 1);
        randomVector.Rotate(randomAngle);
        randomVector.Normalize();
        
        // Calculate the size of the bullet based on the size of the ball...
        float projectileWidth  = (this->gameBall->GetBounds().Radius() * 0.25f) + 
            (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.75f * this->gameBall->GetBounds().Radius());
        float projectileHeight = 1.75f * projectileWidth; 

		Projectile* newProjectile = new BallLaserProjectile(
            this->gameBall->GetCenterPosition2D() + (this->gameBall->GetBounds().Radius() + 
            projectileHeight * 0.5f) * randomVector);
		newProjectile->SetWidth(projectileWidth);
		newProjectile->SetHeight(projectileHeight);

        Vector2D newVelocity = newProjectile->GetVelocityMagnitude() * randomVector +
                               this->gameBall->GetVelocity();
        newProjectile->SetVelocity(newVelocity);

		gameModel->AddProjectile(newProjectile);
    }

    this->timeSinceLastOmniBullets = 0.0;
}