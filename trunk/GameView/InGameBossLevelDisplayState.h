/**
 * InGameBossLevelDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __INGAMEBOSSLEVELDISPLAYSTATE_H__
#define __INGAMEBOSSLEVELDISPLAYSTATE_H__

#include "InGameDisplayState.h"

#include "../BlammoEngine/Animation.h"

/**
 * This class is used as the in-game state instead of the "InGameDisplayState"
 * when the level has a boss in it. This state is intended to accomodate all of
 * the special/added effects for boss intro/outro and victory.
 */
class InGameBossLevelDisplayState : public InGameDisplayState {
public:
	InGameBossLevelDisplayState(GameDisplay* display);
	~InGameBossLevelDisplayState();

    void RenderFrame(double dT);
    DisplayState::DisplayStateType GetType() const;

private:
    static const double TIME_OF_UNPAUSE_BEFORE_INTRO_END;
    static const double FADE_TO_BLACK_TIME_IN_SECS;
    static const double WAIT_TIME_AT_END_OF_OUTRO_IN_SECS;

    // State-related variables
    enum BossState { IntroBossState, FadeInBossState, InPlayBossState, OutroBossState, VictoryBossState };
    BossState currBossState;
    
    // IntroBossState
    double introCountdown;
    // FadeInBossState
    double fadeInCountdown;
    // OutroBossState;
    double outroFinishCountdown;
    AnimationLerp<float> fadeObjectsAnim;
    AnimationLerp<float> paddlePosGetTheHellOutAnim;

    void SetBossState(BossState newState);
    void RenderBossState(double dT);

    void ExecuteIntroBossState(double dT);
    void ExecuteFadeInBossState(double dT);
    void ExecuteInPlayBossState(double dT);
    void ExecuteOutroBossState(double dT);
    void ExecuteVictoryBossState(double dT);
    
    DISALLOW_COPY_AND_ASSIGN(InGameBossLevelDisplayState);
};

inline DisplayState::DisplayStateType InGameBossLevelDisplayState::GetType() const {
    return DisplayState::InGameBossLevel;
}

#endif // __INGAMEBOSSLEVELDISPLAYSTATE_H__