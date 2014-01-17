/**
 * IceBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "IceBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double IceBallItem::ICE_BALL_TIMER_IN_SECS	= 22.0;
const char* IceBallItem::ICE_BALL_ITEM_NAME			  = "IceBall";

IceBallItem::IceBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(ICE_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

IceBallItem::~IceBallItem() {
}

double IceBallItem::Activate() {
	this->isActive = true;

	// Kill other fire ball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;

		// Remove the gravity ball timers from the list of active timers
		if (currTimer->GetTimerItemType() == GameItem::IceBallItem) {
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
		}
		else if (currTimer->GetTimerItemType() == GameItem::FireBallItem) {
			
            // EVENT: Fireball(s) are being cancelled by an iceball item
            const std::set<const GameBall*>& affectedBalls = currTimer->GetAssociatedBalls();
            for (std::set<const GameBall*>::const_iterator ballIter = affectedBalls.begin();
                 ballIter != affectedBalls.end(); ++ballIter) {

                const GameBall* currBall = *ballIter;
                assert(currBall != NULL);
                GameEventManager::Instance()->ActionFireBallCanceledByIceBall(*currBall);
            }            
            
            // If there's a fire ball item going right now then the effects just cancel each other out
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
			return 0.0;
		}
		else {
			++iter;
		}
	}

	// Apply to all balls...
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
    for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
	    GameBall* affectedBall = *iter;
	    assert(affectedBall != NULL);
	    affectedBall->AddBallType(GameBall::IceBall);
    }

	GameItem::Activate();
	return IceBallItem::ICE_BALL_TIMER_IN_SECS;
}

void IceBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Make each ball normal again
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);	
		currBall->RemoveBallType(GameBall::IceBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}