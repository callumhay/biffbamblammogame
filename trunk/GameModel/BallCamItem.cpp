#include "BallCamItem.h"
#include "GameModel.h"

#include "../BlammoEngine/BasicIncludes.h"

const std::string BallCamItem::BALL_CAM_ITEM_NAME	= "EyeBall";
const double BallCamItem::BALL_CAM_TIMER_IN_SECS	= 20.0;

BallCamItem::BallCamItem(const Point2D &spawnOrigin, GameModel *gameModel) :
GameItem(BallCamItem::BALL_CAM_ITEM_NAME, spawnOrigin, gameModel, GameItem::Bad) {
}

BallCamItem::~BallCamItem() {
}

double BallCamItem::Activate() {
	this->isActive = true;

	// Deactivate any currently active paddle camera items and any other associated viewpoint changing items
	// TODO: DONT FORGET ABOUT THIS (SEE COMMENT ABOVE) !!!
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == GameItem::BallCamItem || currTimer->GetTimerItemType() == GameItem::PaddleCamItem) {
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
		}
		else {
			 iter++;
		}
	}

	// Deactivate the paddle camera effect
	this->gameModel->GetTransformInfo()->SetBallCamera(true);
	GameItem::Activate();

	return BallCamItem::BALL_CAM_TIMER_IN_SECS;
}

void BallCamItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Deactivate the paddle camera effect
	this->gameModel->GetTransformInfo()->SetBallCamera(false);

	this->isActive = false;
	GameItem::Deactivate();
}