/**
 * FlameBlasterPaddleItem.cpp
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

#include "FlameBlasterPaddleItem.h"
#include "GameModel.h"
#include "GameItemTimer.h"

const char* FlameBlasterPaddleItem::FLAME_BLASTER_PADDLE_ITEM_NAME      = "FlameBlasterPaddle";
const double FlameBlasterPaddleItem::FLAME_BLASTER_PADDLE_TIMER_IN_SECS = 17.0f;

FlameBlasterPaddleItem::FlameBlasterPaddleItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) :
GameItem(FlameBlasterPaddleItem::FLAME_BLASTER_PADDLE_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Neutral) {
}

FlameBlasterPaddleItem::~FlameBlasterPaddleItem() {
}

double FlameBlasterPaddleItem::Activate() {
    this->isActive = true;
    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);


    // Kill other fire blaster timers
    std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
    for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
        GameItemTimer* currTimer = *iter;

        // Remove the fire blaster timers from the list of active timers
        if (currTimer->GetTimerItemType() == GameItem::FlameBlasterPaddleItem) {
            iter = activeTimers.erase(iter);
            delete currTimer;
            currTimer = NULL;
        }
        else if (currTimer->GetTimerItemType() == GameItem::IceBlasterPaddleItem) {

            // EVENT: An ice blaster is being canceled by a fire blaster 
            GameEventManager::Instance()->ActionPaddleIceBlasterCanceledByFireBlaster(*this->gameModel->GetPlayerPaddle());

            // If there's an ice blaster item going right now then the effects just cancel each other out
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
    paddle->AddPaddleType(PlayerPaddle::FlameBlasterPaddle);

    GameItem::Activate();
    return FlameBlasterPaddleItem::FLAME_BLASTER_PADDLE_TIMER_IN_SECS;
}

void FlameBlasterPaddleItem::Deactivate() {
    if (!this->isActive) {
        return;
    }

    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);
    paddle->RemovePaddleType(PlayerPaddle::FlameBlasterPaddle);
    this->isActive = false;
    GameItem::Deactivate();
}