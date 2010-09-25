/**
 * UpsideDownItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "UpsideDownItem.h"
#include "GameTransformMgr.h"
#include "GameItemTimer.h"

const char* UpsideDownItem::UPSIDEDOWN_ITEM_NAME		  = "UpsideDown";
const double UpsideDownItem::UPSIDEDOWN_TIMER_IN_SECS = 20.0;

UpsideDownItem::UpsideDownItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(UpsideDownItem::UPSIDEDOWN_ITEM_NAME, spawnOrigin, gameModel, GameItem::Bad) {
}

UpsideDownItem::~UpsideDownItem() {
}

double UpsideDownItem::Activate() {
	this->isActive = true;

	unsigned int numItemsAlreadyActive = 0;

	// Kill all other upside down items currently activated
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::UpsideDownItem) {
			removeTimers.push_back(currTimer);
			numItemsAlreadyActive++;
		}
	}
	// Remove the upsidedown item timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
			GameItemTimer* currTimer = removeTimers[i];
			activeTimers.remove(currTimer);
			delete currTimer;
			currTimer = NULL;
	}

	// Flip the game upside down...
	GameTransformMgr* gameXFormInfo = this->gameModel->GetTransformInfo();
	gameXFormInfo->FlipGameUpsideDown();
	
	// Check to see if the game is actually flipped right-side up now
	if (numItemsAlreadyActive > 0) {
		assert(numItemsAlreadyActive == 1);
		// Re-flip the controls back to normal since we just cancelled the flip effect
		this->gameModel->SetFlippedControls(false);
		return GameItemTimer::ZERO_TIME_TIMER_IN_SECS;
	}
	else {
		// Only flip the controls if we are activating the item
		this->gameModel->SetFlippedControls(true);
	}

	GameItem::Activate();
	return UpsideDownItem::UPSIDEDOWN_TIMER_IN_SECS;
}

void UpsideDownItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Reflip the game...
	GameTransformMgr* gameXFormInfo = this->gameModel->GetTransformInfo();
	gameXFormInfo->FlipGameUpsideDown();
	this->gameModel->SetFlippedControls(false);

	this->isActive = false;
	GameItem::Deactivate();
}