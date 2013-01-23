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
    // State-related variables
    enum BossState { IntroBossState, FadeInBossState, InPlayBossState, VictoryBossState };
    BossState currBossState;
    
    // IntroBossState
    double introCountdown;
    // FadeInBossState
    double fadeInCountdown;


    void SetBossState(BossState newState);
    void RenderBossState(double dT);

    void ExecuteIntroBossState(double dT);
    void ExecuteFadeInBossState(double dT);
    
    DISALLOW_COPY_AND_ASSIGN(InGameBossLevelDisplayState);
};

inline DisplayState::DisplayStateType InGameBossLevelDisplayState::GetType() const {
    return DisplayState::InGameBossLevel;
}

#endif // __INGAMEBOSSLEVELDISPLAYSTATE_H__