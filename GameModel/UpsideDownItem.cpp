/**
 * UpsideDownItem.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "UpsideDownItem.h"
#include "GameTransformMgr.h"
#include "GameItemTimer.h"

const char* UpsideDownItem::UPSIDEDOWN_ITEM_NAME      = "UpsideDown";
const double UpsideDownItem::UPSIDEDOWN_TIMER_IN_SECS = 22.0;

UpsideDownItem::UpsideDownItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) :
GameItem(UpsideDownItem::UPSIDEDOWN_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Bad) {
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