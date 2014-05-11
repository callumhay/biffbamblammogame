/**
 * BlackoutItem.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#include "BlackoutItem.h"

const char* BlackoutItem::BLACKOUT_ITEM_NAME	      = "Blackout";
const double BlackoutItem::BLACKOUT_TIMER_IN_SECS		= 20.0;

BlackoutItem::BlackoutItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) :
GameItem(BlackoutItem::BLACKOUT_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Bad) {
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