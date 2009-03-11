#include "BallSpeedItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double BallSpeedItem::BALL_SPEED_TIMER_IN_SECS	= 20.0;
const std::string BallSpeedItem::SLOW_BALL_ITEM_NAME = "Slow Ball (Power-up)";
const std::string BallSpeedItem::FAST_BALL_ITEM_NAME = "Fast Ball (Power-down)";

BallSpeedItem::BallSpeedItem(const BallSpeedType type, const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem((type==SlowBall)?SLOW_BALL_ITEM_NAME:FAST_BALL_ITEM_NAME, spawnOrigin, gameModel, (type==SlowBall)?GameItem::Good:GameItem::Bad), spdType(type) {
}

BallSpeedItem::~BallSpeedItem() {
}

void BallSpeedItem::SwitchSpeed(BallSpeedType newSpd) {
	this->spdType = newSpd;
	switch (newSpd) {
		case FastBall:
			this->type = GameItem::Bad;
			this->name = BallSpeedItem::FAST_BALL_ITEM_NAME;
			break;
		case SlowBall:
			this->type = GameItem::Good;
			this->name = BallSpeedItem::SLOW_BALL_ITEM_NAME;
			break;
		default:
			assert(false);
			break;
	}
}

/**
 * Called to activate this item - the effect will be to slow down
 * the game ball and start a timer for this effect to ware out.
 * Returns: A new timer associated with the effect.
 * NOTE: This function CREATES a new heap object and is not responsible
 * for its destruction, BECAREFUL!
 */
double BallSpeedItem::Activate() {
	this->isActive = true;

	// Kill other ball speed timers
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemName() == BallSpeedItem::SLOW_BALL_ITEM_NAME ||
			  currTimer->GetTimerItemName() == BallSpeedItem::FAST_BALL_ITEM_NAME) {
			removeTimers.push_back(currTimer);
		}
	}

	// Remove the ball speed related timers from the list of active timers
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
		GameItemTimer* currTimer = removeTimers[i];
		activeTimers.remove(currTimer);
		delete currTimer;
		currTimer = NULL;
	}

	// Activate the actual effect of this speed-ball item for all active balls
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	bool allBallsNormalSpd = true;
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ballIter++) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);

		// Figure out how the item will effect the ball's speed
		switch (this->spdType) {
			case FastBall:
				currBall->IncreaseSpeed();
				break;
			case SlowBall:
				currBall->DecreaseSpeed();
				break;
			default:
				assert(false);
				break;
		}
		
		if (currBall->GetSpeed() != GameBall::NormalSpeed) {
			allBallsNormalSpd = false;
		}

	}

	// If all the balls are normal speed then all previous power-ups/downs involving
	// ball speed have been cancelled out an no item is active
	if (allBallsNormalSpd) {
		return GameItemTimer::ZERO_TIME_TIMER_IN_SECS;
	}

	// Old functionality...
	//if (gameBalls.size() == 1) {
	//		GameBall::BallSpeed ballSpd = currBall->GetSpeed();

	//		if (ballSpd == GameBall::NormalSpeed) {
	//			return GameItemTimer::ZERO_TIME_TIMER_IN_SECS;
	//		}
	//		else if (ballSpd > GameBall::NormalSpeed) {
	//			this->SwitchSpeed(FastBall);
	//		}
	//		else {
	//			this->SwitchSpeed(SlowBall);
	//		}
	//}

	GameItem::Activate();
	return BallSpeedItem::BALL_SPEED_TIMER_IN_SECS;
}

/**
 * Called to deactivate the effect of this timer.
 */
void BallSpeedItem::Deactivate() {
	if (!this->isActive) {
		return;
	}

	// Shut off the item for each of the balls
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	for (std::list<GameBall*>::iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ballIter++) {
		GameBall* currBall = *ballIter;
		assert(currBall != NULL);
		currBall->SetSpeed(GameBall::NormalSpeed);
	}

	this->isActive = false;
	GameItem::Deactivate();
}