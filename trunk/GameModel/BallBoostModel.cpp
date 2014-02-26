/**
 * BallBoostModel.cpp
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

#include "BallBoostModel.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "GameTransformMgr.h"

// Amounts of time to fade in and out of bullet time when it's activated/deactivated
const double BallBoostModel::BULLET_TIME_FADE_IN_SECONDS        = 0.35;
const double BallBoostModel::BULLET_TIME_FADE_OUT_SECONDS       = 0.15;

// Default amount of time to set for the duration of ball bullet time
const double BallBoostModel::DEFAULT_BULLET_TIME_DURATION = 1.3;

// The maximum duration of bullet time before the ball is automatically boosted
double BallBoostModel::BULLET_TIME_MAX_DURATION_SECONDS = BallBoostModel::DEFAULT_BULLET_TIME_DURATION;

// The minimum time required for a boost to be release for it to be considered a 'soft' release, in slingshot mode
const long BallBoostModel::SOFT_RELEASE_MIN_TIME_IN_MILLIS = 200;

// Amount of time it takes for a ball boost to charge (default)
const double BallBoostModel::DEFAULT_BOOST_CHARGE_TIME_SECONDS = 16.0;
// When the level is almost complete we give players more readily available boosts so they can finish quicker
const double BallBoostModel::LEVEL_ALMOST_COMPLETE_CHARGE_TIME_SECONDS = BallBoostModel::DEFAULT_BOOST_CHARGE_TIME_SECONDS / 2.0;
const double BallBoostModel::BOSS_LEVEL_CHARGE_TIME_SECONDS = BallBoostModel::DEFAULT_BOOST_CHARGE_TIME_SECONDS / 1.6;

// The time dilation factor used (multiplies the delta time of each game frame)
// when full bullet time is active
const float BallBoostModel::MIN_TIME_DIALATION_FACTOR       = 0.060f;
const float BallBoostModel::INV_MIN_TIME_DIALATION_FACTOR   = 1.0f / MIN_TIME_DIALATION_FACTOR;

BallBoostModel::BallBoostModel(GameModel* gameModel) : 
ballBoostDir(0,0), numAvailableBoosts(0), gameModel(gameModel),
currState(NotInBulletTime), timeDialationAnim(1.0f), totalBulletTimeElapsed(0.0), elapsedBoostChargeTime(0.0),
isBallBoostInverted(gameModel->GetIsBallBoostInverted()),
boostChargeTime(BallBoostModel::DEFAULT_BOOST_CHARGE_TIME_SECONDS) {

    // Setup the initial states for the various animations...
    timeDialationAnim.SetInterpolantValue(1.0f);
    timeDialationAnim.SetRepeat(false);

    // Figure out whether we should speed up the boost charge time or not based on how many blocks are left
    // in the current level...
    if (gameModel->GetCurrentLevel()->IsLevelAlmostComplete()) {
        this->boostChargeTime = BallBoostModel::LEVEL_ALMOST_COMPLETE_CHARGE_TIME_SECONDS;
    }
    else if (gameModel->GetCurrentLevel()->GetHasBoss()) {
        this->boostChargeTime = BallBoostModel::BOSS_LEVEL_CHARGE_TIME_SECONDS;
    }
}

BallBoostModel::~BallBoostModel() {
    this->gameModel->GetTransformInfo()->SetBulletTimeCamera(false);
    // EVENT: Ball Boost lost
    GameEventManager::Instance()->ActionBallBoostLost(true);
}

void BallBoostModel::Tick(const GameModel& gameModel, double dT) {
    static bool animationDone = false;

    switch (this->currState) {

        case NotInBulletTime:
            this->totalBulletTimeElapsed = 0.0;
            this->IncrementBoostChargeByTime(gameModel, dT);
            break;

        case BulletTimeFadeIn:
            if (!this->IsBallAvailableForBoosting() || !this->IsBoostAvailable()) {
                this->ReleaseBulletTime();
                break;
            }

            // We need to multiply the dT by the inverse time dialation since we're currently in bullet time
            animationDone = this->timeDialationAnim.Tick(this->GetInverseTimeDialation() * dT);

            // Recalculate the zoom bounds - this provides the display with the max and min
            // coordinates of the ball(s) in play so it can properly zoom in on it/them
            this->RecalculateBallZoomBounds();
            if (animationDone) {
                this->SetCurrentState(BulletTime);
            }
            break;

        case BulletTime:
            if (!this->IsBallAvailableForBoosting() || !this->IsBoostAvailable()) {
                this->ReleaseBulletTime();
                break;
            }

            // Increment the total bullet time counter, if it exceeds the max allowed bullet time
            // then we automatically boost the ball in its current direction
            // We need to multiply the dT by the inverse time dialation since we're currently in bullet time
            this->totalBulletTimeElapsed += (INV_MIN_TIME_DIALATION_FACTOR * dT);

            // Recalculate the zoom bounds - this provides the display with the max and min
            // coordinates of the ball(s) in play so it can properly zoom in on it/them
            this->RecalculateBallZoomBounds();
            if (this->totalBulletTimeElapsed >= BULLET_TIME_MAX_DURATION_SECONDS) {
                this->BallBoosterPressed();
            }

            break;

        case BulletTimeFadeOut:
            animationDone = this->timeDialationAnim.Tick(this->GetInverseTimeDialation() * dT);

            // Recalculate the zoom bounds - this provides the display with the max and min
            // coordinates of the ball(s) in play so it can properly zoom in on it/them
            this->RecalculateBallZoomBounds();
            if (animationDone) {
                this->SetCurrentState(NotInBulletTime);
            }
            break;

        default:
            assert(false);
            break;
    }
}

void BallBoostModel::BallBoostDirectionPressed(float x, float y) {

    assert(!Vector2D(x,y).IsZero());

    // Check to see whether we can even allow the player to boost the ball to begin with
    if (this->IsBallAvailableForBoosting() && this->IsBoostAvailable()) {
        // Boosting is allowed...

        // NOTE: If the direction button has already been pressed and therefore is being held down,
        // reset the boost direction but do not change anything else
        if (this->currState == NotInBulletTime) {
            // This is the first time (for this boost) that the direction has been pressed - 
            // start up bullet time...
            this->SetCurrentState(BulletTimeFadeIn);
        }

        Vector2D newBallBoostDir(x, y);
        
        // Make sure the boost direction is normalized
        newBallBoostDir.Normalize();

        if (!this->isBallBoostInverted) {
            newBallBoostDir = -newBallBoostDir;
        }

        // Don't let the direction change dramatically
        if (this->ballBoostDir.IsZero() || Vector2D::Dot(this->ballBoostDir, newBallBoostDir) >= 0) {
            this->ballBoostDir = newBallBoostDir;

            //float magnitude = newBallBoostDir.Magnitude();
            //this->boostDirMagnitudeCache.push_back(std::make_pair(BlammoTime::GetSystemTimeInMillisecs(), magnitude));
        }
    }
    else {
        // Boosting is disallowed currently...
        this->ReleaseBulletTime();

        // EVENT: Boost attempt failed
        GameEventManager::Instance()->ActionBoostFailedDueToNoBallsAvailable();
    }
}

void BallBoostModel::BallBoostDirectionReleased() {
    if (this->currState != NotInBulletTime) {

        // If slingshot mode is active then this will execute the ball boost...
        if (this->gameModel->GetBallBoostMode() == BallBoostModel::Slingshot) {

            // Check the cache of ball boost direction magnitudes, if the player let go
            // of the boost 'softly' then we don't do the boost
            if (!this->WasBoostReleasedSoftly()) {
                this->BallBoosterPressed();
            }
        }

        this->ReleaseBulletTime();
    }
}

void BallBoostModel::ReleaseBulletTime() {
    if (this->currState != NotInBulletTime) {

        // The player has decided to NOT boost at this time, clean up the about-to-boost state
        this->ballBoostDir[0] = 0.0f;
        this->ballBoostDir[1] = 0.0f;

        // Clear the boost magnitude cache
        //this->boostDirMagnitudeCache.clear();

        // End the bullet time state...
        this->SetCurrentState(BulletTimeFadeOut);
    }
}

bool BallBoostModel::WasBoostReleasedSoftly() {
    /*
    if (this->boostDirMagnitudeCache.size() <= 1) {
        debug_output("Release was too fast, not a soft release.");
        return false;
    }
 
    // Go through the boost magnitude cache to find the largest magnitude, then take the time difference between it
    // and the last time
    
   
    std::list<std::pair<unsigned long, float> >::reverse_iterator iter = this->boostDirMagnitudeCache.rbegin();
    float lastMagnitude = iter->second;
    float largestMag = lastMagnitude;
    unsigned long timeOfLargestMag = iter->first;
    ++iter;

    for (; iter != this->boostDirMagnitudeCache.rend(); ++iter) {
        // Make sure the magnitude difference is increasing as we go backwards through the cache
        if (lastMagnitude <= iter->second) {

            // Record the largest magnitude and its time
            if (iter->second >= largestMag) {
                largestMag = iter->second;
                timeOfLargestMag = iter->first;
            }
        }
        else {
            break;
        }

        lastMagnitude = iter->second;
    }

    // Check to see the total time between the last magnitude and the largest magnitude
    unsigned long deltaTime = (this->boostDirMagnitudeCache.back().first - timeOfLargestMag);
    
    
    debug_output("Boost release delta time (ms): " << deltaTime);
    // TODO... return deltaTime >= SOFT_RELEASE_MIN_TIME_IN_MILLIS;
    */

    return false;
}

/**
 * Executes a ball boost if possible.
 * Returns: true if there was a ball boost, false otherwise.
 */
bool BallBoostModel::BallBoosterPressed() {
    // In order to boost the ball the player must entirely be in the bullet time state
    if (this->currState != BulletTime) {
        return false;
    }
    assert(!this->ballBoostDir.IsZero());
    
    // Boost any balls that can be boosted
    bool ballWasBoosted = false;
    bool currBallBoosted = false;
    std::list<GameBall*>& balls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {

        GameBall* currBall = *iter;
        currBallBoosted = currBall->ExecuteBallBoost(this->ballBoostDir);
        ballWasBoosted |= currBallBoosted;

        if (currBallBoosted) {
            // If the ball boost is pressed we reset the last thing it collided with,
            // this makes sure that if it gets shot back at the last thing it collided with
            // that it will properly collide with it again
            currBall->SetLastThingCollidedWith(NULL);
        }
    }

    // Sanity: The ball should be allowed to boost!!
    assert(ballWasBoosted);
    // EVENT: Ball(s) have been boosted
    GameEventManager::Instance()->ActionBallBoostExecuted(*this);

    // Force a release of the bullet time state
    this->ReleaseBulletTime();

    // A boost was just expended...
    this->numAvailableBoosts--;
    assert(this->numAvailableBoosts >= 0);

    // EVENT: Ball Boost lost
    GameEventManager::Instance()->ActionBallBoostLost(this->numAvailableBoosts == 0);
    // EVENT: Ball Boost was used
    GameEventManager::Instance()->ActionBallBoostUsed();

    return ballWasBoosted;
}

void BallBoostModel::BallBoostReleasedForBall(const GameBall& ball) {
    UNUSED_PARAMETER(ball);
    // Do nothing. (we now handle this in the state machine)
}

/**
 * Get the total number of balls currently in play.
 */
size_t BallBoostModel::GetCurrentNumBalls() const {
    return this->gameModel->GetGameBalls().size();
}

/**
 * Gets the list of balls that will be / are being boosted by this.
 */
const std::list<GameBall*>& BallBoostModel::GetBalls() const {
    return this->gameModel->GetGameBalls();
}

/**
 * Gets the number of balls that could boost currently.
 * Returns: The number of boostable balls in play.
 */
int BallBoostModel::GetNumBallsAllowedToBoost() const {
    const GameTransformMgr* transformInfo = this->gameModel->GetTransformInfo();
    if (transformInfo->GetIsBallCamAnimationActive() || transformInfo->GetIsPaddleCamAnimationActive()) {
        return 0;
    }

    int count = 0;
    const std::list<GameBall*>& balls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
        const GameBall* currBall = *iter;
        if (currBall->IsBallAllowedToBoost()) {
            count++;
        }
    }

    return count;
}

/**
 * Determines whether any ball in the game is available to be boosted.
 * Returns: true if there's a ball that can be boosted, false otherwise.
 */
bool BallBoostModel::IsBallAvailableForBoosting() const {
    const GameTransformMgr* transformInfo = this->gameModel->GetTransformInfo();

    // No boosting allowed if ball camera or paddle camera is active
    if (this->gameModel->GetPlayerPaddle()->GetIsPaddleCameraOn() || GameBall::GetIsBallCameraOn() ||
        this->gameModel->GetTransformInfo()->GetIsLevelFlipAnimationActive() ||
        transformInfo->GetIsBallCamAnimationActive() || transformInfo->GetIsPaddleCamAnimationActive()) {
        return false;
    }

    bool ballIsAvailable = false;
    const std::list<GameBall*>& balls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
        const GameBall* currBall = *iter;
        if (currBall->IsBallAllowedToBoost()) {
            ballIsAvailable = true;
            break;
        }
    }

    return ballIsAvailable;
}

/**
 * Set the current state of the bullet time in the ball boost model - this will setup
 * any necessary member variables and animations for the new state and then set the state
 * member itself.
 */
void BallBoostModel::SetCurrentState(const BulletTimeState& newState) {
    if (newState == this->currState) {
        return;
    }

    switch (newState) {
        case NotInBulletTime:
            this->timeDialationAnim.SetInterpolantValue(1.0f);
            break;

        case BulletTimeFadeIn:
            this->gameModel->GetTransformInfo()->SetBulletTimeCamera(true);
            this->timeDialationAnim.SetLerp(BULLET_TIME_FADE_IN_SECONDS, MIN_TIME_DIALATION_FACTOR);
            break;

        case BulletTime:
            this->totalBulletTimeElapsed = 0.0;
            this->timeDialationAnim.SetInterpolantValue(MIN_TIME_DIALATION_FACTOR);
            break;

        case BulletTimeFadeOut:
            this->gameModel->GetTransformInfo()->SetBulletTimeCamera(false);
            this->timeDialationAnim.SetLerp(BULLET_TIME_FADE_OUT_SECONDS, 1.0f);
            break;

        default:
            assert(false);
            return;
    }

    this->currState = newState;

    // EVENT: The ball boost model state has changed
    GameEventManager::Instance()->ActionBulletTimeStateChanged(*this);
}

/**
 * Recalculates the maximum extents of the ball(s) that are in play.
 */
void BallBoostModel::RecalculateBallZoomBounds() {
    const std::list<GameBall*>& balls = this->gameModel->GetGameBalls();
    assert(!balls.empty());
    
    std::list<GameBall*>::const_iterator iter = balls.begin();
    const GameBall* currBall = *iter;
    Vector2D radiusVec(currBall->GetBounds().Radius(), currBall->GetBounds().Radius());

    this->ballZoomBounds.SetMin(currBall->GetCenterPosition2D() - radiusVec);
    this->ballZoomBounds.SetMax(currBall->GetCenterPosition2D() + radiusVec);
    ++iter;
    
    for (; iter != balls.end(); ++iter) {
        currBall = *iter;
        radiusVec[0] = radiusVec[1] = currBall->GetBounds().Radius();
        this->ballZoomBounds.AddPoint(currBall->GetCenterPosition2D() - radiusVec);
        this->ballZoomBounds.AddPoint(currBall->GetCenterPosition2D() + radiusVec);
    }
}

// Increments the time counter towards the next boost and add any new boosts if
// the charge time has been hit/exceeded
void BallBoostModel::IncrementBoostChargeByTime(const GameModel& gameModel, double timeInSecs) {
    
    if (this->numAvailableBoosts >= TOTAL_NUM_BOOSTS) {
        return;
    }
    
    // If we're in remote control rocket mode we don't increment the ball boost the game also has
    // to be on hard difficulty
    if (gameModel.GetTransformInfo()->GetIsRemoteControlRocketCameraOn() &&
        gameModel.GetDifficulty() == GameModel::HardDifficulty) {
        return;
    }

    this->elapsedBoostChargeTime += timeInSecs;
    while (this->numAvailableBoosts < TOTAL_NUM_BOOSTS &&
           this->elapsedBoostChargeTime >= this->boostChargeTime) {

        this->elapsedBoostChargeTime -= this->boostChargeTime;
        this->numAvailableBoosts++;

        // EVENT: New ball boost gained
        GameEventManager::Instance()->ActionBallBoostGained();
    }

    assert(this->numAvailableBoosts <= TOTAL_NUM_BOOSTS);
    if (this->numAvailableBoosts == TOTAL_NUM_BOOSTS) {
        this->elapsedBoostChargeTime = 0.0;
    }
}

void BallBoostModel::DebugDraw() const {
    const Collision::AABB2D& bounds = this->GetBallZoomBounds();

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_POINT_BIT);
	glDisable(GL_DEPTH_TEST);

	// Draw bounding lines
	glLineWidth(1.0f);
	glColor3f(1,0,1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(bounds.GetMin()[0], bounds.GetMin()[1]);
	glVertex2f(bounds.GetMin()[0], bounds.GetMax()[1]);
    glVertex2f(bounds.GetMax()[0], bounds.GetMax()[1]);
    glVertex2f(bounds.GetMax()[0], bounds.GetMin()[1]);
	glEnd();

    // Draw the center
    glColor3f(0, 1, 0);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(bounds.GetCenter()[0], bounds.GetCenter()[1]);
    glEnd();

	glPopAttrib();
}