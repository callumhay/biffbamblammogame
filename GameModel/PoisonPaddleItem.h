/**
 * PoisonPaddleItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __POISONPADDLEITEM_H__
#define __POISONPADDLEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "GameItem.h"
#include "GameModel.h"

/**
 * Item that will 'poison' the paddle - has all kinds of terrible effects on the
 * player and their ability to control the paddle. This effect has a timer.
 */
class PoisonPaddleItem : public GameItem {
public:
	static const std::string POISON_PADDLE_ITEM_NAME;
	static const double POISON_PADDLE_TIMER_IN_SECS;

	PoisonPaddleItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~PoisonPaddleItem();

	virtual double Activate();
	virtual void Deactivate();
};
#endif