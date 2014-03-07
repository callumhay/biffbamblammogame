/**
 * LaserBeamPaddleItem.cpp
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

#include "LaserBeamPaddleItem.h"
#include "GameModel.h"
#include "PaddleLaserBeam.h"

const double LaserBeamPaddleItem::LASER_BEAM_PADDLE_TIMER_IN_SECS	= 0.0; // The laser beam is not a timed power-up, it's activated when shot
const char* LaserBeamPaddleItem::LASER_BEAM_PADDLE_ITEM_NAME      = "LaserBeamPaddle";

LaserBeamPaddleItem::LaserBeamPaddleItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) : 
GameItem(LASER_BEAM_PADDLE_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Good) {
}

LaserBeamPaddleItem::~LaserBeamPaddleItem() {
}

double LaserBeamPaddleItem::Activate() {
	this->isActive = true;
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	// Kill other laser beam paddle timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::LaserBeamPaddleItem) {
			delete currTimer;
			currTimer = NULL;
			iter = activeTimers.erase(iter);
		}
		else {
			++iter;
		}
	}

	// Make the paddle have laser beam blasting abilities
	if (paddle->GetIsLaserBeamFiring()) {
		// Just add another beam if the beam is already firing - this will simply
		// extend the lifetime of the already firing beam
        PaddleLaserBeam* paddleLaserBeam = new PaddleLaserBeam(paddle, this->gameModel);
		this->gameModel->AddBeam(paddleLaserBeam);
	}
	
	paddle->AddPaddleType(PlayerPaddle::LaserBeamPaddle);

	GameItem::Activate();
	return LaserBeamPaddleItem::LASER_BEAM_PADDLE_TIMER_IN_SECS;
}

void LaserBeamPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}
	this->isActive = false;
	GameItem::Deactivate();
}