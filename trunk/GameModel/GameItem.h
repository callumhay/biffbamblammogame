#ifndef __GAMEITEM_H__
#define __GAMEITEM_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "GameEventManager.h"

class PlayerPaddle;
class GameModel;
class GameItemTimer;
class GameBall;

class GameItem {
public:
	enum ItemDisposition { Good, Bad, Neutral };
	enum ItemType { BallSafetyNetItem, BallGrowItem, BallShrinkItem, BlackoutItem, GhostBallItem, 
									InvisiBallItem, LaserBulletPaddleItem, MultiBall3Item, MultiBall5Item, 
									OneUpItem, PaddleCamItem, PaddleGrowItem, PaddleShrinkItem, PoisonPaddleItem, 
									StickyPaddleItem, UberBallItem, UpsideDownItem, BallSpeedUpItem, BallSlowDownItem };

protected:
	std::string name;							// Name of this item
	GameModel* gameModel;					// Items have to be able to manipulate what happens in the game...
	Point2D center;								// The center x,y coord that this item is located at
	ItemDisposition disposition;	// The disposition of the item (e.g., power-up, power-down, ...), essentially if it's good or bad for the player
	bool isActive;								// Whether or not this item is currently active (i.e., has been accquired and is effecting the game play)
	
	ColourRGBA colour;													// Colour multiply of the item
	AnimationLerp<ColourRGBA> colourAnimation;	// Animations associated with the colour
	

	// Speed of descent for items
	static const float SPEED_OF_DESCENT;

public:
	// Height and Width for items
	static const float ITEM_WIDTH;
	static const float ITEM_HEIGHT;
	static const float HALF_ITEM_WIDTH;
	static const float HALF_ITEM_HEIGHT;

	static const float ALPHA_ON_PADDLE_CAM;

	GameItem(const std::string& name, const Point2D &spawnOrigin, GameModel *gameModel, const GameItem::ItemDisposition disp);
	virtual ~GameItem();

	virtual const GameBall* GetBallAffected() const {
		return NULL;
	}

	// For obtaining the name of this item
	std::string GetName() const {
		return this->name;
	}

	// For obtaining the center of this item
	Point2D GetCenter() const {
		return this->center;
	}

	// For obtaining the type of item (e.g., power-up, power-down, ...)
	GameItem::ItemDisposition GetItemDisposition() const {
		return this->disposition;
	}

	virtual GameItem::ItemType GetItemType() const = 0;

	// Item colour set/get functions
	ColourRGBA GetItemColour() const {
		return this->colour;
	}
	void SetItemColour(const Colour& c) {
		this->colour = ColourRGBA(c, this->colour.A());
	}
	void SetItemVisiblity(float alpha) {
		this->colour[3] = alpha;
	}
	void AnimateItemFade(float endAlpha, double duration);

	void Tick(double seconds);
	bool CollisionCheck(const PlayerPaddle &paddle);

	// Returns the timer length for this item
	virtual double Activate(){
		// Raise an event for this item being activated...
		GameEventManager::Instance()->ActionItemActivated(*this);
		return -1;
	}	
	virtual void Deactivate() {
		// Raise an event for this item being deactivated
		GameEventManager::Instance()->ActionItemDeactivated(*this);
	}

	friend std::ostream& operator <<(std::ostream& os, const GameItem& item);
};

inline std::ostream& operator <<(std::ostream& os, const GameItem& item) {
  return os << item.name;
};
#endif
