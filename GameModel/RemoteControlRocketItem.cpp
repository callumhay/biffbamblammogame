/**
 * RemoteControlRocketItem.cpp
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

#include "RemoteControlRocketItem.h"
#include "GameModel.h"
#include "PlayerPaddle.h"

const char* RemoteControlRocketItem::REMOTE_CONTROL_ROCKET_ITEM_NAME      = "RemoteControlRocket";
const double RemoteControlRocketItem::REMOTE_CONTROL_ROCKET_TIMER_IN_SECS = 0.0; // The rocket is not a timed power-up, it's activated when shot

RemoteControlRocketItem::RemoteControlRocketItem(const Point2D &spawnOrigin, const Vector2D& dropDir, GameModel *gameModel) :
GameItem(REMOTE_CONTROL_ROCKET_ITEM_NAME, spawnOrigin, dropDir, gameModel, GameItem::Good) {
}

RemoteControlRocketItem::~RemoteControlRocketItem () {
}

double RemoteControlRocketItem::Activate() {
    this->isActive = true;
    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    // Remove any other type of rocket on the paddle
    paddle->RemovePaddleType(PlayerPaddle::RocketPaddle);
    // Add the rocket for this item
    paddle->AddPaddleType(PlayerPaddle::RemoteControlRocketPaddle);

    // If the paddle is invisible then the rocket will have a special status of also being invisible...
    if (paddle->HasPaddleType(PlayerPaddle::InvisiPaddle)) {
        paddle->AddSpecialStatus(PlayerPaddle::InvisibleRocketStatus);
    }

    GameItem::Activate();
    return REMOTE_CONTROL_ROCKET_TIMER_IN_SECS;
}

void RemoteControlRocketItem::Deactivate() {
    if (!this->isActive) {
        return;
    }

    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    // Remove any invisible rocket status
    paddle->RemoveSpecialStatus(PlayerPaddle::InvisibleRocketStatus);

    this->isActive = false;
    GameItem::Deactivate();
}