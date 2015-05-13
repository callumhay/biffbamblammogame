/**
 * PlayerPaddle.cpp
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

#include "PlayerPaddle.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameEventManager.h"
#include "GameModelConstants.h"
#include "PaddleLaserProjectile.h"
#include "PaddleMineProjectile.h"
#include "PaddleRocketProjectile.h"
#include "PaddleRemoteControlRocketProjectile.h"
#include "FireGlobProjectile.h"
#include "PaddleFlameBlasterProjectile.h"
#include "PaddleIceBlasterProjectile.h"
#include "PaddleLaserBeam.h"
#include "BossBodyPart.h"
#include "OneWayBlock.h"

// Default values for the size of the paddle
const float PlayerPaddle::PADDLE_WIDTH_TOTAL = 3.5f;
const float PlayerPaddle::PADDLE_WIDTH_FLAT_TOP = 2.0f;
const float PlayerPaddle::PADDLE_WIDTH_ANGLED_SIDE = (PADDLE_WIDTH_TOTAL - PADDLE_WIDTH_FLAT_TOP) / 2.0f;
const float PlayerPaddle::PADDLE_HEIGHT_TOTAL = 0.80f;
const float PlayerPaddle::PADDLE_DEPTH_TOTAL = 1.0f;
const float PlayerPaddle::PADDLE_HALF_WIDTH = PADDLE_WIDTH_TOTAL / 2.0f;
const float PlayerPaddle::PADDLE_HALF_HEIGHT = PADDLE_HEIGHT_TOTAL / 2.0f;
const float PlayerPaddle::PADDLE_HALF_DEPTH = PADDLE_DEPTH_TOTAL / 2.0f;

const Vector2D PlayerPaddle::DEFAULT_PADDLE_UP_VECTOR(0, 1);
const Vector2D PlayerPaddle::DEFAULT_PADDLE_RIGHT_VECTOR(1, 0);

// The difference in width per size change of the paddle
const float PlayerPaddle::WIDTH_DIFF_PER_SIZE = 0.7f;

// Number of seconds it takes for the paddle to change between sizes
// (bigger is slower, smaller is faster)
const float PlayerPaddle::SECONDS_TO_CHANGE_SIZE = 0.5f;

// Default speed of the paddle (units/sec)
const float PlayerPaddle::DEFAULT_MAX_SPEED = 27.0f;
// Default acceleration/decceleration of the paddle (units/sec^2)
const float PlayerPaddle::DEFAULT_ACCELERATION  = 148.0f;
const float PlayerPaddle::DEFAULT_DECCELERATION = -170.0f;

// Speed amount to diminish from the max speed when the paddle is poisoned
const float PlayerPaddle::POISON_SPEED_DIMINISH = PlayerPaddle::DEFAULT_MAX_SPEED / 6.0f;

// The coefficient angle change of the ball when deflected by a moving paddle
const int PlayerPaddle::MAX_DEFLECTION_DEGREE_ANGLE = 18.0f;

// Delay between shots of the laser-vent
const double PlayerPaddle::PADDLE_LASER_BULLET_DELAY = 0.3;
const double PlayerPaddle::PADDLE_FLAME_BLAST_DELAY  = 0.5;
const double PlayerPaddle::PADDLE_ICE_BLAST_DELAY    = 0.5;
const double PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY  = 0.75;

// The default amount of damage the the paddle shield does to a block, when colliding with the block, per second
const int PlayerPaddle::DEFAULT_SHIELD_DMG_PER_SECOND = 90;

const double PlayerPaddle::PADDLE_FROZEN_TIME_IN_SECS       = 1.7;
const double PlayerPaddle::PADDLE_ON_FIRE_TIME_IN_SECS      = 2.0;
const double PlayerPaddle::PADDLE_ELECTROCUTED_TIME_IN_SECS = 1.5;

bool PlayerPaddle::paddleBallReleaseTimerEnabled = true;
bool PlayerPaddle::paddleBallReleaseEnabled      = true;

const float PlayerPaddle::DEFAULT_PADDLE_SCALE = 1.0f;

float PlayerPaddle::NormalSizeScale = PlayerPaddle::DEFAULT_PADDLE_SCALE;

PlayerPaddle::PlayerPaddle() : 
centerPos(0.0f, 0.0f), minXBound(0.0f), maxXBound(0.0f), currSpeed(0.0f), lastDirection(0.0f), 
maxSpeed(PlayerPaddle::DEFAULT_MAX_SPEED), acceleration(PlayerPaddle::DEFAULT_ACCELERATION), 
decceleration(PlayerPaddle::DEFAULT_DECCELERATION), timeSinceLastLaserBlast(PADDLE_LASER_BULLET_DELAY), 
timeSinceLastBlastShot(PADDLE_FLAME_BLAST_DELAY + PADDLE_ICE_BLAST_DELAY), timeSinceLastMineLaunch(PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY),
moveButtonDown(false), hitWall(false), currType(NormalPaddle), currSize(PlayerPaddle::NormalSize), currSpecialStatus(PlayerPaddle::NoStatus),
attachedBall(NULL), isPaddleCamActive(false), colour(1,1,1,1), isFiringBeam(false), impulse(0.0f), reorientZRotInRads(0.0f),
impulseDeceleration(0.0f), impulseSpdDecreaseCounter(0.0f), lastEntityThatHurtHitPaddle(NULL), lastThingCollidedWith(NULL),
levelBoundsCheckingOn(true), startingXPos(0.0), defaultYPos(0.0), frozenCountdown(0.0), onFireCountdown(0.0), electrocutedCountdown(0.0) {
	this->ResetPaddle();
}

PlayerPaddle::~PlayerPaddle() {
}

// Reset the dimensions and position of this paddle (e.g., after death, start of a level).
void PlayerPaddle::ResetPaddle() {
	this->timeSinceLastLaserBlast = 0.0;
    this->timeSinceLastBlastShot = 0.0;
    this->timeSinceLastMineLaunch = 0.0;
	this->laserBeamTimer = 0.0;
    this->frozenCountdown = 0.0;
    this->onFireCountdown = 0.0;
    this->electrocutedCountdown = 0.0;
    this->reorientZRotInRads = 0.0f;
	this->currSize = PlayerPaddle::NormalSize;
    this->SetDefaultYPosition(PlayerPaddle::PADDLE_HALF_HEIGHT);
	this->centerPos   = this->GetDefaultCenterPosition();
	this->currType = PlayerPaddle::NormalPaddle;
    this->ClearSpecialStatus();
	this->colour = ColourRGBA(1, 1, 1, 1);
	this->isFiringBeam = false;
	this->hitWall = false;
	this->lastDirection = 0.0f;
	this->currSpeed = 0.0f;
	this->maxSpeed      = PlayerPaddle::DEFAULT_MAX_SPEED;
	this->acceleration  = PlayerPaddle::DEFAULT_ACCELERATION; 
	this->decceleration = PlayerPaddle::DEFAULT_DECCELERATION;
	this->impulse = 0.0f;
    this->impulseDeceleration = 0.0f;
    this->impulseSpdDecreaseCounter = 0.0f;
    this->lastEntityThatHurtHitPaddle = NULL;
    this->lastThingCollidedWith = NULL;
    this->levelBoundsCheckingOn = true;
    this->attachedProjectiles.clear();

	this->SetupAnimations();

	// This will set the dimensions the the default and regenerate the bounds of the paddle
	this->SetDimensions(PlayerPaddle::NormalSize);
}

void PlayerPaddle::SetDefaultYPosition(float yPos) {
    this->defaultYPos = yPos;
    this->underPaddleYBound = yPos - (cosf(this->reorientZRotInRads) * GameLevel::MIN_Y_BOUND_BUFFER_SPACE_FOR_PADDLE);
}

void PlayerPaddle::SetPaddleFlipped(bool flipPaddle) {
    float yBoundDiff = fabs(this->underPaddleYBound - this->defaultYPos);
    if (flipPaddle) {
        this->underPaddleYBound = this->defaultYPos + yBoundDiff;
        this->reorientZRotInRads = M_PI;
    }
    else {
        this->underPaddleYBound = this->defaultYPos - yBoundDiff;
        this->reorientZRotInRads = 0.0;
    }

    // EVENT: Paddle just flipped
    GameEventManager::Instance()->ActionPaddleFlipped(*this, flipPaddle);
}

/**
 * Private helper/initializer function for the paddle animations.
 */
void PlayerPaddle::SetupAnimations() {
	// The colour animation is used to change the colour or fade the paddle during 
	// certain animation sequences
	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);
	
	// The beam force downwards animation forces the paddle to move off the level/downwards
	// the amount of units equal to its current interpolated value when the paddle is firing its
	// laser beam power-up
	this->moveDownAnimation.ClearLerp();
	this->moveDownAnimation.SetInterpolantValue(0.0f);
	this->moveDownAnimation.SetRepeat(false);

	// Set the rotation animation - this sets the paddle at a certain rotation about the z-axis
	this->rotAngleZAnimation.ClearLerp();
	this->rotAngleZAnimation.SetInterpolantValue(0.0f);
	this->rotAngleZAnimation.SetRepeat(false);

    // Set the hurt alpha animation to just be 1.0 for now
    this->hurtAlphaAnim.ClearLerp();
    this->hurtAlphaAnim.SetInterpolantValue(1.0f);
    this->hurtAlphaAnim.SetRepeat(false);
}

// Regenerate the bounding lines of the player paddle so that they adhere to the size, position and rotation
// of the paddle as it currently lies.
void PlayerPaddle::RegenerateBounds() {

    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D boundingNorms[MAX_NUM_LINES];
    float bottomY = -this->currHalfHeight + EPSILON;

	if (this->isPaddleCamActive) {
		// When the paddle camera is active we compress the collision boundaries down to the bottom of the paddle
		// this help make for a better game experience since the collision seems more natural when it's on the camera's view plane
        float topY = -0.5f*this->currHalfHeight;
        
		// 'Top' (flat) boundary
		boundingLines[0] = Collision::LineSeg2D(Point2D(this->currHalfWidthFlat, topY), Point2D(-this->currHalfWidthFlat, topY));
		boundingNorms[0] = Vector2D(0, 1);

		// 'Side' (oblique) boundaries
		boundingLines[1] = Collision::LineSeg2D(Point2D(this->currHalfWidthFlat, topY), Point2D(this->currHalfWidthTotal, bottomY));
		boundingNorms[1] = Vector2D(1, 1) / SQRT_2;
		boundingLines[2] = Collision::LineSeg2D(Point2D(-this->currHalfWidthFlat, topY), Point2D(-this->currHalfWidthTotal, bottomY));
		boundingNorms[2] = Vector2D(-1, 1) / SQRT_2;

        // Bottom boundary is even further down now...
        boundingLines[3] = Collision::LineSeg2D(Point2D(this->currHalfWidthTotal, bottomY), Point2D(-this->currHalfWidthTotal, bottomY));
        boundingNorms[3] = Vector2D(0, -1);
	}
	else {
        float topY = this->currHalfHeight;

		// Top boundary
		boundingLines[0] = Collision::LineSeg2D(Point2D(this->currHalfWidthFlat, topY), Point2D(-this->currHalfWidthFlat, topY));
		boundingNorms[0] = Vector2D(0, 1);

		// Side boundaries
		boundingLines[1] = Collision::LineSeg2D(Point2D(this->currHalfWidthFlat, topY), Point2D(this->currHalfWidthTotal, bottomY));
		boundingNorms[1] = Vector2D(1, 1) / SQRT_2;
		boundingLines[2] = Collision::LineSeg2D(Point2D(-this->currHalfWidthFlat, topY), Point2D(-this->currHalfWidthTotal, bottomY));
		boundingNorms[2] = Vector2D(-1, 1) / SQRT_2;

		// Bottom boundary
		boundingLines[3] = Collision::LineSeg2D(Point2D(this->currHalfWidthTotal, bottomY), Point2D(-this->currHalfWidthTotal, bottomY));
		boundingNorms[3] = Vector2D(0, -1);
	}

	this->bounds = BoundingLines(MAX_NUM_LINES, boundingLines, boundingNorms);

	// Rotate the bounds (in the case where the paddle rotation has changed)
	this->bounds.RotateLinesAndNormals(this->GetZRotation(), Point2D(0,0));

	// Translate the bounds into game space
	this->bounds.TranslateBounds(Vector2D(this->GetCenterPosition()[0], this->GetCenterPosition()[1]));
}

/**
 * Set the dimensions of the paddle based on a scale factor given.
 * This will change the scale factor and bounds of the paddle.
 */
void PlayerPaddle::SetDimensions(float newScaleFactor) {
	this->currScaleFactor = newScaleFactor;
	assert(this->currScaleFactor > 0.0f);

	// Update all of the dimension values for the new scale factor
	this->currHalfHeight     = this->currScaleFactor * PlayerPaddle::PADDLE_HALF_HEIGHT;
	this->currHalfWidthTotal = this->currScaleFactor * PlayerPaddle::PADDLE_HALF_WIDTH;
	this->currHalfWidthFlat	 = this->currScaleFactor * PlayerPaddle::PADDLE_WIDTH_FLAT_TOP * 0.5f;
	this->currHalfDepthTotal = this->currScaleFactor * PlayerPaddle::PADDLE_HALF_DEPTH;

	// The momentum of the paddle will change as well - we do a physics hack here where the acceleration/decceleration
	// are effected directly by the inverse scale factor of the paddle
	if (this->currScaleFactor != PlayerPaddle::NormalSizeScale) {
		static const float INTENSIFIER = 1.075f;
	
	    float invCurrScaleFactor = 1.0f / (INTENSIFIER * this->currScaleFactor);
	    this->acceleration  = invCurrScaleFactor * PlayerPaddle::DEFAULT_ACCELERATION;
	    this->decceleration = invCurrScaleFactor * PlayerPaddle::DEFAULT_DECCELERATION;
    }
    else {
    	this->acceleration  = PlayerPaddle::DEFAULT_ACCELERATION;
	    this->decceleration = PlayerPaddle::DEFAULT_DECCELERATION;
    }
    
	this->RegenerateBounds();
}

/**
 * Sets what the 'future' paddle size will be - this function does not immediately
 * change the size of the paddle but will cause the paddle to 'grow'/'shrink' to that
 * size as the tick function is called.
 */
void PlayerPaddle::SetPaddleSize(PlayerPaddle::PaddleSize size) {
	// If the current size is already the size being set then just exit
	if (this->currSize == size) {
		return;
	}
	this->currSize = size;
}

/**
 * Private helper function that shoots the ball attached to the player paddle.
 */
void PlayerPaddle::FireAttachedBall() {
    assert(this->HasBallAttached());

	// Set the ball's new trajectory in order for it to leave the paddle
    Vector2D ballReleaseDir = this->GetUpVector();

	// Get the paddle's velocity
    Vector2D avgPaddleVelDir = this->GetVelocity();

	// Check for the sticky paddle... trajectory will be based on the normal of the surface that the ball is stuck to
	if (this->HasPaddleType(PlayerPaddle::StickyPaddle)) {
		
        std::vector<int> indices = this->bounds.ClosestCollisionIndices(this->attachedBall->GetCenterPosition2D(), 0.01f);
		assert(indices.size() > 0);
		
        ballReleaseDir = 2*this->bounds.GetNormal(indices[0]);
	}

    // Check to see if the paddle has crashed into a wall or is touching it, in this case we reduce
    // the paddle velocity to zero
    float absPaddleSpd = fabs(avgPaddleVelDir[0]);
    if (this->centerPos[0] >= (this->maxXBound - this->GetHalfWidthTotal()) ||
        this->centerPos[0] <= (this->minXBound + this->GetHalfWidthTotal()) ||
        !this->GetIsMoveButtonDown()) {

        avgPaddleVelDir[0] = 0.0f;
        absPaddleSpd = 0.0f;
    }

    // Modify the ball's release velocity to reflect the paddle's movement
    Vector2D newReleaseDir = Vector2D::Normalize(ballReleaseDir + 0.175f * avgPaddleVelDir);
    ballReleaseDir.Normalize();
    if (Vector2D::Dot(newReleaseDir, ballReleaseDir) < 0.0) {
        newReleaseDir = Vector2D::Rotate(NumberFuncs::SignOf(Vector3D::cross(ballReleaseDir, avgPaddleVelDir)[2]) * 90, ballReleaseDir);
    }

	// Set the ball velocity (trajectory it will leave the paddle on)
	this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), newReleaseDir);
    
    // Add a brief impulse to the ball's velocity to give the launch a more visceral feeling
    float ballImpulse = 0.575f * absPaddleSpd;
    this->attachedBall->ApplyImpulseForce(ballImpulse, ballImpulse); 

	// Re-enable the ball's collisions
	this->attachedBall->SetBallBallCollisionsEnabled();
	this->attachedBall->SetBallPaddleCollisionsEnabled();

    // The last thing the ball collided with is this paddle...
    this->attachedBall->SetLastThingCollidedWith(this);

	// EVENT: Ball Shot
	GameEventManager::Instance()->ActionBallShot(*this->attachedBall);

	// Remove the ball from the paddle
	this->attachedBall = NULL;
}

void PlayerPaddle::FireAttachedProjectile() {
    assert(this->HasProjectileAttached());

	// Set the projectile's new trajectory in order for it to leave the paddle
    Vector2D releaseDir = this->GetUpVector();
	// Get the paddle's velocity
	Vector2D avgPaddleVel = this->GetVelocity();

    Projectile* projectileToFire = this->attachedProjectiles.front();
    this->attachedProjectiles.pop_front();
    assert(projectileToFire != NULL);

	std::vector<int> indices = this->bounds.ClosestCollisionIndices(projectileToFire->GetPosition(), 0.01f);
	assert(indices.size() > 0);
	const Vector2D& launchNormal = this->bounds.GetNormal(indices[0]);

	// Move the ball so it's no longer colliding...
    Collision::Circle2D projectileCircleBounds = projectileToFire->BuildBoundingLines().GenerateCircleFromLines();
	projectileToFire->SetPosition(projectileToFire->GetPosition() + projectileCircleBounds.Radius() * launchNormal);
	projectileToFire->SetVelocity(launchNormal, PaddleMineProjectile::MAX_VELOCITY);
    projectileToFire->DetachFromPaddle();
}

/**
 * Private helper function used to solve the issue where a ball that is attached
 * to the paddle when the paddle boundaries change doesn't get moved to the new boundaries.
 * e.g., when in paddle cam mode and then changing back while the sticky paddle is active.
 */
void PlayerPaddle::MoveAttachedObjectsToNewBounds() {
	if (this->HasBallAttached()) {
	    // Make sure the attached ball is sitting on the bounding lines and not inside the paddle...
	    // Move the ball along the up vector until its no longer hitting...
        assert(this->attachedBall != NULL);
	    Point2D closestPtOnBounds = Collision::ClosestPoint(this->attachedBall->GetCenterPosition2D(), this->bounds.GetLine(0));
	    this->attachedBall->SetCenterPosition(closestPtOnBounds + this->attachedBall->GetBounds().Radius() * this->GetUpVector());
    }

    if (this->HasProjectileAttached()) {
        Point2D closestPtOnBounds;

        for (std::list<Projectile*>::iterator iter = this->attachedProjectiles.begin();
             iter != this->attachedProjectiles.end(); ++iter) {

            Projectile* attachedProjectile = *iter;
            assert(attachedProjectile != NULL);
            
            closestPtOnBounds = Collision::ClosestPoint(attachedProjectile->GetPosition(), this->bounds.GetLine(0));
            Collision::Circle2D bounds = attachedProjectile->BuildBoundingLines().GenerateCircleFromLines();
	        attachedProjectile->SetPosition(closestPtOnBounds + bounds.Radius() * this->GetUpVector());
        }
    }
}

void PlayerPaddle::Tick(double seconds, bool pausePaddleMovement, GameModel& gameModel) {

	Point2D startingCenterPos = this->centerPos;
	Point2D defaultCenterPos  = this->GetDefaultCenterPosition();
	this->centerPos[1] = defaultCenterPos[1] - (cosf(this->reorientZRotInRads) * this->moveDownAnimation.GetInterpolantValue());

	// Increment seconds since the various types of previous weapon shots
	// This makes sure the user can't consecutively fire various weapons like crazy
	this->timeSinceLastLaserBlast += seconds;
    this->timeSinceLastBlastShot  += seconds;
    this->timeSinceLastMineLaunch += seconds;

    // If the paddle is on fire then tick down the on-fire countdown
    if (this->HasSpecialStatus(PlayerPaddle::OnFireStatus)) {
        if (this->onFireCountdown <= 0.0) {
            this->RemoveSpecialStatus(PlayerPaddle::OnFireStatus);
        }
        else {
            this->onFireCountdown -= seconds;
        }
    }

    // If the paddle is frozen then it can't be moved by the player until it unfreezes
    if (this->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus)) {
        
        if (this->frozenCountdown <= 0.0) {
            this->RemoveSpecialStatus(PlayerPaddle::FrozenInIceStatus);
            this->RecoverFromPausedPaddle();
        }
        else {
            this->frozenCountdown -= seconds;
            this->RegenerateBounds();
            return;
        }
    }

    // If the paddle is electrocuted, then it can't be moved by the player until it's done sizzlin'
    if (this->HasSpecialStatus(PlayerPaddle::ElectrocutedStatus)) {
        if (this->electrocutedCountdown <= 0.0) {
            this->RemoveSpecialStatus(PlayerPaddle::ElectrocutedStatus);
            this->RecoverFromPausedPaddle();
        }
        else {
            this->electrocutedCountdown -= seconds;
            this->RegenerateBounds();
            return;
        }
    }

    this->moveDownAnimation.Tick(seconds);

    // If the paddle is rotating (might have been hit or knocked around) we want to animate that rotation
    float startingZAnimRotation = this->rotAngleZAnimation.GetInterpolantValue();
    this->rotAngleZAnimation.Tick(seconds);

    // Tick the hurt animation
    this->hurtAlphaAnim.Tick(seconds);

	// Figure out what the current acceleration is based on whether the player
	// is currently telling the paddle to move or not
	float distanceTravelled;
	if (pausePaddleMovement) {
		distanceTravelled = 0.0f;
	}
	else {
		float currAcceleration;
		if (this->moveButtonDown) {
			currAcceleration = this->acceleration;
		}
		else {
			currAcceleration = this->decceleration;
		}
		this->currSpeed = std::max<float>(0.0f, std::min<float>(this->maxSpeed, this->currSpeed + currAcceleration * seconds));
		
        // If the poison paddle is active then the speed is diminished...
        float tempSpd = this->currSpeed;
        if (this->HasPaddleType(PlayerPaddle::PoisonPaddle)) {
            tempSpd = std::max<float>(0.0f, tempSpd - PlayerPaddle::POISON_SPEED_DIMINISH);
        }
        
        // Apply impulse (if any)...
        tempSpd += (this->impulse - this->impulseSpdDecreaseCounter);
        this->impulseSpdDecreaseCounter += this->impulseDeceleration * seconds;
        if (fabs(this->impulseSpdDecreaseCounter) > fabs(this->impulse)) {
            this->impulse = 0.0f;
            this->impulseSpdDecreaseCounter = 0.0f;
        }

        distanceTravelled = tempSpd * this->lastDirection * seconds;
	}

	float newCenterX = this->centerPos[0] + distanceTravelled;
	float minNewXPos = newCenterX - this->currHalfWidthTotal;
	float maxNewXPos = newCenterX + this->currHalfWidthTotal;
	Point2D oldPaddleCenter = this->centerPos;
	
	float halfWidthTotalWithAttachedMin = this->currHalfWidthTotal;
	float halfWidthTotalWithAttachedMax = this->currHalfWidthTotal;

	// If a ball or projectile is attached, it could affect the min and max x boundaries of the paddle so 
	// adjust the boundaries based on the position of the object(s) on the paddle
    if (this->HasBallAttached()) {
        
		float ballX = this->attachedBall->GetBounds().Center()[0] + distanceTravelled;
		float slightlyBiggerRadius = this->attachedBall->GetBounds().Radius();
		float ballMinX = ballX - slightlyBiggerRadius;
		float ballMaxX = ballX + slightlyBiggerRadius;
		
		// If it's the case that the ball increases the size of the paddle on the -x dir
		// then accommodate this, same goes for +x direction
		if (ballMinX < minNewXPos) {
			halfWidthTotalWithAttachedMin += (minNewXPos - ballMinX);
			minNewXPos = ballMinX;
		}
		else if (ballMaxX > maxNewXPos) {
			halfWidthTotalWithAttachedMax += (ballMaxX - maxNewXPos);
			maxNewXPos = ballMaxX;
		}
	}

    if (this->HasProjectileAttached()) {
        Projectile* attachedProjectile;
        float projectileX, slightlyBiggerHalfWidth, projectileMinX, projectileMaxX;

        for (std::list<Projectile*>::iterator iter = this->attachedProjectiles.begin();
             iter != this->attachedProjectiles.end(); ++iter) {
        
            attachedProjectile = *iter;
            assert(attachedProjectile != NULL);
            
            Collision::Circle2D projectileCircleBounds = attachedProjectile->BuildBoundingLines().GenerateCircleFromLines();
            
            projectileX = attachedProjectile->GetPosition()[0] + distanceTravelled;
            slightlyBiggerHalfWidth = projectileCircleBounds.Radius()/2.0f;
            projectileMinX = projectileX - slightlyBiggerHalfWidth;
            projectileMaxX = projectileX + slightlyBiggerHalfWidth;

		    // If it's the case that the projectile increases the size of the paddle on the -x dir
		    // then accommodate this, same goes for +x direction
		    if (projectileMinX < minNewXPos) {
			    halfWidthTotalWithAttachedMin += (minNewXPos - projectileMinX);
			    minNewXPos = projectileMinX;
		    }
		    else if (projectileMaxX > maxNewXPos) {
			    halfWidthTotalWithAttachedMax += (projectileMaxX - maxNewXPos);
			    maxNewXPos = projectileMaxX;
		    }

        }
    }
    if (this->levelBoundsCheckingOn) {
        
        Collision::AABB2D paddleAABB = this->bounds.GenerateAABBFromLines();
        if (this->reorientZRotInRads != 0.0f) {

            if (paddleAABB.GetMax()[1] > this->underPaddleYBound) {

                this->centerPos[1] = this->centerPos[1] - paddleAABB.GetMax()[1] - this->underPaddleYBound;
                float maxMoveDownAmt = fabs(this->defaultYPos - this->centerPos[1]);

                // We have to make sure that any movement animation also accommodates the lower Y boundary...
                std::vector<float>& interpolationValues = this->moveDownAnimation.GetEditableInterpolationValues();
                for (std::vector<float>::iterator iter = interpolationValues.begin(); iter != interpolationValues.end(); ++iter) {
                    if (*iter > maxMoveDownAmt) {
                        *iter = maxMoveDownAmt;
                    }
                }
            }
        }
        else {
            if (paddleAABB.GetMin()[1] < this->underPaddleYBound) {

                this->centerPos[1] = this->centerPos[1] + this->underPaddleYBound - paddleAABB.GetMin()[1];
                float maxMoveDownAmt = fabs(this->defaultYPos - this->centerPos[1]);

                // We have to make sure that any movement animation also accommodates the lower Y boundary...
                std::vector<float>& interpolationValues = this->moveDownAnimation.GetEditableInterpolationValues();
                for (std::vector<float>::iterator iter = interpolationValues.begin(); iter != interpolationValues.end(); ++iter) {
                    if (*iter > maxMoveDownAmt) {
                        *iter = maxMoveDownAmt;
                    }
                }
            }
        }

	    if (minNewXPos <= this->minXBound) {
		    // The paddle bumped into the left wall
		    this->centerPos[0] = this->minXBound + halfWidthTotalWithAttachedMin;
    		
		    // Only do the event if the paddle hit the wall for the 'first' time and is on the normal plane of movement
		    if (!this->hitWall && this->moveDownAnimation.GetInterpolantValue() == 0.0) {
                
                if (gameModel.GetCurrentStateType() == GameState::BallInPlayStateType) {
                    
                    LevelPiece* piece = gameModel.GetCurrentLevel()->GetMinXPaddleBoundPiece(this->defaultYPos);
                    LevelPiece* resultingPiece = piece->CollisionOccurred(&gameModel, *this);

                    if (resultingPiece != piece) {
                        gameModel.PerformLevelCompletionChecks();
                    }
                }

			    // EVENT: paddle hit left wall for first time
			    GameEventManager::Instance()->ActionPaddleHitWall(*this, this->centerPos + Vector2D(-halfWidthTotalWithAttachedMin, 0));
		    }

		    this->hitWall = true;
	    }
	    else if (maxNewXPos >= this->maxXBound) {
		    // The paddle bumped into the right wall
		    this->centerPos[0] = this->maxXBound - halfWidthTotalWithAttachedMax;
    		
		    // Only do the event if the paddle hit the wall for the 'first' time and is on the normal plane of movement
		    if (!this->hitWall && this->moveDownAnimation.GetInterpolantValue() == 0.0) {

                if (gameModel.GetCurrentStateType() == GameState::BallInPlayStateType) {
                    LevelPiece* piece = gameModel.GetCurrentLevel()->GetMaxXPaddleBoundPiece(this->defaultYPos);
                    
                    // Make sure the piece is actually being collided with
                    if (fabs(this->maxXBound - gameModel.GetCurrentLevel()->GetPaddleMaxXBound(this->defaultYPos)) < EPSILON) {
                        LevelPiece* resultingPiece = piece->CollisionOccurred(&gameModel, *this);

                        if (resultingPiece != piece) {
                            gameModel.PerformLevelCompletionChecks();
                        }
                    }
                }

			    // EVENT: paddle hit right wall for first time
			    GameEventManager::Instance()->ActionPaddleHitWall(*this, this->centerPos + Vector2D(halfWidthTotalWithAttachedMax, 0));
		    }
		    this->hitWall = true;
        }
	    else {
		    this->centerPos[0] = newCenterX;
		    this->hitWall = false;
            this->SetLastPieceCollidedWith(NULL);
	    }
	}
	else {
		this->centerPos[0] = newCenterX;
		this->hitWall = false;
	}

	// Change the size gradually (lerp based on some constant time) if need be...
    float targetScaleFactor = PlayerPaddle::CalculateTargetScaleFactor(this->currSize);
	float scaleFactorDiff = targetScaleFactor - this->currScaleFactor;
	if (scaleFactorDiff != 0.0f) {
		this->SetDimensions(this->currScaleFactor + ((scaleFactorDiff * seconds) / SECONDS_TO_CHANGE_SIZE));
	}

	// If there is a ball attached then we need to move it around with the paddle
    if (this->HasBallAttached()) {
		// We need to figure out what the distance is from the paddle center-top to the ball center
		Vector2D paddleTopCenterToBallCenter = this->attachedBall->GetBounds().Center() - (startingCenterPos + Vector2D(0, this->GetHalfHeight()));
		// Maintain the vector obtained above in the new position of the paddle
		this->attachedBall->SetCenterPosition(this->centerPos + Vector2D(0, this->GetHalfHeight()) + paddleTopCenterToBallCenter);
	}
    if (this->HasProjectileAttached()) {
        Projectile* attachedProjectile;
        Vector2D paddleTopCenterToProjectileCenter;
        Point2D topOfPaddle = (startingCenterPos + Vector2D(0, this->GetHalfHeight()));

        for (std::list<Projectile*>::iterator iter = this->attachedProjectiles.begin();
             iter != this->attachedProjectiles.end(); ++iter) {
        
            attachedProjectile = *iter;
            paddleTopCenterToProjectileCenter = attachedProjectile->GetPosition() - topOfPaddle;
            attachedProjectile->SetPosition(this->centerPos + Vector2D(0, this->GetHalfHeight()) + paddleTopCenterToProjectileCenter);
        }
    }

	// Check to see if the center position changed or its rotation changed, if so then regenerate the bounds
	if (startingCenterPos != this->centerPos || startingZAnimRotation != this->rotAngleZAnimation.GetInterpolantValue()) {
		this->RegenerateBounds();
	}
}

/**
 * Increases the paddle size if it can.
 * Returns: true if there was an increase in size, false otherwise.
 */
bool PlayerPaddle::IncreasePaddleSize() {
	if (this->currSize == BiggestSize) { 
		return false; 
	}
	
	this->SetPaddleSize(static_cast<PaddleSize>(this->currSize + 1));

    // Paddle getting bigger shatters the ice it might be frozen in
    this->RemoveSpecialStatus(PlayerPaddle::FrozenInIceStatus);
	
    return true;
}

/**
 * Decreases the paddle size if it can.
 * Returns: true if there was an decrease in size, false otherwise.
 */
bool PlayerPaddle::DecreasePaddleSize() {
	if (this->currSize == SmallestSize) { 
		return false; 
	}

	this->SetPaddleSize(static_cast<PaddleSize>(this->currSize - 1));
	return true;
}

void PlayerPaddle::AddPaddleType(const PaddleType& type) {
    this->currType = this->currType | type;

    switch (type) {
        case PlayerPaddle::InvisiPaddle: {
            if (this->HasPaddleType(RocketPaddle | RemoteControlRocketPaddle)) {
                this->AddSpecialStatus(InvisibleRocketStatus);
            }
            else if (this->HasPaddleType(PlayerPaddle::MineLauncherPaddle)) {
                this->AddSpecialStatus(InvisibleMineStatus);
            }
            break;
        }

        default:
            break;
    }

}

void PlayerPaddle::RemovePaddleType(const PaddleType& type) {
	this->currType = this->currType & ~type;

    switch (type) {
        case PlayerPaddle::InvisiPaddle: {
            if (this->HasPaddleType(RocketPaddle | RemoteControlRocketPaddle)) {
                this->RemoveSpecialStatus(InvisibleRocketStatus);
            }
            else if (this->HasPaddleType(PlayerPaddle::MineLauncherPaddle)) {
                this->RemoveSpecialStatus(InvisibleMineStatus);
            }
            break;
        }


        default:
            break;
    }
}

void PlayerPaddle::AddSpecialStatus(int32_t status) { 
    int32_t statusBefore = this->currSpecialStatus;
    this->currSpecialStatus = this->currSpecialStatus | status;

    // Check for special case where the frozen paddle is canceled by fire or vice-versa
    bool frozenChange = (statusBefore & PlayerPaddle::FrozenInIceStatus) == 0x0 && this->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus);
    bool onFireChange = (statusBefore & PlayerPaddle::OnFireStatus) == 0x0 && this->HasSpecialStatus(PlayerPaddle::OnFireStatus);

    if (frozenChange && (statusBefore & PlayerPaddle::OnFireStatus)) {
        this->CancelFireStatusWithIce();
        return;
    }
    
    if (onFireChange && (statusBefore & PlayerPaddle::FrozenInIceStatus)) {
        this->CancelFrozenStatusWithFire();
        return;
    }

    // Check whether the paddle was just frozen
    if ((status & PlayerPaddle::FrozenInIceStatus) != 0x0) {
        this->hurtAlphaAnim.ClearLerp();
        this->hurtAlphaAnim.SetInterpolantValue(1.0f);

        this->frozenCountdown = PADDLE_FROZEN_TIME_IN_SECS;
        // EVENT: Paddle was just frozen
        GameEventManager::Instance()->ActionPaddleStatusUpdate(*this, PlayerPaddle::FrozenInIceStatus, true);
        
    }
    // Check whether the paddle was just set on fire
    if ((status & PlayerPaddle::OnFireStatus) != 0x0) {
        this->hurtAlphaAnim.ClearLerp();
        this->hurtAlphaAnim.SetInterpolantValue(1.0f);

        this->onFireCountdown = PADDLE_ON_FIRE_TIME_IN_SECS;
        // EVENT: Paddle was just set on fire
        GameEventManager::Instance()->ActionPaddleStatusUpdate(*this, PlayerPaddle::OnFireStatus, true);
    }
    // Check whether the paddle was just electrocuted
    if ((status & PlayerPaddle::ElectrocutedStatus) != 0x0) {
        this->hurtAlphaAnim.ClearLerp();
        this->hurtAlphaAnim.SetInterpolantValue(1.0f);

        this->electrocutedCountdown = PADDLE_ELECTROCUTED_TIME_IN_SECS;
        // EVENT: Paddle was just electrocuted
        GameEventManager::Instance()->ActionPaddleStatusUpdate(*this, PlayerPaddle::ElectrocutedStatus, true);
    }
}

void PlayerPaddle::RemoveSpecialStatus(int32_t status) {
    int32_t statusBefore = this->currSpecialStatus;
    this->currSpecialStatus = this->currSpecialStatus & ~status;

    // Check to see whether the paddle was just unfrozen
    if ((statusBefore & PlayerPaddle::FrozenInIceStatus) != 0x0 && !this->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus)) {

        this->frozenCountdown = 0.0;
        // EVENT: Paddle was just unfrozen
        GameEventManager::Instance()->ActionPaddleStatusUpdate(*this, PlayerPaddle::FrozenInIceStatus, false);
    }

    // Check whether the paddle is no longer on fire
    if ((statusBefore & PlayerPaddle::OnFireStatus) != 0x0 && !this->HasSpecialStatus(PlayerPaddle::OnFireStatus)) {

        this->onFireCountdown = 0.0;
        // EVENT: Paddle stopped being on fire
        GameEventManager::Instance()->ActionPaddleStatusUpdate(*this, PlayerPaddle::OnFireStatus, false);
    }

    if ((statusBefore & PlayerPaddle::ElectrocutedStatus) != 0x0 && !this->HasSpecialStatus(PlayerPaddle::ElectrocutedStatus)) {
        
        this->electrocutedCountdown = 0.0;
        // EVENT: Paddle stopped being electrocuted
        GameEventManager::Instance()->ActionPaddleStatusUpdate(*this, PlayerPaddle::ElectrocutedStatus, false);
    }
}

void PlayerPaddle::SetPaddleCamera(bool isPaddleCamOn) {

    bool wasPaddleCamActive = this->isPaddleCamActive;
    this->isPaddleCamActive = isPaddleCamOn;

    // When the paddle camera is on, we change the collision boundaries to
    // be more natural to the vision of the paddle cam
    this->SetDimensions(this->currSize);
    if (!isPaddleCamOn) {
        this->MoveAttachedObjectsToNewBounds();
    }

    if (!wasPaddleCamActive && this->isPaddleCamActive) {
        // EVENT: Paddle camera is now set
        GameEventManager::Instance()->ActionPaddleCameraSetOrUnset(*this, true);
    }
    else if (wasPaddleCamActive && !this->isPaddleCamActive) {
        // EVENT: Ball camera is now unset
        GameEventManager::Instance()->ActionPaddleCameraSetOrUnset(*this, false);
    }
}

// Tells the paddle that it has started to fire the laser beam (or has stopped firing the laser beam)
void PlayerPaddle::SetIsLaserBeamFiring(bool isFiring) {
	this->isFiringBeam = isFiring;
}

void PlayerPaddle::ShootBall() {
	if (this->HasBallAttached()) {
		this->FireAttachedBall();
	}
}

void PlayerPaddle::ReleaseEverythingAttached() {
    // Release any attached ball
    this->ShootBall();
    
    // Release all attached projectiles
    while (!this->attachedProjectiles.empty()) {
        this->FireAttachedProjectile();
    }
}

void PlayerPaddle::ContinuousShoot(double dT, GameModel* gameModel, float magnitudePercent) {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(magnitudePercent);

    assert(gameModel != NULL);
    assert(magnitudePercent >= 0.0f && magnitudePercent <= 1.0f);

    // TODO: If I ever implement this function...
    // If the paddle has a nasty status, it can't shoot
    //if (this->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus | PlayerPaddle::OnFireStatus | PlayerPaddle::ElectrocutedStatus)) {
    //    return;
    //}

}

/**
 * This will fire any weapons or abilities that paddle currently has - if none
 * exist then this function does nothing.
 */
void PlayerPaddle::DiscreteShoot(GameModel* gameModel) {
    assert(gameModel != NULL);

    // If the paddle has a nasty status, it can't shoot
    if (this->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus | PlayerPaddle::OnFireStatus | PlayerPaddle::ElectrocutedStatus)) {
        return;
    }

    const GameLevel* currentLevel = gameModel->GetCurrentLevel();
    assert(currentLevel != NULL);
    
	// If there's a ball attached to the paddle then we release it and exit
	if (this->HasBallAttached()) {
		this->FireAttachedBall();
        this->UpdatePaddleBounds(
            currentLevel->GetPaddleMinXBound(this->centerPos[0], this->defaultYPos), 
            currentLevel->GetPaddleMaxXBound(this->centerPos[0], this->defaultYPos));
		return;
	}
    else if (this->HasProjectileAttached()) {
        this->FireAttachedProjectile();
        this->UpdatePaddleBounds(
            currentLevel->GetPaddleMinXBound(this->centerPos[0], this->defaultYPos), 
            currentLevel->GetPaddleMaxXBound(this->centerPos[0], this->defaultYPos));
        return;
    }

    // Check for the various paddle types and react appropriately for each...
    bool updateBoundsAfterShooting = false;
	// Check for the paddle rockets (these have top priority)
    if (this->HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle)) {
        
        Point2D rocketSpawnPos;
        float rocketHeight, rocketWidth;
        this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
        
        // The rocket immediately is fired from the paddle - create a projectile for it and add it to the model (i.e., fire it!)...
        PlayerPaddle::PaddleType rocketType;
        RocketProjectile* rocketProjectile = NULL;

	    if (this->HasPaddleType(PlayerPaddle::RocketPaddle)) {
            rocketType = PlayerPaddle::RocketPaddle;
		    rocketProjectile = new PaddleRocketProjectile(rocketSpawnPos, 
                this->GetUpVector(), rocketWidth, rocketHeight);
	    }
        else {
            assert(this->HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle));
            rocketType = PlayerPaddle::RemoteControlRocketPaddle;
            rocketProjectile = new PaddleRemoteControlRocketProjectile(rocketSpawnPos, 
                this->GetUpVector(), rocketWidth, rocketHeight);
        }

        // Make sure the rocket doesn't explode if it's lying up against a block when launched...
        bool foundPiece = false;
        const GameLevel* currLevel = gameModel->GetCurrentLevel();

        std::set<LevelPiece*> levelPieces;
        currLevel->GetLevelPieceCollisionCandidates(0.0, rocketProjectile->GetPosition(), 
            rocketProjectile->BuildBoundingLines(), 0.0, levelPieces);

        for (std::set<LevelPiece*>::const_iterator iter = levelPieces.begin(); iter != levelPieces.end(); ++iter) {
            const LevelPiece* currPiece = *iter;
            if (!currPiece->ProjectilePassesThrough(rocketProjectile)) {
                rocketProjectile->SetLastThingCollidedWith(currPiece);
                foundPiece = true;
                break;
            }
        }

        // Make the last thing the rocket collided with the paddle - if there are no pieces to do this with
        if (!foundPiece) {
            rocketProjectile->SetLastThingCollidedWith(this);
        }

        // Special status condition: if the paddle was invisible when the rocket was acquired then the rocket will
        // also be invisible...
        if (this->HasSpecialStatus(PlayerPaddle::InvisibleRocketStatus)) {
            rocketProjectile->SetIsInvisible(true);
            this->RemoveSpecialStatus(PlayerPaddle::InvisibleRocketStatus);
        }

        gameModel->AddProjectile(rocketProjectile);
        this->RemovePaddleType(rocketType);

        // EVENT: Paddle just fired a rocket
        GameEventManager::Instance()->ActionPaddleWeaponFired(rocketType);
        
        updateBoundsAfterShooting = true;
    }
	// Check for laser beam paddle - this has secondary priority
	else if (this->HasPaddleType(PlayerPaddle::LaserBeamPaddle) && !this->isFiringBeam) {
		
        // We add the beam to the game model, the rest will be taken care of by the beam and model
        PaddleLaserBeam* paddleLaserBeam = new PaddleLaserBeam(this, gameModel);
		gameModel->AddBeam(paddleLaserBeam);

        // EVENT: Paddle just fired a beam
        GameEventManager::Instance()->ActionPaddleWeaponFired(PlayerPaddle::LaserBeamPaddle);
	}
	
    else {
        // All of the rest are basic, multiple shot projectiles that should all execute simultaneously

        // Check for laser bullet paddle (shoots little laser bullets from the paddle)
        if (this->HasPaddleType(PlayerPaddle::LaserBulletPaddle)) {
		    // Make sure we are allowed to fire a new laser bullet
		    if (this->timeSinceLastLaserBlast >= PADDLE_LASER_BULLET_DELAY) {

			    // Calculate the width and height of the laser bullet based on the size of the paddle...
			    float projectileWidth  = this->GetPaddleScaleFactor() * PaddleLaserProjectile::WIDTH_DEFAULT;
			    float projectileHeight = this->GetPaddleScaleFactor() * PaddleLaserProjectile::HEIGHT_DEFAULT;

			    // Create the right type of projectile in the right place
                Vector2D paddleUpDir = this->GetUpVector();
			    Projectile* newProjectile = new PaddleLaserProjectile(
				    this->GetCenterPosition() + (this->currHalfHeight + 0.5f * projectileHeight) * paddleUpDir, paddleUpDir);

                newProjectile->SetLastThingCollidedWith(this);

			    // Modify the fired bullet based on the current paddle's properties...
			    newProjectile->SetWidth(projectileWidth);
			    newProjectile->SetHeight(projectileHeight);

			    // Fire ze laser bullet! - tell the model about it
			    gameModel->AddProjectile(newProjectile);

			    // Reset the timer for the next laser blast
			    this->timeSinceLastLaserBlast = 0.0;

                // EVENT: Paddle just fired a laser bullet
                GameEventManager::Instance()->ActionPaddleWeaponFired(PlayerPaddle::LaserBulletPaddle);
		    }
        }

        // Check for the flame blaster paddle (shoots blasts of fire)
        if (this->HasPaddleType(PlayerPaddle::FlameBlasterPaddle)) {
            if (this->timeSinceLastBlastShot >= PADDLE_FLAME_BLAST_DELAY) {

                PaddleFlameBlasterProjectile* fireProjectile = new PaddleFlameBlasterProjectile(*this);
                gameModel->AddProjectile(fireProjectile);

                this->timeSinceLastBlastShot = 0.0;

                // EVENT: Paddle just fired a blast of flame...
                GameEventManager::Instance()->ActionPaddleWeaponFired(PlayerPaddle::FlameBlasterPaddle);
            }
        }
        else if (this->HasPaddleType(PlayerPaddle::IceBlasterPaddle)) {
            if (this->timeSinceLastBlastShot >= PADDLE_ICE_BLAST_DELAY) {
                PaddleIceBlasterProjectile* iceProjectile = new PaddleIceBlasterProjectile(*this);
                gameModel->AddProjectile(iceProjectile);

                this->timeSinceLastBlastShot = 0.0;

                // EVENT: Paddle just fired a blast of ice...
                GameEventManager::Instance()->ActionPaddleWeaponFired(PlayerPaddle::IceBlasterPaddle);
            }
        }

        // Check for mine launcher paddle (shoots explosive mines from the paddle)
        if (this->HasPaddleType(PlayerPaddle::MineLauncherPaddle)) {
            if (this->timeSinceLastMineLaunch >= PADDLE_MINE_LAUNCH_DELAY) {

			    // Calculate the width and height of the mine based on the size of the paddle...
			    float projectileWidth  = this->GetPaddleScaleFactor() * MineProjectile::WIDTH_DEFAULT;
			    float projectileHeight = this->GetPaddleScaleFactor() * MineProjectile::HEIGHT_DEFAULT;

			    // Create the right type of projectile in the right place
                Vector2D paddleUpDir = this->GetUpVector();
			    Projectile* mineProjectile = new PaddleMineProjectile(
				    this->GetCenterPosition() + this->GetMineProjectileStartingHeightRelativeToPaddle() * paddleUpDir,
                    paddleUpDir, projectileWidth, projectileHeight);
               
                mineProjectile->SetLastThingCollidedWith(this);

                // Special status condition: if the paddle was invisible when the mine was acquired then the mine will
                // also be invisible...
                if (this->HasSpecialStatus(PlayerPaddle::InvisibleMineStatus)) {
                    mineProjectile->SetIsInvisible(true);
                }

			    // Launch ze mine! - tell the model about it
			    gameModel->AddProjectile(mineProjectile);

                this->timeSinceLastMineLaunch = 0.0;

                // EVENT: Paddle just launched a mine
                GameEventManager::Instance()->ActionPaddleWeaponFired(PlayerPaddle::MineLauncherPaddle);
            }
        }

	} // other paddle shooting abilities go here...

    if (updateBoundsAfterShooting) {
        this->UpdatePaddleBounds(
            currentLevel->GetPaddleMinXBound(this->centerPos[0], this->defaultYPos), 
            currentLevel->GetPaddleMaxXBound(this->centerPos[0], this->defaultYPos));
    }
}

bool PlayerPaddle::HasSomethingToShoot() const {
    return this->HasBallAttached() || this->HasProjectileAttached() ||
        this->HasPaddleType(LaserBulletPaddle | RocketPaddle | MineLauncherPaddle | RemoteControlRocketPaddle | 
        FlameBlasterPaddle | IceBlasterPaddle) || (this->HasPaddleType(LaserBeamPaddle) && !this->isFiringBeam);
}

/**
 * Attaches the given ball to the paddle if there is no ball already attached
 * to the paddle.
 * Returns: true if the ball was successfully attached, false otherwise.
 */
bool PlayerPaddle::AttachBall(GameBall* ball) {
	assert(ball != NULL);

	// If the paddle already has a ball attached then we do nothing
	if (this->HasBallAttached()) {
		return false;
	}

    // If the ball is directly below the paddle then we don't attach it
    const Point2D& ballCenter = ball->GetBounds().Center();
    float ballRadius = ball->GetBounds().Radius();

    // Check to make sure the ball is within the x coordinates of the paddle
    if (ballCenter[0] - ballRadius <= this->GetCenterPosition()[0] + this->GetHalfWidthTotal() &&
        ballCenter[0] + ballRadius >= this->GetCenterPosition()[0] - this->GetHalfWidthTotal()) {

        // Check to see if the ball is below the paddle...
        bool isPaddleFlipped = this->GetIsPaddleFlipped();
        bool isUnderPaddle = (isPaddleFlipped && (ballCenter[1] + 0.5f * ballRadius) >= (this->GetCenterPosition()[1] + this->GetHalfHeight())) ||
            (!isPaddleFlipped && (ballCenter[1] - 0.5f * ballRadius) <= (this->GetCenterPosition()[1] - this->GetHalfHeight()));

        if (isUnderPaddle) {
            return false;
        }
    }

	// Attach the ball
	this->attachedBall = ball;
	// Reset the ball so that it has no previous record of the level piece it last hit
	ball->SetLastPieceCollidedWith(NULL);

	// Make sure the position of the ball is sitting on-top of the paddle
	Vector2D normal;
	Collision::LineSeg2D collisionLine;
	double timeUntilCollision;
    Point2D collisionPoint;
	
	if (this->CollisionCheck(*this->attachedBall, 0.0, normal, collisionLine, timeUntilCollision, collisionPoint)) {
		// Position the ball so that it is against the collision line, perpendicular to its normal
        //Point2D closestPtOnCollisionLine = Collision::ClosestPoint(ballCenter, collisionLine);
		this->attachedBall->SetCenterPosition(collisionPoint);
	}

	// Remove the ball's direction but not its speed
	this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), Vector2D(0, 0));

	// Disable collisions for the ball (re-enable them when the ball is detached)
	this->attachedBall->SetBallBallCollisionsDisabled();
	this->attachedBall->SetBallPaddleCollisionsDisabled();

	// Tell the ball not to collide with this paddle again when it's released until it hits something else...
	this->attachedBall->SetLastThingCollidedWith(this);

	return true;
}

void PlayerPaddle::AttachProjectile(Projectile* projectile) {
    assert(projectile != NULL);
    this->attachedProjectiles.push_back(projectile);
}

void PlayerPaddle::DetachProjectile(Projectile* projectile){
    assert(projectile != NULL);
    this->attachedProjectiles.remove(projectile);
}

void PlayerPaddle::HitByBall(const GameBall& ball) {
    // If the ball is a fireball or iceball and this paddle is frozen or on-fire, respectively
    // then the effect will be canceled for the paddle...
    if (ball.HasBallType(GameBall::IceBall) && this->HasSpecialStatus(PlayerPaddle::OnFireStatus)) {
        this->CancelFireStatusWithIce();
    }
    else if (ball.HasBallType(GameBall::FireBall) && this->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus)) {
        this->CancelFrozenStatusWithFire();
    }
}

void PlayerPaddle::HitByBoss(const BossBodyPart& bossPart) {

    Collision::AABB2D bossAABB = bossPart.GenerateWorldAABB();
    float distXToPaddleCenter = this->GetCenterPosition()[0] - bossAABB.GetCenter()[0];

    // Check to see if the paddle is still reeling from a previous hit, if so then
    // ignore this one
    if (this->moveDownAnimation.GetInterpolantValue() != 0.0 ||
        this->rotAngleZAnimation.GetInterpolantValue() != 0.0 &&
        this->lastEntityThatHurtHitPaddle == &bossPart) {

        // Just make sure the paddle isn't touching the boss anymore...
        if (distXToPaddleCenter < 0) {
            this->SetCenterPosition(Point2D(bossAABB.GetMin()[0] - this->GetHalfWidthTotal(), this->GetCenterPosition()[1]));
        }
        else {
            this->SetCenterPosition(Point2D(bossAABB.GetMax()[0] + this->GetHalfWidthTotal(), this->GetCenterPosition()[1]));
        }

        return;
    }

    float distFromCenter = 0.0f;
    
	// Find percent distance from edge to center of the paddle
    float percentNearCenter = this->GetPercentNearPaddleCenter(
        Point2D(bossAABB.GetCenter()[0] + NumberFuncs::SignOf(distXToPaddleCenter) * bossAABB.GetWidth()/4, bossAABB.GetCenter()[1]), distFromCenter);
    if (percentNearCenter < 0) {
        percentNearCenter = std::max<float>(0.1, percentNearCenter);
    }
    else {
        percentNearCenter = std::min<float>(-0.1, percentNearCenter);
    }
    float percentNearEdge = 1.0 - percentNearCenter;

	static const float MAX_HIT_ROTATION = 80.0f;
	
	float rotationAmount = percentNearEdge * MAX_HIT_ROTATION;
	if (distFromCenter > 0.0) {
		// The boss hit the 'right' side of the paddle (typically along the positive x-axis)
		rotationAmount *= -1.0f;
    }
    //else {
    // The projectile hit the 'left' side of the paddle (typically along the negative x-axis)
    //}

    // Re-adjust the rotation sign based on whether the paddle is flipped
    rotationAmount *= cosf(this->reorientZRotInRads);

	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	static const double HIT_EFFECT_TIME = 2.5;
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(HIT_EFFECT_TIME);

	static const float MIN_MOVE_DOWN_AMT = 3.0f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;
	float totalMaxMoveDown = MIN_MOVE_DOWN_AMT + percentNearCenter * 2.25f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);
    this->moveDownAnimation.Tick(0.001);

	times.clear();
	times.reserve(5);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(0.5f);
	times.push_back(1.0f);
	times.push_back(HIT_EFFECT_TIME);

	std::vector<float> rotationValues;
	rotationValues.reserve(5);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() - 0.25f * rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + 0.75f * rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);
    this->rotAngleZAnimation.Tick(0.001);
    this->SetupAlphaFlashAnim(0.25*HIT_EFFECT_TIME);

    this->lastEntityThatHurtHitPaddle = &bossPart;

    // Push the paddle away from the boss by changing its velocity...
    //this->SetCenterPosition(this->GetCenterPosition() + Vector2D(distFromCenter, 0.0f));
    float impulseAmt = 0.5f * NumberFuncs::SignOf(distXToPaddleCenter) * percentNearEdge * PlayerPaddle::DEFAULT_MAX_SPEED;
    this->ApplyImpulseForce(impulseAmt, 3*impulseAmt);

    // EVENT: The paddle was just hit by the boss
    GameEventManager::Instance()->ActionPaddleHitByBoss(*this, bossPart);
}

// Called when the paddle is hit by a projectile
void PlayerPaddle::HitByProjectile(GameModel* gameModel, const Projectile& projectile) {
    
	// The paddle is unaffected if it has a shield active...
	if (this->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
		// EVENT: Paddle shield was just hit by a projectile
		GameEventManager::Instance()->ActionPaddleShieldHitByProjectile(*this, projectile);
		return;
	}

	// Different projectiles have different effects on the paddle...
	switch (projectile.GetType()) {

		case Projectile::CollateralBlockProjectile:
			// Causes the paddle to be violently thrown off course for a short period of time...
			this->CollateralBlockProjectileCollision(projectile);
			break;

        case Projectile::BossOrbBulletProjectile:
            this->OrbProjectileCollision(projectile);
            break;

        case Projectile::BossLightningBoltBulletProjectile:
            this->LightningBoltProjectileCollision(projectile);
            break;

        case Projectile::BossShockOrbBulletProjectile:
            this->ShockOrbProjectileCollision(projectile);
            this->AddSpecialStatus(PlayerPaddle::ElectrocutedStatus);
            break;

        case Projectile::BossLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			this->LaserBulletProjectileCollision(projectile);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<const RocketProjectile*>(&projectile) != NULL);
			this->RocketProjectileCollision(gameModel, *static_cast<const RocketProjectile*>(&projectile));
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            assert(dynamic_cast<const MineProjectile*>(&projectile) != NULL);
            this->MineProjectileCollision(gameModel, *static_cast<const MineProjectile*>(&projectile));
            break;

		case Projectile::FireGlobProjectile:
			this->FireGlobProjectileCollision(*static_cast<const FireGlobProjectile*>(&projectile));
            this->AddSpecialStatus(PlayerPaddle::OnFireStatus);
			break;

        case Projectile::PaddleFlameBlastProjectile:
            // If the paddle is frozen then the flame doesn't hurt the paddle, it just unfreezes it
            if (!this->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus)) {
                this->FlameBlastProjectileCollision(*static_cast<const PaddleFlameBlasterProjectile*>(&projectile));
            }
            this->AddSpecialStatus(PlayerPaddle::OnFireStatus);
            break;

        case Projectile::PaddleIceBlastProjectile:
            if (!this->HasSpecialStatus(PlayerPaddle::OnFireStatus)) {
                this->IceBlastProjectileCollision(*static_cast<const PaddleIceBlasterProjectile*>(&projectile));
            }
            this->AddSpecialStatus(PlayerPaddle::FrozenInIceStatus);
            break;

        case Projectile::PortalBlobProjectile:
            // This shouldn't happen... not yet developed!
            assert(false);
            return;

		default:
			assert(false);
			return;
	}

	// EVENT: Paddle was just hit by a projectile
	GameEventManager::Instance()->ActionPaddleHitByProjectile(*this, projectile);
}

void PlayerPaddle::HitByBeam(const Beam& beam, const BeamSegment& beamSegment) {

    // The paddle is unaffected if it has a shield active...
    if (this->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
        // EVENT: Paddle shield was just hit by a beam
        GameEventManager::Instance()->ActionPaddleShieldHitByBeam(*this, beam, beamSegment);
        return;
    }

    static const long IMMUNITY_TO_BEAMS_TIME_IN_MS = 2000;
    static long lastBeamHitTimeInMS = 0;

    long currSysTime = BlammoTime::GetSystemTimeInMillisecs();
    if (currSysTime - lastBeamHitTimeInMS <= IMMUNITY_TO_BEAMS_TIME_IN_MS) {
        return;
    }
    lastBeamHitTimeInMS = currSysTime;

    this->BeamCollision(beam, beamSegment);

    // EVENT: Paddle was just hit by a beam
    GameEventManager::Instance()->ActionPaddleHitByBeam(*this, beam, beamSegment);
}

// Modify the projectile trajectory in certain special cases when the projectile is colliding with the paddle
// (or the paddle shield or some component of the paddle)
void PlayerPaddle::ModifyProjectileTrajectory(Projectile& projectile) const {

	if (this->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
        switch (projectile.GetType()) {

            case Projectile::BossOrbBulletProjectile:
            case Projectile::BossLaserBulletProjectile:
            case Projectile::BossLightningBoltBulletProjectile:
            case Projectile::BallLaserBulletProjectile:
            case Projectile::PaddleLaserBulletProjectile:
            case Projectile::PaddleRocketBulletProjectile:
            case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
            case Projectile::RocketTurretBulletProjectile:
            case Projectile::BossRocketBulletProjectile:
            case Projectile::LaserTurretBulletProjectile:
            case Projectile::PaddleMineBulletProjectile:
            case Projectile::MineTurretBulletProjectile:
            case Projectile::PaddleFlameBlastProjectile:
            case Projectile::PaddleIceBlastProjectile: {

		        // If the projectile is moving generally upwards and away from the paddle then we ignore this entirely...
		        if (acos(std::max<float>(-1.0f, std::min<float>(1.0f, 
			          Vector2D::Dot(projectile.GetVelocityDirection(), 
				        Vector2D::Normalize(this->GetUpVector()))))) > static_cast<float>(M_PI / 3.0f)) {
    				
			        Vector2D fromShieldCenterToProjectile = projectile.GetPosition() - this->GetCenterPosition();
			        fromShieldCenterToProjectile.Normalize();
			        projectile.SetVelocity(fromShieldCenterToProjectile, projectile.GetVelocityMagnitude());
			        projectile.SetPosition(projectile.GetPosition() + projectile.GetHeight() * fromShieldCenterToProjectile);

			        // Be sure to reset the last piece the projectile collided with since it technically just collided
			        // off of the paddle's shield
			        projectile.SetLastThingCollidedWith(NULL);

			        // EVENT: Projectile deflected by shield
			        GameEventManager::Instance()->ActionProjectileDeflectedByPaddleShield(projectile, *this);
		        }
                break;
            }

            default:
                break;
		}
	}
}

/**
 * It is assumed that at this point, the paddle collides with the given level piece.
 * Based on this collision, we need to update how far to the left/right the paddle can move.
 * If doAttachedBallCollision is true then a collision check will also take place for the attached ball -
 * this is fine if we don't want the ball to break blocks but if the game state is BallInPlayState then just
 * set this to false.
 */
void PlayerPaddle::UpdateBoundsByPieceCollision(const LevelPiece& p, bool doAttachedBallCollision) {

    if (p.GetType() != LevelPiece::NoEntry) {

        std::list<Point2D> collisionPts;
	    if (this->HasPaddleType(PlayerPaddle::ShieldPaddle) &&
            p.GetBounds().GetCollisionPoints(this->CreatePaddleShieldBounds(), collisionPts)) {

            // Separate points into those to the left and right of the paddle center, closest to the paddle center...
	        bool collisionOnMinX = false;
	        bool collisionOnMaxX = false;
            bool collisionUnder  = false;

            float rotationSign = cosf(this->reorientZRotInRads);
            assert(rotationSign != 0);
            bool isOneWayInOKDir = (p.GetType() == LevelPiece::OneWay) && 
                static_cast<const OneWayBlock&>(p).IsGoingTheOneWay(Vector2D(0, -rotationSign));

            float paddleYCollisionCoord = this->defaultYPos - rotationSign*(this->currHalfHeight - EPSILON);

	        for (std::list<Point2D>::iterator iter = collisionPts.begin(); iter != collisionPts.end(); ++iter) {
		        const Point2D& currPt = *iter;
		        // Check to see where the point is relative to the center of the paddle...
		        if (currPt[0] < this->GetCenterPosition()[0]) {
			        // Point is on the left of the paddle...
			        collisionOnMinX = true;
		        }
		        else {
			        // Point is on the right of the paddle (or center-ish)...
			        collisionOnMaxX = true;
		        }

                if (rotationSign > 0 && currPt[1] < paddleYCollisionCoord) {
                    // Not flipped upside down and there's a collision under the paddle
                    collisionUnder = true;
                    if (!isOneWayInOKDir) {
                        this->underPaddleYBound = std::max<float>(this->underPaddleYBound, currPt[1]);
                    }
                }
                else if (rotationSign < 0 && currPt[1] > paddleYCollisionCoord) {
                    // Flipped upside down and there's a collision under the paddle
                    collisionUnder = true;
                    if (!isOneWayInOKDir) {
                        this->underPaddleYBound = std::min<float>(this->underPaddleYBound, currPt[1]);
                    }
                }
 
	        }

            if (collisionUnder || (collisionOnMaxX && collisionOnMinX)) {
                return;
            }

	        if (collisionOnMinX) {
		        this->minXBound = std::max<float>(this->minXBound, this->GetCenterPosition()[0] - this->GetHalfWidthTotal());
	        } 
	        else {
		        this->maxXBound = std::min<float>(this->maxXBound, this->GetCenterPosition()[0] + this->GetHalfWidthTotal());
	        }	

            return;
	    }
        else {
            Collision::AABB2D pieceAABB = p.GetAABB();

            if (this->bounds.CollisionCheck(pieceAABB)) {
	            
                float rotationSign = cosf(this->reorientZRotInRads);
                assert(rotationSign != 0);
                bool isOneWayInOKDir = (p.GetType() == LevelPiece::OneWay) && 
                    static_cast<const OneWayBlock&>(p).IsGoingTheOneWay(Vector2D(0, -rotationSign));
                float paddleYCollisionCoord = this->defaultYPos - rotationSign*(this->currHalfHeight - EPSILON);

                if (rotationSign > 0 && p.GetCenter()[1] < paddleYCollisionCoord && !isOneWayInOKDir) {
                    // Not flipped, update the y bounds of the paddle...
                    this->underPaddleYBound = pieceAABB.GetMax()[1];
                    return;
                }
                else if (rotationSign < 0 && p.GetCenter()[1] > paddleYCollisionCoord && !isOneWayInOKDir) {
                    this->underPaddleYBound = pieceAABB.GetMin()[1];
                    return;
                }

                // We need to find the location of the collision so we can make the paddle stop (by updating its bounds)...
                if (this->GetCenterPosition()[0] < p.GetCenter()[0]) {
                    this->maxXBound = pieceAABB.GetMin()[0];
                }
                else {
                    this->minXBound = pieceAABB.GetMax()[0];
                }

                return;
            }
	        // If there's a rocket attached we need to check its bounds too...
            else if (this->HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle)) {
		        Point2D rocketSpawnPos;
		        float rocketHeight, rocketWidth;
		        this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		        Vector2D widthHeightVec(rocketWidth/1.5f, rocketHeight/1.5f);
                Collision::AABB2D rocketAABB(rocketSpawnPos - widthHeightVec, rocketSpawnPos + widthHeightVec);
                
                if (Collision::IsCollision(pieceAABB, rocketAABB)) {

                    if (rocketAABB.GetCenter()[0] < p.GetCenter()[0]) {
                        this->maxXBound = pieceAABB.GetMin()[0] + (this->GetCenterPosition()[0] + this->GetHalfWidthTotal()) - rocketAABB.GetMax()[0];
                    }
                    else {
                        this->minXBound = pieceAABB.GetMax()[0] + (this->GetCenterPosition()[0] - this->GetHalfWidthTotal()) - rocketAABB.GetMin()[0];
                    }

                    return;
                }
	        }
        }
    }
	
	// If there's a ball attached we need to check its bounds as well...
	if (!doAttachedBallCollision || !this->HasBallAttached()) {
        return;
    }

    // Don't let the piece interfere if it's below the paddle
    float rotationSign = cosf(this->reorientZRotInRads);
    float paddleYCollisionCoord = this->defaultYPos - rotationSign*(this->currHalfHeight - EPSILON);
    if ((rotationSign > 0 && p.GetCenter()[1] < paddleYCollisionCoord) ||
        (rotationSign < 0 && p.GetCenter()[1] > paddleYCollisionCoord)) {

        return;
    }

    const GameBall* attachedBall = this->GetAttachedBall();
    assert(attachedBall != NULL);
    if (!p.GetBounds().CollisionCheck(attachedBall->GetBounds())) {
        return;
    }

    if (attachedBall->GetCenterPosition2D()[0] < p.GetCenter()[0]) {
        float xDistFromBallEdgeToPaddleEdge = (this->GetCenterPosition()[0] + this->GetHalfWidthTotal()) - 
            (this->GetAttachedBall()->GetCenterPosition2D()[0] + this->GetAttachedBall()->GetBounds().Radius());
        
		this->maxXBound = p.GetAABB().GetMin()[0] + xDistFromBallEdgeToPaddleEdge;
    }
    else {
        float xDistFromBallEdgeToPaddleEdge = std::min(0.0f, (this->GetCenterPosition()[0] - this->GetHalfWidthTotal()) - 
            (this->GetAttachedBall()->GetCenterPosition2D()[0] - this->GetAttachedBall()->GetBounds().Radius()));
        
        this->minXBound = p.GetAABB().GetMax()[0] + xDistFromBallEdgeToPaddleEdge;
    }
}

/**
 * Adds an animation to the paddle that fades it in or out based on the
 * given parameter over the given amount of time.
 */
void PlayerPaddle::AnimateFade(bool fadeOut, double duration) {
	ColourRGBA finalColour = this->colour;
	if (fadeOut) {
		finalColour[3] = 0.0f;
	}
	else {
		finalColour[3] = 1.0f;
	}

	this->colourAnimation.SetLerp(duration, finalColour);
}

void PlayerPaddle::SetupAlphaFlashAnim(double totalFlashTime) {
    static const double FLASH_FREQ = 50;
    int numFlashes = static_cast<int>(totalFlashTime*FLASH_FREQ);
    if (numFlashes % 2 == 0) {
        numFlashes++;
    }
    double timePerFlash = totalFlashTime/numFlashes;

    std::vector<double> timeVals(numFlashes, 0.0);
    timeVals[0] = 0.0;
    for (int i = 1; i < numFlashes; i++) {
        timeVals[i] = timeVals[i-1] + timePerFlash;
    }
    
    // WARNING: NEVER SET THE ALPHA TO EXACTLY ZERO FOR THIS ANIMATION!! 
    // THERE ARE SPECIAL CONDITIONS WHEN THE PADDLE ALPHA IS ZERO.
    std::vector<float> alphaVals;
    alphaVals.reserve(numFlashes);
    alphaVals.push_back(1.0f);
    for (int i = 1; i < numFlashes; i += 2) {
        alphaVals.push_back(0.01f); 
        alphaVals.push_back(1.0f);
    }

    this->hurtAlphaAnim.SetLerp(timeVals, alphaVals);
    this->hurtAlphaAnim.SetRepeat(false);
}

bool PlayerPaddle::GetIsStableForGoingThroughPortals() const {
    return fabs(this->centerPos[1] - this->defaultYPos) < 0.1f;
}

#ifdef _DEBUG
void PlayerPaddle::DebugDraw() const {
	this->bounds.DebugDraw();
}
#endif

float PlayerPaddle::GetMineProjectileStartingHeightRelativeToPaddle() const {
    return this->currHalfHeight + 0.25f * this->GetPaddleScaleFactor() * MineProjectile::HEIGHT_DEFAULT;
}

bool PlayerPaddle::GetIsCloseToAWall() const {
    static const float CLOSE_DIST = GameBall::DEFAULT_BALL_RADIUS * 0.1;
    return (this->GetCenterPosition()[0] - this->minXBound) <= CLOSE_DIST ||
           (this->maxXBound - this->GetCenterPosition()[0]) <= CLOSE_DIST;
}

/**
 * Do the paddle collision with the collateral block projectile - this will knock the
 * the paddle off course and rotate it a bit.
 */
void PlayerPaddle::CollateralBlockProjectileCollision(const Projectile& projectile) {
	float distFromCenter = 0.0f;
	// Find percent distance from edge to center of the paddle
	float percentNearCenter = this->GetPercentNearPaddleCenter(projectile.GetPosition(), distFromCenter);
	float percentNearEdge = 1.0 - percentNearCenter;

	static const float MAX_HIT_ROTATION = 80.0f;
	
	float rotationAmount = percentNearEdge * MAX_HIT_ROTATION;
	if (distFromCenter > 0.0) {
		// The projectile hit the 'right' side of the paddle (typically along the positive x-axis)
		rotationAmount *= -1.0f;
	}
	//else {
		// The projectile hit the 'left' side of the paddle (typically along the negative x-axis)
	//}

    // Re-adjust the rotation sign based on whether the paddle is flipped
    rotationAmount *= cosf(this->reorientZRotInRads);

	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	static const double HIT_EFFECT_TIME = 2.5;
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(HIT_EFFECT_TIME);

	static const float MIN_MOVE_DOWN_AMT = 3.0f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;
	float totalMaxMoveDown = MIN_MOVE_DOWN_AMT + percentNearCenter * 2.25f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);

	times.clear();
	times.reserve(5);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(0.5f);
	times.push_back(1.0f);
	times.push_back(HIT_EFFECT_TIME);

	std::vector<float> rotationValues;
	rotationValues.reserve(5);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() - 0.25f * rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + 0.75f * rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);
    this->SetupAlphaFlashAnim(0.3*HIT_EFFECT_TIME);

    this->lastEntityThatHurtHitPaddle = &projectile;
}

void PlayerPaddle::OrbProjectileCollision(const Projectile& projectile) {
	float currHeight = 2.0f * this->GetHalfHeight();
	this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 1.6f, currHeight, currHeight, 17.0f);
    this->hurtAlphaAnim.ClearLerp();
    this->hurtAlphaAnim.SetInterpolantValue(1.0f);
}

void PlayerPaddle::LightningBoltProjectileCollision(const Projectile& projectile) {
    float currHeight = 2.0f * this->GetHalfHeight();
    this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 1.7f, currHeight, currHeight, 19.0f);
    this->hurtAlphaAnim.ClearLerp();
    this->hurtAlphaAnim.SetInterpolantValue(1.0f);
}

void PlayerPaddle::ShockOrbProjectileCollision(const Projectile& projectile) {
    float currHeight = 2.0f * this->GetHalfHeight();
    this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 2.0f, currHeight, currHeight, 25.0f);
    this->hurtAlphaAnim.ClearLerp();
    this->hurtAlphaAnim.SetInterpolantValue(1.0f);
}

// A laser bullet just collided with the paddle...
void PlayerPaddle::LaserBulletProjectileCollision(const Projectile& projectile) {
	float currHeight = 2.0f * this->GetHalfHeight();
	this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 1.5f, currHeight, currHeight, 15.0f);
    this->hurtAlphaAnim.ClearLerp();
    this->hurtAlphaAnim.SetInterpolantValue(1.0f);
}

// Rocket projectile just collided with the paddle - causes the paddle to fly wildly off course
void PlayerPaddle::RocketProjectileCollision(GameModel* gameModel, const RocketProjectile& projectile) {
	float distFromCenter = 0.0f;
	// Find percent distance from edge to center of the paddle
	float percentNearCenter = this->GetPercentNearPaddleCenter(projectile.GetPosition(), distFromCenter);
	float percentNearEdge = 1.0 - percentNearCenter;

    const float MAX_HIT_ROTATION = 170.0f * projectile.GetForcePercentageFactor();
	
	float rotationAmount = percentNearEdge * MAX_HIT_ROTATION;
	if (distFromCenter > 0.0) {
		// The projectile hit the 'right' side of the paddle (typically along the positive x-axis)
		rotationAmount *= -1.0f;
	}
    //else {
    // The projectile hit the 'left' side of the paddle (typically along the negative x-axis)
    //}

    // Re-adjust the rotation sign based on whether the paddle is flipped
    rotationAmount *= cosf(this->reorientZRotInRads);

	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	const double HIT_EFFECT_TIME = 5.0 * projectile.GetForcePercentageFactor();
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(HIT_EFFECT_TIME);

	static const float MIN_MOVE_DOWN_AMT = 4.0f * PlayerPaddle::PADDLE_HEIGHT_TOTAL * projectile.GetForcePercentageFactor();
	float totalMaxMoveDown = MIN_MOVE_DOWN_AMT + percentNearCenter * 2.5f * PlayerPaddle::PADDLE_HEIGHT_TOTAL * projectile.GetForcePercentageFactor();

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);

	times.clear();
	times.reserve(5);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(1.0f);
	times.push_back(2.5f);
	times.push_back(HIT_EFFECT_TIME);

	std::vector<float> rotationValues;
	rotationValues.reserve(5);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() - 0.75f * rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + 0.50f * rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);
    this->SetupAlphaFlashAnim(0.2*HIT_EFFECT_TIME);

    this->lastEntityThatHurtHitPaddle = &projectile;

	// Rocket explosion!!
	GameLevel* currentLevel = gameModel->GetCurrentLevel();
	assert(currentLevel != NULL);

	// Figure out the position in space where the rocket hit and find what block is at that position...
	std::set<LevelPiece*> levelPieces;
    currentLevel->GetLevelPieceCollisionCandidatesNoSort(projectile.GetPosition(), EPSILON, levelPieces);
	if (!levelPieces.empty()) {
		currentLevel->RocketExplosion(gameModel, &projectile, *levelPieces.begin());
	}
	else {
		currentLevel->RocketExplosionNoPieces(&projectile);
	}
}

void PlayerPaddle::MineProjectileCollision(GameModel* gameModel, const MineProjectile& projectile) {
    
    // There is only an explosion if the paddle doesn't have sticky paddle activated 
    if ((this->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
        return;
    }

    static const double HIT_EFFECT_TIME = 3.5;
    float currHeight = 2.0f * this->GetHalfHeight();
    this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), HIT_EFFECT_TIME, 3.0f * currHeight, 2.0f * currHeight, 70.0f);
    this->lastEntityThatHurtHitPaddle = &projectile;
    this->SetupAlphaFlashAnim(0.2*HIT_EFFECT_TIME);

    // Mine explosion!
    GameLevel* currentLevel = gameModel->GetCurrentLevel();
	assert(currentLevel != NULL);
	currentLevel->MineExplosion(gameModel, &projectile);
}

void PlayerPaddle::FireGlobProjectileCollision(const FireGlobProjectile& fireGlobProjectile) {

	float currHeight = 2.0f * this->GetHalfHeight();
	switch (fireGlobProjectile.GetRelativeSize()) {
		case FireGlobProjectile::Small:
			this->SetPaddleHitByProjectileAnimation(fireGlobProjectile.GetPosition(), 2.0f, currHeight, currHeight, 20.0f);
			break;

		case FireGlobProjectile::Medium:
			this->SetPaddleHitByProjectileAnimation(fireGlobProjectile.GetPosition(), 2.75f, 2.0f * currHeight, 1.5f * currHeight, 35.0f);
			break;

		case FireGlobProjectile::Large:
			this->SetPaddleHitByProjectileAnimation(fireGlobProjectile.GetPosition(), 3.25f, 2.5f * currHeight, 2.0f * currHeight, 70.0f);
			break;

		default:
			assert(false);
			break;
	}

    this->lastEntityThatHurtHitPaddle = &fireGlobProjectile;
}

void PlayerPaddle::FlameBlastProjectileCollision(const PaddleFlameBlasterProjectile& flameBlastProjectile) {

    float multiplier = flameBlastProjectile.GetSizeMultiplier();
    float currHeight = 2.0f * this->GetHalfHeight();

    this->SetPaddleHitByProjectileAnimation(flameBlastProjectile.GetPosition(), 
        multiplier * 3.0f, multiplier * currHeight, currHeight * 2.0f, 60.0f);

    float force = NumberFuncs::SignOf(this->GetCenterPosition()[0] - flameBlastProjectile.GetPosition()[0]) *
        0.5f * PlayerPaddle::DEFAULT_MAX_SPEED;
    this->ApplyImpulseForce(force, 2.5*force);

    this->lastEntityThatHurtHitPaddle = &flameBlastProjectile;
}

void PlayerPaddle::IceBlastProjectileCollision(const PaddleIceBlasterProjectile& iceBlastProjectile) {

    float multiplier = iceBlastProjectile.GetSizeMultiplier();
    float currHeight = 2.0f * this->GetHalfHeight();

    float totalRotationAmt;
    float totalMaxMoveDown;
    this->GetPaddleHitByProjectileEffect(iceBlastProjectile.GetPosition(), 30.0f, multiplier * currHeight * 1.2f, 
        currHeight, totalMaxMoveDown, totalRotationAmt);

    // Apply instantaneous effect to the paddle's rotation and downwards movement
    totalMaxMoveDown += this->moveDownAnimation.GetInterpolantValue();
    this->moveDownAnimation.ClearLerp();
    this->moveDownAnimation.SetInterpolantValue(totalMaxMoveDown);

    totalRotationAmt += this->rotAngleZAnimation.GetInterpolantValue();
    this->rotAngleZAnimation.ClearLerp();
    this->rotAngleZAnimation.SetInterpolantValue(totalRotationAmt);
    
    this->lastEntityThatHurtHitPaddle = &iceBlastProjectile;
}

void PlayerPaddle::BeamCollision(const Beam& beam, const BeamSegment& beamSegment) {
    UNUSED_PARAMETER(beam);

    float intensityMultiplier = NumberFuncs::Clamp(2.0f*(beamSegment.GetRadius() / this->GetHalfWidthTotal()), 0.1f, 2.0f);
    assert(intensityMultiplier > 0.0f && intensityMultiplier < 2.0f);

    float currHeight = 2.0f * this->GetHalfHeight();

    double hitEffectTime = intensityMultiplier * 5.0;
    this->SetPaddleHitByProjectileAnimation(beamSegment.GetEndPoint(), 
        hitEffectTime, intensityMultiplier * 2.5f * currHeight, 
        intensityMultiplier * 6.0f * currHeight, 80.0f);

    this->SetupAlphaFlashAnim(0.25*hitEffectTime);

    this->lastEntityThatHurtHitPaddle = &beamSegment;
}

// Get an idea of how close the given projectile is to the center of the paddle as a percentage
// where 0 is a far away from the center as possible and 1 is at the center. Also returns the value
// of distFromCenter which is assigned a negative/positive value based on the distance from the center
// of the paddle on the left/right side respectively.
float PlayerPaddle::GetPercentNearPaddleCenter(const Point2D& projectileCenter, float& distFromCenter) const {
	// Figure out what side of the paddle the projectile is hitting...
	// Use a line test...
	Vector2D vecToProjectile = projectileCenter - this->GetCenterPosition();
	Vector2D positivePaddleAxis = Vector2D::Rotate(this->rotAngleZAnimation.GetInterpolantValue(), PlayerPaddle::DEFAULT_PADDLE_RIGHT_VECTOR);

	// Use a dot product to determine whether the projectile is on the positive side or not
	distFromCenter = Vector2D::Dot(vecToProjectile, positivePaddleAxis);

	// Find percent distance from edge to center of the paddle
	float percentNearCenter = std::max<float>(0.0f, this->currHalfWidthTotal - fabs(distFromCenter)) / this->currHalfWidthTotal;
	assert(percentNearCenter >= 0.0f && percentNearCenter <= 1.0);
	return percentNearCenter;
}

void PlayerPaddle::GetPaddleHitByProjectileEffect(const Point2D& projectileCenter, float maxRotationInDegs, float minMoveDown, 
                                                   float closeToCenterCoeff, float& totalMoveDownAmt, float& totalRotationInDegs) const {
    float distFromCenter = 0.0;
    float percentNearCenter = this->GetPercentNearPaddleCenter(projectileCenter, distFromCenter);
    float percentNearEdge = 1.0 - percentNearCenter;
    totalRotationInDegs = percentNearEdge * maxRotationInDegs;
    if (distFromCenter > 0.0) {
        // The projectile hit the 'right' side of the paddle (typically along the positive x-axis)
        totalRotationInDegs *= -1.0f;
    }
    //else {
    // The projectile hit the 'left' side of the paddle (typically along the negative x-axis)
    //}

    // Re-adjust the rotation sign based on whether the paddle is flipped
    totalRotationInDegs *= cosf(this->reorientZRotInRads);

    totalMoveDownAmt = minMoveDown + percentNearCenter * closeToCenterCoeff;
}

// Helper function for setting the animation for when a projectile hits the paddle
// projectileCenter    - position where the projectile is
// totalHitEffectTime  - total time of the animation
// minMoveDown         - minimum distance the paddle will be pushed down
// closeToCenterCoeff  - added maximum effect of the paddle being hit right in the center
// maxRotationInDegs   - maximum rotation amount (when the paddle is hit on its outer edges)
void PlayerPaddle::SetPaddleHitByProjectileAnimation(const Point2D& projectileCenter, 
                                                     double totalHitEffectTime, 
                                                     float minMoveDown, 
                                                     float closeToCenterCoeff, float maxRotationInDegs) {
	
    // If the paddle is currently frozen then there is no effect...
    if (this->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus | PlayerPaddle::ElectrocutedStatus)) {
        return;
    }

    // Get the maximum effect of the projectile's collision
    // in terms of movement downward and the rotation for the paddle
	float rotationAmount;
    float totalMaxMoveDown;
    this->GetPaddleHitByProjectileEffect(projectileCenter, maxRotationInDegs, minMoveDown, closeToCenterCoeff, totalMaxMoveDown, rotationAmount);

	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(totalHitEffectTime);

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);

	std::vector<float> rotationValues;
	rotationValues.reserve(3);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);
}

/**
 * When the paddle magnet is active this routine will potentially modify the given vector so that the object associated
 * with it (as its velocity) and the given center position will start to migrate towards the paddle
 * as if being attracted by it.
 */
bool PlayerPaddle::AugmentDirectionOnPaddleMagnet(double seconds, float degreesChangePerSec,
                                                  const Point2D& currCenter, Vector2D& vectorToAugment) const {

    // If the paddle has the magnet item active and the projectile is moving towards the paddle, then we need to
    // modify the velocity to make it move towards the paddle...
    if (!this->HasPaddleType(PlayerPaddle::MagnetPaddle)) {
        return false;
    }
        
    // Also, don't keep augmenting the direction if the projectile has passed the paddle going out of bounds...
    float rotationSign = cosf(this->reorientZRotInRads);
    if ((rotationSign > 0 && currCenter[1] < this->GetCenterPosition()[1]) ||
        (rotationSign < 0 && currCenter[1] > this->GetCenterPosition()[1])) {

        return false;
    }

    // Figure out the vector from the projectile to the paddle...
    Vector2D projectileToPaddleVec = this->GetCenterPosition() - currCenter;
    if (projectileToPaddleVec.IsZero()) {
        return false;
    }

    projectileToPaddleVec.Normalize();

    if (Vector2D::Dot(vectorToAugment, projectileToPaddleVec) <= 0.01) {
        // The projectile is not within approximately a 90 degree angle of the vector from the projectile to the paddle
        return false;
    }
    
    bool isFlipped = this->GetIsPaddleFlipped();
    const float MIN_RANGE = isFlipped ? Trig::degreesToRadians(30)  : Trig::degreesToRadians(-150);
    const float MAX_RANGE = isFlipped ? Trig::degreesToRadians(150) : Trig::degreesToRadians(-30);

    float currRotAngle = atan2(vectorToAugment[1], vectorToAugment[0]);
    
    // Don't let the angle get too sharp or the projectile will collide with the paddle no matter what
    bool wasInRange = true;
    if (currRotAngle > MAX_RANGE || currRotAngle < MIN_RANGE) {
        wasInRange = false;
    }

    float targetRotAngle = atan2(projectileToPaddleVec[1],  projectileToPaddleVec[0]);
    if (fmod(fabs(targetRotAngle - currRotAngle), static_cast<float>(M_PI_MULT2)) <= 0.01f) {
        // If the direction is already pointing at the paddle then exit, no need to do anything else
        return false;
    }

    Vector3D crossProd = Vector3D::cross(vectorToAugment, projectileToPaddleVec);
    float rotSgn = NumberFuncs::SignOf(crossProd[2]);
    float rotationAmt = rotSgn * seconds * degreesChangePerSec;
    Vector2D newVector(vectorToAugment);
    newVector.Rotate(rotationAmt);
    newVector.Normalize();

    if (wasInRange) {
        // Make sure the new vector is in range as well...
        float newRotAngle = atan2(newVector[1], newVector[0]);
        if (newRotAngle > MAX_RANGE || newRotAngle < MIN_RANGE) {
            // We've gone out of range, don't change anything and exit
            return false;
        }
    }

    vectorToAugment = newVector;
    return true;
}

// Obtain an AABB encompassing the entire paddle's current collision area
Collision::AABB2D PlayerPaddle::GetPaddleAABB(bool includeAttachedBall) const {
	Point2D minPt, maxPt;
	if (this->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
		Vector2D sphereRadiusVec(this->GetHalfWidthTotal(), this->GetHalfWidthTotal());
		sphereRadiusVec = 1.2f * sphereRadiusVec;
		minPt = this->GetCenterPosition() - sphereRadiusVec;
		maxPt = this->GetCenterPosition() + sphereRadiusVec;
	}
	else {
		Vector2D halfWidthHeight(this->GetHalfWidthTotal(), this->GetHalfHeight());
		halfWidthHeight = 1.2f * halfWidthHeight;
		minPt = this->GetCenterPosition() - halfWidthHeight;
		maxPt = this->GetCenterPosition() + halfWidthHeight;
	}

	Collision::AABB2D paddleAABB(minPt, maxPt);
	if (includeAttachedBall && this->HasBallAttached()) {
		const Collision::Circle2D& ballBounds = this->GetAttachedBall()->GetBounds();
		Vector2D ballRadiusVec(ballBounds.Radius(), ballBounds.Radius());
		ballRadiusVec = 1.2f * ballRadiusVec;
		paddleAABB.AddPoint(ballBounds.Center() + ballRadiusVec);
		paddleAABB.AddPoint(ballBounds.Center() - ballRadiusVec);
	}

    if (this->HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle)) {
		Point2D rocketSpawnPos;
		float rocketHeight, rocketWidth;
		this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		Vector2D widthHeightVec(rocketWidth/1.5f, rocketHeight/1.5f);

		paddleAABB.AddPoint(rocketSpawnPos - widthHeightVec);
		paddleAABB.AddPoint(rocketSpawnPos + widthHeightVec);
	}

	return paddleAABB;
}

/**
 * Check to see if the given set of bounding lines collides with this paddle.
 */
bool PlayerPaddle::CollisionCheck(const BoundingLines& bounds, bool includeAttachedBallCheck) const {
	bool didCollide = false;
	// If the paddle has a shield around it do the collision with the shield
	if (this->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
		didCollide = bounds.CollisionCheck(this->CreatePaddleShieldBounds());
	}
	else {
		didCollide = this->bounds.CollisionCheck(bounds);
	}

	if (includeAttachedBallCheck && !didCollide && this->HasBallAttached()) {
		didCollide = bounds.CollisionCheck(this->GetAttachedBall()->GetBounds());
	}

	// If there's a rocket attached we need to check for collisions with it!
    if (!didCollide) {

        if (this->HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle)) {
		    Point2D rocketSpawnPos;
		    float rocketHeight, rocketWidth;
		    this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		    Vector2D widthHeightVec(rocketWidth/1.5f, rocketHeight/1.5f);

		    Collision::AABB2D rocketAABB(rocketSpawnPos - widthHeightVec, rocketSpawnPos + widthHeightVec);
		    didCollide = bounds.CollisionCheck(rocketAABB);
        }
	}
   
	return didCollide;
}

bool PlayerPaddle::IsBallUnderPaddle(const GameBall& ball) const {
    bool isFlipped = this->GetIsPaddleFlipped();
    float ballYAdjustment = this->GetHalfHeight() + 0.9f*ball.GetBounds().Radius();
    return (!isFlipped && (ball.GetCenterPosition2D()[1] < this->GetCenterPosition()[1] - ballYAdjustment)) ||
        (isFlipped && (ball.GetCenterPosition2D()[1] > this->GetCenterPosition()[1] + ballYAdjustment));
}

// Check to see if the given ball collides, return the normal of the collision and the line of the collision as well
// as the time since the collision occurred
bool PlayerPaddle::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, 
                                  Collision::LineSeg2D& collisionLine, 
                                  double& timeUntilCollision, Point2D& pointOfCollision) {

    if (ball.IsLastThingCollidedWith(this)) {
        return false;
    }

    // If the paddle has a shield around it do the collision with the shield
    if (this->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
        Collision::Circle2D shieldBounds = this->CreatePaddleShieldBounds();
        Point2D shieldPtOfCollision;
        return shieldBounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n,
            collisionLine, timeUntilCollision, pointOfCollision, shieldPtOfCollision, this->GetVelocity());
    }
    else {
        assert(this->bounds.GetNumLines() == 4);

        bool ballIsUnderPaddle = this->IsBallUnderPaddle(ball);
        bool ballIsGoingUp     = Vector2D::Dot(ball.GetDirection(), this->GetUpVector()) > 0;
        if (ballIsUnderPaddle && ballIsGoingUp) {

            // Ball is traveling upwards at the paddle from below it...
            return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, 
                timeUntilCollision, pointOfCollision, this->GetVelocity());
        }
        else {

            // Check to see if the ball is actually traveling down and whether it has collided recently, if
            // so then we don't allow another collision with the paddle
            if (ballIsGoingUp && ball.GetTimeSinceLastCollision() <= 2.0*dT) {
                return false;
            }

            // Ball is traveling downwards/parallel at the paddle
            BoundingLines boundsNoBottom;
            this->GetBoundsWithoutBottomCollisionLine(boundsNoBottom);

            bool wasCollision = boundsNoBottom.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, 
              collisionLine, timeUntilCollision, pointOfCollision, this->GetVelocity());

            return wasCollision;
        }
    }
}

bool PlayerPaddle::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
    
    // If the paddle has a shield around it do the collision with the shield
    if (this->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
        Collision::Circle2D shieldBounds = this->CreatePaddleShieldBounds();
        return Collision::IsCollision(ray, shieldBounds, rayT);
    }

    return this->bounds.CollisionCheck(ray, rayT);
}

// Check for a collision with the given projectile
bool PlayerPaddle::CollisionCheckWithProjectile(const Projectile& projectile, const BoundingLines& bounds) const {

    switch (projectile.GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::PaddleFlameBlastProjectile:
        case Projectile::PaddleIceBlastProjectile: {

            // These projectiles can only collide with the paddle if they're NOT going upwards from the paddle
            if (Vector2D::Dot(projectile.GetVelocityDirection(), this->GetUpVector()) <= EPSILON ) {
                return this->CollisionCheck(bounds, true);
            }
            break;
        }

        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
            // Paddle shouldn't even be visible/ticking while a remote control rocket is active,
            // there's no way it could collide with the paddle.
            return false;

        case Projectile::PortalBlobProjectile:
            // Not implemented!
            return false;

        default:
            return this->CollisionCheck(bounds, true);
    }

    return false;
}

// The paddle destroys all projectiles that collide with it, currently
bool PlayerPaddle::ProjectilePassesThrough(const Projectile& projectile) const {

    // Projectiles can pass through when reflected by the paddle shield
    if (this->HasPaddleType(PlayerPaddle::ShieldPaddle)) {

        switch (projectile.GetType()) {

            case Projectile::BossOrbBulletProjectile:
            case Projectile::BossLaserBulletProjectile:
            case Projectile::BossLightningBoltBulletProjectile:
            case Projectile::BallLaserBulletProjectile:
            case Projectile::PaddleLaserBulletProjectile:
            case Projectile::PaddleRocketBulletProjectile:
            case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
            case Projectile::RocketTurretBulletProjectile:
            case Projectile::LaserTurretBulletProjectile:
            case Projectile::PaddleMineBulletProjectile:
            case Projectile::MineTurretBulletProjectile:
            case Projectile::BossRocketBulletProjectile:
            case Projectile::PaddleFlameBlastProjectile:
            case Projectile::PaddleIceBlastProjectile:
                return true;

            default:
                break;
        }
    }

    return false;
}

bool PlayerPaddle::ProjectileIsDestroyedOnCollision(const Projectile& projectile) const {
    // Special case: sticky paddles always attach mines to themselves
    if (this->HasPaddleType(PlayerPaddle::StickyPaddle) &&
        (projectile.GetType() == Projectile::PaddleMineBulletProjectile ||
         projectile.GetType() == Projectile::MineTurretBulletProjectile)) {

        return false;
    }

    return !this->ProjectilePassesThrough(projectile); 
}

/**
 * Get the damage multiplier for this ball based on its current state and attributes.
 */
float PlayerPaddle::GetDamageMultiplier() const {

    return std::max<float>(GameModelConstants::GetInstance()->MIN_PADDLE_DAMAGE_MULTIPLIER, 
        std::min<float>(GameModelConstants::GetInstance()->MAX_PADDLE_DAMAGE_MULTIPLIER,
        this->GetPaddleScaleFactor()));
}

bool PlayerPaddle::HasBeenPausedAndRemovedFromGame(int pauseBitField) const {
    return ((pauseBitField & GameModel::PausePaddle) != 0x0) && (this->GetAlpha() < 1.0f);
}

void PlayerPaddle::UpdateLevel(const GameLevel& level) {
    this->UpdatePaddleBounds(level.GetPaddleMinXBound(0), level.GetPaddleMaxXBound(0));
    this->startingXPos = level.GetPaddleStartingXPosition();

    // Reset the paddle to the center of the new bounds
    this->ResetPaddle();
}


void PlayerPaddle::ApplyImpulseForce(float xDirectionalForce, float deaccel) {
    if (xDirectionalForce == 0.0f || this->impulseSpdDecreaseCounter < this->impulse || xDirectionalForce == 0.0) {
        // Ignore the impulse if there already is one
        return;
    }

	this->lastDirection = 1;
	this->impulse = xDirectionalForce;
    this->impulseDeceleration = fabs(deaccel);
    this->impulseSpdDecreaseCounter = 0.0f;
}

void PlayerPaddle::GenerateRocketDimensions(Point2D& spawnPos, float& width, float& height) const {
    if (this->HasPaddleType(PlayerPaddle::RocketPaddle)) {
        height = this->currScaleFactor * PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT;
        width  = this->currScaleFactor * PaddleRocketProjectile::PADDLEROCKET_WIDTH_DEFAULT;
    }
    else {
        assert(this->HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle));
        height = this->currScaleFactor * PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_HEIGHT_DEFAULT;
        width  = this->currScaleFactor * PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT;
    }

    spawnPos = this->GetCenterPosition() + (this->currHalfHeight + 0.5f * height) * this->GetUpVector();
}

void PlayerPaddle::CancelFireStatusWithIce() {
    // Cancel the fire
    this->currSpecialStatus &= ~PlayerPaddle::OnFireStatus;
    this->onFireCountdown = 0.0;

    // EVENT: The On-fire paddle is no longer...
    GameEventManager::Instance()->ActionOnFirePaddleCanceledByIce(*this);
}

void PlayerPaddle::CancelFrozenStatusWithFire() {
    // Cancel the ice
    this->currSpecialStatus &= ~PlayerPaddle::FrozenInIceStatus;
    this->frozenCountdown = 0.0;
    this->RecoverFromPausedPaddle();

    // EVENT: The frozen paddle is no longer...
    GameEventManager::Instance()->ActionFrozenPaddleCanceledByFire(*this);
}

void PlayerPaddle::RecoverFromPausedPaddle() {
    static const double PADDLE_RECOVER_TIME_IN_SECS = 0.2;
    if (!this->rotAngleZAnimation.GetHasInterpolationSet() || 
        this->rotAngleZAnimation.GetFinalInterpolationValue() != 0.0f) {

            this->rotAngleZAnimation.SetLerp(0.0, PADDLE_RECOVER_TIME_IN_SECS, 
                this->rotAngleZAnimation.GetInterpolantValue(), 0.0f);
    }
    if (!this->moveDownAnimation.GetHasInterpolationSet() || 
        this->moveDownAnimation.GetFinalInterpolationValue() != 0.0f) {

            this->moveDownAnimation.SetLerp(0.0, PADDLE_RECOVER_TIME_IN_SECS, 
                this->moveDownAnimation.GetInterpolantValue(), 0.0f);
    } 
}

void PlayerPaddle::GetBoundsWithoutBottomCollisionLine(BoundingLines& boundsNoBottom) const {
    boundsNoBottom.Clear();

    boundsNoBottom.AddBound(this->bounds.GetLine(0), this->bounds.GetNormal(0), this->bounds.GetOnInside(0));
    boundsNoBottom.AddBound(this->bounds.GetLine(1), this->bounds.GetNormal(1), this->bounds.GetOnInside(1));
    boundsNoBottom.AddBound(this->bounds.GetLine(2), this->bounds.GetNormal(2), this->bounds.GetOnInside(2));
}