#include "PaddleCamItem.h"
#include "GameModel.h"

const std::string PaddleCamItem::PADDLE_CAM_ITEM_NAME	= "Paddle Cam (power-down)";
const double PaddleCamItem::PADDLE_CAM_TIMER_IN_SECS	= 20.0;

PaddleCamItem::PaddleCamItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(PaddleCamItem::PADDLE_CAM_ITEM_NAME, spawnOrigin, gameModel, GameItem::Bad) {
}

PaddleCamItem::~PaddleCamItem() {
}

double PaddleCamItem::Activate() {
	this->isActive = true;

	// Deactivate any currently active paddle camera items and any other associated
	// viewpoint changing items
	// TODO: DONT FORGET ABOUT THIS (SEE COMMENT ABOVE) !!!
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemName() == PaddleCamItem::PADDLE_CAM_ITEM_NAME) {
			iter = activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
		}
		else {
			 iter++;
		}
	}

	// Activate the paddle camera effect
	this->gameModel->GetTransformInfo()->SetPaddleCamera(true);
	GameItem::Activate();

	return PaddleCamItem::PADDLE_CAM_TIMER_IN_SECS;
}

void PaddleCamItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Deactivate the paddle camera effect
	this->gameModel->GetTransformInfo()->SetPaddleCamera(false);

	this->isActive = false;
	GameItem::Deactivate();
}