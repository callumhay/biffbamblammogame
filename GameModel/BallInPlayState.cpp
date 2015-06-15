/**
 * BallInPlayState.cpp
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

#include "BallInPlayState.h"
#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "BallDeathState.h"
#include "LevelCompleteState.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "GameItem.h"
#include "GameItemFactory.h"
#include "Beam.h"
#include "BallBoostModel.h"
#include "SafetyNet.h"
#include "PaddleRemoteControlRocketProjectile.h"
#include "CannonBlock.h"
#include "GameTransformMgr.h"

struct BallCollisionChangeInfo {
public:
    BallCollisionChangeInfo() : posChanged(false), paddleRedirectBallVelocity(false), impulseApplied(false), impulseAmt(0), impulseDecel(0) {}
    BallCollisionChangeInfo(bool posChanged) : posChanged(posChanged), paddleRedirectBallVelocity(false), impulseApplied(false), impulseAmt(0), impulseDecel(0) {}
    BallCollisionChangeInfo(bool posChanged, bool impulseApplied, float impulseAmt, float impulseDecel) :
      posChanged(posChanged), impulseApplied(impulseApplied), impulseAmt(impulseAmt), impulseDecel(impulseDecel), paddleRedirectBallVelocity(false) {}
    BallCollisionChangeInfo(const BallCollisionChangeInfo& copy) : posChanged(copy.posChanged), 
        paddleRedirectBallVelocity(copy.paddleRedirectBallVelocity), impulseApplied(copy.impulseApplied), 
        impulseAmt(copy.impulseAmt), impulseDecel(copy.impulseDecel) {}
    ~BallCollisionChangeInfo(){}

    BallCollisionChangeInfo& operator=(const BallCollisionChangeInfo& copy) {
        this->posChanged = copy.posChanged;
        this->paddleRedirectBallVelocity = copy.paddleRedirectBallVelocity;
        this->impulseApplied = copy.impulseApplied;
        this->impulseAmt = copy.impulseAmt;
        this->impulseDecel = copy.impulseDecel;
        return *this;
    }

    bool posChanged;
    bool paddleRedirectBallVelocity;
    bool impulseApplied;
    
    float impulseAmt;
    float impulseDecel;
};

BallInPlayState::BallInPlayState(GameModel* gm) : 
GameState(gm), timeSinceGhost(DBL_MAX) {

    // The ball in play state is the only state that allows for ball boosting -
    // create a ball boost model for it in the game model
    assert(this->gameModel->boostModel == NULL);
    this->gameModel->boostModel = new BallBoostModel(gm);
}

BallInPlayState::~BallInPlayState() {
    assert(this->gameModel->boostModel != NULL);
    // Clean up the boost model - it's only allowed to exist while the ball is in play
    delete this->gameModel->boostModel;
    this->gameModel->boostModel = NULL;
}

/**
 * Drop an item manually via debug hot keys and commands.
 */
void BallInPlayState::DebugDropItem(GameItem* item) {
	assert(item != NULL);
	this->gameModel->GetLiveItems().push_back(item);
	GameEventManager::Instance()->ActionItemSpawned(*item);
}

/**
 * The action key was pressed while in play; that could mean several things
 * based on the items currently active for the player.
 */
void BallInPlayState::ShootActionReleaseUse() {

    // If there's a remote control rocket then we apply thrust to it...
    PaddleRemoteControlRocketProjectile* remoteControlRocket = this->gameModel->GetActiveRemoteControlRocket();
    if (remoteControlRocket != NULL) {
        remoteControlRocket->ControlRocketThrust(1.0f);
    }

    if ((this->gameModel->GetPauseState() & GameModel::PauseBall) == 0x0) {
        // Always boost the ball when possible (if the player is indicating that they want to),
        // Regardless, this will be ignored if the boost model isn't in the appropriate state
        assert(this->gameModel->boostModel != NULL);
        if (this->gameModel->boostModel->BallBoosterPressed()) {
            return;
        }
    }

    // Deal with the special case where the player is in ball camera mode and the ball is in a cannon:
    // The player has control to shoot the ball out of the cannon
    if (GameBall::GetIsBallCameraOn()) {
        const std::list<GameBall*>& balls = this->gameModel->GetGameBalls();
        for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
            GameBall* currBall = *iter;
            if (currBall->IsLoadedInCannonBlock()) {
                
                CannonBlock* cannon = currBall->GetCannonBlock();
                assert(cannon != NULL);

                if (currBall->CanShootBallCamOutOfCannon(*cannon, *this->gameModel->GetCurrentLevel())) {
                    cannon->Fire();
                }
                else {
                    // EVENT: Not allowed to fire the ball out of the cannon due to obstruction
                    GameEventManager::Instance()->ActionCantFireBallCamFromCannon();
                }
                break;
            }
        }
    }

    // Be absolutely sure we aren't still in bullet time before we apply controls to the paddle,
    // also make sure the paddle isn't paused in any way
    assert(this->gameModel->boostModel != NULL);
    if (this->gameModel->boostModel->GetBulletTimeState() == BallBoostModel::NotInBulletTime &&
        (this->gameModel->GetPauseState() & (GameModel::PausePaddle | GameModel::PausePaddleControls)) == 0x0) {

	    // Check for paddle items that use the action key...
	    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	    paddle->DiscreteShoot(this->gameModel);
    }
}

void BallInPlayState::ShootActionContinuousUse(double dT, float magnitudePercent) {
    
    // If there's a remote control rocket then we apply thrust to it...
    PaddleRemoteControlRocketProjectile* remoteControlRocket = this->gameModel->GetActiveRemoteControlRocket();
    if (remoteControlRocket != NULL) {
        remoteControlRocket->ControlRocketThrust(magnitudePercent);
        return;
    }

    // Other continuous-use cases:
  
    // For special paddle types...
    // Be absolutely sure we aren't still in bullet time before we apply controls to the paddle,
    // also make sure the paddle isn't paused in any way
    assert(this->gameModel->boostModel != NULL);
    if (this->gameModel->boostModel->GetBulletTimeState() == BallBoostModel::NotInBulletTime &&
        (this->gameModel->GetPauseState() & (GameModel::PausePaddle | GameModel::PausePaddleControls)) == 0x0) {

        PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
        paddle->ContinuousShoot(dT, this->gameModel, magnitudePercent);
    }
}

void BallInPlayState::MoveKeyPressedForPaddle(int dir, float magnitudePercent) {

    // Deal with the special case where the player is in ball camera mode and the ball is in a cannon:
    // The player can control the rotation of the cannon
    if (GameBall::GetIsBallCameraOn()) {
        const std::list<GameBall*>& balls = this->gameModel->GetGameBalls();
        for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
            
            GameBall* currBall = *iter;
            if (currBall->IsLoadedInCannonBlock()) {
                CannonBlock* cannon = currBall->GetCannonBlock();
                assert(cannon != NULL);
                cannon->SetRotationSpeed(dir, magnitudePercent);
                
                if (dir != 0 && magnitudePercent > 0.0f) {
                    // EVENT: Player just controlled the rotation of the cannon
                    GameEventManager::Instance()->ActionBallCameraCannonRotation(*currBall, *cannon, 
                        currBall->CanShootBallCamOutOfCannon(*cannon, *this->gameModel->GetCurrentLevel()));
                }

                // Ensure that all other things being controlled are told to stop
                GameState::MoveKeyPressedForPaddle(0, 0);

                return;
            }
        }
    }

    GameState::MoveKeyPressedForPaddle(dir, magnitudePercent);
}

void BallInPlayState::MoveKeyPressedForOther(int dir, float magnitudePercent) {
    // If there's a remote control rocket we have to move it...
    PaddleRemoteControlRocketProjectile* remoteControlRocket = this->gameModel->GetActiveRemoteControlRocket();
    if (remoteControlRocket != NULL) {
        remoteControlRocket->ControlRocketSteering(static_cast<PaddleRemoteControlRocketProjectile::RocketSteering>(dir), magnitudePercent);
    }
}

void BallInPlayState::BallBoostDirectionPressed(float x, float y, bool allowLargeChangeInDirection) {
    assert(this->gameModel->boostModel != NULL);

    // Ignore applying the boost direction when the ball is paused
    if ((this->gameModel->GetPauseState() & GameModel::PauseBall) == GameModel::PauseBall) {
        return;
    }

    // Modify the direction of the boost based on the current game transform information
    Vector2D dir(x,y);
    if (this->gameModel->AreControlsFlippedForOther()) {
        dir *= -1;
    }
    dir.Rotate(this->gameModel->GetTransformInfo()->GetGameZRotationInDegs());

    this->gameModel->boostModel->BallBoostDirectionPressed(dir[0], dir[1], allowLargeChangeInDirection);
}

void BallInPlayState::BallBoostDirectionReleased() {
    assert(this->gameModel->boostModel != NULL);
    this->gameModel->boostModel->BallBoostDirectionReleased();
}

void BallInPlayState::BallBoostReleasedForBall(const GameBall& ball) {
    assert(this->gameModel->boostModel != NULL);
    this->gameModel->boostModel->BallBoostReleasedForBall(ball);
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
	
	// Obtain some of the level pieces needed to figure out what is happening in the game
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	GameLevel *currLevel = this->gameModel->GetCurrentLevel();

	// Check for item-paddle collisions
	this->DoItemCollision();
	
    // Check for projectile collisions
    this->gameModel->DoProjectileCollisions(seconds);

	// Update any timers that are currently active
	this->gameModel->UpdateActiveTimers(seconds);
	// Update any item drops that are currently active
	this->gameModel->UpdateActiveItemDrops(seconds);
	// Update any particles that are currently active
	this->gameModel->UpdateActiveProjectiles(seconds);
	// Update any beams that are currently active
	this->gameModel->UpdateActiveBeams(seconds);

	// Variables that will be needed for collision detection
	Vector2D n, bestNormal;
    Point2D collisionPt, otherCollisionPt;
	Collision::LineSeg2D collisionLine;
	double timeUntilCollision, bestTimeUntilCollision;
    float ballMoveImpulse;
	bool didCollideWithPaddle = false;
	bool didCollideWithBlock = false;
	bool didCollideWithTeslaLightning = false;
    std::vector<LevelPiece*> collisionPieces(12, NULL);
	
    // The last ball to hit the paddle is the one with priority for item effects
	GameBall* ballToMoveToFront = NULL;
	// The current set of balls in play
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();

    // Boss collisions with the paddle
    if (currLevel->GetHasBoss() && !paddle->HasBeenPausedAndRemovedFromGame(gameModel->GetPauseState())) {
        Boss* boss = currLevel->GetBoss();
        assert(boss != NULL);
        
        BossBodyPart* collisionBossPart = boss->CollisionCheck(*paddle);
        if (collisionBossPart != NULL) {
            
            boss->CollisionOccurred(this->gameModel, *paddle, collisionBossPart);
            if (boss->CanHurtPaddleWithBody()) {
                paddle->HitByBoss(*collisionBossPart);

                // Cancel any active paddle shield
                gameModel->RemoveActiveGameItemsOfGivenType(GameItem::ShieldPaddleItem);
            }

            // Make sure the paddle is no longer colliding with the boss
            Collision::AABB2D bossAABB = collisionBossPart->GenerateWorldAABB();
            float distXToPaddleCenter = paddle->GetCenterPosition()[0] - bossAABB.GetCenter()[0];
            if (distXToPaddleCenter < 0) {
                paddle->SetCenterPosition(Point2D(bossAABB.GetMin()[0] - paddle->GetHalfWidthTotal() - EPSILON, paddle->GetCenterPosition()[1]));
            }
            else {
                paddle->SetCenterPosition(Point2D(bossAABB.GetMax()[0] + paddle->GetHalfWidthTotal() + EPSILON, paddle->GetCenterPosition()[1]));
            }
        }
    }

	// Pause the ball from moving
	if ((this->gameModel->GetPauseState() & GameModel::PauseBall) == 0x0) {

	    // Gravity vector in world space
	    Vector3D worldGravityDir = this->gameModel->GetGravityDir();
	    Vector2D worldGravity2D(worldGravityDir[0], worldGravityDir[1]);

        // Clear out any dead balls...
	    for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end();) {
		    GameBall *currBall = *iter;
        
		    // Check for death (ball went out of bounds)
		    if (this->gameModel->IsOutOfGameBoundsForBall(currBall->GetBounds().Center())) {
			    if (gameBalls.size() == 1) {
                    this->gameModel->SetNextState(new BallDeathState(currBall, this->gameModel));
				    return;
			    }
			    else {
				    // The ball is now dead and needs to be removed from the game...

		            // EVENT: Ball died
		            GameEventManager::Instance()->ActionBallDied(*currBall);

		            // SPECIAL CASE:
		            // We need to be very careful - if the player is in ball camera mode but there are multiple balls and the one
		            // that the camera is in just died then we need to deactivate ball camera mode but keep the player alive!
		            if (currBall == GameBall::GetBallCameraBall()) {
			            // Deactivate the ball camera item!!!
			            bool wasRemoved = this->gameModel->RemoveActiveGameItemsOfGivenType(GameItem::BallCamItem);
                        UNUSED_VARIABLE(wasRemoved);
			            assert(wasRemoved);
		            }

                    this->gameModel->RemoveActiveGameItemsForThisBallOnly(currBall);

		            iter = gameBalls.erase(iter);
		            delete currBall;
		            currBall = NULL;
                    continue;
			    }
		    }

            ++iter;
        }

        std::vector<BallCollisionChangeInfo> ballChangedByCollision(gameBalls.size(), BallCollisionChangeInfo());
        int ballIdx = 0;
	    for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter, ballIdx++) {
		    GameBall *currBall = *iter;

		    // Check to see if the ball is allowed to collide with paddles, if so run the ball-paddle
		    // collision simulation
		    if (currBall->CanCollideWithPaddles()) {

			    didCollideWithPaddle = paddle->CollisionCheck(*currBall, seconds, n, collisionLine, timeUntilCollision, collisionPt);
			    if (didCollideWithPaddle) {
				    // The ball no longer has a last piece that it collided with - it gets reset when it hits the paddle
				    currBall->SetLastPieceCollidedWith(NULL);

				    // Don't do anything if this ball is the one attached to the paddle
				    if (paddle->GetAttachedBall() == currBall) {
					    continue;
				    }
                    paddle->HitByBall(*currBall);

                    bool ballHitPaddleundercarriage = (paddle->GetBottomCollisionNormal() == n);
				    // EVENT: Ball-paddle collision
				    GameEventManager::Instance()->ActionBallPaddleCollision(*currBall, *paddle, ballHitPaddleundercarriage);

				    // If the sticky paddle power-up is activated then the ball will simply be attached to
				    // the player paddle (if there are no balls already attached) ... unless the paddle has a shield active as well
				    if (paddle->HasPaddleType(PlayerPaddle::StickyPaddle) && !paddle->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
                        
                        // Check to see what collision line on the paddle the ball collided with, if it's the undercarriage then we
                        // will not allow the ball to stick to the paddle
                        if (!ballHitPaddleundercarriage) {
					        if (this->gameModel->GetPlayerPaddle()->AttachBall(currBall)) {
                                // Reset the multiplier
                                this->gameModel->SetNumInterimBlocksDestroyed(0);
						        continue;
					        }
                        }
                        else {
                            currBall->SetLastThingCollidedWith(paddle);
                        }
				    }

				    // Do ball-paddle collision
                    double timeSinceLastBallCollision = currBall->GetTimeSinceLastCollision();
                    this->DoBallPaddleCollision(*currBall, *paddle, n, seconds, timeUntilCollision, 
                        GameBall::MIN_BALL_ANGLE_ON_PADDLE_HIT_IN_DEGS, paddle->GetVelocity());
                    BallCollisionChangeInfo& collisionInfo = ballChangedByCollision[ballIdx];                    
                    collisionInfo.posChanged = true;

                    // Only apply the impulse if the paddle is not close to a wall, the ball hasn't collided
                    // recently (not including the current collision with the paddle), and both the paddle and ball are going in the same
                    // general direction
                    if (!paddle->GetIsHittingAWall() && !paddle->GetIsCloseToAWall() && timeSinceLastBallCollision > 2*seconds &&
                        Vector2D::Dot(currBall->GetDirection(), paddle->GetVelocity()) > 0) {
                        collisionInfo.impulseApplied = true;
                        collisionInfo.impulseAmt     = (GameBall::GetNormalSpeed() / currBall->GetSpeed()) * fabs(paddle->GetSpeed()) / 5.0f;
                        collisionInfo.impulseDecel   = collisionInfo.impulseAmt;
                    }

				    // Tell the model that a ball collision occurred with the paddle
				    this->gameModel->BallPaddleCollisionOccurred(*currBall);

				    if (!paddle->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
                        collisionInfo.paddleRedirectBallVelocity = true;
				    }

				    // If there are multiple balls and the one that just hit the paddle is not
				    // the first one in the list, then we need to move this one to the front
				    if (gameBalls.size() > 1 && currBall != (*gameBalls.begin())) {
					    ballToMoveToFront = currBall;
				    }
			    }
            }

		    // Ball-ball collisions - choose the next set of balls after this one
		    // so that collisions are not duplicated
		    std::list<GameBall*>::iterator nextIter = iter;
		    nextIter++;
		    for (; nextIter != gameBalls.end(); ++nextIter) {
			    GameBall* otherBall = *nextIter;
			    assert(currBall != otherBall);

			    // Make sure to check if both balls collide with each other
			    if (currBall->CollisionCheck(seconds, *otherBall, n, collisionLine, timeUntilCollision, otherCollisionPt, collisionPt)) {

                    this->DoBallCollision(*currBall, *otherBall, collisionPt, otherCollisionPt, seconds, timeUntilCollision);
				    // EVENT: Two balls have collided
				    GameEventManager::Instance()->ActionBallBallCollision(*currBall, *otherBall);
			    }
		    }

		    // Ball Safety Net Collisions:
            this->DoBallSafetyNetCollision(this->gameModel->bottomSafetyNet, *currBall, seconds, n, collisionLine, 
                timeUntilCollision, collisionPt, ballChangedByCollision[ballIdx]);
            this->DoBallSafetyNetCollision(this->gameModel->topSafetyNet, *currBall, seconds, n, collisionLine, 
                timeUntilCollision, collisionPt, ballChangedByCollision[ballIdx]);

		    // Ball - Tesla lightning arc collisions:
		    didCollideWithTeslaLightning = currLevel->TeslaLightningCollisionCheck(*currBall, seconds, n, collisionLine, timeUntilCollision, collisionPt);
		    if (didCollideWithTeslaLightning) {
			    // Clear the last piece collided with since the ball is now colliding with a Tesla lightning arc
			    currBall->SetLastPieceCollidedWith(NULL);
    			
			    // Calculate the ball position/velocity after collision
			    this->DoBallCollision(*currBall, n, seconds, timeUntilCollision, 
                    GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS, ballMoveImpulse);
                ballChangedByCollision[ballIdx].posChanged = true;
		    }

		    // Make sure the ball can collide with level pieces (blocks) before running the block collision simulation
		    if (currBall->CanCollideWithBlocksAndBosses()) {

                // Now do boss collisions with the ball...
                if (currLevel->GetHasBoss()) {
                    Boss* boss = currLevel->GetBoss();
                    assert(boss != NULL);
                    
                    // Collide with the boss until there are no more collisions
                    BossBodyPart* collisionBossPart = NULL;

                    // NOTE: For bosses, ghost-balls never pass through.
                    // NOTE: For bosses we don't worry about issues with the paddle having a ball on it.
                    collisionBossPart = boss->CollisionCheck(*currBall, seconds, n, collisionLine, timeUntilCollision, collisionPt);
                    if (collisionBossPart != NULL) {
                        Vector2D bossVel = collisionBossPart->GetCollisionVelocity();
                        
                        // Make the ball react to the collision...
                        this->DoBallCollision(*currBall, n, seconds, timeUntilCollision, 
                            GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS, ballMoveImpulse, bossVel);
                        currBall->RemoveImpulseForce();
                        
                        BallCollisionChangeInfo& collisionInfo = ballChangedByCollision[ballIdx];   
                        collisionInfo.posChanged = true;
                        if (!bossVel.IsZero()) {
                            collisionInfo.impulseApplied = true;
                            collisionInfo.impulseAmt = ballMoveImpulse;
                            collisionInfo.impulseDecel = collisionInfo.impulseAmt;
                        }

                        // Now make the boss react to the collision...
                        this->gameModel->CollisionOccurred(*currBall, boss, collisionBossPart);
                    }
                }

			    // Check for ball collision with level pieces
			    // Get the small set of level pieces based on the position of the ball...
			    collisionPieces.clear();
                currLevel->GetLevelPieceCollisionCandidates(seconds, currBall->GetBounds().Center(),
                    currBall->GetBounds().Radius(), currBall->GetSpeed(), collisionPieces);

                // Start by finding the best candidate out of the possible collisions...
                LevelPiece* bestPiece = NULL;
                bestTimeUntilCollision = std::numeric_limits<double>::max();
                for (std::vector<LevelPiece*>::iterator pieceIter = collisionPieces.begin(); 
                    pieceIter != collisionPieces.end(); ++pieceIter) {

                    LevelPiece *currPiece = *pieceIter;
                    didCollideWithBlock = currPiece->CollisionCheck(*currBall, seconds,
                        n, collisionLine, timeUntilCollision, collisionPt);

                    if (didCollideWithBlock && timeUntilCollision < bestTimeUntilCollision) {
                        bestTimeUntilCollision = timeUntilCollision;
                        bestNormal = n;
                        bestPiece = currPiece;
                    }
                }

                // Do the collision if a piece was found
                if (bestPiece != NULL) {
                    
			        // Check to see if the ball is a ghost ball, if so there's a chance the ball will 
			        // lose its ability to collide for 1 second, also check to see if we're already in ghost mode
			        // if so we won't collide with anything (except solid blocks)...
			        if (currBall->HasBallType(GameBall::GhostBall) && bestPiece->GhostballPassesThrough()) {
						
				        if (this->timeSinceGhost < GameModelConstants::GetInstance()->LENGTH_OF_GHOSTMODE) {
					        continue;
				        }

				        // If the ball is in ghost mode then it cannot hit anything other than the paddle and solid blocks...
				        // NOTE: we divide the probability of entering full ghost mode (i.e., not hitting blocks) by the number of balls
				        // because theoretically the rate of hitting blocks should increase multiplicatively with the number of balls
				        if (this->timeSinceGhost >= GameModelConstants::GetInstance()->LENGTH_OF_GHOSTMODE &&
					        Randomizer::GetInstance()->RandomNumZeroToOne() <= 
					        (GameModelConstants::GetInstance()->PROB_OF_GHOSTBALL_BLOCK_MISS / static_cast<float>(gameBalls.size()))) {
							
					        this->timeSinceGhost = 0.0;
					        continue;
				        }
			        }

			        // If the piece doesn't have bounds to bounce off of then don't bounce the ball OR
			        // In the case that the ball is uber then we only reflect if the ball is not green OR
			        // The ball is attached to the paddle (special case)
			        if (!bestPiece->BallBouncesOffWhenHit() ||
                        (currBall->HasBallType(GameBall::UberBall) && bestPiece->BallBlastsThrough(*currBall))) {

				        // Ignore collision...
			        }
			        else if (paddle->GetAttachedBall() == currBall) {
                        // Ignore this, we deal with it later on in DoUpdateToPaddleBoundriesAndCollisions
			        }
			        else {
				        // Make the ball react to the collision
				        this->DoBallCollision(*currBall, bestNormal, seconds, bestTimeUntilCollision,
                            GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS, ballMoveImpulse);
                        ballChangedByCollision[ballIdx].posChanged = true;
			        }
					
                    // Tell the model that a ball collision occurred with currPiece
                    this->gameModel->CollisionOccurred(*currBall, bestPiece);
                }
            }

        } // All GameBalls loop

	    // Tick the balls
        ballIdx = 0;
	    for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter, ballIdx++) {
		    GameBall *currBall = *iter;
            BallCollisionChangeInfo& currCollisionInfo = ballChangedByCollision[ballIdx];

		    // Update the current ball, we don't simulate any ball movement if the ball's position has already
            // been augmented by a previous collision during this frame/tick
            currBall->Tick(!currCollisionInfo.posChanged, seconds, worldGravity2D, this->gameModel);
             
            bool dirWasAugmentedByPaddle = false;
            if (currCollisionInfo.paddleRedirectBallVelocity) {
                // Make sure the ball's velocity direction is not downward - it's annoying to hit the ball with a paddle and
                // still see it fly into the void. If the shield is active then no help is provided
                dirWasAugmentedByPaddle = this->AugmentBallDirectionToBeNotTooDownwards(*currBall, *paddle, n);
            }

            if (!dirWasAugmentedByPaddle && currCollisionInfo.impulseApplied) {
                currBall->ApplyImpulseForce(currCollisionInfo.impulseAmt, currCollisionInfo.impulseDecel);
            }
	    }

        // NOTE: DO THIS LAST SINCE IT CHANGES THE ORDERING OF THE BALLS!
	    // Move the last ball that hit the paddle to the front of the list of balls
	    if (ballToMoveToFront != NULL) {
		    gameBalls.remove(ballToMoveToFront);
		    gameBalls.push_front(ballToMoveToFront);
	    }

	} // Pause ball

	// Quick check to see if the paddle collided with the safety net - this will just register necessary
	// events and destroy the net if it exists and there is a collision
	if (!paddle->HasBeenPausedAndRemovedFromGame(this->gameModel->GetPauseState())) {
        
        if (this->gameModel->IsBottomSafetyNetActive() && 
            this->gameModel->bottomSafetyNet->PaddleCollisionCheck(*paddle)) {

            this->gameModel->DestroyBottomSafetyNet();
            
            // EVENT: The paddle just destroyed the safety net
            GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*paddle, true);
        }
        else if (this->gameModel->IsTopSafetyNetActive() &&
            this->gameModel->topSafetyNet->PaddleCollisionCheck(*paddle)) {
            
            this->gameModel->DestroyTopSafetyNet();

            // EVENT: The paddle just destroyed the safety net
            GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*paddle, false);
        }
    }

	// Paddle-block collisions / boundary update (so that the paddle crashes into potential blocks at its sides).
    this->DoUpdateToPaddleBoundriesAndCollisions(seconds, true);
	
    // Tick/update any level pieces that require it...
	this->gameModel->DoPieceStatusUpdates(seconds);

    // Update the boost model for the ball(s)
    this->gameModel->boostModel->Tick(*this->gameModel, seconds);

    // Update AI entities
    if ((this->gameModel->GetPauseState() & GameModel::PauseAI) == 0x00000000) {
        this->gameModel->GetCurrentLevel()->TickAIEntities(seconds, this->gameModel);
    }
}

// n must be normalized
// d is the distance from the center of the ball to the line that was collided with
// when d is negative the ball is inside the line, when positive it is outside
void BallInPlayState::DoBallCollision(GameBall& b, const Vector2D& n, 
                                      double dT, double timeUntilCollision, 
                                      float minAngleInDegs, float& ballNewtonsThirdLawImpulse, 
                                      const Vector2D& lineVelocity) {
	b.BallCollided();

	// Calculate the time of collision and then the difference up to this point
	// based on the velocity and then move it to that position...
    double timeToMoveInReflectionDir = std::max<double>(0.0, dT - timeUntilCollision);

    // Position the ball so that it is at the location it was at when it collided...
    b.SetCenterPosition(b.GetCenterPosition2D() + timeUntilCollision * b.GetVelocity());

	// Make sure that the direction of the ball is against that of the normal, otherwise we adjust it to be so
	Vector2D reflVecHat;

	if (Vector2D::Dot(b.GetDirection(), n) >= 0) {
		// Somehow the ball is traveling away from the normal but is hitting the line...

        // The ball will just keep its direction
        reflVecHat = b.GetDirection();
	}
	else {
        // Typical bounce off the normal: figure out the reflection vector
        reflVecHat = Vector2D::Normalize(Reflect(b.GetDirection(), n));

        // In the case of gravity we decrease the reflection
        if (b.HasBallType(GameBall::GraviBall)) {
            reflVecHat = Vector2D::Normalize(0.5f * (reflVecHat + n));
        }
	}

	float reflSpd = b.GetSpeed();
	
	// This should NEVER happen
	if (reflSpd == GameBall::GetZeroSpeed()) {
		assert(false);
		return;
	}

    // Figure out the angle between the normal and the reflection...
    float angleOfReflInDegs = Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(n, reflVecHat)))));
    float diffAngleInDegs   = minAngleInDegs - fabs(angleOfReflInDegs);

    // Make sure the reflection is big enough to not cause an annoying slow down in the game
    if (diffAngleInDegs > 0) {

        // We need to figure out which way to rotate the velocity
        // to ensure it's at least the minAngleInDegs
        Vector2D newReflVelHat = Vector2D::Rotate(diffAngleInDegs, reflVecHat);

        // Check to see if it's still the case, if it is then we rotated the wrong way
        float newAngleInDegs =  Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(n, newReflVelHat)))));
        if (newAngleInDegs < minAngleInDegs) {
	        newReflVelHat = Vector2D::Rotate(-diffAngleInDegs, reflVecHat);
        }
		
        reflVecHat = newReflVelHat;
    }
    else {
        // We also don't want the ball to 'grace' any edges too closely -- we figure this out by checking to see
        // if the angle difference is too obtuse
        diffAngleInDegs = fabs(angleOfReflInDegs) - GameBall::MAX_GRACING_ANGLE_ON_HIT_IN_DEGS;

        if (diffAngleInDegs > 0) {
	        Vector2D newReflVelHat = Vector2D::Rotate(diffAngleInDegs, reflVecHat);

	        // Check to see if it's still the case, if it is then we rotated the wrong way
	        float newAngleInDegs =  Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(n, newReflVelHat)))));
	        if (newAngleInDegs > GameBall::MAX_GRACING_ANGLE_ON_HIT_IN_DEGS) {
		        newReflVelHat = Vector2D::Rotate(-diffAngleInDegs, reflVecHat);
	        }

	        reflVecHat = newReflVelHat;
        }
    }

    // Reflect the ball off the normal... this will have some dependence on whether there is a velocity for the line...
    Vector2D moveVel(0,0);
    ballNewtonsThirdLawImpulse = 0;
    if (!lineVelocity.IsZero()) {

        Vector2D reflectionVel = reflSpd * reflVecHat;
        Vector2D augmentedReflectionVecHat = Vector2D::Normalize(reflectionVel + lineVelocity);
        reflVecHat = augmentedReflectionVecHat;

        if (Vector2D::Dot(reflVecHat, lineVelocity) > 0.0f) {
            moveVel += lineVelocity;
            ballNewtonsThirdLawImpulse = std::max<float>(0, lineVelocity.Magnitude() - 0.66f*b.GetSpeed());
        }
    }
   
    b.SetVelocity(reflSpd, reflVecHat);
    moveVel += b.GetVelocity();

	// Now move the ball in that direction over how ever much time was lost during the collision
	b.SetCenterPosition(b.GetCenterPosition2D() + timeToMoveInReflectionDir * moveVel);
}

void BallInPlayState::DoBallPaddleCollision(GameBall& b, PlayerPaddle& p, const Vector2D& n, double dT, 
                                            double timeUntilCollision, float minAngleInDegs, const Vector2D& lineVelocity) {
    UNUSED_PARAMETER(p);
    b.BallCollided();

    // Calculate the time of collision and then the difference up to this point
    // based on the velocity and then move it to that position...
    double timeToMoveInReflectionDir = std::max<double>(0.0, dT - timeUntilCollision);

    // Position the ball so that it is at the location it was at when it collided...
    b.SetCenterPosition(b.GetCenterPosition2D() + timeUntilCollision * b.GetVelocity());

    // Make sure that the direction of the ball is against that of the normal, otherwise we adjust it to be so
    Vector2D reflVecHat;

    if (Vector2D::Dot(b.GetDirection(), n) >= 0) {
        // Somehow the ball is traveling away from the normal but is hitting the line...

        // The ball will just keep its direction
        reflVecHat = b.GetDirection();
    }
    else {
        // Typical bounce off the normal: figure out the reflection vector
        reflVecHat = Vector2D::Normalize(Reflect(b.GetDirection(), n));

        // In the case of gravity we decrease the reflection
        if (b.HasBallType(GameBall::GraviBall)) {
            reflVecHat = Vector2D::Normalize(0.5f * (reflVecHat + n));
        }
    }

    float reflSpd = b.GetSpeed();

    // This should NEVER happen
    if (reflSpd == GameBall::GetZeroSpeed()) {
        assert(false);
        return;
    }

    // Reflect the ball off the normal... this will have some dependence on whether there is a velocity for the line...
    Vector2D moveVel(0,0);
    if (!lineVelocity.IsZero()) {

        Vector2D reflectionVel = reflSpd * reflVecHat;
        Vector2D augmentedReflectionVecHat = Vector2D::Normalize(reflectionVel + lineVelocity);
        reflVecHat = augmentedReflectionVecHat;

        if (Vector2D::Dot(reflVecHat, lineVelocity) > 0.0f) {
            moveVel += lineVelocity;
        }

    }
   
    // Only have to check whether the ball is acceptably far enough away from the +/-x directions
    float angleAwayFromX =  Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(Vector2D(1,0), reflVecHat)))));
    if (angleAwayFromX < minAngleInDegs) {
        // Rotate so that its going to fire upwards a bit from the (1,0) direction
        reflVecHat = Vector2D::Rotate(minAngleInDegs - angleAwayFromX, reflVecHat);
    }
    else {
        float oneEightyMinusMinAngle = 180.0f - minAngleInDegs;
        if (angleAwayFromX > oneEightyMinusMinAngle) {
            // Rotate so that its going to fire upwards a bit from the (-1,0) direction
            reflVecHat = Vector2D::Rotate(oneEightyMinusMinAngle - angleAwayFromX, reflVecHat);
        }
    }

    b.SetVelocity(reflSpd, reflVecHat);
    moveVel += b.GetVelocity();

    // Now move the ball in that direction over how ever much time was lost during the collision
    b.SetCenterPosition(b.GetCenterPosition2D() + timeToMoveInReflectionDir * moveVel);
}

/**
 * Private helper function to calculate the new velocities for two balls that just
 * collided with each other.
 */
void BallInPlayState::DoBallCollision(GameBall& ball1, GameBall& ball2, const Point2D& ball1PtOfCollision, 
                                      const Point2D& ball2PtOfCollision, double dT, double timeUntilCollision) {
	ball1.BallCollided();
    ball1.SetLastThingCollidedWith(&ball2);
	ball2.BallCollided();
    ball2.SetLastThingCollidedWith(&ball1);

    double timeToMoveInReflectionDir = std::max<double>(0.0, dT - timeUntilCollision);
    
    // Get the normalized direction of the difference vector between the collision points
    Vector2D ptOfCollisionDiffVec = ball2PtOfCollision - ball1PtOfCollision;
    ptOfCollisionDiffVec.Normalize();

    float pValue = Vector2D::Dot(ball1.GetVelocity(), ptOfCollisionDiffVec) - Vector2D::Dot(ball2.GetVelocity(), ptOfCollisionDiffVec);
    Vector2D ball1NewVelDir = ball1.GetVelocity() - pValue * ptOfCollisionDiffVec;
    ball1NewVelDir.Normalize();
    Vector2D ball2NewVelDir = ball2.GetVelocity() + pValue * ptOfCollisionDiffVec;
    ball2NewVelDir.Normalize();
    
    ball1.SetVelocity(ball1.GetSpeed(), ball1NewVelDir);
    ball2.SetVelocity(ball2.GetSpeed(), ball2NewVelDir);

    // Place the balls at their respective points of collision, and them move them
    // the fraction of the tick in the direction of their respective ricochets
    ball1.SetCenterPosition(ball1PtOfCollision + timeToMoveInReflectionDir * ball1.GetVelocity());
    ball2.SetCenterPosition(ball2PtOfCollision + timeToMoveInReflectionDir * ball2.GetVelocity());
}

/**
 * Private helper function for checking to see if an item collided with 
 * the player paddle and carrying out the necessary activity if one did.
 */
void BallInPlayState::DoItemCollision() {

    // Check to see if the paddle hit any items, if so, activate those items
    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();

    // Ignore item collisions if the paddle has been removed from play
    if (paddle->HasBeenPausedAndRemovedFromGame(this->gameModel->GetPauseState())) {
        return;
    }

	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::list<GameItem*>& currLiveItems     = this->gameModel->GetLiveItems();
	for (std::list<GameItem*>::iterator iter = currLiveItems.begin(); iter != currLiveItems.end();) {
		GameItem *currItem = *iter;
		
		if (currItem->CollisionCheck(*paddle)) {

			// EVENT: Item was obtained by the player paddle
			GameEventManager::Instance()->ActionItemPaddleCollision(*currItem, *paddle);
            
            // Increment the number of items acquired
            this->gameModel->IncrementNumAcquiredItems(currItem);

			// There was a collision with the item and the player paddle: activate the item
			// and then delete it. If the item causes the creation of a timer then add the timer to the list
			// of active timers.
			GameItemTimer* newTimer = new GameItemTimer(currItem);
			assert(newTimer != NULL);
			activeTimers.push_back(newTimer);	
			
            GameEventManager::Instance()->ActionItemRemoved(*currItem);
            iter = currLiveItems.erase(iter);
		}
        else {
            ++iter;
        }
	}
}

void BallInPlayState::DoBallSafetyNetCollision(SafetyNet* safetyNet, GameBall& ball, double seconds, Vector2D& n,
                                               Collision::LineSeg2D& collisionLine, double& timeUntilCollision, 
                                               Point2D& collisionPt, BallCollisionChangeInfo& collisionChangeInfo) {
    if (safetyNet == NULL || !safetyNet->BallCollisionCheck(ball, seconds, n, collisionLine, timeUntilCollision, collisionPt)) {
        return;
    }

    bool isBottomSafetyNet = (safetyNet == this->gameModel->bottomSafetyNet);
    bool ballBlastsThroughSafetyNet = safetyNet->BallBlastsThrough(ball);
    this->gameModel->DestroySafetyNet(safetyNet);
    safetyNet = NULL;

    // EVENT: The ball just destroyed the safety net
    GameEventManager::Instance()->ActionBallSafetyNetDestroyed(ball, isBottomSafetyNet);

    if (!ballBlastsThroughSafetyNet) {
        float ballMoveImpulse;
        this->DoBallCollision(ball, n, seconds, timeUntilCollision, GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS, ballMoveImpulse);
        collisionChangeInfo.posChanged = true;
    }

}

// Makes sure that the ball's direction is upwards and changes it to be upwards - when it gets
// hit by the paddle this is called.
bool BallInPlayState::AugmentBallDirectionToBeNotTooDownwards(GameBall& b, const PlayerPaddle& p, const Vector2D& collisionNormal) {

    // We are unforgiving if the collision normal was the paddle's bottom boundary normal OR
    // if the ball is below the paddle
    if (collisionNormal[1] == p.GetBottomCollisionNormal()[1] || p.IsBallUnderPaddle(b)) {
        return false;
    }

	Vector2D updatedBallDir = b.GetDirection();
    bool paddleFlipped = p.GetIsPaddleFlipped();

    if (paddleFlipped && updatedBallDir[1] > 0) {
        updatedBallDir[1] = -fabs(updatedBallDir[0] * tanf(GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_RADS));
        updatedBallDir.Normalize();
        b.SetDirection(updatedBallDir);
        return true;
    }
    else if (!paddleFlipped && updatedBallDir[1] < 0) {
        updatedBallDir[1] = fabs(updatedBallDir[0] * tanf(GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_RADS));
        updatedBallDir.Normalize();
        b.SetDirection(updatedBallDir);
        return true;
    }

    return false;
}
