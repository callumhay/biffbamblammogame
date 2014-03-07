/**
 * BallSafetyNetItem.cpp
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

#include "BallSafetyNetItem.h"
#include "GameLevel.h"
#include "GameEventManager.h"

const char* BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME	  = "BallSafetyNet";
const double BallSafetyNetItem::BALL_SAFETY_NET_TIMER_IN_SECS = 0.0;

BallSafetyNetItem::BallSafetyNetItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) :
GameItem(BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Good) {
}

BallSafetyNetItem::~BallSafetyNetItem() {
}

double BallSafetyNetItem::Activate() {
	this->isActive = true;

	// Add the ball safety net to the level
    if (this->gameModel->ActivateSafetyNet()) {
		// We activated the game item (wasn't active before)
		GameItem::Activate();
    }

	return BallSafetyNetItem::BALL_SAFETY_NET_TIMER_IN_SECS;
}

void BallSafetyNetItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	this->isActive = false;
	GameItem::Deactivate();
}