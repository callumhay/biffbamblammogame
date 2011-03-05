/**
 * BallBoostModel.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "BallBoostModel.h"
#include "GameEventManager.h"

// Amounts of time to fade in and out of bullet time when it's activated/deactivated
const double BallBoostModel::BULLET_TIME_FADE_IN_SECONDS        = 0.30;
const double BallBoostModel::BULLET_TIME_FADE_OUT_SECONDS       = 0.20;
// The maximum duration of bullet time before the ball is automatically boosted
const double BallBoostModel::BULLET_TIME_MAX_DURATION_SECONDS   = 3.0;

// The time dialation factor used (multiplies the delta time of each game frame)
// when full bullet time is active
const float BallBoostModel::MIN_TIME_DIALATION_FACTOR       = 0.1f;

BallBoostModel::BallBoostModel(const std::list<GameBall*>* balls) : 
balls(balls), ballBoostDir(0,0), numAvailableBoosts(3000), 
isBallBoostDirPressed(false), currState(NotInBulletTime), 
timeDialationAnim(1.0f), totalBulletTimeElapsed(0.0) {
    assert(balls != NULL);
    assert(!balls->empty());

    // Setup the initial states for the various animations...
    timeDialationAnim.SetInterpolantValue(1.0f);
    timeDialationAnim.SetRepeat(false);

}

BallBoostModel::~BallBoostModel() {
}

void BallBoostModel::Tick(double dT) {
    static bool animationDone = false;

    switch (this->currState) {

        case NotInBulletTime:
            this->totalBulletTimeElapsed = 0.0;
            break;

        case BulletTimeFadeIn:
            animationDone = this->timeDialationAnim.Tick(dT);
            if (animationDone) {
                this->SetCurrentState(BulletTime);
            }
            break;

        case BulletTime:
            // Increment the total bullet time counter, if it exceeds the max allowed bullet time
            // then we automatically boost the ball in its current direction
            this->totalBulletTimeElapsed += dT;
            if (this->totalBulletTimeElapsed > BULLET_TIME_MAX_DURATION_SECONDS) {
                this->BallBoosterPressed();
            }

            break;

        case BulletTimeFadeOut:
            animationDone = this->timeDialationAnim.Tick(dT);
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
        if (!this->isBallBoostDirPressed) {
            // This is the first time (for this boost) that the direction has been pressed,
            // we need to setup the time dialation animation
            this->isBallBoostDirPressed = true;

            // Recalculate the zoom bounds - this provides the display with the max and min
            // coordinates of the ball(s) in play so it can properly zoom in on it/them
            this->RecalculateBallZoomBounds();

            // Start the bullet time state...
            this->SetCurrentState(BulletTimeFadeIn);
        }

        this->ballBoostDir[0] = x;
        this->ballBoostDir[1] = y;
    }
    else {
        // Boosting is disallowed currently, make sure the members are properly set
        this->ballBoostDir[0]       = 0.0f;
        this->ballBoostDir[1]       = 0.0f;
        this->isBallBoostDirPressed = false;
        this->SetCurrentState(NotInBulletTime);
    }
}

void BallBoostModel::BallBoostDirectionReleased() {
    if (this->isBallBoostDirPressed) {
        // The player has decided to NOT boost at this time, clean up the about-to-boost state
        this->ballBoostDir[0] = 0.0f;
        this->ballBoostDir[1] = 0.0f;
        this->isBallBoostDirPressed = false;

        // End the bullet time state...
        this->SetCurrentState(BulletTimeFadeOut);
    }
}

/**
 * Executes a ball boost if possible.
 * Returns: true if there was a ball boost, false otherwise.
 */
bool BallBoostModel::BallBoosterPressed() {
    if (!this->isBallBoostDirPressed) {
        return false;
    }
    assert(!this->ballBoostDir.IsZero());

    // Make sure the boost direction is normalized
    this->ballBoostDir.Normalize();
    
    // Boost any balls that can be boosted
    bool ballWasBoosted = false;
    for (std::list<GameBall*>::const_iterator iter = this->balls->begin(); iter != this->balls->end(); ++iter) {

        GameBall* currBall = *iter;
        ballWasBoosted |= currBall->ExecuteBallBoost(this->ballBoostDir);
    }

    // Sanity: The ball should be allowed to boost!!
    assert(ballWasBoosted);

    // Force a release of the boost direction and the entire boosting state
    this->BallBoostDirectionReleased();
    // A boost was just expended...
    this->numAvailableBoosts--;
    assert(this->numAvailableBoosts >= 0);

    // End the bullet time state...
    this->SetCurrentState(BulletTimeFadeOut);

    return ballWasBoosted;
}

/**
 * Determines whether any ball in the game is available to be boosted.
 * Returns: true if there's a ball that can be boosted, false otherwise.
 */
bool BallBoostModel::IsBallAvailableForBoosting() const {
    bool ballIsAvailable = false;
    for (std::list<GameBall*>::const_iterator iter = this->balls->begin(); iter != this->balls->end(); ++iter) {
        const GameBall* currBall = *iter;
        if (currBall->IsBallAllowedToBoost()) {
            ballIsAvailable = true;
            break;
        }
    }

    return ballIsAvailable;
}

void BallBoostModel::SetCurrentState(const BulletTimeState& newState) {
    switch (newState) {
        case NotInBulletTime:
            this->timeDialationAnim.SetInterpolantValue(1.0f);
            break;

        case BulletTimeFadeIn:
            this->timeDialationAnim.SetLerp(BULLET_TIME_FADE_IN_SECONDS, MIN_TIME_DIALATION_FACTOR);
            break;

        case BulletTime:
            this->totalBulletTimeElapsed = 0.0;
            this->timeDialationAnim.SetInterpolantValue(MIN_TIME_DIALATION_FACTOR);
            break;

        case BulletTimeFadeOut:
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
    assert(!this->balls->empty());
    
    std::list<GameBall*>::const_iterator iter = this->balls->begin();
    const GameBall* currBall = *iter;
    Vector2D radiusVec(currBall->GetBounds().Radius(), currBall->GetBounds().Radius());

    this->ballZoomBounds.SetMin(currBall->GetCenterPosition2D() - radiusVec);
    this->ballZoomBounds.SetMax(currBall->GetCenterPosition2D() + radiusVec);
    ++iter;
    
    for (; iter != this->balls->end(); ++iter) {
        currBall = *iter;
        radiusVec[0] = radiusVec[1] = currBall->GetBounds().Radius();
        this->ballZoomBounds.AddPoint(currBall->GetCenterPosition2D() - radiusVec);
        this->ballZoomBounds.AddPoint(currBall->GetCenterPosition2D() + radiusVec);
    }
}