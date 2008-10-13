#include "SlowBallItem.h"
#include "FastBallItem.h"

#include "GameModel.h"
#include "GameItemTimer.h"

const double SlowBallItem::SLOW_BALL_TIMER_IN_SECS	= 20.0;
const std::string SlowBallItem::SLOW_BALL_ITEM_NAME = "Slow Ball (Power-up)";

SlowBallItem::SlowBallItem(const Point2D &spawnOrigin, GameModel *gameModel) : 
GameItem(SLOW_BALL_ITEM_NAME, spawnOrigin, gameModel, GameItem::Good) {
}

SlowBallItem::~SlowBallItem() {
}

/**
 * Called to activate this item - the effect will be to slow down
 * the game ball and start a timer for this effect to ware out.
 * Returns: A new timer associated with the effect.
 * NOTE: This function CREATES a new heap object and is not responsible
 * for its destruction, BECAREFUL!
 */
GameItemTimer* SlowBallItem::Activate() {
	// Activate the actual effect of the slow ball item
	GameBall* gameBall = this->gameModel->GetGameBall();
	assert(gameBall != NULL);

	gameBall->DecreaseSpeed();

	// Kill other ball speed timers
	std::vector<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<unsigned int> removeIndices;
	for (unsigned int i = 0; i < activeTimers.size(); i++) {
		GameItemTimer* currTimer = activeTimers[i];
		if (currTimer->GetTimerType() == GameItemTimer::FastBallTimer ||
			  currTimer->GetTimerType() == GameItemTimer::SlowBallTimer) {
			delete currTimer;
			currTimer = NULL;
			removeIndices.push_back(i);
		}
	}

	// Remove the ball speed related timers from the list of active timers
	for (std::vector<unsigned int>::iterator iter = removeIndices.begin(); iter != removeIndices.end(); iter++) {
		activeTimers.erase(activeTimers.begin() + *iter);
	}

	// Figure out if we need a new timer...
	if (gameBall->GetSpeed() == GameBall::NormalSpeed) {
		// Since the effect is useless - i.e., it has cancelled out a previous effect
		// and set the ball to its regular speed, we just ignore the effect of this item
		return new GameItemTimer(this, GameItemTimer::NoTimer, 0.0);
	}
	else if (gameBall->GetSpeed() > GameBall::NormalSpeed) {
		return new GameItemTimer(this, GameItemTimer::FastBallTimer, FastBallItem::FAST_BALL_TIMER_IN_SECS);
	}
	else {
		return new GameItemTimer(this, GameItemTimer::SlowBallTimer, SLOW_BALL_TIMER_IN_SECS);
	}
}

/**
 * Called to deactivate the effect of this timer.
 */
void SlowBallItem::Deactivate() {
	GameBall* gameBall = this->gameModel->GetGameBall();
	assert(gameBall != NULL);
	gameBall->SetSpeed(GameBall::NormalSpeed);
}