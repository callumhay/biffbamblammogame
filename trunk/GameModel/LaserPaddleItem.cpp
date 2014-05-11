/**
 * LaserPaddleItem.cpp
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

#include "LaserPaddleItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double LaserPaddleItem::LASER_PADDLE_TIMER_IN_SECS = 15.0;
const char* LaserPaddleItem::LASER_PADDLE_ITEM_NAME      = "LaserBulletPaddle";

LaserPaddleItem::LaserPaddleItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) : 
GameItem(LASER_PADDLE_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Good) {
}

LaserPaddleItem::~LaserPaddleItem() {
}

double LaserPaddleItem::Activate() {
	this->isActive = true;
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	// Kill other laser paddle timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::LaserBulletPaddleItem) {
			removeTimers.push_back(currTimer);
		}
	}
	// Remove the laser paddle timers from the list of active timers
	for (int i = 0; i < static_cast<int>(removeTimers.size()); i++) {
	    GameItemTimer* currTimer = removeTimers[i];
	    activeTimers.remove(currTimer);
	    delete currTimer;
	    currTimer = NULL;
	}

	// Make the paddle have laser shooting abilities
	paddle->AddPaddleType(PlayerPaddle::LaserBulletPaddle);

	GameItem::Activate();
	return LaserPaddleItem::LASER_PADDLE_TIMER_IN_SECS;
}

void LaserPaddleItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);
	paddle->RemovePaddleType(PlayerPaddle::LaserBulletPaddle);
	this->isActive = false;
	GameItem::Deactivate();
}