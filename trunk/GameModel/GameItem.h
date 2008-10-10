#ifndef __GAMEITEM_H__
#define __GAMEITEM_H__

#include <string>

#include "../Utils/Includes.h"
#include "../Utils/Point.h"

class PlayerPaddle;
class GameModel;
class GameItemTimer;

class GameItem {
public:
	enum ItemType { PowerUp, PowerDown };

protected:
	std::string name;			// Name of this item
	GameModel* gameModel;	// Items have to be able to manipulate what happens in the game...
	Point2D center;				// The center x,y coord that this item is located at
	ItemType type;				// The type of item (e.g., power-up, power-down, ...), essentially if it's good or bad for the player

	// Height and Width for items
	static const float ITEM_WIDTH;
	static const float ITEM_HEIGHT;
	static const float HALF_ITEM_WIDTH;
	static const float HALF_ITEM_HEIGHT;

	// Speed of descent for items
	static const float SPEED_OF_DESCENT;

public:
	GameItem(const std::string& name, const Point2D &spawnOrigin, GameModel *gameModel, const ItemType type);
	virtual ~GameItem();

	// For obtaining the name of this item
	std::string GetName() const {
		return this->name;
	}

	// For obtaining the center of this item
	Point2D GetCenter() const {
		return this->center;
	}

	// For obtaining the type of item (e.g., power-up, power-down, ...)
	ItemType GetItemType() const {
		return this->type;
	}

	void Tick(double seconds);
	bool CollisionCheck(const PlayerPaddle &paddle);

	virtual GameItemTimer* Activate() = 0;
	virtual void Deactivate() = 0;

	friend std::ostream& operator <<(std::ostream& os, const GameItem& item);
};

inline std::ostream& operator <<(std::ostream& os, const GameItem& item) {
  return os << item.name;
};
#endif
