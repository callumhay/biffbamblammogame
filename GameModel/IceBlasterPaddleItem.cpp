/**
 * IceBlasterPaddleItem.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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