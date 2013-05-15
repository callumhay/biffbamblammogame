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

const char* CrazyBallItem::CRAZY_BALL_ITEM_NAME				= "CrazyBall";
const double CrazyBallItem::CRAZY_BALL_TIMER_IN_SECS	= 0.0;

CrazyBallItem::CrazyBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(CRAZY_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

CrazyBallItem::~CrazyBallItem() {
}

double CrazyBallItem::Activate() {
	this->isActive = true;

	// Make the last ball to hit the paddle into a crazy ball
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
        GameBall* affectedBall = *iter;
	    assert(affectedBall != NULL);
    	
	    // Make the ball crazy, but slow it down a bit as well (in order to accomodate the craziness)
	    affectedBall->AddBallType(GameBall::CrazyBall);
	    //affectedBall->DecreaseSpeed();
    }

	GameItem::Activate();
	return CrazyBallItem::CRAZY_BALL_TIMER_IN_SECS;
}

void CrazyBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}
	this->isActive = false;
	GameItem::Deactivate();
}