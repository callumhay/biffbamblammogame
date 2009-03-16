#include "MultiBallItem.h"
#include "GameModel.h"

const std::string MultiBallItem::MULTI3_BALL_ITEM_NAME	= "Multi Ball 3 (power-up)";
const std::string MultiBallItem::MULTI5_BALL_ITEM_NAME	= "Multi Ball 5 (power-up)";
const double MultiBallItem::MULTI_BALL_TIMER_IN_SECS	= 0.0;

const float MultiBallItem::MIN_SPLIT_DEGREES = 360.0f / (static_cast<float>(MAX_NUM_SPLITBALLS) + 1.0f);

MultiBallItem::MultiBallItem(const Point2D &spawnOrigin, GameModel *gameModel, NumMultiBalls numBalls) : 
GameItem(MultiBallItem::NameFromNumBalls(numBalls), spawnOrigin, gameModel, GameItem::Good), numNewSpawnedBalls(static_cast<unsigned int>(numBalls)-1) {
	assert(numNewSpawnedBalls >= MIN_NUM_SPLITBALLS && numNewSpawnedBalls <= MAX_NUM_SPLITBALLS);
}

MultiBallItem::~MultiBallItem() {
}

/**
 * Activate the multiball item - this will split the ball up into this
 * multiball item's number of balls
 */
double MultiBallItem::Activate() {

	// Calculate the average number of degrees (in 360) devoted to each ball...
	float maxDegreesPerBall = 360.0f / (static_cast<float>(this->numNewSpawnedBalls) + 1.0f);
	float maxDegreesDiff    = maxDegreesPerBall - MultiBallItem::MIN_SPLIT_DEGREES;
	assert(maxDegreesDiff >= 0);

	float currRotationInDegs = 0.0f;

	// Go through each game ball making 'numBalls' copies of it and setting those copies
	// with the appropriate characteristics
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	std::vector<GameBall*> newBalls;
	newBalls.reserve(gameBalls.size() * this->numNewSpawnedBalls);
	for (std::list<GameBall*>::const_iterator ballIter = gameBalls.begin(); ballIter != gameBalls.end(); ballIter++) {
		const GameBall* currBall = *ballIter;
	
		// Create all the copies of the current ball
		for (unsigned int copyNum = 0; copyNum < this->numNewSpawnedBalls; copyNum++) {
			GameBall* newBall = new GameBall(*currBall);
			
			// We need to change the moving direction of the copied ball to something random but also reasonable:
			Vector2D ballDir = currBall->GetDirection();
			currRotationInDegs += MultiBallItem::MIN_SPLIT_DEGREES + Randomizer::GetInstance()->RandomNumZeroToOne()*maxDegreesDiff;
			newBall->SetVelocity(currBall->GetSpeed(), Rotate(currRotationInDegs, ballDir));

			newBalls.push_back(newBall);
		}
	}

	// Now add all the newly created balls into the game model's list of balls
	for (std::vector<GameBall*>::iterator iter = newBalls.begin(); iter != newBalls.end(); iter++) {
		gameBalls.push_back(*iter);
	}

	GameItem::Activate();
	return MultiBallItem::MULTI_BALL_TIMER_IN_SECS;
}

/**
 * Does nothing since the multiball has no timer or way of being cancelled out.
 */
void MultiBallItem::Deactivate() {
}