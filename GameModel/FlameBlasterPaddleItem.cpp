/**
 * FlameBlasterPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "FlameBlasterPaddleItem.h"
#include "GameModel.h"
#include "GameItemTimer.h"

const char* FlameBlasterPaddleItem::FLAMETHROWER_PADDLE_ITEM_NAME      = "FlameBlasterPaddle";
const double FlameBlasterPaddleItem::FLAMETHROWER_PADDLE_TIMER_IN_SECS = 17.0f;

FlameBlasterPaddleItem::FlameBlasterPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(FlameBlasterPaddleItem::FLAMETHROWER_PADDLE_ITEM_NAME, spawnOrigin, gameModel, GameItem::Neutral) {
}

FlameBlasterPaddleItem::~FlameBlasterPaddleItem() {
}

double FlameBlasterPaddleItem::Activate() {
    this->isActive = true;
    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
    assert(paddle != NULL);

    // Remove all currently active paddle flamethrower items
    std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
    for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
        GameItemTimer* currTimer = *iter;
        if (currTimer->GetTimerItemType() == GameItem::FlameBlasterPaddleItem) {
            iter = activeTimers.erase(iter);
            delete currTimer;
            currTimer = NULL;
        }
        else {
            ++iter;
        }
    }

    // Activate a new paddle flamethrower item
    paddle->AddPaddleType(PlayerPaddle::FlameBlasterPaddle);

    GameItem::Activate();
    return FlameBlasterPaddleItem::FLAMETHROWER_PADDLE_TIMER_IN_SECS;
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