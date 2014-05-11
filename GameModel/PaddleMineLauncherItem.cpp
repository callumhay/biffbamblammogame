/**
 * PaddleMineLauncherItem.cpp
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

#include "PaddleMineLauncherItem.h"
#include "GameModel.h"

const double PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_TIMER_IN_SECS	= 17.0;
const char* PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_ITEM_NAME	    = "MineLauncherPaddle";

PaddleMineLauncherItem::PaddleMineLauncherItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) : 
GameItem(PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Good) {
}

PaddleMineLauncherItem::~PaddleMineLauncherItem() {
}

double PaddleMineLauncherItem::Activate() {
	this->isActive = true;
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

	// Kill other mine launcher paddle timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();

	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::MineLauncherPaddleItem) {
			delete currTimer;
			currTimer = NULL;
			iter = activeTimers.erase(iter);
		}
		else {
			++iter;
		}
	}

	// Make the paddle have mine launching abilities
	paddle->AddPaddleType(PlayerPaddle::MineLauncherPaddle);
    
    // If the paddle is invisible then the mine will have a special status of also being invisible...
    if (paddle->HasPaddleType(PlayerPaddle::InvisiPaddle)) {
        paddle->AddSpecialStatus(PlayerPaddle::InvisibleMineStatus);
    }

	GameItem::Activate();
	return PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_TIMER_IN_SECS;
}

void PaddleMineLauncherItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    paddle->RemovePaddleType(PlayerPaddle::MineLauncherPaddle);
    
    // Remove any invisible mine status
    paddle->RemoveSpecialStatus(PlayerPaddle::InvisibleMineStatus);

    this->isActive = false;
	GameItem::Deactivate();
}