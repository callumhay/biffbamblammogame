/**
 * BlackoutItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BlackoutItem.h"

const char* BlackoutItem::BLACKOUT_ITEM_NAME	      = "Blackout";
const double BlackoutItem::BLACKOUT_TIMER_IN_SECS		= 20.0;

BlackoutItem::BlackoutItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(BlackoutItem::BLACKOUT_ITEM_NAME, spawnOrigin, gameModel, GameItem::Bad) {
}

BlackoutItem::~BlackoutItem() {
}

double BlackoutItem::Activate() {
	this->isActive = true;

	// Get rid of any previous blackout effect timers...
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::BlackoutItem) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the blackout timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
			GameItemTimer* currTimer = removeTimers[i];
			activeTimers.remove(currTimer);
			delete currTimer;
			currTimer = NULL;
	}

	// Set the blackout effect anew
	this->gameModel->SetBlackoutEffect(true);

	GameItem::Activate();
	return BlackoutItem::BLACKOUT_TIMER_IN_SECS;
}

void BlackoutItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Remove the blackout effect
	this->gameModel->SetBlackoutEffect(false);

	this->isActive = false;
	GameItem::Deactivate();
}