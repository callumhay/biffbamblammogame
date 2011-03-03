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

// Amounts of time to fade in and out of bullet time when it's activated/deactivated
const double BallBoostModel::BULLET_TIME_FADE_IN_SECONDS    = 0.33;
const double BallBoostModel::BULLET_TIME_FADE_OUT_SECONDS   = 0.2;

// The time dialation factor used (multiplies the delta time of each game frame)
// when full bullet time is active
const float BallBoostModel::MIN_TIME_DIALATION_FACTOR       = 0.1f;

BallBoostModel::BallBoostModel(const std::list<GameBall*>* balls) : 
balls(balls), ballBoostDir(0,0), timeDialationFactor(1.0f), numAvailableBoosts(3000), 
isBallBoostDirPressed(false) {
    assert(balls != NULL);
    assert(!balls->empty());
}

BallBoostModel::~BallBoostModel() {
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
            // TODO: time dialation animation...
            this->timeDialationFactor = MIN_TIME_DIALATION_FACTOR;

            // Recalculate the zoom bounds - this provides the display with the max and min
            // coordinates of the ball(s) in play so it can properly zoom in on it/them
            this->RecalculateBallZoomBounds();
        }

        this->ballBoostDir[0] = x;
        this->ballBoostDir[1] = y;
    }
    else {
        // Boosting is disallowed currently, make sure the members are properly set
        assert(!this->isBallBoostDirPressed);
        assert(this->ballBoostDir.IsZero());
    }
}

void BallBoostModel::BallBoostDirectionReleased() {
    if (this->isBallBoostDirPressed) {
        // The player has decided to NOT boost at this time, clean up the about-to-boost state
        // TODO: time dialation animation...
        this->timeDialationFactor   = 1.0f;

        this->ballBoostDir[0] = 0.0f;
        this->ballBoostDir[1] = 0.0f;
        this->isBallBoostDirPressed = false;
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

    if (ballWasBoosted) {
        // Force a release of the boost direction and the entire boosting state
        this->BallBoostDirectionReleased();
        // A boost was just expended...
        this->numAvailableBoosts--;
        assert(this->numAvailableBoosts >= 0);
    }

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