/**
 * MagnetPaddleItem.cpp
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

#include "MagnetPaddleItem.h"
#include "GameModel.h"

const char* MagnetPaddleItem::MAGNET_PADDLE_ITEM_NAME       = "MagnetPaddle";
const double MagnetPaddleItem::MAGNET_PADDLE_TIMER_IN_SECS  = 38.0;

MagnetPaddleItem::MagnetPaddleItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) :
GameItem(MagnetPaddleItem::MAGNET_PADDLE_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Neutral) {
}

MagnetPaddleItem::~MagnetPaddleItem() {
}

double MagnetPaddleItem::Activate() {
	this->isActive = true;

	// Remove any previous magnet paddle item power-up timers and replace it with this one
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::MagnetPaddleItem) {
			 removeTimers.push_back(currTimer);
		}
	}
	// Remove the magnet paddle timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
        GameItemTimer* currTimer = removeTimers[i];
        currTimer->SetDeactivateItemOnStop(false);
        activeTimers.remove(currTimer);
        delete currTimer;
        currTimer = NULL;
	}

	// Activate the magnet paddle
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    paddle->AddPaddleType(PlayerPaddle::MagnetPaddle);
	
	GameItem::Activate();
	return MagnetPaddleItem::MAGNET_PADDLE_TIMER_IN_SECS;
}

void MagnetPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}
	
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	paddle->RemovePaddleType(PlayerPaddle::MagnetPaddle);

	this->isActive = false;
	GameItem::Deactivate();
}