#include "SlowBallItem.h"
#include "GameModel.h"

SlowBallItem::SlowBallItem(GameModel *gameModel) : GameItem(gameModel) {
}

SlowBallItem::~SlowBallItem() {
}

void SlowBallItem::Activate() {
	GameBall* gameBall = this->gameModel->GetGameBall();

}