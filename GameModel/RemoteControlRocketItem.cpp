/**
 * RemoteControlRocketItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "RemoteControlRocketItem.h"
#include "GameModel.h"
#include "PlayerPaddle.h"

const char* RemoteControlRocketItem::REMOTE_CONTROL_ROCKET_ITEM_NAME      = "RemoteControlRocket";
const double RemoteControlRocketItem::REMOTE_CONTROL_ROCKET_TIMER_IN_SECS = 0.0; // The rocket is not a timed power-up, it's activated when shot

RemoteControlRocketItem::RemoteControlRocketItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(REMOTE_CONTROL_ROCKET_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
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

    GameItem::Activate();
    return REMOTE_CONTROL_ROCKET_TIMER_IN_SECS;
}

void RemoteControlRocketItem::Deactivate() {
    if (!this->isActive) {
        return;
    }
    this->isActive = false;
    GameItem::Deactivate();
}