/**
 * LevelStartState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LEVELSTARTSTATE_H__
#define __LEVELSTARTSTATE_H__

#include "GameState.h"
#include "GameModel.h"

class LevelStartState : public GameState {

public:
    LevelStartState(GameModel* gm) : GameState(gm) {
    }
    ~LevelStartState() {}

	GameState::GameStateType GetType() const {
		return GameState::LevelStartStateType;
	}

    void Tick(double seconds) {
        UNUSED_PARAMETER(seconds);

	    // Reset the multiplier and the max number of blocks destroyed
	    this->gameModel->SetNumInterimBlocksDestroyed(0);
        this->gameModel->SetMaxConsecutiveBlocksDestroyed(0);

        // Reset the score
        this->gameModel->ResetScore();

        // Reset other stats for a level
        this->gameModel->ResetNumAcquiredItems();
        this->gameModel->ResetLevelTime();

        this->gameModel->livesAtStartOfLevel = this->gameModel->GetLivesLeft();

        // Place the ball back on the paddle, and let the level begin!
        this->gameModel->SetNextState(GameState::BallOnPaddleStateType);
    }

	void BallReleaseKeyPressed() { /* does nothing */ } 
};

#endif // __LEVELSTARTSTATE_H__