/**
 * OmniLaserBallItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "OmniLaserBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const char* OmniLaserBallItem::OMNI_LASER_BALL_ITEM_NAME      = "OmniLaserBall";
const double OmniLaserBallItem::OMNI_LASER_BALL_TIMER_IN_SECS = 20.0;

OmniLaserBallItem::OmniLaserBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(OMNI_LASER_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

OmniLaserBallItem::~OmniLaserBallItem() {
}

double OmniLaserBallItem::Activate() {
	this->isActive = true;

	// Kill other omni laser ball timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::OmniLaserBallItem) {
            iter = activeTimers.erase(iter);
            delete currTimer;
            currTimer = NULL;
		}
        else {
            ++iter;
        }
	}

	// Make the ball have omni-laser shooting abilities
    GameBall* ball = this->GetBallAffected();
    assert(ball != NULL);
    ball->AddBallType(GameBall::OmniLaserBulletBall);

	GameItem::Activate();
	return OmniLaserBallItem::OMNI_LASER_BALL_TIMER_IN_SECS;
}

void OmniLaserBallItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Make each ball normal again
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);	
		currBall->RemoveBallType(GameBall::OmniLaserBulletBall);
	}

	this->isActive = false;
	GameItem::Deactivate();
}