#include "SlowBallItem.h"
#include "GameModel.h"

const std::string SlowBallItem::SLOW_BALL_ITEM_NAME = "Slow Ball (Power-up)";

SlowBallItem::SlowBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
	GameItem(SLOW_BALL_ITEM_NAME, spawnOrigin, gameModel) {
}

SlowBallItem::~SlowBallItem() {
}

void SlowBallItem::Activate() {
	GameBall* gameBall = this->gameModel->GetGameBall();
	
	// TODO: figure out how to do timers for this stuff
	gameBall->SetSpeed(GameBall::SlowSpeed);
	// ...
}