#include "GameItemTimer.h"

GameItemTimer::GameItemTimer(GameItem* gameItem) : assocGameItem(gameItem), timeElapsedInSecs(0.0) {
	assert(gameItem != NULL);
	this->timeLengthInSecs = gameItem->Activate();
}

GameItemTimer::~GameItemTimer() {
	//assert(this->assocGameItem != NULL);
	if (this->assocGameItem != NULL) {
		delete this->assocGameItem;
		this->assocGameItem = NULL;
	}
}

/**
 * This will update the timer, i.e., make it tick away until all
 * its time has elapsed.
 */
void GameItemTimer::Tick(double seconds) {
	// Do nothing if time has already expired... 
	if (this->HasExpired()) {
		return;
	}

	this->timeElapsedInSecs += seconds;

	// This should only happen once - obviously when a timer expires
	// so must its associated game item's effect!
	if (this->HasExpired()) {
		this->StopTimer();
	}
}