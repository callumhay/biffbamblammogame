/**
 * IceBlasterPaddleItem.cpp
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

#include "IceBlasterPaddleItem.h"
#include "GameModel.h"
#include "GameItemTimer.h"

const char* IceBlasterPaddleItem::ICE_BLASTER_PADDLE_ITEM_NAME      = "IceBlasterPaddle";
const double IceBlasterPaddleItem::ICE_BLASTER_PADDLE_TIMER_IN_SECS = 17.0f;

IceBlasterPaddleItem::IceBlasterPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(IceBlasterPaddleItem::ICE_BLASTER_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

IceBlasterPaddleItem::~IceBlasterPaddleItem() {
}

double IceBlasterPaddleItem::Activate() {
    this->isActive = true;
    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    // Kill other ice blaster timers
    std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
    for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
        GameItemTimer* currTimer = *iter;

        // Remove the fire blaster timers from the list of active timers
        if (currTimer->GetTimerItemType() == GameItem::IceBlasterPaddleItem) {
            iter = activeTimers.erase(iter);
            delete currTimer;
            currTimer = NULL;
        }
        else if (currTimer->GetTimerItemType() == GameItem::FlameBlasterPaddleItem) {

            // EVENT: A fire blaster is being canceled by an ice blaster
            GameEventManager::Instance()->ActionPaddleFireBlasterCanceledByIceBlaster(*this->gameModel->GetPlayerPaddle());

            // If there's a fire blaster item going right now then the effects just cancel each other out
            iter = activeTimers.erase(iter);
            delete currTimer;
            currTimer = NULL;

            return 0.0;
        }
        else {
            ++iter;
        }
    }

    // Activate a new paddle flamethrower item
    paddle->AddPaddleType(PlayerPaddle::IceBlasterPaddle);

    GameItem::Activate();
    return IceBlasterPaddleItem::ICE_BLASTER_PADDLE_TIMER_IN_SECS;
}

void IceBlasterPaddleItem::Deactivate() {
    if (!this->isActive) {
        return;
    }

    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    paddle->RemovePaddleType(PlayerPaddle::IceBlasterPaddle);
    this->isActive = false;
    GameItem::Deactivate();
}