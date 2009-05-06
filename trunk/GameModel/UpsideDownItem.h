#ifndef __UPSIDEDOWNITEM_H__
#define __UPSIDEDOWNITEM_H__

#include "../BlammoEngine/BlammoEngine.h"

#include "GameItem.h"
#include "GameModel.h"

/**
 * Power-down item responsible for turning the level upside down.
 */
class UpsideDownItem : public GameItem {

public:
	static const std::string UPSIDEDOWN_ITEM_NAME;
	static const double UPSIDEDOWN_TIMER_IN_SECS;

	UpsideDownItem(const Point2D &spawnOrigin, GameModel *gameModel);
	virtual ~UpsideDownItem();

	virtual double Activate();
	virtual void Deactivate();

};
#endif