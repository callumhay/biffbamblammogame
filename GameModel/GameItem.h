#ifndef __GAMEITEM_H__
#define __GAMEITEM_H__

#include "../Utils/Includes.h"
#include "../Utils/Point.h"

class PlayerPaddle;
class GameModel;

class GameItem {

protected:
	GameModel* gameModel;	// Items have to be able to manipulate what happens in the game...

	Point2D center;	// The center x,y coord that this item is located at
	
	// Height and Width for items
	static const float ITEM_WIDTH;
	static const float ITEM_HEIGHT;
	static const float HALF_ITEM_WIDTH;
	static const float HALF_ITEM_HEIGHT;

	// Speed of descent for items
	static const float SPEED_OF_DESCENT;

public:
	GameItem(GameModel *gameModel);
	virtual ~GameItem();

	// For obtaining the center of this item
	Point2D GetCenter() const {
		return this->center;
	}

	void Tick(double seconds);
	bool CollisionCheck(const PlayerPaddle &paddle);

	virtual void Activate() = 0;

};
#endif
