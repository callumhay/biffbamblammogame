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
#include "PaddleLaserBeam.h"
#include "CollateralBlock.h"

GameModel::GameModel() : 
currWorldNum(0), currState(NULL), currPlayerScore(0), 
currLivesLeft(0), pauseBitField(GameModel::NoPause), isBlackoutActive(false), areControlsFlipped(false),
gameTransformInfo(new GameTransformMgr()), nextState(NULL), doingPieceStatusListIteration(false) {
	
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
	this->SetCurrentWorld(GameModelConstants::GetInstance()->INITIAL_WORLD_NUM);

	// Reset the score, lives, etc.
	this->currPlayerScore	= GameModelConstants::GetInstance()->INIT_SCORE;
	this->SetInitialNumberOfLives(GameModelConstants::GetInstance()->INIT_LIVES_LEFT);
	this->numConsecutiveBlocksHit = GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT;	// Don't use set here, we don't want an event
	this->gameTransformInfo->Reset();

	this->SetNextState(new BallOnPaddleState(this));
}

// Called in order to make sure the game is no longer processing or generating anything
void GameModel::ClearGameState() {
	// Delete all world items, timers and thingys
	this->ClearStatusUpdatePieces();
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

	// Setup the world for this object and make sure the world has its zeroth (i.e., first) level set for play
	this->currWorldNum = worldNum;
	world->SetCurrentLevel(this, 0);
	GameLevel* currLevel = world->GetCurrentLevel();
	assert(currLevel != NULL);

	// EVENT: World started...
	GameEventManager::Instance()->ActionWorldStarted(*world);
	// EVENT: New Level Started
	GameEventManager::Instance()->ActionLevelStarted(*world, *currLevel);

	// Tell the paddle what the boundries of the level are and reset the paddle
	this->playerPaddle->SetNewMinMaxLevelBound(currLevel->GetPaddleMinBound(), currLevel->GetPaddleMaxBound()); // resets the paddle
	//this->playerPaddle->ResetPaddle();
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
		if ((this->pauseBitField & GameModel::PauseState) == 0x00000000) {
			this->currState->Tick(seconds);
		}

		if (this->playerPaddle != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0) {
			this->playerPaddle->Tick(seconds, (this->pauseBitField & GameModel::PauseState) == GameModel::PauseState);
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

void GameModel::CollisionOccurred(Projectile* projectile, PlayerPaddle* paddle) {
	assert(projectile != NULL);
	assert(paddle != NULL);

	// Tell the paddle it got hit by a projectile
	paddle->HitByProjectile(this, *projectile);

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

	// If the ball is attached to the paddle then we always set the last piece it collided with to NULL
	// This ensures that the ball ALWAYS collides with everything in its way while on the paddle
	if (this->playerPaddle->GetAttachedBall() == &ball) {
		ball.SetLastPieceCollidedWith(NULL);
	}

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
	//ball.SetLastThingCollidedWith(this->playerPaddle);

	// Reset the multiplier
	this->SetNumConsecutiveBlocksHit(GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT);

	// Modify the ball velocity using the current paddle speed
	Vector2D paddleVel = this->playerPaddle->GetVelocity();
	
	if (fabs(paddleVel[0]) > EPSILON) {
		// The paddle has a considerable velocity, we should augment the ball's
		// tragectory based on this...
		int angleDecSgn = -NumberFuncs::SignOf(paddleVel[0]);
		float fractionOfSpeed = fabs(paddleVel[0]) / PlayerPaddle::DEFAULT_MAX_SPEED;
		float angleChange = angleDecSgn * fractionOfSpeed * PlayerPaddle::DEFLECTION_DEGREE_ANGLE;

		// Set the ball to be just off the paddle
		ball.SetCenterPosition(Point2D(ball.GetBounds().Center()[0], 
			this->playerPaddle->GetCenterPosition()[1] + this->playerPaddle->GetHalfHeight() + ball.GetBounds().Radius() + EPSILON));

		Vector2D ballVel    = ball.GetVelocity();
		Vector2D ballVelHat = Vector2D::Normalize(ballVel);
		float ballSpd			  = ball.GetSpeed(); 

		// Rotate the ball's velocity vector to reflect the momentum of the paddle
		ball.SetVelocity(ballSpd, Vector2D::Rotate(angleChange, ballVelHat));
		
		// Make sure the ball goes upwards (it can't reflect downwards off the paddle or the game would suck)
		if (acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(ballVelHat, Vector2D(0, 1))))) > ((M_PI / 2.0f) - GameBall::MIN_BALL_ANGLE_IN_RADS)) {
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
 * Called to update each piece that requires per-frame updates in the current level.
 */
void GameModel::DoPieceStatusUpdates(double dT) {
	LevelPiece* currLevelPiece;
	int32_t statusesToRemove = static_cast<int32_t>(LevelPiece::NormalStatus);
	bool pieceMustBeRemoved = false;
	this->doingPieceStatusListIteration = true;	// This makes sure that no other functions try to modify the status update pieces
																							// when we're currently in the process of doing so

	for (std::map<LevelPiece*, int32_t>::iterator iter = this->statusUpdatePieces.begin(); iter != this->statusUpdatePieces.end();) {
		currLevelPiece = iter->first;

		// When we tick the statuses on a piece we get a result that tells us whether the piece is completely
		// removed of all its status effects in which case we remove it from the map and move on - this tends to mean
		// that the piece has been destroyed by its own status effects
		pieceMustBeRemoved = currLevelPiece->StatusTick(dT, this, statusesToRemove);
		if (pieceMustBeRemoved) {
			iter = this->statusUpdatePieces.erase(iter);
			continue;
		}

		// In this case the piece still exists and we need to check if any status effects must be removed from it
		if (statusesToRemove == static_cast<int32_t>(LevelPiece::NormalStatus)) {
			++iter;
		}
		else {
			// Remove the status from the updates... if the resulting status is NormalStatus then remove entirely
			// Sanity: The status to remove better already be applied!!! - 
			// NOTE: We don't have to remove it from the piece
			// since the piece should have already done that for itself when we called statusTick - do it anyway though, just to be safe
			assert((iter->second & statusesToRemove) == statusesToRemove);
			iter->second = (iter->second & ~statusesToRemove);
			currLevelPiece->RemoveStatuses(statusesToRemove);

			if (iter->second == static_cast<int32_t>(LevelPiece::NormalStatus)) {
				iter = this->statusUpdatePieces.erase(iter);
			}
			else {
				++iter;
			}
		}
	}
	this->doingPieceStatusListIteration = false;

	// Check to see if the level is done
	GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
	if (currLevel->IsLevelComplete()) {
		// The level was completed, move to the level completed state
		this->SetNextState(new LevelCompleteState(this));
	}
}

/**
 * Execute projectile collisions on everything relevant that's in play in the game model.
 */
void GameModel::DoProjectileCollisions() {

#define PROJECTILE_CLEANUP(p) GameEventManager::Instance()->ActionProjectileRemoved(*p); \
													    delete p; \
													    p = NULL
													 
	// Grab a list of all paddle-related projectiles and test each one for collisions...
	std::list<Projectile*>& gameProjectiles = this->GetActiveProjectiles();
	bool destroyProjectile = false;
	bool didCollide = false;
	bool incIter = true;

	for (std::list<Projectile*>::iterator iter = gameProjectiles.begin(); iter != gameProjectiles.end();) {
		Projectile* currProjectile = *iter;
		
		// Grab bounding lines from the projectile to test for collision
		BoundingLines projectileBoundingLines = currProjectile->BuildBoundingLines();

		// Check to see if the projectile collided with the player paddle
		if (this->playerPaddle->CollisionCheckWithProjectile(currProjectile->GetType(), projectileBoundingLines)) {
			
			this->CollisionOccurred(currProjectile, this->playerPaddle);

			// Destroy the projectile if necessary
			destroyProjectile = !this->playerPaddle->ProjectilePassesThrough(*currProjectile);
			if (destroyProjectile) {
				iter = gameProjectiles.erase(iter);
				PROJECTILE_CLEANUP(currProjectile);
				continue;
			}
			
			this->playerPaddle->ModifyProjectileTrajectory(*currProjectile);
			++iter;
			continue;
		}

		// Check to see if the projectile collided with a safety net (if one is active)
		if (this->GetCurrentLevel()->ProjectileSafetyNetCollisionCheck(*currProjectile, projectileBoundingLines)) {
			// This doesn't destroy projectiles currently
			++iter;
			continue;
		}

		// Find the any level pieces that the current projectile may have collided with and test for collision
		incIter = true;	// Keep track of whether we need to increment the iterator or not
		std::set<LevelPiece*> collisionPieces = this->GetCurrentLevel()->GetLevelPieceCollisionCandidates(*currProjectile);
		for (std::set<LevelPiece*>::iterator pieceIter = collisionPieces.begin(); pieceIter != collisionPieces.end(); ++pieceIter) {
			LevelPiece *currPiece = *pieceIter;
			
			// Test for a collision between the projectile and current level piece
			didCollide = currPiece->CollisionCheck(projectileBoundingLines);
			if (didCollide) {
				// This needs to be before the call to CollisionOccurred or else the currPiece may already be destroyed.
				destroyProjectile = !currPiece->ProjectilePassesThrough(currProjectile);
				this->CollisionOccurred(currProjectile, currPiece);	

				// Check to see if the collision is supposed to destroy the projectile
				if (destroyProjectile) {
					// Despose of the projectile...
					iter = gameProjectiles.erase(iter);
					PROJECTILE_CLEANUP(currProjectile);
					incIter = false;
				}

				break;	// Important that we break out of the loop since some blocks may no longer exist after a collision
								// (and we may have destroyed the projectile anyway).
			}
		}

		if (incIter) {
			++iter;
		}
	}

#undef PROJECTILE_CLEANUP

}

/**
 * Private helper function to determine if the given position is
 * out of game bounds or not.
 * Returns: true if out of bounds, false otherwise.
 */
bool GameModel::IsOutOfGameBounds(const Point2D& pos) {
	GameLevel* currLevel = this->GetCurrentLevel();
	float levelWidthBounds	= currLevel->GetLevelUnitWidth()  + GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE;
	float levelHeightBounds = currLevel->GetLevelUnitHeight() + GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE;

	return pos[1] <= GameLevel::Y_COORD_OF_DEATH || 
				 pos[1] >= levelHeightBounds ||
				 pos[0] <= -GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE ||
				 pos[0] >= levelWidthBounds;
}

/**
 * Private helper function for updating all active timers and deleting/removing
 * all expired timers.
 */
void GameModel::UpdateActiveTimers(double seconds) {
	std::list<GameItemTimer*>& activeTimers = this->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->HasExpired()) {
			// Timer has expired, dispose of it
			removeTimers.push_back(currTimer);
		}
		else {
			currTimer->Tick(seconds);
		}
	}

	// Delete any timers that have expired
	for (unsigned int i = 0; i < removeTimers.size(); i++) {
		GameItemTimer* currTimer = removeTimers[i];
		activeTimers.remove(currTimer);
		delete currTimer;
		currTimer = NULL;
	}
}

/**
 * Private helper function for updating active item drops and removing those
 * that are out of bounds.
 */
void GameModel::UpdateActiveItemDrops(double seconds) {
	// Update any items that may have been created
	std::vector<GameItem*> removeItems;
	std::list<GameItem*>& currLiveItems = this->GetLiveItems();
	for(std::list<GameItem*>::iterator iter = currLiveItems.begin(); iter != currLiveItems.end(); ++iter) {
		GameItem *currItem = *iter;
		currItem->Tick(seconds);
		
		// Check to see if any have left the playing area - if so destroy them
		if (this->IsOutOfGameBounds(currItem->GetCenter())) {
			removeItems.push_back(currItem);
		}
	}

	// Delete any items that have left play
	for (unsigned int i = 0; i < removeItems.size(); i++) {
		GameItem *currItem = removeItems[i];
		currLiveItems.remove(currItem);
		GameEventManager::Instance()->ActionItemRemoved(*currItem);
		delete currItem;
		currItem = NULL;
	}
}

/**
 * Private helper function for updating active in-game projectiles, this will
 * essentially just 'tick' each projectile in play and remove out-of-bounds projectiles.
 */
void GameModel::UpdateActiveProjectiles(double seconds) {
	// Tick all the active projectiles and check for ones which are out of bounds
	std::list<Projectile*>& currProjectiles = this->GetActiveProjectiles();
	
	for (std::list<Projectile*>::iterator iter = currProjectiles.begin(); iter != currProjectiles.end();) {
		Projectile* currProjectile = *iter;
		currProjectile->Tick(seconds);

		// If the projectile is out of game bounds, destroy it
		if (this->IsOutOfGameBounds(currProjectile->GetPosition())) {
			iter = currProjectiles.erase(iter);

			// EVENT: Projectile is being removed...
			GameEventManager::Instance()->ActionProjectileRemoved(*currProjectile);

			delete currProjectile;
			currProjectile = NULL;
		}
		else {
			++iter;
		}
	}

}

/**
 * Private helper function for updating the collisions and effects of the various 
 * active beams (if any) in the game during the current tick.
 */
void GameModel::UpdateActiveBeams(double seconds) {
	const GameLevel* currentLevel = this->GetCurrentLevel();
	std::list<Beam*>& activeBeams = this->GetActiveBeams();
	bool beamIsDead = false;

	for (std::list<Beam*>::iterator beamIter = activeBeams.begin(); beamIter != activeBeams.end();) {
		// Get the current beam being iterated on and execute its effect on all the
		// level pieces it affects...
		Beam* currentBeam = *beamIter;
		assert(currentBeam != NULL);

		// Update the beam collisions... this needs to be done before anything or there
		// might be level pieces that were destroyed by the ball that we are trying to access in the beam
		currentBeam->UpdateCollisions(currentLevel);

		std::list<BeamSegment*>& beamParts = currentBeam->GetBeamParts();
		for (std::list<BeamSegment*>::iterator segIter = beamParts.begin(); segIter != beamParts.end(); ++segIter) {
			BeamSegment* currentBeamSeg = *segIter;
			assert(currentBeamSeg != NULL);

			// Cause the beam to collide for the given tick with the level piece, find out what
			// happened to the level piece and act accordingly...
			LevelPiece* collidingPiece = currentBeamSeg->GetCollidingPiece();
			if (collidingPiece != NULL) {
				LevelPiece* resultPiece = collidingPiece->TickBeamCollision(seconds, currentBeamSeg, this);
				
				// Check to see if the level is done
				if (currentLevel->IsLevelComplete()) {
					// The level was completed, move to the level completed state
					this->SetNextState(new LevelCompleteState(this));
				}

				// HACK: If we destroy a piece get out of this loop - so that if the piece is attached to
				// another beam segment we don't try to access it and we update collisions
				if (resultPiece != collidingPiece) {
					break;
				}

			}
		}

		// Tick the beam, check for its death and clean up if necessary
		beamIsDead = currentBeam->Tick(seconds);
		if (beamIsDead) {
			beamIter = activeBeams.erase(beamIter);

			// EVENT: Beam removed...
			GameEventManager::Instance()->ActionBeamRemoved(*currentBeam);

			delete currentBeam;
			currentBeam = NULL;
		}
		else {
			++beamIter;
		}
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
 * Remove all active items of the given type from the game - their effects will be turned off after this function call.
 * Returns: true if any items of the given type were fournd and removed, false otherwise.
 */
bool GameModel::RemoveActiveGameItemsOfGivenType(const GameItem::ItemType& type) {
	bool foundItemType = false;
	for (std::list<GameItemTimer*>::iterator iter = this->activeTimers.begin(); iter != this->activeTimers.end();) {
		GameItemTimer* currTimer = *iter;
		if (currTimer->GetTimerItemType() == type) {
			iter = this->activeTimers.erase(iter);
			delete currTimer;
			currTimer = NULL;
			foundItemType = true;
		}
		else {
			 ++iter;
		}
	}
	return foundItemType;
}

/**
 * Function for adding a possible item drop for the given level piece.
 */
void GameModel::AddPossibleItemDrop(const LevelPiece& p) {
	// Make sure we're in a ball in play state...
	if (this->currState->GetType() != GameState::BallInPlayStateType) {
		return;
	}

	// Make sure we don't drop more items than the max allowable...
	if (this->currLiveItems.size() >= GameModelConstants::GetInstance()->MAX_LIVE_ITEMS) {
		return;
	}

	// Make sure we don't drop an item close to another dropping item in the same column of the level...
	for (std::list<GameItem*>::const_iterator iter = this->currLiveItems.begin(); iter != this->currLiveItems.end(); ++iter) {
		const GameItem* currItem = *iter;
		if (fabs(currItem->GetCenter()[0] - p.GetCenter()[0]) < EPSILON) {
			if (fabs(currItem->GetCenter()[1] - p.GetCenter()[1]) < 3 * GameItem::HALF_ITEM_HEIGHT) {
				return;
			}
		}
	}

	// We will drop an item based on probablility
	// TODO: Add probability based off multiplier and score stuff...
	double itemDropProb = GameModelConstants::GetInstance()->PROB_OF_ITEM_DROP;
	double randomNum    = Randomizer::GetInstance()->RandomNumZeroToOne();
	
	debug_output("Probability of drop: " << itemDropProb << " Number for deciding: " << randomNum);

	if (randomNum <= itemDropProb) {
		// If there are no allowable item drops for the current level then we drop nothing anyway
		if (this->GetCurrentLevel()->GetAllowableItemDropTypes().empty()) {
			return;
		}
		GameItem::ItemType itemType = GameItemFactory::GetInstance()->CreateRandomItemTypeForCurrentLevel(this, true);
		this->AddItemDrop(p, itemType);
	}
}

void GameModel::AddItemDrop(const LevelPiece& p, const GameItem::ItemType& itemType) {
	// We always drop items in this manor, even if we've exceeded the max!
	GameItem* newGameItem = GameItemFactory::GetInstance()->CreateItem(itemType, p.GetCenter(), this);
	this->currLiveItems.push_back(newGameItem);
	// EVENT: Item has been created and added to the game
	GameEventManager::Instance()->ActionItemSpawned(*newGameItem);
}

/**
 * Add a projectile to the game. The gamemodel object takes ownership of the provided projectile.
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
		case Beam::PaddleLaserBeam: {
				
				bool foundOtherLaserBeam = false;
				// Reset the time on the previous beam if there is one, otherwise spawn a new beam
				for (std::list<Beam*>::iterator iter = this->beams.begin(); iter != this->beams.end(); ++iter) {
					Beam* currBeam = *iter;
					if (currBeam->GetBeamType() == Beam::PaddleLaserBeam) {
						assert(foundOtherLaserBeam == false);
						foundOtherLaserBeam = true;
						currBeam->ResetTimeElapsed();
					}
				}
				if (foundOtherLaserBeam) {
					return;
				}

				addedBeam = new PaddleLaserBeam(this->GetPlayerPaddle(), this->GetCurrentLevel());
			}
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

/**
 * Add a level piece that requires 'ticking' (updates) every frame due to status effects applied to it, when in the "BallInPlay" state.
 * This allows us to selectively apply long term effects to level pieces.
 * Returns: true if the piece is freshly added as a update piece, false if it has already been added previously.
 */
bool GameModel::AddStatusUpdateLevelPiece(LevelPiece* p, const LevelPiece::PieceStatus& status) {
	assert(p != NULL);

	// First try to find the level piece among existing level pieces with status updates
	std::map<LevelPiece*, int32_t>::iterator findIter = this->statusUpdatePieces.find(p);
	if (findIter == this->statusUpdatePieces.end()) {
		// Nothing was found, add a new entry for the piece with the given status...
		this->statusUpdatePieces.insert(std::make_pair(p, static_cast<int32_t>(status)));
		assert(p->HasStatus(status));
		return true;
	}

	// We found an existing piece, check to see if it already is in the update list for the given status if it is then
	// we just ignore it otherwise we append the new status as well
	if ((findIter->second & status) == status) {
		return false;
	}

	findIter->second = (findIter->second | status);
	return true;
}

void GameModel::WipePieceFromAuxLists(LevelPiece* piece) {
	if (!this->doingPieceStatusListIteration) {
		// If the piece is in the status update piece list then we remove it from it (as
		// long as we aren't currently traversing the list!)
		std::map<LevelPiece*, int32_t>::iterator findIter = this->statusUpdatePieces.find(piece);
		if (findIter != this->statusUpdatePieces.end()) {
			this->statusUpdatePieces.erase(findIter);
		}
	}
}