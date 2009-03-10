#include "BallInPlayState.h"
#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "GameItem.h"
#include "GameItemFactory.h"

#include "../BlammoEngine/BlammoEngine.h"

BallInPlayState::BallInPlayState(GameModel* gm) : 
GameState(gm), debugItemDrop(NULL), timeSinceGhost(DBL_MAX) {
}

BallInPlayState::~BallInPlayState() {
	// If we are exiting being in play then clear all projectiles, items and timers
	this->gameModel->ClearProjectiles();
	this->gameModel->ClearLiveItems();
	this->gameModel->ClearActiveTimers();
}

void BallInPlayState::DebugDropItem(GameItem* item) {
	assert(item != NULL);
	if (this->debugItemDrop == NULL) {
		this->debugItemDrop = item;
	}
	else {
		delete item;
		item = NULL;
	}
}

/**
 * The action key was pressed while in play; that could mean several things
 * based on the items currently active for the player.
 */
void BallInPlayState::BallReleaseKeyPressed() {

	// Check for paddle items that use the action key...
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->Shoot(this->gameModel);

	
	// TODO: More action button related things go here...
	// - stickypaddle
	// - rocketpaddle
	// ...
}

/**
 * Since the ball is now in play this function will be doing A LOT, including:
 * - updating the ball's location and checking for collisions/death
 * - ... TBA
 */
void BallInPlayState::Tick(double seconds) {
	if (this->timeSinceGhost < GameModelConstants::GetInstance()->LENGTH_OF_GHOSTMODE) {
		this->timeSinceGhost += seconds;
	}	
	
	// The paddle can move while the ball is in play, of course
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->Tick(seconds);

	// Update the ball's current position
	GameBall* ball = this->gameModel->GetGameBall();
	ball->Tick(seconds);

	// Check for item-paddle collisions
	this->DoItemCollision();
	
	// Update any timers that are currently active in play
	this->UpdateActiveTimers(seconds);
	// Update any item drops that are currently active in play
	this->UpdateActiveItemDrops(seconds);
	// Update any particles that are currently active in play
	this->UpdateActiveProjectiles(seconds);

	// First check for ball-paddle collision
	Vector2D n;
	float d;
	bool didCollideWithPaddle = paddle->CollisionCheck(ball->GetBounds(), n, d);
	bool didCollideWithBlock = false;

	if (didCollideWithPaddle) {
		// Do ball-paddle collision
		this->DoBallCollision(*ball, n, d);
		// Tell the model that a ball collision occurred with the paddle
		this->gameModel->BallPaddleCollisionOccurred();
		return;
	}

	// Check for death (ball went out of bounds)
	if (this->IsOutOfGameBounds(ball->GetBounds().Center())) {
		this->gameModel->PlayerDied();
		return;
	}

	// Check for ball-levelpiece collisions
	const GameLevel *currLevel = this->gameModel->GetCurrentLevel();

	// Get the small set (maximum 4) of levelpieces based on the position of the ball...
	std::set<LevelPiece*> collisionPieces = currLevel->GetCollisionCandidates(*ball);
	for (std::set<LevelPiece*>::iterator pieceIter = collisionPieces.begin(); pieceIter != collisionPieces.end(); pieceIter++) {
		
		LevelPiece *currPiece = *pieceIter;
		didCollideWithBlock = currPiece->CollisionCheck(ball->GetBounds(), n, d);
		
		if (didCollideWithBlock) {
			
			// Check to see if the ball is a ghost ball, if so there's a chance the ball will 
			// lose its ability to collide for 1 second, also check to see if we're already in ghost mode
			// if so we won't collide with anything (except solid blocks)...
			if ((ball->GetBallType() & GameBall::GhostBall) == GameBall::GhostBall && currPiece->GhostballPassesThrough()) {
				
				if (this->timeSinceGhost < GameModelConstants::GetInstance()->LENGTH_OF_GHOSTMODE) {
					continue;
				}

				// If the ball is in ghost mode then it cannot hit anything other than the paddle and solid blocks...
				if (this->timeSinceGhost >= GameModelConstants::GetInstance()->LENGTH_OF_GHOSTMODE &&
					  Randomizer::GetInstance()->RandomNumZeroToOne() <= GameModelConstants::GetInstance()->PROB_OF_GHOSTBALL_BLOCK_MISS) {
					this->timeSinceGhost = 0.0;
					continue;
				}
			}

			// In the case that the ball is uber then we only reflect if the ball is not green
			if (((ball->GetBallType() & GameBall::UberBall) == GameBall::UberBall) && currPiece->UberballBlastsThrough()) {
				// Ignore collision...
			}
			else {
				this->DoBallCollision(*ball, n, d);
			}
			
			// Tell the model that a ball collision occurred with currPiece
			bool stateChanged = true;
			this->gameModel->CollisionOccurred(*ball, currPiece, stateChanged);

			// If stateChanged is set to true then this has been deleted and we need to exit immediately.
			if (stateChanged) {
				return;
			}
			break;	// Important that we break out of the loop since some blocks may no longer exist after a collision
		}
	}

	// Check for other misc. collisions (e.g., projectiles)
	// Grab a list of all paddle-related projectiles and test each one for collisions...
	std::list<Projectile*>& gameProjectiles = gameModel->GetActiveProjectiles();
	std::vector<std::list<Projectile*>::iterator> projectilesToDestroy;

	for(std::list<Projectile*>::iterator iter = gameProjectiles.begin(); iter != gameProjectiles.end(); iter++) {
		Projectile* currProjectile = *iter;
		
		// Find the any level pieces that the current projectile may have collided with and test for collision
		std::set<LevelPiece*> collisionPieces = currLevel->GetCollisionCandidates(*currProjectile);
		for (std::set<LevelPiece*>::iterator pieceIter = collisionPieces.begin(); pieceIter != collisionPieces.end(); pieceIter++) {
			LevelPiece *currPiece = *pieceIter;
			
			Collision::AABB2D projectileAABB(currProjectile->GetPosition() - Vector2D(currProjectile->GetHalfWidth(), currProjectile->GetHalfHeight()),
																			 currProjectile->GetPosition() + Vector2D(currProjectile->GetHalfWidth(), currProjectile->GetHalfHeight()));
			bool didCollide = currPiece->CollisionCheck(projectileAABB);
			
			if (didCollide) {
				bool stateChanged = true;
				this->gameModel->CollisionOccurred(currProjectile, currPiece, stateChanged);	// WARNING: This will destroy the projectile.
				
				// Despose of the projectile...
				projectilesToDestroy.push_back(iter);

				// If stateChanged is set to true then this has been deleted and we need to exit immediately.
				if (stateChanged) {
					return;
				}
				break;	// Important that we break out of the loop since some blocks may no longer exist after a collision
			}
		}
	}

	// Remove and delete all collided projectiles...
	for (std::vector<std::list<Projectile*>::iterator>::iterator iter = projectilesToDestroy.begin(); iter != projectilesToDestroy.end(); iter++) {
		Projectile* projectileToRemove = (**iter);
		gameProjectiles.erase(*iter);
		GameEventManager::Instance()->ActionProjectileRemoved(*projectileToRemove);
		delete projectileToRemove;
		projectileToRemove = NULL;
	}
		
}

/**
 * Private helper function for updating all active timers and deleting/removing
 * all expired timers.
 */
void BallInPlayState::UpdateActiveTimers(double seconds) {
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::vector<GameItemTimer*> removeTimers;
	for (std::list<GameItemTimer*>::iterator iter = activeTimers.begin(); iter != activeTimers.end(); iter++) {
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
void BallInPlayState::UpdateActiveItemDrops(double seconds) {
	// Update any items that may have been created
	std::vector<GameItem*> removeItems;
	std::list<GameItem*>& currLiveItems = this->gameModel->GetLiveItems();
	for(std::list<GameItem*>::iterator iter = currLiveItems.begin(); iter != currLiveItems.end(); iter++) {
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

	// Debug Item drop
	if (this->debugItemDrop != NULL) {
		currLiveItems.push_back(this->debugItemDrop);
		// EVENT: Item has been created and added to the game
		GameEventManager::Instance()->ActionItemSpawned(*this->debugItemDrop);
		this->debugItemDrop = NULL;
	}
}

/**
 * Private helper function for updating active in-game projectiles, this will
 * essentially just 'tick' each projectile in play and remove out-of-bounds projectiles.
 */
void BallInPlayState::UpdateActiveProjectiles(double seconds) {
	// Tick all the active projectiles and check for ones which are out of bounds
	std::vector<std::list<Projectile*>::iterator> removeProjectiles;
	std::list<Projectile*>& currProjectiles = gameModel->GetActiveProjectiles();
	for (std::list<Projectile*>::iterator iter = currProjectiles.begin(); iter != currProjectiles.end(); iter++) {
		Projectile* currProjectile = *iter;
		currProjectile->Tick(seconds);

		// If the projectile is out of game bounds, destroy it
		if (this->IsOutOfGameBounds(currProjectile->GetPosition())) {
			removeProjectiles.push_back(iter);
		}
	}

	// Remove any projectiles that are no longer active
	for (std::vector<std::list<Projectile*>::iterator>::iterator iter = removeProjectiles.begin(); iter != removeProjectiles.end(); iter++) {
		Projectile* currProjectile = (**iter);
		currProjectiles.erase(*iter);
		GameEventManager::Instance()->ActionProjectileRemoved(*currProjectile);
		delete currProjectile;
		currProjectile = NULL;
	}
}

/**
 * Private helper function for checking to see if an item collided with 
 * the player paddle and carrying out the necessary activity if one did.
 */
void BallInPlayState::DoItemCollision() {
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::list<GameItem*>& currLiveItems			= this->gameModel->GetLiveItems();
	std::vector<GameItem*> removeItems;

	// Check to see if the paddle hit any items, if so, activate those items
	for(std::list<GameItem*>::iterator iter = currLiveItems.begin(); iter != currLiveItems.end(); iter++) {
		GameItem *currItem = *iter;
		
		if (currItem->CollisionCheck(*this->gameModel->GetPlayerPaddle())) {
			// EVENT: Item was obtained by the player paddle
			GameEventManager::Instance()->ActionItemPaddleCollision(*currItem, *this->gameModel->GetPlayerPaddle());
			
			// There was a collision with the item and the player paddle: activate the item
			// and then delete it. If the item causes the creation of a timer then add the timer to the list
			// of active timers.
			GameItemTimer* newTimer = new GameItemTimer(currItem);
			assert(newTimer != NULL);
			activeTimers.push_back(newTimer);	
			removeItems.push_back(currItem);
			GameEventManager::Instance()->ActionItemRemoved(*currItem);
		}
	}

	// Remove any items from the live items list if they have been collided with
	for (unsigned int i = 0; i < removeItems.size(); i++) {
		GameItem* currItem = removeItems[i];
		currLiveItems.remove(currItem);
	}

}

// n must be normalized
// d is the distance from the center of the ball to the line that was collided with
// when d is negative the ball is inside the line, when positive it is outside
void BallInPlayState::DoBallCollision(GameBall& b, const Vector2D& n, float d) {

	// Position the ball so that it is against the collision line, exactly
	if (fabs(d) > EPSILON) {
		int signDist = NumberFuncs::SignOf(d);
		if (signDist == -1) {
			// The ball is on the other side of the collision line, fix its position so that
			// it is exactly against the line
			b.SetCenterPosition(b.GetBounds().Center() + (-d + (b.GetBounds().Radius() + EPSILON)) * n);
		}
		else {
			// The ball is still on the out facing normal side of the line, but fix its position so that
			// it is exactly against the line
			b.SetCenterPosition(b.GetBounds().Center() + (b.GetBounds().Radius() + EPSILON - d) * n);
		}
	}

	// Figure out the reflection vector and speed
	Vector2D reflVecHat					= Vector2D::Normalize(Reflect(b.GetVelocity(), n));
	GameBall::BallSpeed reflSpd	= b.GetSpeed();
	
	// This should NEVER happen
	assert(reflSpd != GameBall::ZeroSpeed);
	if (reflSpd == GameBall::ZeroSpeed) {
		return;
	}

	// Figure out the angle between the normal and the reflection...
	float angleOfReflInDegs = Trig::radiansToDegrees(acosf(Vector2D::Dot(n, reflVecHat)));
	float diffAngleInDegs		= GameBall::MIN_BALL_ANGLE_IN_DEGS - angleOfReflInDegs;

	// Make sure the reflection is big enough to not cause 
	// an annoying slow down in the game
	if (diffAngleInDegs > EPSILON) {

		// We need to figure out which way to rotate the velocity
		// to ensure it's at least the MIN_BALL_ANGLE_IN_RADS
		Vector2D newReflVelHat = Rotate(diffAngleInDegs, reflVecHat);

		// Check to see if it's still the case, if it is then we rotated the wrong way
		float newAngleInDegs =  Trig::radiansToDegrees(acosf(Vector2D::Dot(n, newReflVelHat)));
		if (newAngleInDegs < GameBall::MIN_BALL_ANGLE_IN_DEGS) {
			newReflVelHat = Rotate(-diffAngleInDegs, reflVecHat);
		}
		
		reflVecHat = newReflVelHat;
	}

	// Reflect the ball off the normal
	b.SetVelocity(reflSpd, reflVecHat);
}

/**
 * Private helper function to determine if the given position is
 * out of game bounds or not.
 * Returns: true if out of bounds, false otherwise.
 */
bool BallInPlayState::IsOutOfGameBounds(const Point2D& pos) {
	GameLevel* currLevel = this->gameModel->GetCurrentLevel();
	float levelWidthBounds	= currLevel->GetLevelUnitWidth()  + GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE;
	float levelHeightBounds = currLevel->GetLevelUnitHeight() + GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE;

	return pos[1] <= GameLevel::Y_COORD_OF_DEATH || 
				 pos[1] >= levelHeightBounds ||
				 pos[0] <= -GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE ||
				 pos[0] >= levelWidthBounds;
}