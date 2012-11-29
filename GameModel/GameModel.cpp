/**
 * GameModel.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameModel.h"

#include "BallInPlayState.h"
#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "GameOverState.h"
#include "LevelCompleteState.h"
#include "LevelStartState.h"
#include "PaddleLaserBeam.h"
#include "CollateralBlock.h"
#include "PointAward.h"
#include "BallBoostModel.h"
#include "GameProgressIO.h"
#include "SafetyNet.h"
#include "PaddleMineProjectile.h"

#include "../BlammoEngine/StringHelper.h"
#include "../ResourceManager.h"

GameModel::GameModel(const GameModel::Difficulty& initDifficulty, bool ballBoostIsInverted) : 
currWorldNum(0), currState(NULL), currPlayerScore(0), numStarsAwarded(0), currLivesLeft(0),
livesAtStartOfLevel(0), numLivesLostInLevel(0), maxNumLivesAllowed(0),
pauseBitField(GameModel::NoPause), isBlackoutActive(false), areControlsFlipped(false), gameTransformInfo(new GameTransformMgr()), 
nextState(NULL), boostModel(NULL), doingPieceStatusListIteration(false), progressLoadedSuccessfully(false),
droppedLifeForMaxMultiplier(false), safetyNet(NULL), ballBoostIsInverted(ballBoostIsInverted), difficulty(initDifficulty) {
	
	// Initialize the worlds for the game - the set of worlds can be found in the world definition file
    std::istringstream* inFile = ResourceManager::GetInstance()->FilepathToInStream(GameModelConstants::GetInstance()->GetWorldDefinitonFilePath());
    std::string currWorldPath;
    bool success = true;
    while (std::getline(*inFile, currWorldPath)) {
        currWorldPath = stringhelper::trim(currWorldPath);
        if (!currWorldPath.empty()) {
            GameWorld* newGameWorld = new GameWorld(GameModelConstants::GetInstance()->GetResourceWorldDir() + 
                                                    std::string("/") + currWorldPath, *this->gameTransformInfo);

            success = newGameWorld->Load();
            assert(success);

            this->worlds.push_back(newGameWorld);
            currWorldPath.clear();
        }
    }
    delete inFile;
    inFile = NULL;

    // Load data for all the worlds
    this->progressLoadedSuccessfully = GameProgressIO::LoadGameProgress(this);

	// Initialize paddle and the first ball
	this->playerPaddle = new PlayerPaddle();
	this->balls.push_back(new GameBall());

    this->SetDifficulty(initDifficulty);
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

    // The boost model should always have been destroyed by now!
    assert(this->boostModel == NULL);
}

void GameModel::ResetLevelValues(int numLives) {
	// Reset the score, lives, etc.
	this->currPlayerScore = GameModelConstants::GetInstance()->INIT_SCORE;
    this->numStarsAwarded = 0;
	this->SetInitialNumberOfLives(numLives);
	this->numInterimBlocksDestroyed = 0;	// Don't use set here, we don't want an event
    this->maxInterimBlocksDestroyed = 0;
    this->droppedLifeForMaxMultiplier = false;
    this->ResetNumAcquiredItems();
    this->ResetLevelTime();
    this->DestroySafetyNet();
    
	// Clear up the model
    this->ClearStatusUpdatePieces();
    this->ClearProjectiles();
    this->ClearBeams();
    this->ClearLiveItems();
    this->ClearActiveTimers();
	this->gameTransformInfo->Reset();

    this->ResetScore();
    this->ResetLevelTime();
    this->ResetNumAcquiredItems();
    this->ResetLivesLostCounter();
}

void GameModel::PerformLevelCompletionChecks() {
    GameWorld* currWorld = this->GetCurrentWorld();
    GameLevel* currLevel = currWorld->GetCurrentLevel();
	
    if (currLevel->IsLevelComplete()) {
		// The level was completed, move to the level completed state
		this->SetNextState(GameState::LevelCompleteStateType);
	}
    else if (currLevel->IsLevelAlmostComplete()) {
        currLevel->SignalLevelAlmostCompleteEvent();

        // When a level is almost complete we decrease the amount of time it takes for a boost to
        // charge so that the player doesn't feel like they have to wait too long to finish
        if (this->boostModel != NULL) {
            this->boostModel->SetBoostChargeTime(BallBoostModel::LEVEL_ALMOST_COMPLETE_CHARGE_TIME_SECONDS);
        }
    }

}

/**
 * Called in order to completely reset the game state and load the
 * given zero-based index world and level number.
 */
void GameModel::StartGameAtWorldAndLevel(int worldNum, int levelNum) {
	this->SetCurrentWorldAndLevel(worldNum, levelNum, true);
    this->ResetLevelValues(GameModelConstants::GetInstance()->INIT_LIVES_LEFT);
    this->SetNextState(GameState::LevelStartStateType);
    this->UpdateState();
}

/**
 * Called when we want to begin/restart the model from the
 * beginning of the game, this will reinitialize the model to
 * the first world, level, etc.
 */
void GameModel::BeginOrRestartGame() {
    this->StartGameAtWorldAndLevel(GameModelConstants::GetInstance()->INITIAL_WORLD_NUM, 0);
}

void GameModel::ResetCurrentLevel() {
    this->SetNextState(GameState::LevelStartStateType);
    this->ResetLevelValues(this->GetLivesAtStartOfLevel());
    this->SetCurrentWorldAndLevel(this->currWorldNum, static_cast<int>(this->GetCurrentLevel()->GetLevelNumIndex()), false);
}

// Called in order to make sure the game is no longer processing or generating anything
void GameModel::ClearGameState() {
	// Delete all world items, timers and thingys
	this->ClearStatusUpdatePieces();
	this->ClearLiveItems();
	this->ClearActiveTimers();
	this->ClearProjectiles();
	this->ClearBeams();
    this->DestroySafetyNet();

	// Delete the state
	delete this->currState;
	this->currState = NULL;
    if (this->nextState != NULL) {
        delete this->nextState;
        this->nextState = NULL;
    }
    
	this->gameTransformInfo->Reset();
    this->ResetScore();
    this->ResetLevelTime();
    this->ResetNumAcquiredItems();
    this->ResetLivesLostCounter();
}

void GameModel::SetCurrentWorldAndLevel(int worldNum, int levelNum, bool sendNewWorldEvent) {
	assert(worldNum >= 0 && worldNum < static_cast<int>(this->worlds.size()));
	
    // NOTE: We don't unload anything since it all gets loaded when the game model is initialized
    // worlds don't take up too much memory so this is fine
	// Clean up the previous world
	//GameWorld* prevWorld = this->GetCurrentWorld();
	//prevWorld->Unload();

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
    assert(levelNum >= 0 && levelNum < static_cast<int>(world->GetNumLevels()));
	world->SetCurrentLevel(this, levelNum);
	GameLevel* currLevel = world->GetCurrentLevel();
	assert(currLevel != NULL);

    if (sendNewWorldEvent) {
	    // EVENT: World started...    
	    GameEventManager::Instance()->ActionWorldStarted(*world);
    }

	// EVENT: New Level Started
	GameEventManager::Instance()->ActionLevelStarted(*world, *currLevel);

	// Tell the paddle what the boundries of the level are and reset the paddle
	this->playerPaddle->SetNewMinMaxLevelBound(currLevel->GetPaddleMinBound(), currLevel->GetPaddleMaxBound()); // resets the paddle
	//this->playerPaddle->ResetPaddle();

    // Reload all progress numbers
    this->progressLoadedSuccessfully = GameProgressIO::LoadGameProgress(this);

    this->PerformLevelCompletionChecks();
}

// Get the world with the given name in this model, NULL if no such world exists
GameWorld* GameModel::GetWorldByName(const std::string& name) {
    for (std::vector<GameWorld*>::const_iterator iter = this->worlds.begin(); iter != this->worlds.end(); ++iter) {
        GameWorld* world = *iter;
        if (world->GetName().compare(name) == 0) {
            return world;
        }
    }
    return NULL;
}

void GameModel::SetDifficulty(const GameModel::Difficulty& difficulty) {
    static const float EASY_DIFFICULTY_BALL_SPEED_DELTA = -1.33f;
    static const float MED_DIFFICULTY_BALL_SPEED_DELTA  = 0.0f;
    static const float HARD_DIFFICULTY_BALL_SPEED_DELTA = 1.5f;

    //float ballSpeedDeltaBefore = 0.0f;
    //float ballSpeedDeltaAfter  = 0.0f;

    //switch (this->GetDifficulty()) {
    //    case GameModel::EasyDifficulty:
    //        ballSpeedDeltaBefore = EASY_DIFFICULTY_BALL_SPEED_DELTA;
    //        break;
    //    case GameModel::MediumDifficulty:
    //        ballSpeedDeltaBefore = MED_DIFFICULTY_BALL_SPEED_DELTA;
    //        break;
    //    case GameModel::HardDifficulty:
    //        ballSpeedDeltaBefore = HARD_DIFFICULTY_BALL_SPEED_DELTA;
    //        break;
    //    default:
    //        assert(false);
    //        return;
    //}

    switch (difficulty) {
        case GameModel::EasyDifficulty:

            // Disable the paddle release timer
            PlayerPaddle::SetEnablePaddleReleaseTimer(false);
            // Make the boost time longer
            BallBoostModel::SetMaxBulletTimeDuration(4.0);
            // Make ball speed slower
            GameBall::SetNormalSpeed(GameBall::DEFAULT_NORMAL_SPEED + EASY_DIFFICULTY_BALL_SPEED_DELTA);

            // Make chance of nice item drops higher (done automatically in GameItemFactory)

            //ballSpeedDeltaAfter = EASY_DIFFICULTY_BALL_SPEED_DELTA;
            break;
        
        case GameModel::MediumDifficulty:
            // Enable the paddle release timer
            PlayerPaddle::SetEnablePaddleReleaseTimer(true);
            // Make the boost time typical
            BallBoostModel::SetMaxBulletTimeDuration(2.0);
            // Make ball speed normal
            GameBall::SetNormalSpeed(GameBall::DEFAULT_NORMAL_SPEED + MED_DIFFICULTY_BALL_SPEED_DELTA);

            // Make item drops normal random (done automatically in GameItemFactory)

            //ballSpeedDeltaAfter = MED_DIFFICULTY_BALL_SPEED_DELTA;
            break;
        
        case GameModel::HardDifficulty:
            // Enable the paddle release timer
            PlayerPaddle::SetEnablePaddleReleaseTimer(true);
            // Make the boost time short
            BallBoostModel::SetMaxBulletTimeDuration(1.25);
            // Make ball speed faster
            GameBall::SetNormalSpeed(GameBall::DEFAULT_NORMAL_SPEED + HARD_DIFFICULTY_BALL_SPEED_DELTA);
            
            // Make chance of not-so-nice item drops higher (done automatically in GameItemFactory)

            //ballSpeedDeltaAfter = HARD_DIFFICULTY_BALL_SPEED_DELTA;
            break;

        default:
            assert(false);
            return;

    }
    this->difficulty = difficulty;

    // If the ball(s) are in play then we automatically increase/decrease it's speed by the delta in
    // the difficulty change...
    // DONT DO THIS - THE PLAYER CAN CHEAT!!!
    //if (this->currState != NULL && this->currState->GetType() == GameState::BallInPlayStateType) {
    //    float ballSpeedDelta = ballSpeedDeltaAfter - ballSpeedDeltaBefore;
    //    for (std::list<GameBall*>::iterator iter = this->balls.begin(); iter != this->balls.end(); ++iter) {
    //        GameBall* currBall = *iter;
    //        currBall->SetSpeed(currBall->GetSpeed() + ballSpeedDelta);
    //    }
    //}
}

bool GameModel::ActivateSafetyNet() {
    if (this->safetyNet != NULL) {
        return false;
    }

    this->safetyNet = new SafetyNet(*this->GetCurrentLevel());
    // EVENT: We just created a brand new ball safety net...
	GameEventManager::Instance()->ActionBallSafetyNetCreated();
    return true;
}

void GameModel::DestroySafetyNet() {
    if (this->safetyNet != NULL) {
        delete this->safetyNet;
        this->safetyNet = NULL;
    }
}

/**
 * Called when a collision occurs between a projectile and a level piece.
 * Deals with all the important effects a collision could have on the game model.
 * Precondition: p != NULL
 */
void GameModel::CollisionOccurred(Projectile* projectile, LevelPiece* p) {
	assert(p != NULL);
	assert(p->GetType() != LevelPiece::Empty);

	bool alreadyCollided = projectile->IsLastThingCollidedWith(p) || projectile->IsAttachedToSomething();

	// Collide the projectile with the piece...
	LevelPiece* pieceAfterCollision = p->CollisionOccurred(this, projectile); 	// WARNING: This can destroy p.

    if (!alreadyCollided) {
        projectile->LevelPieceCollisionOccurred(p);
    }

	// EVENT: First-time Projectile-Block Collision
	if (!alreadyCollided) {
		GameEventManager::Instance()->ActionProjectileBlockCollision(*projectile, *pieceAfterCollision);
	}

	// Check to see if the level is done
	this->PerformLevelCompletionChecks();
}

void GameModel::CollisionOccurred(Projectile* projectile, PlayerPaddle* paddle) {
	assert(projectile != NULL);
	assert(paddle != NULL);

    // Inform the projectile and paddle of the collision
    projectile->PaddleCollisionOccurred(paddle);
	paddle->HitByProjectile(this, *projectile);

	// Check to see if the level is done
    this->PerformLevelCompletionChecks();
}

/**
 * Called when a collision occurs between the ball and a level piece.
 * Deals with all the important effects a collision could have on the game model.
 * Precondition: p != NULL
 */
void GameModel::CollisionOccurred(GameBall& ball, LevelPiece* p) {
	assert(p != NULL);
    assert(p->GetType() != LevelPiece::Empty);

	// EVENT: Ball-Block Collision
	GameEventManager::Instance()->ActionBallBlockCollision(ball, *p);

	// Collide the ball with the level piece directly
	//LevelPiece* pieceAfterCollision =
    p->CollisionOccurred(this, ball); // WARNING: This can destroy p.
	ball.BallCollided();

	// If the ball is attached to the paddle then we always set the last piece it collided with to NULL
	// This ensures that the ball ALWAYS collides with everything in its way while on the paddle
	if (this->playerPaddle->GetAttachedBall() == &ball) {
		ball.SetLastPieceCollidedWith(NULL);
	}

	// Check to see if the ball is being teleported in ball-camera mode - in this case
	// we move into a new game state to play the wormhole minigame
	//if (pieceAfterCollision->GetType() == LevelPiece::Portal && ball.GetIsBallCameraOn()) {
		// TODO
	//}

	// Check to see if the level is done
	this->PerformLevelCompletionChecks();
}

float GameModel::GetTimeDialationFactor() const {
    if (this->boostModel == NULL) { return 1.0f; }
    return this->boostModel->GetTimeDialationFactor();
}

void GameModel::SetInvertBallBoostDir(bool isInverted) {
    this->ballBoostIsInverted = isInverted;
    if (this->boostModel != NULL) {
        this->boostModel->SetInvertBallBoostDir(isInverted);
    }
}

void GameModel::BallPaddleCollisionOccurred(GameBall& ball) {
	ball.BallCollided();
	//ball.SetLastThingCollidedWith(this->playerPaddle);

	// Modify the ball velocity using the current paddle speed
	Vector2D paddleVel = this->playerPaddle->GetVelocity();
	
	if (fabs(paddleVel[0]) > EPSILON) {
		// The paddle has a considerable velocity, we should augment the ball's
		// tragectory based on this...
		int angleDecSgn = -NumberFuncs::SignOf(paddleVel[0]);
		float fractionOfSpeed = fabs(paddleVel[0]) / PlayerPaddle::DEFAULT_MAX_SPEED;
		float angleChange = angleDecSgn * fractionOfSpeed * PlayerPaddle::DEFLECTION_DEGREE_ANGLE;

        if ((this->playerPaddle->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
            // When the shield is active the ball should be just off the shield...
            // NOTE: This is already should be done automatically
        }
        else {
            float bottomYOfPaddle = this->playerPaddle->GetCenterPosition()[1] - this->playerPaddle->GetHalfHeight();
            if (ball.GetCenterPosition2D()[1] < bottomYOfPaddle) {
                // Set the ball to be just off the paddle in cases where the ball is below it
                ball.SetCenterPosition(Point2D(ball.GetCenterPosition2D()[0], bottomYOfPaddle));
            }
        }

		Vector2D ballVel    = ball.GetVelocity();
		Vector2D ballVelHat = Vector2D::Normalize(ballVel);
		float ballSpd		= ball.GetSpeed(); 

		// Rotate the ball's velocity vector to reflect the momentum of the paddle
		ball.SetVelocity(ballSpd, Vector2D::Rotate(angleChange, ballVelHat));
		
		// Make sure the ball goes upwards (it can't reflect downwards off the paddle or the game would suck)
		if (acosf(std::min<float>(1.0f, 
            std::max<float>(-1.0f, Vector2D::Dot(ballVelHat, Vector2D(0, 1))))) > 
            ((M_PI / 2.0f) - GameBall::MIN_BALL_ANGLE_IN_RADS)) {

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

	// Reset the number of blocks that have been destroyed since the last paddle hit - this will
    // also get rid of any score multiplier
	this->SetNumInterimBlocksDestroyed(0);
}

/**
 * Called when the player/ball dies (falls into the bottomless pit of doom). 
 * This will deal with all the effects a death has on the game model.
 */
void GameModel::BallDied(GameBall* deadBall, bool& stateChanged) {
	assert(deadBall != NULL);

	// EVENT: A Ball has died
	GameEventManager::Instance()->ActionBallDied(*deadBall);
    this->RemoveActiveGameItemsForThisBallOnly(deadBall);

	// Do nasty stuff to player only if that was the last ball left
	if (this->balls.size() == 1) {
		// Reset the multiplier
		this->SetNumInterimBlocksDestroyed(0);
        // Increment the number of lives lost in the current level
        this->numLivesLostInLevel++;
		
		// Decrement the number of lives left
		this->SetLivesLeft(this->currLivesLeft-1);

		// EVENT: All Balls are dead
		GameEventManager::Instance()->ActionAllBallsDead(this->currLivesLeft);

		deadBall->ResetBallAttributes();

		// Set the appropriate state based on the number of lives left...
		if (this->IsGameOver()) {
			// Game Over
            this->SetNextState(GameState::GameOverStateType);
		}
		else {
            this->SetNextState(GameState::BallOnPaddleStateType);
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
	this->PerformLevelCompletionChecks();
}

/**
 * Execute projectile collisions on everything relevant that's in play in the game model.
 */
void GameModel::DoProjectileCollisions(double dT) {

#define PROJECTILE_CLEANUP(p) GameEventManager::Instance()->ActionProjectileRemoved(*p); \
                              delete p; \
                              p = NULL
													 
	// Grab a list of all paddle-related projectiles and test each one for collisions...
	std::list<Projectile*>& gameProjectiles = this->GetActiveProjectiles();
	bool destroyProjectile = false;
	bool didCollide = false;
	bool incIter = true;

    GameLevel* currLevel = this->GetCurrentLevel();

	for (std::list<Projectile*>::iterator iter = gameProjectiles.begin(); iter != gameProjectiles.end();) {
		Projectile* currProjectile = *iter;
		
		// Grab bounding lines from the projectile to test for collision
		BoundingLines projectileBoundingLines = currProjectile->BuildBoundingLines();

		// Check to see if the projectile collided with the player paddle
		if (this->playerPaddle->CollisionCheckWithProjectile(*currProjectile, projectileBoundingLines)) {
			
			this->CollisionOccurred(currProjectile, this->playerPaddle);

			// Destroy the projectile if necessary
			destroyProjectile = this->playerPaddle->ProjectileIsDestroyedOnCollision(*currProjectile);
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
        if (this->IsSafetyNetActive()) {
            if (this->safetyNet->ProjectileCollisionCheck(projectileBoundingLines)) {
                
                currProjectile->SafetyNetCollisionOccurred(this->safetyNet);
                
                // EVENT: The projectile was just destroyed by the safety net
                GameEventManager::Instance()->ActionProjectileSafetyNetCollision(*currProjectile, *this->safetyNet);

                if (currProjectile->BlastsThroughSafetyNets()) {
                    this->DestroySafetyNet();
                    // EVENT: The projectile just destroyed the safety net
                    GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*currProjectile);
                }

                // Check to see if the projectile will be destroyed due to the collision...
                if (currProjectile->IsDestroyedBySafetyNets()) {
                    iter = gameProjectiles.erase(iter);
				    PROJECTILE_CLEANUP(currProjectile);
				    continue;
                }

			    ++iter;
			    continue;
		    }
        }

        // Check if the projectile collided with any tesla lightning arcs...
        if (currLevel->IsDestroyedByTelsaLightning(*currProjectile)) {
            if (currLevel->TeslaLightningCollisionCheck(projectileBoundingLines)) {
                
                // In the special case of a rocket projectile we cause an explosion...
                if (currProjectile->IsRocket()) {
                    std::set<LevelPiece*> collisionPieces = 
                        this->GetCurrentLevel()->GetLevelPieceCollisionCandidates(currProjectile->GetPosition(), EPSILON);
                    if (!collisionPieces.empty()) {
                        assert(dynamic_cast<RocketProjectile*>(currProjectile) != NULL);
                        currLevel->RocketExplosion(this, static_cast<RocketProjectile*>(currProjectile), *collisionPieces.begin());
                    }
                }
                // In the other special case of a mine projectile we cause an explosion...
                else if (currProjectile->GetType() == Projectile::PaddleMineBulletProjectile) {
                    assert(dynamic_cast<PaddleMineProjectile*>(currProjectile) != NULL);
                    currLevel->MineExplosion(this, static_cast<PaddleMineProjectile*>(currProjectile));
                }

			    // Despose of the projectile...
                iter = gameProjectiles.erase(iter);
                PROJECTILE_CLEANUP(currProjectile);
                continue;    
            }
        }

		// Find the any level pieces that the current projectile may have collided with and test for collision
		std::set<LevelPiece*> collisionPieces = this->GetCurrentLevel()->GetLevelPieceCollisionCandidates(*currProjectile);
        incIter = true;	// Keep track of whether we need to increment the iterator or not
		for (std::set<LevelPiece*>::iterator pieceIter = collisionPieces.begin(); pieceIter != collisionPieces.end(); ++pieceIter) {
			LevelPiece *currPiece = *pieceIter;
			
			// Test for a collision between the projectile and current level piece
			didCollide = currPiece->CollisionCheck(projectileBoundingLines, currProjectile->GetVelocityDirection());
			if (didCollide) {

				// WARNING/IMPORTANT!!!! This needs to be before the call to CollisionOccurred or else the
                // currPiece may already be destroyed.
				destroyProjectile = currPiece->ProjectileIsDestroyedOnCollision(currProjectile);

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

        // Lastly we perform a modify level update for the projectile - certain projectiles can act
        // 'intelligently' based on the level state and cause alteration to it as well
        if (currProjectile != NULL && currProjectile->ModifyLevelUpdate(dT, *this)) {
            iter = gameProjectiles.erase(iter);
		    PROJECTILE_CLEANUP(currProjectile);
		    continue;
        }

		if (incIter) {
			++iter;
		}
	}

#undef PROJECTILE_CLEANUP

	// Check to see if the level is done
    this->PerformLevelCompletionChecks();
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
		currItem->Tick(seconds, *this);
		
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
		currProjectile->Tick(seconds, *this);

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
				this->PerformLevelCompletionChecks();

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

// Increment the player's score in the game
void GameModel::IncrementScore(PointAward& pointAward) {

	if (pointAward.GetPointAmount() != 0) {
        
        // Let the point award augment the current multiplier based on its properties...
        pointAward.SetMultiplierAmount(this->GetCurrentMultiplier());
        float currMultiplier = pointAward.GetMultiplierAmount();

        // Calculate the final score increment amount
        int incrementAmt = static_cast<int>(currMultiplier * pointAward.GetPointAmount());
        this->currPlayerScore += incrementAmt;

		// EVENT: Score was changed
        GameEventManager::Instance()->ActionScoreChanged(this->currPlayerScore);
        // EVENT: Point notification
        GameEventManager::Instance()->ActionPointNotification(pointAward);

        // Check to see if the number of stars changed...
        GameLevel* currLevel = this->GetCurrentLevel();
        int numStarsForCurrScore = currLevel->GetNumStarsForScore(this->currPlayerScore);
        if (this->numStarsAwarded != numStarsForCurrScore) {
            int oldNumStars = this->numStarsAwarded;
            this->numStarsAwarded = numStarsForCurrScore;
            // EVENT: The number of stars changed
            GameEventManager::Instance()->ActionNumStarsChanged(oldNumStars, this->numStarsAwarded);
        }
    }
}
//void GameModel::IncrementScore(std::list<PointAward>& pointAwardsList) {
//    for (std::list<PointAward>::iterator iter = pointAwardsList.begin(); iter != pointAwardsList.end(); ++iter) {
//        this->IncrementScore(*iter);
//    }
//}

// Set the number of consecutive blocks hit by the ball in the interrum between
// when it leaves and returns to the player paddle
void GameModel::SetNumInterimBlocksDestroyed(int value, const Point2D& pos) {
	assert(value >= 0);
	if (value != this->numInterimBlocksDestroyed) {

        int oldMultiplier = this->GetCurrentMultiplier();
		this->numInterimBlocksDestroyed = value;
        this->maxInterimBlocksDestroyed = std::max<int>(this->maxInterimBlocksDestroyed, this->numInterimBlocksDestroyed);

        int newMultiplier = this->GetCurrentMultiplier();
		
        // EVENT: The value has changed for the number of interim blocks destroyed (the multiplier part counter)
        GameEventManager::Instance()->ActionScoreMultiplierCounterChanged(this->numInterimBlocksDestroyed);

		// EVENT: The score multiplier has changed
        if (oldMultiplier != newMultiplier) {
		    GameEventManager::Instance()->ActionScoreMultiplierChanged(newMultiplier, pos);
        }

        // Reset the dropped life flag if the multiplier is lower than the max multiplier
        if (newMultiplier < 4) {
            this->droppedLifeForMaxMultiplier = false;
        }
	}
}

/**
 * Gets the current score multiplier based on the number of blocks that have been
 * currently destroyed between paddle hits.
 */
int GameModel::GetCurrentMultiplier() const {
    if (this->numInterimBlocksDestroyed >=
        GameModelConstants::GetInstance()->FOUR_TIMES_MULTIPLIER_NUM_BLOCKS) {
        return 4;
    }
    else if (this->numInterimBlocksDestroyed >= 
        GameModelConstants::GetInstance()->THREE_TIMES_MULTIPLIER_NUM_BLOCKS) {
        return 3;
    }
    else if (this->numInterimBlocksDestroyed >= 
        GameModelConstants::GetInstance()->TWO_TIMES_MULTIPLIER_NUM_BLOCKS) {
        return 2;
    }

    return 1;
}

void GameModel::IncrementNumAcquiredItems(const GameItem* item) {
    assert(item != NULL);
    switch (item->GetItemDisposition()) {
        case GameItem::Good:
            this->numGoodItemsAcquired++;
            break;
        case GameItem::Neutral:
            this->numNeutralItemsAcquired++;
            break;
        case GameItem::Bad:
            this->numBadItemsAcquired++;
            break;
        default:
            assert(false);
            break;
    }
}

/**
 * Clears the list of all projectiles that are currently in existance in the game.
 */
void GameModel::ClearProjectiles() {
    
	for (std::list<Projectile*>::iterator iter = this->projectiles.begin();
         iter != this->projectiles.end(); ++iter) {

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

void GameModel::ClearBallsToOne() {
	// Set the number of balls that exist to just 1
	if (this->balls.size() == 1) {
        return;
    }
    else if (this->balls.size() == 0) {
        this->balls.push_back(new GameBall());
        return;
    }

	std::list<GameBall*>::iterator ballIter = this->balls.begin();
	++ballIter;
	for (; ballIter != this->balls.end(); ++ballIter) {
		GameBall* ballToDestroy = *ballIter;
		delete ballToDestroy;
		ballToDestroy = NULL;
	}

	// Grab the last ball left and make it the only element in the entire game balls array
	GameBall* onlyBallLeft = (*this->balls.begin());
	assert(onlyBallLeft != NULL);
	this->balls.clear();
	this->balls.push_back(onlyBallLeft);

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

void GameModel::RemoveActiveGameItemsForThisBallOnly(const GameBall* ball) {
    // Check to see if there are any active effect timers that JUST concern the ball that died,
    // if so we expire and remove them 
    for (std::list<GameItemTimer*>::iterator iter = this->activeTimers.begin(); iter != this->activeTimers.end();) {
	    GameItemTimer* currTimer = *iter;
        if (currTimer->GetAssociatedBalls().find(ball) != currTimer->GetAssociatedBalls().end()) {
            currTimer->RemoveAssociatedBall(ball);
            if (currTimer->GetAssociatedBalls().empty()) {
		        iter = this->activeTimers.erase(iter);
		        delete currTimer;
		        currTimer = NULL;
                continue;
            }
        }
        ++iter;
    }
}

/**
 * Function for adding a possible item drop for the given level piece.
 */
void GameModel::AddPossibleItemDrop(const LevelPiece& p) {
	// Make sure we're in a ball in play state...
	if (this->currState->GetType() != GameState::BallInPlayStateType) {
		return;
	}

    // Special case where the multiplier is about to change (or already changed) to the max multiplier
    // and the player is missing a life - in this case we will be dropping a life-up item
    if (!this->droppedLifeForMaxMultiplier &&
        this->numInterimBlocksDestroyed >= GameModelConstants::GetInstance()->FOUR_TIMES_MULTIPLIER_NUM_BLOCKS - 1 &&
        this->currLivesLeft < GameModelConstants::GetInstance()->MAXIMUM_POSSIBLE_LIVES) {

        this->AddItemDrop(p.GetCenter(), GameItem::LifeUpItem);
        this->droppedLifeForMaxMultiplier = true;
        return;
    }

	// Make sure we don't drop more items than the max allowable...
	if (this->currLiveItems.size() >= GameModelConstants::GetInstance()->MAX_LIVE_ITEMS) {
		return;
	}

	// Make sure we don't drop an item close to another dropping item in the same column of the level...
	for (std::list<GameItem*>::const_iterator iter = this->currLiveItems.begin();
         iter != this->currLiveItems.end(); ++iter) {

		const GameItem* currItem = *iter;
		if (fabs(currItem->GetCenter()[0] - p.GetCenter()[0]) < EPSILON) {
			if (fabs(currItem->GetCenter()[1] - p.GetCenter()[1]) < 5 * GameItem::HALF_ITEM_HEIGHT) {
				return;
			}
		}
	}

	// We will drop an item based on probablility, we add greater probability based off the current multiplier
    double itemDropProb     = GameModelConstants::GetInstance()->PROB_OF_ITEM_DROP + (0.02 * this->GetCurrentMultiplier());
    double halfItemDropProb = itemDropProb / 2.0;
	double randomNum        = Randomizer::GetInstance()->RandomNumZeroToOne();
	
	debug_output("Probability of drop: " << itemDropProb << " Number for deciding: " << randomNum);

	if (randomNum < halfItemDropProb || randomNum > (1.0 - halfItemDropProb)) {
		// If there are no allowable item drops for the current level then we drop nothing anyway
		if (this->GetCurrentLevel()->GetAllowableItemDropTypes().empty()) {
			return;
		}
		GameItem::ItemType itemType = GameItemFactory::GetInstance()->CreateRandomItemTypeForCurrentLevel(this, true);
        this->AddItemDrop(p.GetCenter(), itemType);
	}
}

void GameModel::AddItemDrop(const Point2D& p, const GameItem::ItemType& itemType) {
	// We always drop items in this manor, even if we've exceeded the max!
	GameItem* newGameItem = GameItemFactory::GetInstance()->CreateItem(itemType, p, this);
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
		p->AddStatus(status);
		std::pair<std::map<LevelPiece*, int32_t>::iterator, bool> insertResult = 
			this->statusUpdatePieces.insert(std::make_pair(p, static_cast<int32_t>(status)));
		assert(insertResult.second);
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

bool GameModel::RemoveStatusForLevelPiece(LevelPiece* p, const LevelPiece::PieceStatus& status) {
	assert(p != NULL);

	// First try to find the level piece among existing level pieces with status updates
	std::map<LevelPiece*, int32_t>::iterator findIter = this->statusUpdatePieces.find(p);
	if (findIter == this->statusUpdatePieces.end()) {
		return false;
	}
	// Make sure the status is currently being applied to the piece
	if ((findIter->second & status) != status) {
		return false;
	}

	// Remove the status, if all statuses are removed by doing this then remove the piece
	// from the status update map
	findIter->second = (findIter->second & ~status);
	findIter->first->RemoveStatus(status);
	if (findIter->second == static_cast<int32_t>(LevelPiece::NormalStatus)) {
		this->statusUpdatePieces.erase(findIter);
	}

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

float GameModel::GetPercentBallReleaseTimerElapsed() const {
    if (this->GetCurrentStateType() != GameState::BallOnPaddleStateType) {
        return 1.0f;
    }
    return (static_cast<BallOnPaddleState*>(this->currState))->GetReleaseTimerPercentDone();
}


#ifdef _DEBUG
void GameModel::DropItem(GameItem::ItemType itemType) {
	BallInPlayState* state = dynamic_cast<BallInPlayState*>(this->currState);
	if (state != NULL) {
		Vector2D levelDim = this->GetLevelUnitDimensions();
		state->DebugDropItem(GameItemFactory::GetInstance()->CreateItem(itemType, Point2D(0,0) + 0.5f*levelDim, this));
	}
}
#endif