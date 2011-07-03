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
#include "GameModel.h"

// Amounts of time to fade in and out of bullet time when it's activated/deactivated
const double BallBoostModel::BULLET_TIME_FADE_IN_SECONDS        = 0.35;
const double BallBoostModel::BULLET_TIME_FADE_OUT_SECONDS       = 0.15;
// The maximum duration of bullet time before the ball is automatically boosted
const double BallBoostModel::BULLET_TIME_MAX_DURATION_SECONDS   = 1.3;

// Amount of time it takes for a ball boost to charge
const float BallBoostModel::BOOST_CHARGE_TIME_SECONDS = 16.0f;

// The time dialation factor used (multiplies the delta time of each game frame)
// when full bullet time is active
const float BallBoostModel::MIN_TIME_DIALATION_FACTOR       = 0.060f;
const float BallBoostModel::INV_MIN_TIME_DIALATION_FACTOR   = 1.0f / MIN_TIME_DIALATION_FACTOR;

BallBoostModel::BallBoostModel(GameModel* gameModel) : 
ballBoostDir(0,0), numAvailableBoosts(0), gameModel(gameModel),
currState(NotInBulletTime), timeDialationAnim(1.0f), totalBulletTimeElapsed(0.0), currBoostChargeTime(0.0) {

    // Setup the initial states for the various animations...
    timeDialationAnim.SetInterpolantValue(1.0f);
    timeDialationAnim.SetRepeat(false);

}

BallBoostModel::~BallBoostModel() {
    this->gameModel->GetTransformInfo()->SetBulletTimeCamera(false);
    while (this->numAvailableBoosts > 0) {
        this->numAvailableBoosts--;

        // EVENT: Ball Boost lost
        GameEventManager::Instance()->ActionBallBoostLost();
    }
}

void BallBoostModel::Tick(double dT) {
    static bool animationDone = false;

    switch (this->currState) {

        case NotInBulletTime:
            this->totalBulletTimeElapsed = 0.0;
            
            // Increment the time towards the next boost and add any new boosts if
            // the charge time has been hit/exceeded
            if (this->numAvailableBoosts < TOTAL_NUM_BOOSTS) {
                this->currBoostChargeTime += dT;
                if (this->currBoostChargeTime >= BOOST_CHARGE_TIME_SECONDS) {
                    this->currBoostChargeTime = 0.0;
                    this->numAvailableBoosts++;
                    // EVENT: New ball boost gained
                    GameEventManager::Instance()->ActionBallBoostGained();
                }
            }
            
            break;

        case BulletTimeFadeIn:
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

        this->ballBoostDir[0] = x;
        this->ballBoostDir[1] = y;

        // Make sure the boost direction is normalized
        this->ballBoostDir.Normalize();
    }
    else {
        // Boosting is disallowed currently, make sure the members are properly set
        this->ballBoostDir[0]       = 0.0f;
        this->ballBoostDir[1]       = 0.0f;
        this->SetCurrentState(NotInBulletTime);
    }
}

void BallBoostModel::BallBoostDirectionReleased() {
    if (this->currState != NotInBulletTime) {
        // The player has decided to NOT boost at this time, clean up the about-to-boost state
        this->ballBoostDir[0] = 0.0f;
        this->ballBoostDir[1] = 0.0f;

        // End the bullet time state...
        this->SetCurrentState(BulletTimeFadeOut);
    }
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
    std::list<GameBall*>& balls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {

        GameBall* currBall = *iter;
        ballWasBoosted |= currBall->ExecuteBallBoost(this->ballBoostDir);
    }

    // Sanity: The ball should be allowed to boost!!
    assert(ballWasBoosted);
    // EVENT: Ball(s) have been boosted
    GameEventManager::Instance()->ActionBallBoostExecuted(*this);

    // Force a release of the boost direction and the entire boosting state
    this->BallBoostDirectionReleased();

    // A boost was just expended...
    this->numAvailableBoosts--;
    assert(this->numAvailableBoosts >= 0);
    // EVENT: Ball Boost lost
    GameEventManager::Instance()->ActionBallBoostLost();

    // End the bullet time state...
    this->SetCurrentState(BulletTimeFadeOut);

    return ballWasBoosted;
}

/**
 * Determines whether any ball in the game is available to be boosted.
 * Returns: true if there's a ball that can be boosted, false otherwise.
 */
bool BallBoostModel::IsBallAvailableForBoosting() const {
    // No boosting allowed if ball camera or paddle camera is active
    if (gameModel->GetPlayerPaddle()->GetIsPaddleCameraOn() || GameBall::GetIsBallCameraOn()) {
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