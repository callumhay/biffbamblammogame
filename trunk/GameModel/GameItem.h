/**
 * GameItem.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMEITEM_H__
#define __GAMEITEM_H__

#include "../BlammoEngine/IPositionObject.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "GameEventManager.h"

class PlayerPaddle;
class GameModel;
class GameItemTimer;
class GameBall;
class LevelPiece;

class GameItem : public IPositionObject {
public:
	enum ItemDisposition { Good, Bad, Neutral };
	enum ItemType { BallSafetyNetItem = 0, 
                    BallGrowItem, 
                    BallShrinkItem, 
                    BlackoutItem, 
                    GhostBallItem, 
                    InvisiBallItem, 
                    LaserBulletPaddleItem, 
                    LaserBeamPaddleItem, 
                    MultiBall3Item, 
                    MultiBall5Item, 
                    LifeUpItem, 
                    PaddleCamItem, 
                    BallCamItem, 
                    PaddleGrowItem, 
                    PaddleShrinkItem, 
                    PoisonPaddleItem, 
                    StickyPaddleItem, 
                    UberBallItem, 
                    UpsideDownItem, 
                    BallSpeedUpItem, 
                    BallSlowDownItem, 
                    GravityBallItem, 
                    RocketPaddleItem, 
                    CrazyBallItem, 
                    ShieldPaddleItem,
                    FireBallItem,
                    IceBallItem,
                    OmniLaserBallItem,
                    InvisiPaddleItem,
                    MagnetPaddleItem,
                    MineLauncherPaddleItem,
                    RemoteCtrlRocketItem,
                    FlameBlasterPaddleItem,
                    IceBlasterPaddleItem,

                    RandomItem // MAKE SURE RANDOM ITEM IS ALWAYS THE LAST ONE!!
	};

	// Height and Width for items
	static const float ITEM_WIDTH;
	static const float ITEM_HEIGHT;
	static const float HALF_ITEM_WIDTH;
	static const float HALF_ITEM_HEIGHT;

	static const float ALPHA_ON_PADDLE_CAM;
	static const float ALPHA_ON_BALL_CAM;
	
	virtual ~GameItem();

    virtual std::set<const GameBall*> GetBallsAffected() const {
        // By default no balls are affected...
        return std::set<const GameBall*>();
    }

	// For obtaining the name of this item
	const std::string& GetName() const {
		return this->name;
	}

	// For obtaining the center of this item
	const Point2D& GetCenter() const {
		return this->center;
	}

    // Inherited from the IPositionObject interface
    Point3D GetPosition3D() const {
        return Point3D(this->GetCenter(), 0.0f);
    }

	// For obtaining the type of item (e.g., power-up, power-down, ...)
	const GameItem::ItemDisposition& GetItemDisposition() const {
		return this->disposition;
	}

	virtual GameItem::ItemType GetItemType() const = 0;

	// Item colour set/get functions
	const ColourRGBA& GetItemColour() const {
		return this->colour;
	}
	void SetItemColour(const Colour& c) {
		this->colour = ColourRGBA(c, this->colour.A());
	}
	void SetItemVisiblity(float alpha) {
		this->colour[3] = alpha;
	}
	void AnimateItemFade(float endAlpha, double duration);

	void Tick(double seconds, const GameModel& model);
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

protected:
	std::string name;            // Name of this item
	GameModel* gameModel;        // Items have to be able to manipulate what happens in the game...
	Point2D center;              // The center x,y coordinate that this item is located at
	ItemDisposition disposition; // The disposition of the item (e.g., power-up, power-down, ...), essentially if it's good or bad for the player
	bool isActive;               // Whether or not this item is currently active (i.e., has been acquired and is effecting the game play)
	Vector2D currVelocityDir;    // The direction that this item is moving in

	ColourRGBA colour;                          // Colour multiply of the item
	AnimationLerp<ColourRGBA> colourAnimation;	// Animations associated with the colour
	
    std::set<const LevelPiece*> portalsEntered; // Keep track of all the portals that this item has teleported through

	// Speed of descent for items
	static const float SPEED_OF_DESCENT;

	GameItem(const std::string& name, const Point2D &spawnOrigin, GameModel *gameModel, const GameItem::ItemDisposition disp);

	void SetItemDisposition(const ItemDisposition& d) {
		this->disposition = d;
	}
	void SetItemName(const std::string& n) {
		this->name = n;
	}
};

inline std::ostream& operator <<(std::ostream& os, const GameItem& item) {
  return os << item.name;
};
#endif
