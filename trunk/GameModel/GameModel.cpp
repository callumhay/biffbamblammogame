/**
 * GameModel.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameModel.h"

#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "GameOverState.h"
#include "LevelCompleteState.h"
#include "Beam.h"
#include "CollateralBlock.h"

GameModel::GameModel() : 
currWorldNum(0), currState(NULL), currPlayerScore(0), 
currLivesLeft(0), pauseBitField(GameModel::NoPause), isBlackoutActive(false), areControlsFlipped(false),
gameTransformInfo(new GameTransformMgr()), nextState(NULL) {
	
	// Initialize the worlds for the game
	for (size_t i = 0; i < GameModelConstants::GetInstance()->WORLD_PATHS.size(); i++) {
		this->worlds.push_back(new GameWorld(GameModelConstants::GetInstance()->WORLD_PATHS[i], *this->gameTransformInfo));
	}

	// Initialize paddle and the first ball
	this->playerPaddle = new PlayerPaddle();
	this->balls.push_back(new GameBall());
}

GameModel::~GameModel() {
	// Delete all items and timers - make sure we do this first since
	// destroying some of these objects causes them to shutdown/use other objects
	// in the game model
	this->ClearGameState();
	
	// Delete the transform manager
	delete this->gameTransformInfo;
	this->gameTransformInfo = NULL;

	// Delete all loaded worlds
	for (size_t i = 0; i < this->worlds.size(); i++) {
		delete this->worlds[i];
		this->worlds[i] = NULL;
	}

	// Delete balls and paddle
	for (std::list<GameBall*>::iterator ballIter = this->balls.begin(); ballIter != this->balls.end(); ++ballIter) {
		GameBall* currBall = *ballIter;
		delete currBall;
		currBall = NULL;
	}
	delete this->playerPaddle;
	this->playerPaddle = NULL;
}

/**
 * Called when we want to begin/restart the model from the
 * beginning of the game, this will reinitialize the model to
 * the first world, level, etc.
 */
void GameModel::BeginOrRestartGame() {
	this->SetNextState(new BallOnPaddleState(this));
	this->SetCurrentWorld(GameModelConstants::GetInstance()->INITIAL_WORLD_NUM);

	// Reset the score, lives, etc.
	this->currPlayerScore	= GameModelConstants::GetInstance()->INIT_SCORE;
	this->SetInitialNumberOfLives(GameModelConstants::GetInstance()->INIT_LIVES_LEFT);
	this->numConsecutiveBlocksHit = GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT;	// Don't use set here, we don't want an event
	this->gameTransformInfo->Reset();

}

// Called in order to make sure the game is no longer processing or generating anything
void GameModel::ClearGameState() {
	// Delete all world items, timers and thingys
	this->ClearLiveItems();
	this->ClearActiveTimers();
	this->ClearProjectiles();
	this->ClearBeams();

	// Delete the state
	delete this->currState;
	this->currState = NULL;	
	this->SetNextState(NULL);

	this->gameTransformInfo->Reset();
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

	// Tell the paddle what the boundries of the level are and reset the paddle
	this->playerPaddle->SetMinMaxLevelBound(currLevel->GetPaddleMinBound(), currLevel->GetPaddleMaxBound());
	this->playerPaddle->ResetPaddle();
}

/**
 * Cause the game model to execute over the given amount of time in seconds.
 */
void GameModel::Tick(double seconds) {
	// If the entire game has been paused then we exit immediately
	if ((this->pauseBitField & GameModel::PauseGame) == GameModel::PauseGame) {
		return;
	}

	if (currState != NULL) {
		if ((this->pauseBitField & GameModel::PauseState) == 0x0) {
			this->currState->Tick(seconds);
		}

		if (this->playerPaddle != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0) {
			this->playerPaddle->Tick(seconds);
		}

	}

	this->gameTransformInfo->Tick(seconds, *this);
}

/**
 * Called when a collision occurs between a projectile and a level piece.
 * Deals with all the important effects a collision could have on the game model.
 * Precondition: p != NULL
 */
void GameModel::CollisionOccurred(Projectile* projectile, LevelPiece* p) {
	assert(p != NULL);
	
	int pointValue = p->GetPointValueForCollision();
	assert(pointValue >= 0);
	this->IncrementScore(pointValue);

	bool alreadyCollided = projectile->IsLastLevelPieceCollidedWith(p);

	// Collide the projectile with the piece...
	LevelPiece* pieceAfterCollision = p->CollisionOccurred(this, projectile); 	// WARNING: This can destroy p.

	// EVENT: First-time Projectile-Block Collision
	if (!alreadyCollided) {
		GameEventManager::Instance()->ActionProjectileBlockCollision(*projectile, *pieceAfterCollision);
	}

	// Check to see if the level is done
	GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
	if (currLevel->IsLevelComplete()) {

		// The level was completed, move to the level completed state
		this->SetNextState(new LevelCompleteState(this));
	}
}

/**
 * Called when a collision occurs between the ball and a level piece.
 * Deals with all the important effects a collision could have on the game model.
 * Precondition: p != NULL
 */
void GameModel::CollisionOccurred(GameBall& ball, LevelPiece* p) {
	assert(p != NULL);
	
	LevelPiece* pieceBefore = p;

	// Set the score appropriately
	int pointValue = p->GetPointValueForCollision();
	assert(pointValue >= 0);
	this->IncrementScore(pointValue);

	// EVENT: Ball-Block Collision
	GameEventManager::Instance()->ActionBallBlockCollision(ball, *p);

	// Collide the ball with the level piece directly, then if there was a change
	// tell the level about it
	GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
	LevelPiece* pieceAfterCollision = p->CollisionOccurred(this, ball);	// WARNING: This can destroy p.
	ball.BallCollided();

	// Check to see if the ball is being teleported in ball-camera mode - in this case
	// we move into a new game state to play the wormhole minigame
	if (pieceAfterCollision->GetType() == LevelPiece::Portal && ball.GetIsBallCameraOn()) {
		// TODO
	}

	// TODO: figure out this multiplier stuffs...
	// If the piece was destroyed then increase the multiplier
	//if (p->GetType() == LevelPiece::Empty) {
	//	this->SetNumConsecutiveBlocksHit(this->numConsecutiveBlocksHit+1);
	//}

	// Check to see if the level is done
	if (currLevel->IsLevelComplete()) {
		// The level was completed, move to the level completed state
		this->SetNextState(new LevelCompleteState(this));
	}
}

void GameModel::BallPaddleCollisionOccurred(GameBall& ball) {
	ball.BallCollided();

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
		float ballSpd			  = ball.GetSpeed(); 

		// Rotate the ball's velocity vector to reflect the momentum of the paddle
		ball.SetVelocity(ballSpd, Vector2D::Rotate(angleChange, ballVelHat));
		
		// Make sure the ball goes upwards (it can't reflect downwards off the paddle or the game would suck)
		if (acosf(Vector2D::Dot(ballVelHat, Vector2D(0, 1))) > ((M_PI / 2.0f) - GameBall::MIN_BALL_ANGLE_IN_RADS)) {
			// Inline: The ball either at a very sharp angle w.r.t. the paddle or it is aimed downwards
			// even though the paddle has deflected it, adjust the angle to be suitable for the game
			if (ballVel[0] < 0) {
				ball.SetVelocity(ballSpd, Vector2D::Rotate(-GameBall::MIN_BALL_ANGLE_IN_DEGS, Vector2D(-1, 0)));
			}
			else {
				ball.SetVelocity(ballSpd, Vector2D::Rotate(GameBall::MIN_BALL_ANGLE_IN_DEGS, Vector2D(1, 0)));
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
			this->SetNextState(new GameOverState(this));
		}
		else {
			this->SetNextState(new BallOnPaddleState(this));
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
	for (std::list<Projectile*>::iterator iter = this->projectiles.begin(); iter != this->projectiles.end(); ++iter) {
		Projectile* currProjectile = *iter;
		// EVENT: Projectile removed from the game
		GameEventManager::Instance()->ActionProjectileRemoved(*currProjectile);
		delete currProjectile;
		currProjectile = NULL;
	}
	this->projectiles.clear();
}

/**
 * Clears the list of all beams that are currently in existance in the game.
 */
void GameModel::ClearBeams() {
	for (std::list<Beam*>::iterator iter = this->beams.begin(); iter != this->beams.end(); ++iter) {
		Beam* currBeam = *iter;
		// EVENT: Beam is removed from the game
		GameEventManager::Instance()->ActionBeamRemoved(*currBeam);
		delete currBeam;
		currBeam = NULL;
	}
	this->beams.clear();
}

/**
 * Clears the list of items that are currently alive in a level.
 */
void GameModel::ClearLiveItems() {
	// Destroy any left-over game items
	for(std::list<GameItem*>::iterator iter = this->currLiveItems.begin(); iter != this->currLiveItems.end(); ++iter) {
		GameItem* currItem = *iter;
		// EVENT: Item removed from the game
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
	for(std::list<GameItemTimer*>::iterator iter = this->activeTimers.begin(); iter != this->activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		currTimer->StopTimer();
		delete currTimer;
		currTimer = NULL;
	}
	this->activeTimers.clear();
}

/**
 * Add a projectile to the game.
 */
void GameModel::AddProjectile(Projectile* projectile) {
	assert(projectile != NULL);

	// Add it to the list of in-game projectiles
	this->projectiles.push_back(projectile);

	// EVENT: Projectile creation
	GameEventManager::Instance()->ActionProjectileSpawned(*projectile);	
}

/**
 * Add a beam of the given type to the model - this will be updated and tested appropriate to
 * its execution.
 */
void GameModel::AddBeam(int beamType) {

	// Create the beam based on type...
	Beam* addedBeam = NULL;
	switch (beamType) {
		case Beam::PaddleLaserBeam:
			// Remove any previous paddle laser beams and add the new one...
			for (std::list<Beam*>::iterator iter = this->beams.begin(); iter != this->beams.end(); ++iter) {
				Beam* currBeam = *iter;
				if (currBeam->GetBeamType() == Beam::PaddleLaserBeam) {
					this->beams.erase(iter);
					// EVENT: Beam was removed (being replaced)
					GameEventManager::Instance()->ActionBeamRemoved(*currBeam);
					delete currBeam;
					currBeam = NULL;
					break;
				}
			}
			addedBeam = new PaddleLaserBeam(this->GetPlayerPaddle(), this->GetCurrentLevel());
			break;

		default:
			assert(false);
			return;
	}
	assert(addedBeam != NULL);

	// Add the beam to the list of in-game beams
	this->beams.push_back(addedBeam);

	// EVENT: Beam creation
	GameEventManager::Instance()->ActionBeamSpawned(*addedBeam);
}