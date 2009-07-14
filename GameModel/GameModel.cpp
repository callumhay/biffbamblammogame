#include "GameModel.h"

#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "GameOverState.h"

GameModel::GameModel() : 
currWorldNum(0), currState(NULL), currPlayerScore(0), 
currLivesLeft(0), gameIsPaused(false), isBlackoutActive(false), areControlsFlipped(false) {
	
	// Initialize the worlds for the game
	for (size_t i = 0; i < GameModelConstants::GetInstance()->WORLD_PATHS.size(); i++) {
		this->worlds.push_back(new GameWorld(GameModelConstants::GetInstance()->WORLD_PATHS[i]));
	}

	// Initialize paddle and the first ball
	this->playerPaddle = new PlayerPaddle();
	this->balls.push_back(new GameBall());
}

GameModel::~GameModel() {
	// Delete all loaded worlds
	for (size_t i = 0; i < this->worlds.size(); i++) {
		delete this->worlds[i];
		this->worlds[i] = NULL;
	}
	
	// Delete the state
	delete this->currState;
	this->currState = NULL;

	// Delete balls and paddle
	for (std::list<GameBall*>::iterator ballIter = this->balls.begin(); ballIter != this->balls.end(); ballIter++) {
		GameBall* currBall = *ballIter;
		delete currBall;
		currBall = NULL;
	}
	delete this->playerPaddle;
	this->playerPaddle = NULL;

	// Delete all items and timers
	this->ClearLiveItems();
	this->ClearActiveTimers();
	this->ClearProjectiles();
}

/**
 * Called when we want to begin/restart the model from the
 * beginning of the game, this will reinitialize the model to
 * the first world, level, etc.
 */
void GameModel::BeginOrRestartGame() {
	this->SetCurrentState(new BallOnPaddleState(this));
	this->SetCurrentWorld(GameModelConstants::GetInstance()->INITIAL_WORLD_NUM);

	// Reset the score, lives, etc.
	this->currPlayerScore	= GameModelConstants::GetInstance()->INIT_SCORE;
	this->SetLivesLeft(GameModelConstants::GetInstance()->INIT_LIVES_LEFT);
	this->numConsecutiveBlocksHit = GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT;	// Don't use set here, we don't want an event
}

void GameModel::SetCurrentWorld(unsigned int worldNum) {
	assert(worldNum < this->worlds.size());
	
	// Clean up the previous world
	GameWorld* prevWorld = this->GetCurrentWorld();
	prevWorld->Unload();

	// Get the world we want to set as current
	GameWorld* world = this->worlds[worldNum];
	assert(world != NULL);
	
	// Make sure the world loaded properly.
	if (!world->Load()) {
		debug_output("ERROR: Could not load world " << worldNum);
		assert(false);
		return;
	}

	this->currWorldNum = worldNum;

	GameLevel* currLevel = world->GetCurrentLevel();
	assert(currLevel != NULL);

	this->playerPaddle->SetMinMaxLevelBound(currLevel->GetPaddleMinBound(), currLevel->GetPaddleMaxBound());
}

/**
 * Determines the next level to increment to and then loads the appropriate
 * level, world or end of game depending on what comes next.
 */
void GameModel::IncrementLevel() {
	// Reset the multiplier
	this->SetNumConsecutiveBlocksHit(GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT);

	// Set the number of balls that exist to just 1
	if (this->balls.size() > 1) {
		std::list<GameBall*>::iterator ballIter = this->balls.begin();
		ballIter++;
		for (; ballIter != this->balls.end(); ballIter++) {
			GameBall* ballToDestroy = *ballIter;
			delete ballToDestroy;
			ballToDestroy = NULL;
		}
		GameBall* onlyBallLeft = (*this->balls.begin());
		assert(onlyBallLeft != NULL);
		this->balls.clear();
		this->balls.push_back(onlyBallLeft);
	}

	GameWorld* currWorld = this->GetCurrentWorld();
	GameLevel* currLevel = currWorld->GetCurrentLevel();

	// EVENT: Level is complete
	GameEventManager::Instance()->ActionLevelCompleted(*currWorld, *currLevel);

	if (currWorld->IsLastLevel()) {
		// EVENT: World is complete
		GameEventManager::Instance()->ActionWorldCompleted(*currWorld);

		// The last level of the world... need to check for end of the game state
		if (this->IsLastWorld()) {
			// EVENT: Game is complete
			GameEventManager::Instance()->ActionGameCompleted();

			// No more levels, just switch states
			this->SetCurrentState(new GameCompleteState(this));
		}
		else {
			// Increment to the next world
			this->SetCurrentWorld(this->currWorldNum + 1); 
			// Place the ball back on the paddle
			this->SetCurrentState(new BallOnPaddleState(this));
		}
	}
	else {
		// Increment to the next level, the world hasn't changed
		currWorld->IncrementLevel();
		// Place the ball back on the paddle
		this->SetCurrentState(new BallOnPaddleState(this));
	}
}

/**
 * Cause the game model to execute over the given amount of time in seconds.
 */
void GameModel::Tick(double seconds) {
	
	if (currState != NULL && !this->gameIsPaused) {
		this->currState->Tick(seconds);
	}

	this->gameTransformInfo.Tick(seconds);
}

/**
 * Called when a collision occurs between a projectile and a level piece.
 * Deals with all the important effects a collision could have on the game model.
 * Precondition: p != NULL
 */
void GameModel::CollisionOccurred(Projectile* projectile, LevelPiece* p, bool& stateChanged) {
	assert(p != NULL);
	
	int pointValue = p->GetPointValueForCollision();
	assert(pointValue >= 0);
	this->IncrementScore(pointValue);

	// Collide the projectile with the piece...
	LevelPiece* pieceAfterCollision = p->CollisionOccurred(this, *projectile); 	// WARNING: This can destroy p.

	// EVENT: Ball-Block Collision
	GameEventManager::Instance()->ActionProjectileBlockCollision(*projectile, *pieceAfterCollision);

	// Check to see if the level is done
	GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
	if (currLevel->IsLevelComplete()) {

		// The level was completed, increment the level - note this function
		// is robust and will increment level, world and determine if we have finished
		// the game all in one
		this->IncrementLevel();
		stateChanged = true;
	}
	else {
		stateChanged = false;
	}
}

/**
 * Called when a collision occurs between the ball and a level piece.
 * Deals with all the important effects a collision could have on the game model.
 * Precondition: p != NULL
 */
void GameModel::CollisionOccurred(const GameBall& ball, LevelPiece* p, bool& stateChanged) {
	assert(p != NULL);
	
	LevelPiece* pieceBefore = p;

	// Set the score appropriately
	int pointValue = p->GetPointValueForCollision();
	assert(pointValue >= 0);
	this->IncrementScore(pointValue);

	// Collide the ball with the level piece directly, then if there was a change
	// tell the level about it
	GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
	LevelPiece* pieceAfterCollision = p->CollisionOccurred(this, ball);	// WARNING: This can destroy p.

	// EVENT: Ball-Block Collision
	GameEventManager::Instance()->ActionBallBlockCollision(ball, *pieceAfterCollision);


	// TODO: figure out this multiplier stuffs...
	// If the piece was destroyed then increase the multiplier
	//if (p->GetType() == LevelPiece::Empty) {
	//	this->SetNumConsecutiveBlocksHit(this->numConsecutiveBlocksHit+1);
	//}

	// Check to see if the level is done
	if (currLevel->IsLevelComplete()) {

		// The level was completed, increment the level - note this function
		// is robust and will increment level, world and determine if we have finished
		// the game all in one
		this->IncrementLevel();
		stateChanged = true;
	}
	else {
		stateChanged = false;
	}
}

void GameModel::BallPaddleCollisionOccurred(GameBall& ball) {

	// Reset the multiplier
	this->SetNumConsecutiveBlocksHit(GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT);

	// Modify the ball velocity using the current paddle velocity
	Vector2D paddleVel = this->playerPaddle->GetAvgVelocity();
	
	if (fabs(paddleVel[0]) > EPSILON) {
		// The paddle has a considerable velocity, we should augment the ball's
		// tragectory based on this (transfer of momentum)
		int angleDecSgn = -NumberFuncs::SignOf(paddleVel[0]);
		float fractionOfSpeed = fabs(paddleVel[0]) / PlayerPaddle::DEFAULT_SPEED;
		float angleChange = angleDecSgn * fractionOfSpeed * PlayerPaddle::RAND_DEG_ANG;

		// Set the ball to be just off the paddle
		ball.SetCenterPosition(Point2D(ball.GetBounds().Center()[0], 
			this->playerPaddle->GetCenterPosition()[1] + this->playerPaddle->GetHalfHeight() + ball.GetBounds().Radius() + EPSILON));

		Vector2D ballVel    = ball.GetVelocity();
		Vector2D ballVelHat = Vector2D::Normalize(ballVel);
		GameBall::BallSpeed ballSpd = ball.GetSpeed(); 

		// Rotate the ball's velocity vector to reflect the momentum of the paddle
		ball.SetVelocity(ballSpd, Rotate(angleChange, ballVelHat));
		
		// Make sure the ball goes upwards (it can't reflect downwards off the paddle or the game would suck)
		if (acosf(Vector2D::Dot(ballVelHat, Vector2D(0, 1))) > ((M_PI / 2.0f) - GameBall::MIN_BALL_ANGLE_IN_RADS)) {
			// Inline: The ball either at a very sharp angle w.r.t. the paddle or it is aimed downwards
			// even though the paddle has deflected it, adjust the angle to be suitable for the game
			if (ballVel[0] < 0) {
				ball.SetVelocity(ballSpd, Rotate(-GameBall::MIN_BALL_ANGLE_IN_DEGS, Vector2D(-1, 0)));
			}
			else {
				ball.SetVelocity(ballSpd, Rotate(GameBall::MIN_BALL_ANGLE_IN_DEGS, Vector2D(1, 0)));
			}
		}
	}
}

/**
 * Called when the player/ball dies (falls into the bottomless pit of doom). 
 * This will deal with all the effects a death has on the game model.
 */
void GameModel::BallDied(GameBall* deadBall, bool& stateChanged) {
	assert(deadBall != NULL);

	// EVENT: A Ball has died
	GameEventManager::Instance()->ActionBallDied(*deadBall);

	// Do nasty stuff to player only if that was the last ball left
	if (this->balls.size() == 1) {
		// Reset the multiplier
		this->SetNumConsecutiveBlocksHit(GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT);
		
		// Decrement the number of lives left
		this->SetLivesLeft(this->currLivesLeft-1);

		// EVENT: All Balls are dead
		GameEventManager::Instance()->ActionAllBallsDead(this->currLivesLeft);

		deadBall->ResetBallAttributes();

		// Set the appropriate state based on the number of lives left...
		if (this->IsGameOver()) {
			// Game Over
			this->SetCurrentState(new GameOverState(this));
		}
		else {
			this->SetCurrentState(new BallOnPaddleState(this));
		}
		stateChanged = true;
	}
	else {
		// There is still at least 1 ball left in play - delete the given dead ball from the game
		
		stateChanged = false;
	}
}

/**
 * Clears the list of all projectiles that are currently in existance in the game.
 */
void GameModel::ClearProjectiles() {
	for (std::list<Projectile*>::iterator iter = this->projectiles.begin(); iter != this->projectiles.end(); iter++) {
		Projectile* currProjectile = *iter;
		GameEventManager::Instance()->ActionProjectileRemoved(*currProjectile);
		delete currProjectile;
		currProjectile = NULL;
	}
	this->projectiles.clear();
}

/**
 * Clears the list of items that are currently alive in a level.
 */
void GameModel::ClearLiveItems() {
	// Destroy any left-over game items
	for(std::list<GameItem*>::iterator iter = this->currLiveItems.begin(); iter != this->currLiveItems.end(); iter++) {
		GameItem* currItem = *iter;
		GameEventManager::Instance()->ActionItemRemoved(*currItem);
		delete currItem;
		currItem = NULL;
	}
	this->currLiveItems.clear();
}
/**
 * Clears all of the active timers in the game.
 */
void GameModel::ClearActiveTimers() {
	for(std::list<GameItemTimer*>::iterator iter = this->activeTimers.begin(); iter != this->activeTimers.end(); iter++) {
		GameItemTimer* currTimer = *iter;
		currTimer->StopTimer();
		delete currTimer;
		currTimer = NULL;
	}
	this->activeTimers.clear();
}

/**
 * Add a projectile of the given type at the given spawn location
 */
void GameModel::AddProjectile(Projectile::ProjectileType type, const Point2D& spawnLoc) {
	// Create a new projectile based on values given
	Projectile* addedProjectile = Projectile::CreateProjectile(type, spawnLoc);
	assert(addedProjectile != NULL);

	// Add it to the list of in-game projectiles
	this->projectiles.push_back(addedProjectile);

	// EVENT: Projectile creation
	GameEventManager::Instance()->ActionProjectileSpawned(*addedProjectile);
}