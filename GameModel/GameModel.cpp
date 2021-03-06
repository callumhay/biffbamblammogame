/**
 * GameModel.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "PaddleRemoteControlRocketProjectile.h"
#include "GameTransformMgr.h"
#include "CannonBlock.h"

#include "../BlammoEngine/StringHelper.h"
#include "../GameSound/GameSound.h"
#include "../ResourceManager.h"

GameModel::GameModel(GameSound* sound, const GameModel::Difficulty& initDifficulty, bool ballBoostIsInverted,
                     const BallBoostModel::BallBoostMode& ballBoostMode) : 
currWorldNum(0), currState(NULL), currPlayerScore(0), numStarsAwarded(0), currLivesLeft(0),
livesAtStartOfLevel(0), numLivesLostInLevel(0), maxNumLivesAllowed(0),
pauseBitField(GameModel::NoPause), isBlackoutActive(false), 
areControlsFlipped(false), gameTransformInfo(new GameTransformMgr()), 
nextState(NULL), boostModel(NULL), doingPieceStatusListIteration(false), progressLoadedSuccessfully(false),
droppedLifeForMaxMultiplier(false), bottomSafetyNet(NULL), topSafetyNet(NULL),
ballBoostIsInverted(ballBoostIsInverted), difficulty(initDifficulty),
ballBoostMode(ballBoostMode), sound(sound), numInterimBlocksDestroyed(0), maxInterimBlocksDestroyed(0),
numGoodItemsAcquired(0), numNeutralItemsAcquired(0), numBadItemsAcquired(0), totalLevelTimeInSeconds(0.0) {
	
    assert(sound != NULL);

	// Initialize the worlds for the game - the set of worlds can be found in the world definition file
    this->LoadWorldsFromFile();

    // Load data for all the worlds
    this->progressLoadedSuccessfully = GameProgressIO::LoadGameProgress(this);

	// Initialize paddle and the first ball
	this->playerPaddle = new PlayerPaddle();
	this->balls.push_back(new GameBall());

    this->SetDifficulty(initDifficulty);
    this->SetBallBoostMode(ballBoostMode);
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
    this->worlds.clear();

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

void GameModel::GetFurthestProgressWorldAndLevel(int& worldIdx, int& levelIdx) const {

    for (int i = 0; i < static_cast<int>(this->worlds.size()); i++) {
        const GameWorld* world = this->worlds[i];
        assert(world != NULL);

        if (!world->GetHasBeenUnlocked()) {
            break;
        }

        int lastLevelPassedIdx = world->GetLastLevelIndexPassed();

        // If the last level passed in the world was the last level in that world then
        // the progress goes beyond the current world...
        // TODO: Game Completion Condition?
        if (lastLevelPassedIdx == world->GetLastLevelIndex()) {
            
            // Special condition: the game has been completed, choose the very last world's level
            if (i == static_cast<int>(this->worlds.size())-1) {
                worldIdx = i;
                levelIdx = lastLevelPassedIdx;
                break;
            }

            continue;
        }
        else {
            worldIdx = i;
            if (lastLevelPassedIdx == GameWorld::NO_LEVEL_PASSED) {
                // If no level has been passed in this world then the current progress
                // is the first level of the current world
                levelIdx = 0;
            }
            else {
                // The next level is the one after the last level that has been passed in
                // the current world
                levelIdx = lastLevelPassedIdx + 1;
            }
            assert(levelIdx >= 0 && levelIdx <= world->GetLastLevelIndex());
            break;
        }
    }

}

void GameModel::OverrideGetFurthestProgress(int& worldIdx, int& levelIdx) {
    worldIdx = -1;
    levelIdx = -1;

    for (int i = 0; i < static_cast<int>(this->worlds.size()); i++) {
        GameWorld* world = this->worlds[i];
        assert(world != NULL);

        if (!world->GetHasBeenUnlocked()) {
            
            if (worldIdx == -1) {
                world->SetHasBeenUnlocked(true);
                worldIdx = i;
                levelIdx = world->GetLastLevelIndexPassed();
                if (levelIdx == -1) {
                    levelIdx = 0;
                }
            }

            break;
        }

        int lastLevelPassedIdx = world->GetLastLevelIndexPassed();

        // If the last level passed in the world was the last level in that world then
        // the progress goes beyond the current world...
        // TODO: Game Completion Condition?
        if (lastLevelPassedIdx == world->GetLastLevelIndex()) {

            // Special condition: the game has been completed, choose the very last world's level
            if (i == static_cast<int>(this->worlds.size())-1) {
                worldIdx = i;
                levelIdx = lastLevelPassedIdx;
                break;
            }

            continue;
        }
        else {
            worldIdx = i;
            if (lastLevelPassedIdx == GameWorld::NO_LEVEL_PASSED) {
                // If no level has been passed in this world then the current progress
                // is the first level of the current world
                levelIdx = 0;
            }
            else {
                // The next level is the one after the last level that has been passed in
                // the current world
                levelIdx = lastLevelPassedIdx + 1;
            }
            assert(levelIdx >= 0 && levelIdx <= world->GetLastLevelIndex());
            break;
        }
    }
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
    this->DestroyBottomSafetyNet();
    this->DestroyTopSafetyNet();
    
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

void GameModel::LoadWorldsFromFile() {
    assert(this->worlds.empty());

    std::istringstream* inFile = ResourceManager::GetInstance()->FilepathToInStream(GameModelConstants::GetInstance()->GetWorldDefinitonFilePath());
    std::string currWorldPath;
    bool success = true;
    while (std::getline(*inFile, currWorldPath)) {
        currWorldPath = stringhelper::trim(currWorldPath);
        if (!currWorldPath.empty()) {
            GameWorld* newGameWorld = new GameWorld(GameModelConstants::GetInstance()->GetResourceWorldDir() + 
                                                    std::string("/") + currWorldPath, *this->gameTransformInfo);

            success = newGameWorld->Load(this);
            assert(success);

            this->worlds.push_back(newGameWorld);
            currWorldPath.clear();
        }
    }

    delete inFile;
    inFile = NULL;

    // Make sure that the first world is always unlocked
    this->worlds.front()->SetHasBeenUnlocked(true);
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
            this->totalLevelTimeInSeconds += seconds;
		}
		if (this->playerPaddle != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0) {
			this->playerPaddle->Tick(seconds, (this->pauseBitField & GameModel::PauseState) == GameModel::PauseState, *this);
		}
	}
	this->gameTransformInfo->Tick(seconds, *this);
}

/**
 * Called in order to completely reset the game state and load the
 * given zero-based index world and level number.
 */
void GameModel::StartGameAtWorldAndLevel(int worldIdx, int levelIdx) {
	this->SetNextState(GameState::LevelStartStateType);
    
    // IMPORTANT: ORDER MATTERS HERE!! We need to reset the level values to clear out buffers of the current
    // level's blocks before we set the current world/level which will flush the memory
    this->ResetLevelValues(GameModelConstants::GetInstance()->INIT_LIVES_LEFT);
    this->SetCurrentWorldAndLevel(worldIdx, levelIdx, true);
    
    this->UpdateState();
}

void GameModel::ResetCurrentLevel() {
    // EVENT: Level is resetting
    GameEventManager::Instance()->ActionLevelResetting();

    this->SetNextState(GameState::LevelStartStateType);
    
    this->ClearBallsToOne();
    this->GetGameBalls().front()->ResetBallAttributes();
    
    this->ResetLevelValues(GameModelConstants::GetInstance()->INIT_LIVES_LEFT);
    this->SetCurrentWorldAndLevel(this->currWorldNum, static_cast<int>(this->GetCurrentLevel()->GetLevelIndex()), false);
}

// Called in order to make sure the game is no longer processing or generating anything
void GameModel::ClearGameState() {
	// Delete all world items, timers and other stuff
	this->ClearStatusUpdatePieces();
	this->ClearLiveItems();
	this->ClearActiveTimers();
	this->ClearProjectiles();
	this->ClearBeams();
    this->DestroyBottomSafetyNet();
    this->DestroyTopSafetyNet();

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

void GameModel::ClearAllGameProgress() {
    // Delete all of the loaded worlds...
	for (int i = 0; i < static_cast<int>(this->worlds.size()); i++) {
		delete this->worlds[i];
		this->worlds[i] = NULL;
	}
    this->worlds.clear();

    // Reload all of the worlds...
    this->LoadWorldsFromFile();
}

void GameModel::ClearAllGameProgressNoReload(bool arcadeMode) {
    for (int i = 0; i < static_cast<int>(this->worlds.size()); i++) {
        GameWorld* world = this->worlds[i];
        world->ClearProgress(arcadeMode);
    }
    // Always keep the first world unlocked
    this->worlds.front()->SetHasBeenUnlocked(true);
}

int GameModel::GetTotalStarsCollectedInGame() const {
    int total = 0;
    for (int i = 0; i < static_cast<int>(this->worlds.size()); i++) {
        const GameWorld* world = this->worlds[i];
        total += world->GetNumStarsCollectedInWorld();
    }
    return total;
}

void GameModel::SetCurrentWorldAndLevel(int worldIdx, int levelIdx, bool sendNewWorldEvent) {
	assert(worldIdx >= 0 && worldIdx < static_cast<int>(this->worlds.size()));
	
    // NOTE: We don't unload anything since it all gets loaded when the game model is initialized
    // worlds don't take up too much memory so this is fine

	// Get the world we want to set as current
	GameWorld* world = this->worlds[worldIdx];
	assert(world != NULL);
	
	// Make sure the world loaded properly.
	if (!world->Load(this)) {
		debug_output("ERROR: Could not load world " << worldIdx);
		assert(false);
		return;
	}

	// Setup the world for this object and make sure the world has its zeroth (i.e., first) level set for play
	this->currWorldNum = worldIdx;
    assert(levelIdx >= 0 && levelIdx < static_cast<int>(world->GetNumLevels()));
    
    // IMPORTANT: The sound needs to be updated to the next level's translation so that it properly
    // transforms all sounds that get made between now and the next time the sound module is updated
    this->sound->SetLevelTranslation(world->GetLevelByIndex(levelIdx)->GetTranslationToMiddle());

	world->SetCurrentLevel(this, levelIdx);
	GameLevel* currLevel = world->GetCurrentLevel();
	assert(currLevel != NULL);

    if (sendNewWorldEvent) {
	    // EVENT: World started...    
	    GameEventManager::Instance()->ActionWorldStarted(*world);
    }

	// EVENT: New Level Started
	GameEventManager::Instance()->ActionLevelStarted(*world, *currLevel);

	// Tell the paddle what the boundaries of the level are and reset the paddle
	this->playerPaddle->UpdateLevel(*currLevel); // resets the paddle
	//this->playerPaddle->ResetPaddle();

    // Reload all progress numbers
    this->progressLoadedSuccessfully = GameProgressIO::LoadGameProgress(this);

    this->PerformLevelCompletionChecks();
}

// Get the world with the given name in this model, NULL if no such world exists
GameWorld* GameModel::GetWorldByName(const std::string& name) const {
    for (std::vector<GameWorld*>::const_iterator iter = this->worlds.begin(); iter != this->worlds.end(); ++iter) {
        GameWorld* world = *iter;
        if (world->GetName().compare(name) == 0) {
            return world;
        }
    }
    return NULL;
}

void GameModel::SetDifficulty(const GameModel::Difficulty& difficulty) {
    static const float EASY_DIFFICULTY_BALL_SPEED_DELTA = -1.70f;
    static const float MED_DIFFICULTY_BALL_SPEED_DELTA  =  0.0f;
    static const float HARD_DIFFICULTY_BALL_SPEED_DELTA =  1.5f;

    switch (difficulty) {

        case GameModel::EasyDifficulty:
            // Disable the paddle release timer
            PlayerPaddle::SetEnablePaddleReleaseTimer(false);
            // Make the boost time longer
            BallBoostModel::SetMaxBulletTimeDuration(4.0);
            // Make ball speed slower
            GameBall::SetNormalSpeed(GameBall::DEFAULT_NORMAL_SPEED + EASY_DIFFICULTY_BALL_SPEED_DELTA);
            // Make the paddle slightly larger (1 ball default diameter larger)
            // WARNING: Don't make the paddle too big or it won't be able to fit under blocks that are 1 block up from the bottom,
            // this is required in order to solve certain puzzles in the game!!!
            PlayerPaddle::SetNormalScale((PlayerPaddle::PADDLE_WIDTH_TOTAL + 1.5f * GameBall::DEFAULT_BALL_RADIUS) / PlayerPaddle::PADDLE_WIDTH_TOTAL);

            // N.B., We also make the chance of nice item drops higher (done automatically in GameItemFactory)

            break;
        
        case GameModel::MediumDifficulty:
            // Enable the paddle release timer
            PlayerPaddle::SetEnablePaddleReleaseTimer(true);
            // Make the boost time typical
            BallBoostModel::SetMaxBulletTimeDuration(2.0);
            // Make ball speed normal
            GameBall::SetNormalSpeed(GameBall::DEFAULT_NORMAL_SPEED + MED_DIFFICULTY_BALL_SPEED_DELTA);
            // Make the paddle standard scale
            PlayerPaddle::SetNormalScale(1.0f);

            // N.B., We also make the item drops normal random (done automatically in GameItemFactory)

            break;
        
        case GameModel::HardDifficulty:
            // Enable the paddle release timer
            PlayerPaddle::SetEnablePaddleReleaseTimer(true);
            // Make the boost time short
            BallBoostModel::SetMaxBulletTimeDuration(1.25);
            // Make ball speed faster
            GameBall::SetNormalSpeed(GameBall::DEFAULT_NORMAL_SPEED + HARD_DIFFICULTY_BALL_SPEED_DELTA);
            // Make the paddle standard scale
            PlayerPaddle::SetNormalScale(1.0f);

            // N.B., We also make the chance of not-so-nice item drops higher (done automatically in GameItemFactory)

            break;

        default:
            assert(false);
            return;

    }

    
    this->difficulty = difficulty;

    // EVENT: Difficulty change
    GameEventManager::Instance()->ActionDifficultyChanged(difficulty);
}

bool GameModel::ActivateSafetyNet() {
    // Figure out which safety net is becoming active...
    if (this->playerPaddle->GetIsPaddleFlipped() && SafetyNet::CanTopSafetyNetBePlaced(*this->GetCurrentLevel())) {
        if (this->topSafetyNet != NULL) {
            return false;
        }
        this->topSafetyNet = new SafetyNet(*this->GetCurrentLevel(), false);
        
        // EVENT: We just created a brand new ball safety net...
        GameEventManager::Instance()->ActionBallSafetyNetCreated(false);
    }
    else {
        if (this->bottomSafetyNet != NULL) {
            return false;
        }
        this->bottomSafetyNet = new SafetyNet(*this->GetCurrentLevel(), true);

        // EVENT: We just created a brand new ball safety net...
        GameEventManager::Instance()->ActionBallSafetyNetCreated(true);
    }


    return true;
}

void GameModel::DestroySafetyNet(SafetyNet* netToDestroy) {
    if (this->bottomSafetyNet == netToDestroy) {
        this->DestroyBottomSafetyNet();
    }
    else if (this->topSafetyNet == netToDestroy) {
        this->DestroyTopSafetyNet();
    }
}

void GameModel::DestroyBottomSafetyNet() {
    if (this->bottomSafetyNet != NULL) {
        delete this->bottomSafetyNet;
        this->bottomSafetyNet = NULL;
    }
}

void GameModel::DestroyTopSafetyNet() {
    if (this->topSafetyNet != NULL) {
        delete this->topSafetyNet;
        this->topSafetyNet = NULL;
    }
}

void GameModel::CleanUpAfterLevelEnd() {
    this->ClearStatusUpdatePieces();
    this->ClearProjectiles();
    this->ClearBeams();
    this->ClearLiveItems();
    this->ClearActiveTimers();
    this->GetPlayerPaddle()->RemovePaddleType(PlayerPaddle::RocketPaddle);
    this->GetPlayerPaddle()->RemovePaddleType(PlayerPaddle::RemoteControlRocketPaddle);
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
    if (!alreadyCollided) {
        projectile->LevelPieceCollisionOccurred(p);
    }

	// Collide the projectile with the piece...
	LevelPiece* pieceAfterCollision = p->CollisionOccurred(this, projectile); 	// WARNING: This can destroy p.

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

void GameModel::CollisionOccurred(Projectile* projectile, GameBall* ball) {
    assert(projectile != NULL);
    assert(ball != NULL);

    // Check to see if there was already a collision...
    bool alreadyCollided = ball->IsLastThingCollidedWith(projectile);
    if (!alreadyCollided) {
        // Inform the projectile of the collision -- this might have an effect on the ball
        projectile->BallCollisionOccurred(ball);
    }
    
    // EVENT: First-time Projectile-Ball Collision
    if (!alreadyCollided) {
        GameEventManager::Instance()->ActionProjectileBallCollision(*projectile, *ball);
    }

    // These kinds of collisions should NEVER be able to end the level.
    // NO NEED FOR THIS: this->PerformLevelCompletionChecks();
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

/**
 * Called when a collision occurs between a ball and a part of a boss in the current level.
 */
void GameModel::CollisionOccurred(GameBall& ball, Boss* boss, BossBodyPart* bossPart) {
    assert(boss != NULL);
    assert(bossPart != NULL);

    // EVENT: Ball-Boss Collision
    GameEventManager::Instance()->ActionBallBossCollision(ball, *boss, *bossPart);

    // Collide the ball with the boss
    boss->CollisionOccurred(this, ball, bossPart);
    ball.BallCollided();

	// If the ball is attached to the paddle then we always set the last piece it collided with to NULL
	// This ensures that the ball ALWAYS collides with everything in its way while on the paddle
	if (this->playerPaddle->GetAttachedBall() == &ball) {
		ball.SetLastPieceCollidedWith(NULL);
	}

	// Check to see if the level is done
	this->PerformLevelCompletionChecks();
}

/**
 * Called when a collision occurs between a projectile and a part of a boss in the current level.
 */
void GameModel::CollisionOccurred(Projectile* projectile, Boss* boss, BossBodyPart* bossPart) {
    assert(projectile != NULL);
    assert(boss != NULL);
    assert(bossPart != NULL);

    bool alreadyCollided = projectile->IsLastThingCollidedWith(bossPart) || projectile->IsAttachedToSomething();
    if (!alreadyCollided) {

        boss->CollisionOccurred(this, projectile, bossPart);
        projectile->BossCollisionOccurred(boss, bossPart);

        // EVENT: Projectile-boss collision
        GameEventManager::Instance()->ActionProjectileBossCollision(*projectile, *boss, *bossPart);
    }
}

void GameModel::ToggleAllowPaddleBallLaunching(bool allow) {
    if (this->currState->GetType() == GameState::BallOnPaddleStateType) {
        BallOnPaddleState* onPaddleState = static_cast<BallOnPaddleState*>(this->currState);
        onPaddleState->ToggleAllowPaddleBallLaunching(allow);
    }
}

void GameModel::BallBoostDirectionPressed(float x, float y, bool allowLargeChangeInDirection) {
	if (this->currState != NULL) {
		this->currState->BallBoostDirectionPressed(x, y, allowLargeChangeInDirection);
	}
}
void GameModel::BallBoostDirectionReleased() {
	if (this->currState != NULL) {
		this->currState->BallBoostDirectionReleased();
	}
}

void GameModel::BallBoostReleasedForBall(const GameBall& ball) {
	if (this->currState != NULL) {
		this->currState->BallBoostReleasedForBall(ball);
        this->gameTransformInfo->SetBulletTimeCamera(false);
	}
}

void GameModel::ReleaseBulletTime() {
    if (this->boostModel != NULL) {
        this->boostModel->ReleaseBulletTime();
    }
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

Projectile* GameModel::GetFirstBeamProjectileCollider(const Collision::Ray2D& ray, 
                                                      const std::set<const void*>& ignoreThings, 
                                                      float& rayT) const {
    
    Projectile* closestCollider = NULL;
    rayT = std::numeric_limits<float>::max();

    for (ProjectileMapConstIter mapIter = this->projectiles.begin(); mapIter != this->projectiles.end(); ++mapIter) {
        const ProjectileList& currProjectileList = mapIter->second;

        for (ProjectileListConstIter iter = currProjectileList.begin(); iter != currProjectileList.end(); ++iter) {

            Projectile* currProjectile = *iter;
            float tempRayT = std::numeric_limits<float>::max();

            if (currProjectile->CanCollideWithBeams() && ignoreThings.find(currProjectile) == ignoreThings.end() && 
                Collision::IsCollision(ray, currProjectile->BuildAABB(), tempRayT)) {

                if (tempRayT < rayT) {

                    rayT = tempRayT;
                    closestCollider = currProjectile;
                }
            }
        }
    }

    return closestCollider;
}

void GameModel::BallPaddleCollisionOccurred(GameBall& ball) {
	ball.BallCollided();

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
            this->SetNextState(new BallOnPaddleState(this, 0.5));
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
    GameLevel* currLevel = this->GetCurrentLevel();
    
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
			currLevelPiece->RemoveStatuses(currLevel, statusesToRemove);

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
 * Execute projectile collisions on everything relevant that's in play in the GameModel.
 */
void GameModel::DoProjectileCollisions(double dT) {

#define PROJECTILE_CLEANUP(p) p->Teardown(*this); \
                              GameEventManager::Instance()->ActionProjectileRemoved(*p); \
                              delete p; \
                              p = NULL
													 
	// Grab a list of all paddle-related projectiles and test each one for collisions...
	bool destroyProjectile = false;
	bool didCollide = false;
	bool incIter = true;

    std::set<LevelPiece*> collisionPieces;
    std::set<LevelPiece*> alreadyCollidedWithPieces;

    GameLevel* currLevel = this->GetCurrentLevel();
    Boss* boss = currLevel->GetBoss();

	for (ProjectileMapIter mapIter = this->projectiles.begin(); mapIter != this->projectiles.end(); ++mapIter) {
        ProjectileList& currProjectileList = mapIter->second;

        for (ProjectileListIter iter = currProjectileList.begin(); iter != currProjectileList.end();) {

		    Projectile* currProjectile = *iter;
            Vector2D currProjectileVel = currProjectile->GetVelocity();
    		
		    // Grab bounding lines from the projectile to test for collision -- we cache and reuse this
            // throughout the loop so that we aren't constantly regenerating projectile bounds for each use
		    BoundingLines projectileBoundingLines = currProjectile->BuildBoundingLines();

		    // Check to see if the projectile collided with the player paddle
		    if (!this->playerPaddle->HasBeenPausedAndRemovedFromGame(this->pauseBitField) &&
                this->playerPaddle->CollisionCheckWithProjectile(*currProjectile, projectileBoundingLines)) {
    			
			    this->CollisionOccurred(currProjectile, this->playerPaddle);

			    // Destroy the projectile if necessary
			    destroyProjectile = this->playerPaddle->ProjectileIsDestroyedOnCollision(*currProjectile);
			    if (destroyProjectile) {
				    iter = currProjectileList.erase(iter);
				    PROJECTILE_CLEANUP(currProjectile);
				    continue;
			    }
    			
			    this->playerPaddle->ModifyProjectileTrajectory(*currProjectile);
			    ++iter;
			    continue;
		    }

            // Check for collisions with balls (NOTE: most projectiles don't collide with balls)
            if (currProjectile->CanCollideWithBall()) {

                for (std::list<GameBall*>::iterator ballIter = this->balls.begin(); ballIter != this->balls.end(); ++ballIter) {
                    GameBall* currBall = *ballIter;
                    
                    if (currBall->CollisionCheckWithProjectile(dT, *currProjectile, projectileBoundingLines)) {
                        
                        this->CollisionOccurred(currProjectile, currBall);
                        
                        destroyProjectile = currBall->ProjectileIsDestroyedOnCollision(*currProjectile);
                        if (destroyProjectile) {
                            iter = currProjectileList.erase(iter);
                            PROJECTILE_CLEANUP(currProjectile);
                            break;
                        }

                        currBall->ModifyProjectileTrajectory(*currProjectile);
                    }
                }
                if (destroyProjectile) {
                    ++iter;
                    continue;
                }
            }

            if (currProjectile->CanCollideWithProjectiles()) {
                for (ProjectileMapIter mapIter2 = this->projectiles.begin(); mapIter2 != this->projectiles.end(); ++mapIter2) {
                    ProjectileList& currProjectileList2 = mapIter2->second;

                    for (ProjectileListIter iter2 = currProjectileList2.begin(); iter2 != currProjectileList2.end(); ++iter2) {

                        Projectile* otherProjectile = *iter2;
                        if (otherProjectile == currProjectile || otherProjectile->IsLastThingCollidedWith(currProjectile)) {
                            // No self-collisions or repeated collisions
                            continue;
                        }
                        if (projectileBoundingLines.CollisionCheck(otherProjectile->BuildBoundingLines(), dT, otherProjectile->GetVelocity())) {
                            currProjectile->ProjectileCollisionOccurred(otherProjectile);
                        }
                    }
                }
            }

		    // Check to see if the projectile collided with a safety net (if one is active)
            if (this->IsBottomSafetyNetActive() && 
                this->bottomSafetyNet->ProjectileCollisionCheck(projectileBoundingLines, dT, currProjectileVel)) {
                    
                currProjectile->SafetyNetCollisionOccurred(this->bottomSafetyNet);
                if (currProjectile->IsMine()) {
                    MineProjectile* mine = static_cast<MineProjectile*>(currProjectile);
                    if (mine->GetIsAttachedToSafetyNet()) {
                        ++iter;
                        continue;
                    }
                }
                
                // EVENT: The projectile was just destroyed by the safety net
                GameEventManager::Instance()->ActionProjectileSafetyNetCollision(*currProjectile, *this->bottomSafetyNet);

                if (currProjectile->BlastsThroughSafetyNets()) {
                    this->DestroyBottomSafetyNet();
                    // EVENT: The projectile just destroyed the safety net
                    GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*currProjectile, true);
                }

                // Check to see if the projectile will be destroyed due to the collision...
                if (currProjectile->IsDestroyedBySafetyNets()) {
                    iter = currProjectileList.erase(iter);
			        PROJECTILE_CLEANUP(currProjectile);
			        continue;
                }

		        ++iter;
		        continue;

            }
            else if (this->IsTopSafetyNetActive() && 
                this->topSafetyNet->ProjectileCollisionCheck(projectileBoundingLines, dT, currProjectileVel)) {

                currProjectile->SafetyNetCollisionOccurred(this->topSafetyNet);
                if (currProjectile->IsMine()) {
                    MineProjectile* mine = static_cast<MineProjectile*>(currProjectile);
                    if (mine->GetIsAttachedToSafetyNet()) {
                        ++iter;
                        continue;
                    }
                }

                // EVENT: The projectile was just destroyed by the safety net
                GameEventManager::Instance()->ActionProjectileSafetyNetCollision(*currProjectile, *this->topSafetyNet);

                if (currProjectile->BlastsThroughSafetyNets()) {
                    this->DestroyTopSafetyNet();
                    // EVENT: The projectile just destroyed the safety net
                    GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*currProjectile, false);
                }

                // Check to see if the projectile will be destroyed due to the collision...
                if (currProjectile->IsDestroyedBySafetyNets()) {
                    iter = currProjectileList.erase(iter);
                    PROJECTILE_CLEANUP(currProjectile);
                    continue;
                }

                ++iter;
                continue;
            }

            // Check if the projectile collided with any Tesla lightning arcs...
            if (currLevel->IsDestroyedByTelsaLightning(*currProjectile)) {

                if (currLevel->TeslaLightningCollisionCheck(projectileBoundingLines)) {
                    
                    // In the special case of a rocket projectile we cause an explosion...
                    if (currProjectile->IsRocket()) {
                        collisionPieces.clear();
                        currLevel->GetLevelPieceCollisionCandidatesNoSort(currProjectile->GetPosition(), EPSILON, collisionPieces);
                        if (!collisionPieces.empty()) {
                            assert(dynamic_cast<RocketProjectile*>(currProjectile) != NULL);
                            currLevel->RocketExplosion(this, static_cast<RocketProjectile*>(currProjectile), *collisionPieces.begin());
                        }
                    }
                    // In the other special case of a mine projectile we cause an explosion...
                    else if (currProjectile->GetType() == Projectile::PaddleMineBulletProjectile ||
                             currProjectile->GetType() == Projectile::MineTurretBulletProjectile) {
                        assert(dynamic_cast<MineProjectile*>(currProjectile) != NULL);
                        currLevel->MineExplosion(this, static_cast<MineProjectile*>(currProjectile));
                    }

			        // Dispose of the projectile...
                    iter = currProjectileList.erase(iter);
                    PROJECTILE_CLEANUP(currProjectile);
                    continue;    
                }
            }

            // Check to see if the projectile collided with a boss
            if (boss != NULL && !boss->ProjectilePassesThrough(currProjectile)) {
                BossBodyPart* hitBodyPart = boss->CollisionCheck(projectileBoundingLines, dT, currProjectileVel);
                if (hitBodyPart != NULL) {

                    this->CollisionOccurred(currProjectile, boss, hitBodyPart);
                    
                    // In the special case of a rocket projectile we cause an explosion...
                    if (currProjectile->IsRocket()) {
                        collisionPieces.clear();
                        currLevel->GetLevelPieceCollisionCandidatesNoSort(currProjectile->GetPosition(), EPSILON, collisionPieces);
                        if (!collisionPieces.empty()) {
                            assert(dynamic_cast<RocketProjectile*>(currProjectile) != NULL);
                            currLevel->RocketExplosion(this, static_cast<RocketProjectile*>(currProjectile), *collisionPieces.begin());
                        }
                    }

                    if (boss->ProjectileIsDestroyedOnCollision(currProjectile, hitBodyPart)) {
                        // Dispose of the projectile...
                        iter = currProjectileList.erase(iter);
                        PROJECTILE_CLEANUP(currProjectile);
                        continue;  
                    }
                }
            }

            incIter = true;	// Keep track of whether we need to increment the iterator or not

            if (currProjectile->CanCollideWithBlocks()) {

                // Find the any level pieces that the current projectile may have collided with and test for collision
                collisionPieces.clear();
                currLevel->GetLevelPieceCollisionCandidates(dT, currProjectile->GetPosition(), projectileBoundingLines, 
                    currProjectile->GetVelocityMagnitude(), collisionPieces);
                alreadyCollidedWithPieces.clear();

		        for (std::set<LevelPiece*>::iterator pieceIter = collisionPieces.begin(); pieceIter != collisionPieces.end(); ++pieceIter) {

			        LevelPiece *currPiece = *pieceIter;
        			
			        // Test for a collision between the projectile and current level piece
			        didCollide = currPiece->CollisionCheck(projectileBoundingLines, dT, currProjectileVel) && 
                        alreadyCollidedWithPieces.find(currPiece) == alreadyCollidedWithPieces.end();
			        if (didCollide) {

				        // WARNING/IMPORTANT!!!! This needs to be before the call to CollisionOccurred or else the
                        // currPiece may already be destroyed.
				        destroyProjectile = currPiece->ProjectileIsDestroyedOnCollision(currProjectile);

                        // Check to see if the piece is a cannon and whether it now has the projectile loaded in it, in which case we
                        // exit from this loop immediately
                        if (!destroyProjectile && currPiece->GetType() == LevelPiece::Cannon && 
                            currPiece->GetType() != LevelPiece::FragileCannon) {
                            
                            this->CollisionOccurred(currProjectile, currPiece);
                            // currPiece will still exist since cannons cannot be destroyed

                            if (static_cast<const CannonBlock*>(currPiece)->IsProjectileLoaded(currProjectile)) {
                                alreadyCollidedWithPieces.insert(currPiece);
                                break;
                            }
                        }
                        else {
                            this->CollisionOccurred(currProjectile, currPiece);	
                            // currPiece may now be destroyed...
                        }

				        // Check to see if the collision is supposed to destroy the projectile
				        if (destroyProjectile) {
					        // Dispose of the projectile...
					        iter = currProjectileList.erase(iter);
					        PROJECTILE_CLEANUP(currProjectile);
					        incIter = false;
                            break;
				        }
                        else {


                            // NOTE: We need to be careful since some blocks may no longer exist after a collision at this
                            // point so we re-populate the set of pieces to check and keep track of the ones we've already collided with
                            collisionPieces.clear();
                            currLevel->GetLevelPieceCollisionCandidates(
                                dT, currProjectile->GetPosition(), projectileBoundingLines, currProjectile->GetVelocityMagnitude(), collisionPieces);
                            pieceIter = collisionPieces.begin();
                        }
			        }
                    alreadyCollidedWithPieces.insert(currPiece);
		        }
            }

            // Lastly we perform a modify level update for the projectile - certain projectiles can act
            // 'intelligently' based on the level state and cause alteration to it as well
            if (currProjectile != NULL && currProjectile->ModifyLevelUpdate(dT, *this)) {

                iter = currProjectileList.erase(iter);
		        PROJECTILE_CLEANUP(currProjectile);
		        continue;
            }

		    if (incIter) {
			    ++iter;
		    }
        }
	}

#undef PROJECTILE_CLEANUP

	// Check to see if the level is done
    this->PerformLevelCompletionChecks();
}

bool GameModel::IsOutOfPaddedLevelBounds(const Point2D& pos, float minXPadding, float minYPadding, float maxXPadding, float maxYPadding) const {
    const GameLevel* currLevel = this->GetCurrentLevel();
    float levelWidthBounds	= currLevel->GetLevelUnitWidth()  + maxXPadding;
    float levelHeightBounds = currLevel->GetLevelUnitHeight() + maxYPadding;

    return pos[1] <= -minYPadding || 
        pos[1] >= levelHeightBounds ||
        pos[0] <= -minXPadding ||
        pos[0] >= levelWidthBounds;
}

/**
 * Private helper function for updating all active timers and deleting/removing
 * all expired timers.
 */
void GameModel::UpdateActiveTimers(double seconds) {

    // We don't do this if timers are paused
    if ((this->GetPauseState() & GameModel::PauseTimers) == GameModel::PauseTimers) {
        return;
    }

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
		if (this->IsOutOfGameBoundsForItem(currItem->GetCenter())) {
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
	for (ProjectileMapIter mapIter = this->projectiles.begin(); mapIter != this->projectiles.end(); ++mapIter) {
		
        ProjectileList& currProjectiles = mapIter->second;
        for (ProjectileListIter iter = currProjectiles.begin(); iter != currProjectiles.end();) {

            Projectile* currProjectile = *iter;
		    currProjectile->Tick(seconds, *this);

		    // If the projectile is out of game bounds, destroy it
		    if (this->IsOutOfGameBoundsForProjectile(currProjectile->GetPosition())) {

			    iter = currProjectiles.erase(iter);
                currProjectile->Teardown(*this);

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

}

/**
 * Private helper function for updating the collisions and effects of the various 
 * active beams (if any) in the game during the current tick.
 */
void GameModel::UpdateActiveBeams(double seconds) {

	std::list<Beam*>& activeBeams = this->GetActiveBeams();
	for (std::list<Beam*>::iterator beamIter = activeBeams.begin(); beamIter != activeBeams.end();) {
		// Get the current beam being iterated on and execute its effect on all the
		// level pieces it affects...
		Beam* currentBeam = *beamIter;
		assert(currentBeam != NULL);

		// Update the beam collisions... this needs to be done before anything or there
		// might be level pieces that were destroyed by the ball that we are trying to access in the beam
		currentBeam->UpdateCollisions(this);

        if (currentBeam->CanDestroyLevelPieces()) {
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
        }

		// Tick the beam, check for its death and clean up if necessary
		if (currentBeam->Tick(seconds, this)) {
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

        // No points, stars or score notifications on boss levels
        const GameLevel* currLevel = this->GetCurrentLevel();
        if (!currLevel->GetHasBoss()) {
		    // EVENT: Score was changed
            GameEventManager::Instance()->ActionScoreChanged(this->currPlayerScore);
            // EVENT: Point notification
            GameEventManager::Instance()->ActionPointNotification(pointAward);
        

            // Check to see if the number of stars changed...
            int numStarsForCurrScore = currLevel->GetNumStarsForScore(this->currPlayerScore);
            if (this->numStarsAwarded != numStarsForCurrScore) {
                int oldNumStars = this->numStarsAwarded;
                this->numStarsAwarded = numStarsForCurrScore;
                // EVENT: The number of stars changed
                GameEventManager::Instance()->ActionNumStarsChanged(&pointAward, oldNumStars, this->numStarsAwarded);
            }
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
        int oldNumInterimBlocksDestroyed = this->numInterimBlocksDestroyed;
		this->numInterimBlocksDestroyed = value;
        
        // NOTE: This is no longer really used (was a stat for showing the player in level summary)
        this->maxInterimBlocksDestroyed = std::max<int>(this->maxInterimBlocksDestroyed, this->numInterimBlocksDestroyed);

        int newMultiplier = this->GetCurrentMultiplier();
		
        // No events for boss levels
        if (!this->GetCurrentLevel()->GetHasBoss()) {

            // EVENT: The value has changed for the number of interim blocks destroyed (the multiplier part counter)
            GameEventManager::Instance()->ActionScoreMultiplierCounterChanged(oldNumInterimBlocksDestroyed, this->numInterimBlocksDestroyed);

		    // EVENT: The score multiplier has changed
            if (oldMultiplier != newMultiplier) {
		        GameEventManager::Instance()->ActionScoreMultiplierChanged(oldMultiplier, newMultiplier, pos);
            }
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
    
	for (ProjectileMapIter mapIter = this->projectiles.begin(); mapIter != this->projectiles.end(); ++mapIter) {
        
        ProjectileList& currProjectiles = mapIter->second;
        for (ProjectileListIter listIter = currProjectiles.begin(); listIter != currProjectiles.end(); ++listIter) {

		    Projectile* currProjectile = *listIter;
		    currProjectile->Teardown(*this);

            // EVENT: Projectile removed from the game
		    GameEventManager::Instance()->ActionProjectileRemoved(*currProjectile);

		    delete currProjectile;
		    currProjectile = NULL;
        }
	}
	this->projectiles.clear();
}

/**
 * Clears the list of all beams that are currently in existance in the game.
 */
void GameModel::ClearBeams() {
	for (std::list<Beam*>::iterator iter = this->beams.begin(); iter != this->beams.end();) {
		Beam* currBeam = *iter;

        // NOTE: We must remove the beam BEFORE we raise the event!
        iter = this->beams.erase(iter);

		// EVENT: Beam is removed from the game
		GameEventManager::Instance()->ActionBeamRemoved(*currBeam);
		delete currBeam;
		currBeam = NULL;
	}
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

    // Remove any safety nets that might be active
    this->DestroyBottomSafetyNet();
    this->DestroyTopSafetyNet();
}
/**
 * Clears all of the active timers in the game.
 */
void GameModel::ClearActiveTimers() {
	for(std::list<GameItemTimer*>::iterator iter = this->activeTimers.begin(); iter != this->activeTimers.end(); ++iter) {
		GameItemTimer* currTimer = *iter;
		currTimer->StopTimer(false);
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

void GameModel::RemoveBallDeathMineProjectiles() {
    for (ProjectileMapIter mapIter = this->projectiles.begin(); mapIter != this->projectiles.end(); ++mapIter) {

        ProjectileList& currProjectiles = mapIter->second;
        for (ProjectileListIter listIter = currProjectiles.begin(); listIter != currProjectiles.end();) {

            Projectile* currProjectile = *listIter;
            if (currProjectile->IsMine()) {
                MineProjectile* currMine = static_cast<MineProjectile*>(currProjectile);
                if (currMine->GetIsAttachedToPaddle() || currMine->GetIsAttachedToSafetyNet()) {
                    currMine->Teardown(*this);

                    // EVENT: Projectile removed from the game
                    GameEventManager::Instance()->ActionProjectileRemoved(*currMine);

                    delete currMine;
                    currMine = NULL;

                    listIter = currProjectiles.erase(listIter);
                    continue;
                }
            }

            ++listIter;
        }
    }
}

Vector2D GameModel::GetGameSpacePaddleRightUnitVec() const {
    Vector2D rightVec(1,0);
    rightVec.Rotate(this->gameTransformInfo->GetGameZRotationInDegs());
    return rightVec;
}
Vector2D GameModel::GetGameSpacePaddleUpUnitVec() const {
    Vector2D rightVec(0,1);
    rightVec.Rotate(this->gameTransformInfo->GetGameZRotationInDegs());
    return rightVec;
}

/**
 * Function for adding a possible item drop for the given level piece.
 */
void GameModel::AddPossibleItemDrop(const LevelPiece& p) {
    static bool droppedItemLastTime = false;

    // If this is the last piece in the level then we don't drop anything
    if (this->GetCurrentLevel()->GetNumPiecesLeft() == 1) {
        return;
    }

    if (droppedItemLastTime) {
        // Do a test for consecutive item drops -- there's a probability of items not
        // dropping consecutively
        double randomNum = Randomizer::GetInstance()->RandomNumZeroToOne();
        if (randomNum > GameModelConstants::GetInstance()->PROB_OF_CONSECTUIVE_ITEM_DROP) {
            droppedItemLastTime = false;
		    return;
        }
    }

	// Make sure we're in a ball in play state...
	if (this->currState->GetType() != GameState::BallInPlayStateType) {
        droppedItemLastTime = false;
		return;
	}

    // Check to see if the level piece is outside of the boundaries for the paddle to even get it,
    // if it is then we exit -- don't want to drop items that just frustrate the player
    const GameLevel* currLevel = this->GetCurrentLevel();
    assert(currLevel != NULL);
    const LevelPiece* minBoundPiece = currLevel->GetMinXPaddleBoundPiece(this->playerPaddle->GetDefaultYPosition());
    const LevelPiece* maxBoundPiece = currLevel->GetMaxXPaddleBoundPiece(this->playerPaddle->GetDefaultYPosition());
    
    if (p.GetWidthIndex() <= minBoundPiece->GetWidthIndex() ||
        p.GetWidthIndex() >= maxBoundPiece->GetWidthIndex()) {

        droppedItemLastTime = false;
        return;
    }

    // Special case where the multiplier is about to change (or already changed) to the max multiplier
    // and the player is missing a life - in this case we will be dropping a life-up item
    if (!this->droppedLifeForMaxMultiplier &&
        this->numInterimBlocksDestroyed >= GameModelConstants::GetInstance()->FOUR_TIMES_MULTIPLIER_NUM_BLOCKS - 1 &&
        this->currLivesLeft < GameModelConstants::GetInstance()->MAXIMUM_POSSIBLE_LIVES) {

        this->AddItemDrop(p.GetCenter(), GameItem::LifeUpItem);
        this->droppedLifeForMaxMultiplier = true;
        droppedItemLastTime = true;
        return;
    }

	// Make sure we don't drop more items than the max allowable...
	if (this->currLiveItems.size() >= GameModelConstants::GetInstance()->MAX_LIVE_ITEMS) {
        droppedItemLastTime = false;
		return;
	}

	// If there are no allowable item drops for the current level then we drop nothing anyway
	if (this->GetCurrentLevel()->GetAllowableItemDropTypes().empty()) {
        droppedItemLastTime = false;
		return;
	}

	// Make sure we don't drop an item close to another dropping item in the same column of the level...
	for (std::list<GameItem*>::const_iterator iter = this->currLiveItems.begin();
         iter != this->currLiveItems.end(); ++iter) {

		const GameItem* currItem = *iter;
		if (fabs(currItem->GetCenter()[0] - p.GetCenter()[0]) < EPSILON) {
			if (fabs(currItem->GetCenter()[1] - p.GetCenter()[1]) < 5 * GameItem::HALF_ITEM_HEIGHT) {
                droppedItemLastTime = false;
				return;
			}
		}
	}

	// We will drop an item based on probablility, we add greater probability based off the current multiplier
    double itemDropProb = GameModelConstants::GetInstance()->PROB_OF_ITEM_DROP + (0.0125 * this->GetCurrentMultiplier());

    // We also effect the probability based on how many active item timers are currently in play:
    // After 3 timers the probability gets lower...
    itemDropProb -= 0.01 * std::max<int>(0, static_cast<int>(this->GetActiveTimers().size()) - 3);

    // Probability must be in [0,1]
    itemDropProb = std::max<double>(0.0, std::min<double>(1.0, itemDropProb));

	double randomNum = Randomizer::GetInstance()->RandomNumZeroToOne();
	
	debug_output("Probability of drop: " << itemDropProb << " Number for deciding: " << randomNum);
	if (randomNum <= itemDropProb) {
		GameItem::ItemType itemType = GameItemFactory::GetInstance()->CreateRandomItemTypeForCurrentLevel(this, true);
        this->AddItemDrop(p.GetCenter(), itemType);
        droppedItemLastTime = true;
	}
    else {
        droppedItemLastTime = false;
    }
}

void GameModel::AddItemDrop(const Point2D& p, const GameItem::ItemType& itemType) {
    
    // Don't add the item if the ball is not in play
    if (this->currState->GetType() != GameState::BallInPlayStateType) {
        return;
    }

    // Decide which way the item will drop...
    Vector2D dropDir(0,-1);
    if (this->playerPaddle->GetIsPaddleFlipped()) {
        dropDir[1] = 1;
    }

	// We always drop items in this manor, even if we've exceeded the max!
	GameItem* newGameItem = GameItemFactory::GetInstance()->CreateItem(itemType, p, dropDir, this);
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
    this->projectiles[projectile->GetType()].push_back(projectile);
    
    // Call the setup function so the projectile can do some setup if need be
    projectile->Setup(*this);

	// EVENT: Projectile creation
	GameEventManager::Instance()->ActionProjectileSpawned(*projectile);	
}

/**
 * Add a beam of the given type to the model - this will be updated and tested appropriate to
 * its execution.
 */
void GameModel::AddBeam(Beam* beam) {
    assert(beam != NULL);

	// Create the beam based on type...
    bool success = false;
	switch (beam->GetType()) {
		case Beam::PaddleBeam: {
				
				bool foundOtherLaserBeam = false;
				// Reset the time on the previous beam if there is one, otherwise spawn a new beam
				for (std::list<Beam*>::iterator iter = this->beams.begin(); iter != this->beams.end(); ++iter) {
					Beam* currBeam = *iter;
					if (currBeam->GetType() == Beam::PaddleBeam) {
						assert(foundOtherLaserBeam == false);
						foundOtherLaserBeam = true;
						currBeam->ResetTimeElapsed();
					}
				}
                success = !foundOtherLaserBeam;
			}
			break;

        case Beam::BossBeam:
            success = true;
            break;

		default:
            success = false;
			assert(false);
			return;
	}
	
    if (success) {
	    // Add the beam to the list of in-game beams
	    this->beams.push_back(beam);

	    // EVENT: Beam creation
	    GameEventManager::Instance()->ActionBeamSpawned(*beam);
    }
    else {
        delete beam;
        beam = NULL;
    }
}

/**
 * Add a level piece that requires 'ticking' (updates) every frame due to status effects applied to it, when in the "BallInPlay" state.
 * This allows us to selectively apply long term effects to level pieces.
 * Returns: true if the piece is freshly added as a update piece, false if it has already been added previously.
 */
bool GameModel::AddStatusUpdateLevelPiece(LevelPiece* p, const LevelPiece::PieceStatus& status) {
	assert(p != NULL);

    GameLevel* currLevel = this->GetCurrentLevel();

	// First try to find the level piece among existing level pieces with status updates
	std::map<LevelPiece*, int32_t>::iterator findIter = this->statusUpdatePieces.find(p);
	if (findIter == this->statusUpdatePieces.end()) {
		// Nothing was found, add a new entry for the piece with the given status...
		p->AddStatus(currLevel, status);
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

    GameLevel* currLevel = this->GetCurrentLevel();

	// Remove the status, if all statuses are removed by doing this then remove the piece
	// from the status update map
	findIter->second = (findIter->second & ~status);
	findIter->first->RemoveStatus(currLevel, status);
	if (findIter->second == static_cast<int32_t>(LevelPiece::NormalStatus)) {
		this->statusUpdatePieces.erase(findIter);
	}

	return true;
}

long GameModel::GetTotalBestScore(bool arcadeMode) const {
    long totalScore = 0;
    // Go through every level up to the last level passed by the player and accumulate the score
    for (int worldIdx = 0; worldIdx < static_cast<int>(this->worlds.size()); worldIdx++) {
        const GameWorld* world = this->GetWorldByIndex(worldIdx);
        // Accumulate score for the world...
        long worldScore = world->GetTotalHighscoresInWorld(arcadeMode);
        if (worldScore == 0) {
            break;
        }
        totalScore += worldScore;
    }

    return totalScore;
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

void GameModel::ClearSpecificBeams(const Beam::BeamType& beamType) {
    for (std::list<Beam*>::iterator iter = this->beams.begin(); iter != this->beams.end();) {
        Beam* currBeam = *iter;
        if (currBeam->GetType() == beamType) {

            // NOTE: We must remove the beam BEFORE we raise the event!
            iter = this->beams.erase(iter);

            // EVENT: Beam is removed from the game
            GameEventManager::Instance()->ActionBeamRemoved(*currBeam);
            delete currBeam;
            currBeam = NULL;
        }
        else {
             ++iter;
        }
    }
}

void GameModel::ClearSpecificProjectiles(const Projectile::ProjectileType& projectileType) {
    ProjectileMapIter findIter = this->projectiles.find(projectileType);
    if (findIter == this->projectiles.end()) {
        return;
    }

    ProjectileList& projectileList = findIter->second;
    for (ProjectileListIter iter = projectileList.begin(); iter != projectileList.end(); ++iter) {
        
        Projectile* projectile = *iter;
        projectile->Teardown(*this);

        // EVENT: Projectile removed from the game
        GameEventManager::Instance()->ActionProjectileRemoved(*projectile);

        delete projectile;
        projectile = NULL;
    }
    projectileList.clear();
}

bool GameModel::IsTimerTypeActive(const GameItem::ItemType& type) const {
    for (std::list<GameItemTimer*>::const_iterator iter = this->activeTimers.begin();
        iter != this->activeTimers.end(); ++iter) {

        const GameItemTimer* timer = *iter;
        if (timer->GetTimerItemType() == type) {
            return true;
        }
    }
    return false;
}

bool GameModel::GetIsUnusualCameraModeActive() const {
    bool unusualCamModeIsActive = (this->gameTransformInfo->GetIsPaddleCameraOn() || this->gameTransformInfo->GetIsBallCameraOn() ||
        this->gameTransformInfo->GetIsRemoteControlRocketCameraOn());

    if (this->boostModel != NULL) {
        unusualCamModeIsActive |= (this->boostModel->GetBulletTimeState() != BallBoostModel::NotInBulletTime);
    }
    
    return unusualCamModeIsActive;
}

Point2D GameModel::GetAvgBallLoc() const {
    Point2D avgLoc(0,0);
    for (std::list<GameBall*>::const_iterator iter = this->balls.begin(); iter != this->balls.end(); ++iter) {
        GameBall* currBall = *iter;
        const Point2D& ballPos = currBall->GetCenterPosition2D();
        avgLoc[0] += ballPos[0];
        avgLoc[1] += ballPos[1];
    }

    avgLoc /= static_cast<float>(this->balls.size());
    return avgLoc;
}

Vector3D GameModel::GetGravityDir() const {
    Vector3D gravityDir = this->GetTransformInfo()->GetGameXYZTransform() * Vector3D(0, -1, 0);
    gravityDir.Normalize();
    return gravityDir;
}

Vector3D GameModel::GetGravityRightDir() const {
    Vector3D gravityRightDir =  this->GetTransformInfo()->GetGameXYZTransform() * Vector3D(1, 0, 0);
    gravityRightDir.Normalize();
    return gravityRightDir;
}

// Get the maximum distance from the ball to the paddle line, before the ghost paddle will 
// appear in ball camera mode
float GameModel::GetGhostPaddleDistance() const {
    switch (this->difficulty) {

        case GameModel::EasyDifficulty:
            return 15.0f;

        case GameModel::MediumDifficulty:
            return 12.0f;

        case GameModel::HardDifficulty:
            return 10.0f;

        default:
            assert(false);
            break;
    }

    return 0.0f;
}

float GameModel::GetPercentBallReleaseTimerElapsed() const {
    if (this->GetCurrentStateType() != GameState::BallOnPaddleStateType) {
        return 1.0f;
    }
    return (static_cast<BallOnPaddleState*>(this->currState))->GetReleaseTimerPercentDone();
}

void GameModel::AddPercentageToBoostMeter(double percent) {
    assert(percent >= 0.0 && percent <= 1.0);
    if (this->boostModel != NULL) {
        this->boostModel->IncrementBoostChargeByPercent(*this, percent);
    }
}

PaddleRemoteControlRocketProjectile* GameModel::GetActiveRemoteControlRocket() const {

    ProjectileMapConstIter findIter = this->projectiles.find(Projectile::PaddleRemoteCtrlRocketBulletProjectile);

    if (findIter != this->projectiles.end()) {
        const GameModel::ProjectileList& remoteCtrlRocketList = findIter->second;
        if (!remoteCtrlRocketList.empty()) {
            // A remote control rocket is active!
            assert(remoteCtrlRocketList.size() == 1); // There should only, at most, be one remote control rocket active at a time!

            assert(dynamic_cast<PaddleRemoteControlRocketProjectile*>(*remoteCtrlRocketList.begin()) != NULL);
            return static_cast<PaddleRemoteControlRocketProjectile*>(*remoteCtrlRocketList.begin());
        }
    }

    return NULL;
}

#ifdef _DEBUG
void GameModel::DropItem(GameItem::ItemType itemType) {
	BallInPlayState* state = dynamic_cast<BallInPlayState*>(this->currState);
	if (state != NULL) {
		Vector2D levelDim = this->GetLevelUnitDimensions();
        Vector2D dropDir(0,-1);
        if (playerPaddle->GetIsPaddleFlipped()) {
            dropDir[1] = 1;
        }
		state->DebugDropItem(GameItemFactory::GetInstance()->CreateItem(itemType, 
            Point2D(playerPaddle->GetCenterPosition()[0], 0.5f*levelDim[1]), dropDir, this));
	}
}

void GameModel::DropThreeItems(GameItem::ItemType itemType1, GameItem::ItemType itemType2, GameItem::ItemType itemType3) {
    
    BallInPlayState* state = dynamic_cast<BallInPlayState*>(this->currState);
    if (state != NULL) {
        const Vector2D distToOtherItem(1.5f*GameItem::ITEM_WIDTH, 0.0f);
        Point2D midDropPt   = Point2D(0,0) + 0.5f*this->GetLevelUnitDimensions();
        Point2D leftDropPt  = midDropPt - distToOtherItem;
        Point2D rightDropPt = midDropPt + distToOtherItem;
        Vector2D dropDir(0,-1);

        state->DebugDropItem(GameItemFactory::GetInstance()->CreateItem(itemType1, leftDropPt, dropDir,  this));
        state->DebugDropItem(GameItemFactory::GetInstance()->CreateItem(itemType2, midDropPt, dropDir,   this));
        state->DebugDropItem(GameItemFactory::GetInstance()->CreateItem(itemType3, rightDropPt, dropDir, this));
    }
}
#endif