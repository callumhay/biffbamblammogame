/**
 * BallOnPaddleState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BALLONPADDLESTATE_H__
#define __BALLONPADDLESTATE_H__

#include "GameState.h"

class GameBall;

class BallOnPaddleState : public GameState {
public:
    static const double START_RELEASE_TIMER_TIME_IN_SECS;
    static const double TOTAL_RELEASE_TIMER_TIME_IN_SECS;

	BallOnPaddleState(GameModel* gm);
	~BallOnPaddleState();

    double GetReleaseTimerPercentDone() const;

	GameState::GameStateType GetType() const {
		return GameState::BallOnPaddleStateType;
	}
	void Tick(double seconds);
	void BallReleaseKeyPressed();

    void ToggleAllowPaddleBallLaunching(bool allow);

private:
	bool firstTick;
    bool releaseTimerStarted;
    double releaseTimerCounter;

	void UpdateBallPosition();

	GameBall* GetGameBall();

    DISALLOW_COPY_AND_ASSIGN(BallOnPaddleState);
};

inline double BallOnPaddleState::GetReleaseTimerPercentDone() const {
    return std::max<double>(0.0, this->releaseTimerCounter - START_RELEASE_TIMER_TIME_IN_SECS) / 
        (TOTAL_RELEASE_TIMER_TIME_IN_SECS - START_RELEASE_TIMER_TIME_IN_SECS);
}

#endif