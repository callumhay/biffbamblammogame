#include "GameItemTimer.h"


GameItemTimer::GameItemTimer(GameItem* gameItem, const TimerType type, const double totalLengthInSecs) : 
timeElapsedInSecs(0.0), timeLengthInSecs(totalLengthInSecs), type(type), assocGameItem(gameItem){
	assert(gameItem != NULL);
	assert(type != NoTimer || totalLengthInSecs == 0); // (type == NoTimer) implies (totalLengthInSecs == 0)
}

GameItemTimer::~GameItemTimer() {
	assert(this->assocGameItem != NULL);
	delete this->assocGameItem;
	this->assocGameItem = NULL;
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
		this->timeElapsedInSecs = this->timeLengthInSecs;
		this->assocGameItem->Deactivate();
	}
}