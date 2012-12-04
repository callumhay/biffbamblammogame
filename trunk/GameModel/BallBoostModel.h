/**
 * BallBoostModel.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BALLBOOSTMODEL_H__
#define __BALLBOOSTMODEL_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Collision.h"
#include "GameBall.h"
#include "GameModel.h"

/**
 * Encapsulates the model/state of the ball boost for the GameModel.
 */
class BallBoostModel {
public:
    enum BulletTimeState { NotInBulletTime, BulletTimeFadeIn, BulletTime, BulletTimeFadeOut };
    
    static const int TOTAL_NUM_BOOSTS = 3;
    static const double DEFAULT_BULLET_TIME_DURATION;

    static const float MIN_TIME_DIALATION_FACTOR;
    static const float INV_MIN_TIME_DIALATION_FACTOR;

    static const double BULLET_TIME_FADE_IN_SECONDS;
    static const double BULLET_TIME_FADE_OUT_SECONDS;

    static const double DEFAULT_BOOST_CHARGE_TIME_SECONDS;
    static const double LEVEL_ALMOST_COMPLETE_CHARGE_TIME_SECONDS;

    static void SetMaxBulletTimeDuration(double seconds);
    static double GetMaxBulletTimeDuration();

    BallBoostModel(GameModel* gameModel);
    ~BallBoostModel();

    void Tick(double dT);

    // Events that indicate changes to the boost model's state - based on input from the player
    void BallBoostDirectionPressed(float x, float y);
    void BallBoostDirectionReleased();
    bool BallBoosterPressed();

    float GetTimeDialationFactor() const;
    float GetInverseTimeDialation() const;
    const Vector2D& GetBallBoostDirection() const;
    const Collision::AABB2D& GetBallZoomBounds() const;

    size_t GetCurrentNumBalls() const;
    const std::list<GameBall*>& GetBalls() const;
    int GetNumBallsAllowedToBoost() const;
    bool IsBoostAvailable() const;
    bool IsBallAvailableForBoosting() const;

    bool IsInBulletTime() const;
    const BallBoostModel::BulletTimeState& GetBulletTimeState() const;
    double GetTotalBulletTimeElapsed() const;

    float GetBoostChargePercentage() const;
    int GetNumAvailableBoosts() const;

    void SetBoostChargeTime(double timeInSeconds);
    double GetBoostChargeTime() const;

    void SetInvertBallBoostDir(bool isInverted);

    void DebugDraw() const;

private:
    static double BULLET_TIME_MAX_DURATION_SECONDS;

    BulletTimeState currState;                  // The current bullet time state
    AnimationLerp<float> timeDialationAnim;     // Animation lerp for time dialation, changes based on state
    double totalBulletTimeElapsed;              // Counter for the total seconds of elapsed bullet time (in the BulletTime state)

    GameModel* gameModel;
    int numAvailableBoosts;             // The number of available boosts left for use by the player
    Vector2D ballBoostDir;              // The direction to boost the ball in when the player triggers it - NOT necessarily normalized!
    Collision::AABB2D ballZoomBounds;   // The 2D rectangle that holds all balls when bullet-time is activated for a ball boost

    double elapsedBoostChargeTime;  // Total time elapsed (in seconds) towards charging the next boost
    double boostChargeTime;         // Currently set amount of time it takes for a boost to be gained

    bool isBallBoostInverted;  // Whether ball boosting controls are inverted or not

    void SetCurrentState(const BulletTimeState& newState);
    void RecalculateBallZoomBounds();
        
    DISALLOW_COPY_AND_ASSIGN(BallBoostModel);
};

inline void BallBoostModel::SetMaxBulletTimeDuration(double seconds) {
    assert(seconds > 0.0);
    BallBoostModel::BULLET_TIME_MAX_DURATION_SECONDS = seconds;
}

inline double BallBoostModel::GetMaxBulletTimeDuration() {
    return BallBoostModel::BULLET_TIME_MAX_DURATION_SECONDS;
}

/**
 * Gets the multiplier of the current dT per tick of the game - this can add a 'bullet-time'
 * like effect to the game.
 */
inline float BallBoostModel::GetTimeDialationFactor() const {
    return this->timeDialationAnim.GetInterpolantValue();
}

inline float BallBoostModel::GetInverseTimeDialation() const {
    return 1.0f / this->timeDialationAnim.GetInterpolantValue();
}

/**
 * Gets the direction that the ball is being told to boost towards.
 */
inline const Vector2D& BallBoostModel::GetBallBoostDirection() const {
    return this->ballBoostDir;
}

/**
 * Get the maximum extents of all balls in play.
 */
inline const Collision::AABB2D& BallBoostModel::GetBallZoomBounds() const {
    return this->ballZoomBounds;
}

/**
 * Get the total number of balls currently in play.
 */
inline size_t BallBoostModel::GetCurrentNumBalls() const {
    return this->gameModel->GetGameBalls().size();
}

/**
 * Gets the list of balls that will be / are being boosted by this.
 */
inline const std::list<GameBall*>& BallBoostModel::GetBalls() const {
    return this->gameModel->GetGameBalls();
}

/**
 * Gets the number of balls that could boost currently.
 * Returns: The number of boostable balls in play.
 */
inline int BallBoostModel::GetNumBallsAllowedToBoost() const {
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
 * Gets whether any boosts are available for the ball.
 * Returns: true if there are boosts available, false otherwise.
 */
inline bool BallBoostModel::IsBoostAvailable() const {
    return (this->numAvailableBoosts > 0);
}

inline bool BallBoostModel::IsInBulletTime() const {
    return this->currState != BallBoostModel::NotInBulletTime;
}

/**
 * Get the current bullet time state.
 */
inline const BallBoostModel::BulletTimeState& BallBoostModel::GetBulletTimeState() const {
    return this->currState;
}

/**
 * Get the total time in seconds that bullet time has been active for.
 */
inline double BallBoostModel::GetTotalBulletTimeElapsed() const {
    return this->totalBulletTimeElapsed;
}

/**
 * Get the percentage towards the next boost.
 */
inline float BallBoostModel::GetBoostChargePercentage() const {
    if (this->numAvailableBoosts == TOTAL_NUM_BOOSTS) {
        return 1.0f;
    }
    return static_cast<float>(this->elapsedBoostChargeTime / this->boostChargeTime);
}

/**
 * Get the number of currently available boosts.
 */
inline int BallBoostModel::GetNumAvailableBoosts() const {
    return this->numAvailableBoosts;
}

inline void BallBoostModel::SetBoostChargeTime(double timeInSeconds) {
    this->boostChargeTime = timeInSeconds;
}

inline double BallBoostModel::GetBoostChargeTime() const {
    return this->boostChargeTime;
}

inline void BallBoostModel::SetInvertBallBoostDir(bool isInverted) {
    this->isBallBoostInverted = isInverted;
}

#endif // __BALLBOOSTMODEL_H__