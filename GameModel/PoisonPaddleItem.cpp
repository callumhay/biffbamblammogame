/**
 * PoisonPaddleItem.cpp
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

#include "PoisonPaddleItem.h"

const char* PoisonPaddleItem::POISON_PADDLE_ITEM_NAME       = "PoisonPaddle";
const double PoisonPaddleItem::POISON_PADDLE_TIMER_IN_SECS	= 18.0f;

PoisonPaddleItem::PoisonPaddleItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) :
GameItem(PoisonPaddleItem::POISON_PADDLE_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Bad) {
}

PoisonPaddleItem::~PoisonPaddleItem() {
}

/**
 * Removes all other active poison items, re-poisons the paddle and
 * returns the length of the new poison timer.
 */
double PoisonPaddleItem::Activate() {
	this->isActive = true;

	// Get rid of all other active poison items
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::PoisonPaddleItem) {
			removeTimers.push_back(currTimer);
		}
	}
	
	// Remove the poison timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
	    GameItemTimer* currTimer = removeTimers[i];
	    activeTimers.remove(currTimer);
	    delete currTimer;
	    currTimer = NULL;
	}

	// Re-poison the paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->AddPaddleType(PlayerPaddle::PoisonPaddle);
	GameItem::Activate();

	return PoisonPaddleItem::POISON_PADDLE_TIMER_IN_SECS;
}

void PoisonPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// De-poisonify the paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->RemovePaddleType(PlayerPaddle::PoisonPaddle);

	this->isActive = false;
	GameItem::Deactivate();
}