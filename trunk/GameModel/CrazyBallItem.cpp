/**
 * CrazyBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CrazyBallItem.h"

const char* CrazyBallItem::CRAZY_BALL_ITEM_NAME       = "CrazyBall";
const double CrazyBallItem::CRAZY_BALL_TIMER_IN_SECS  = 28.0;

CrazyBallItem::CrazyBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(CRAZY_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

CrazyBallItem::~CrazyBallItem() {
}

double CrazyBallItem::Activate() {
	this->isActive = true;

    // Kill other crazy ball timers
    std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
    for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
        GameItemTimer* currTimer = *iter;
        if (currTimer->GetTimerItemType() == GameItem::CrazyBallItem) {
            iter = activeTimers.erase(iter);
            delete currTimer;
            currTimer = NULL;
        }
        else {
            ++iter;
        }
    }

	// Make all the balls crazy!
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
        GameBall* affectedBall = *iter;
	    assert(affectedBall != NULL);
    	
	    // Make the ball(s) crazy
	    affectedBall->AddBallType(GameBall::CrazyBall);
    }

	GameItem::Activate();
	return CrazyBallItem::CRAZY_BALL_TIMER_IN_SECS;
}

void CrazyBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

    // Make each ball not-so crazy again
    std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
        GameBall* currBall = *ballIter;
        assert(currBall != NULL);	
        currBall->RemoveBallType(GameBall::CrazyBall);
    }

	this->isActive = false;
	GameItem::Deactivate();
}