/**
 * BallBoostModel.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
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

/**
 * Encapsulates the model/state of the ball boost for the GameModel.
 */
class BallBoostModel {
public:
    BallBoostModel(const std::list<GameBall*>* balls);
    ~BallBoostModel();

    void Tick(double dT);

    // Events that indicate changes to the boost model's state - based on input from the player
    void BallBoostDirectionPressed(float x, float y);
    void BallBoostDirectionReleased();
    bool BallBoosterPressed();

    float GetTimeDialationFactor() const;
    const Vector2D& GetBallBoostDirection() const;
    const Collision::AABB2D& GetBallZoomBounds() const;

    int GetNumBallsAllowedToBoost() const;
    bool IsBoostAvailable() const;
    bool IsBallAvailableForBoosting() const;

private:
    static const float MIN_TIME_DIALATION_FACTOR;
    const std::list<GameBall*>* balls;

    int numAvailableBoosts;

    bool isBallBoostDirPressed;
    Vector2D ballBoostDir;         // The direction to boost the ball in when the player triggers it - NOT necessarily normalized!
    float timeDialationFactor;     // Used to slow time down 'bullet-time', when the ball boost power is activated

    Collision::AABB2D ballZoomBounds;   // The 2D rectangle that holds all balls when bullet-time is activated for a ball boost

    void RecalculateBallZoomBounds();
        
    DISALLOW_COPY_AND_ASSIGN(BallBoostModel);
};

inline void BallBoostModel::Tick(double dT) {
    UNUSED_PARAMETER(dT);
    // TODO...
}

/**
 * Gets the multiplier of the current dT per tick of the game - this can add a 'bullet-time'
 * like effect to the game.
 */
inline float BallBoostModel::GetTimeDialationFactor() const {
    return this->timeDialationFactor;
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
 * Gets the number of balls that could boost currently.
 * Returns: The number of boostable balls in play.
 */
inline int BallBoostModel::GetNumBallsAllowedToBoost() const {
    int count = 0;
    for (std::list<GameBall*>::const_iterator iter = this->balls->begin(); iter != this->balls->end(); ++iter) {
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

#endif // __BALLBOOSTMODEL_H__