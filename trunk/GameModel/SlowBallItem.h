#ifndef __SLOWBALLITEM_H__
#define __SLOWBALLITEM_H__

#include "GameItem.h"

class GameModel;

class SlowBallItem : public GameItem {

public:
	SlowBallItem(GameModel *gameModel);
	virtual ~SlowBallItem();

	virtual void Activate();

};
#endif