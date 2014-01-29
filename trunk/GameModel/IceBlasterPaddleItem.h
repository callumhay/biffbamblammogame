/**
 * IceBlasterPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ICEBLASTERPADDLEITEM_H__
#define __ICEBLASTERPADDLEITEM_H__

#include "GameItem.h"

class IceBlasterPaddleItem : public GameItem {
public:
    static const char* ICE_BLASTER_PADDLE_ITEM_NAME;
    static const double ICE_BLASTER_PADDLE_TIMER_IN_SECS;

    IceBlasterPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
    ~IceBlasterPaddleItem();

    double Activate();
    void Deactivate();

    GameItem::ItemType GetItemType() const {
        return GameItem::IceBlasterPaddleItem;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(IceBlasterPaddleItem);
};

#endif // __ICEBLASTERPADDLEITEM_H__