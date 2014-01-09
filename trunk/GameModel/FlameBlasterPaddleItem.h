/**
 * FlameBlasterPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __FLAMEBLASTERPADDLEITEM_H__
#define __FLAMEBLASTERPADDLEITEM_H__

#include "GameItem.h"

class FlameBlasterPaddleItem : public GameItem {
public:
    static const char* FLAMETHROWER_PADDLE_ITEM_NAME;
    static const double FLAMETHROWER_PADDLE_TIMER_IN_SECS;

    FlameBlasterPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
    ~FlameBlasterPaddleItem();

    double Activate();
    void Deactivate();

    GameItem::ItemType GetItemType() const {
        return GameItem::FlameBlasterPaddleItem;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(FlameBlasterPaddleItem);
};

#endif // __FLAMEBLASTERPADDLEITEM_H__