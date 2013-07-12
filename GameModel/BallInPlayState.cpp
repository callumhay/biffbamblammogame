/**
 * BallInPlayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

struct BallCollisionChangeInfo {
public:
    BallCollisionChangeInfo() : posChanged(false), impulseApplied(false), impulseAmt(0), impulseDecel(0) {}
    BallCollisionChangeInfo(bool posChanged) : posChanged(posChanged), impulseApplied(false), impulseAmt(0), impulseDecel(0) {}
    BallCollisionChangeInfo(bool posChanged, bool impulseApplied, float impulseAmt, float impulseDecel) :
      posChanged(posChanged), impulseApplied(impulseApplied), impulseAmt(impulseAmt), impulseDecel(impulseDecel) {}
    BallCollisionChangeInfo(const BallCollisionChangeInfo& copy) : posChanged(copy.posChanged), 
        impulseApplied(copy.impulseApplied), impulseAmt(copy.impulseAmt), impulseDecel(copy.impulseDecel) {}
    ~BallCollisionChangeInfo(){}

    BallCollisionChangeInfo& operator=(const BallCollisionChangeInfo& copy) {
        this->posChanged = copy.posChanged;
        this->impulseApplied = copy.impulseApplied;
        this->impulseAmt = copy.impulseAmt;
        this->impulseDecel = copy.impulseDecel;
        return *this;
    }

    bool posChanged;
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
void BallInPlayState::BallReleaseKeyPressed() {

    // Always boost the ball when possible (if the player is indicating that they want to)
    assert(this->gameModel->boostModel != NULL);
    if (this->gameModel->boostModel->BallBoosterPressed()) {
        return;
    }

	// Check for paddle items that use the action key...
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	paddle->Shoot(this->gameModel);
}

void BallInPlayState::MoveKeyPressed(int dir, float magnitudePercent) {

    // We need to deal with moving the paddle, and if there's a remote control rocket we move the rocket instead...
    PaddleRemoteControlRocketProjectile* remoteControlRocket = this->gameModel->GetActiveRemoteControlRocket();
    if (remoteControlRocket != NULL) {
        remoteControlRocket->ControlRocketSteering(static_cast<PaddleRemoteControlRocketProjectile::RocketSteering>(dir), magnitudePercent);
    }

    // Default functionality (just apply movement to the paddle)
    GameState::MoveKeyPressed(dir, magnitudePercent);
}

void BallInPlayState::BallBoostDirectionPressed(int x, int y) {
    assert(this->gameModel->boostModel != NULL);

    // Ignore applying the boost direction when the ball is paused
    if ((this->gameModel->GetPauseState() & GameModel::PauseBall) == GameModel::PauseBall) {
        return;
    }

    if (this->gameModel->AreControlsFlipped()) {
        this->gameModel->boostModel->BallBoostDirectionPressed(-x, -y);
    }
    else {
        this->gameModel->boostModel->BallBoostDirectionPressed(x, y);
    }
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
	Vector2D n;
	Collision::LineSeg2D collisionLine;
	double timeUntilCollision;
	bool didCollideWithPaddle = false;
	bool didCollideWithBlock = false;
	bool didCollideWithTeslaLightning = false;
	
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
		    if (this->gameModel->IsOutOfGameBounds(currBall->GetBounds().Center())) {
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

			    didCollideWithPaddle = paddle->CollisionCheck(*currBall, seconds, n, collisionLine, timeUntilCollision);
			    if (didCollideWithPaddle) {
				    // The ball no longer has a last piece that it collided with - it gets reset when it hits the paddle
				    currBall->SetLastPieceCollidedWith(NULL);

				    // Don't do anything if this ball is the one attached to the paddle
				    if (paddle->GetAttachedBall() == currBall) {
					    continue;
				    }

				    // EVENT: Ball-paddle collision
				    GameEventManager::Instance()->ActionBallPaddleCollision(*currBall, *paddle);

				    // If the sticky paddle power-up is activated then the ball will simply be attached to
				    // the player paddle (if there are no balls already attached) ... unless the paddle has a shield active as well
				    if (paddle->HasPaddleType(PlayerPaddle::StickyPaddle) && !paddle->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
					    bool couldAttach = this->gameModel->GetPlayerPaddle()->AttachBall(currBall);
					    if (couldAttach) {
                            // Reset the multiplier
                            this->gameModel->SetNumInterimBlocksDestroyed(0);
						    continue;
					    }
				    }

				    // Do ball-paddle collision
				    this->DoBallCollision(*currBall, n, collisionLine, seconds, timeUntilCollision, 
                        GameBall::MIN_BALL_ANGLE_ON_PADDLE_HIT_IN_DEGS, paddle->GetVelocity(), true);
                    
                    BallCollisionChangeInfo& collisionInfo = ballChangedByCollision[ballIdx];                    
                    collisionInfo.posChanged     = true;
                    collisionInfo.impulseApplied = true;
                    collisionInfo.impulseAmt     = (GameBall::GetNormalSpeed() / currBall->GetSpeed()) * fabs(paddle->GetSpeed()) / 5.0f;
                    collisionInfo.impulseDecel   = collisionInfo.impulseAmt;

				    // Tell the model that a ball collision occurred with the paddle
				    this->gameModel->BallPaddleCollisionOccurred(*currBall);

				    if (!paddle->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
				        // Make sure the ball's velocity direction is not downward - it's annoying to hit the ball with a paddle and
				        // still see it fly into the void. If the shield is active then no help is provided
                        this->AugmentBallDirectionToBeNotTooDownwards(*currBall, *paddle, n);
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
			    if (currBall->CollisionCheck(*otherBall)) {
				    // EVENT: Two balls have collided
				    GameEventManager::Instance()->ActionBallBallCollision(*currBall, *otherBall);
				    this->DoBallCollision(*currBall, *otherBall);
			    }
		    }

		    // Ball Safety Net Collisions:
            if (this->gameModel->IsSafetyNetActive()) {
		        if (this->gameModel->safetyNet->BallCollisionCheck(*currBall, seconds, n, collisionLine, timeUntilCollision)) {
                    
                    this->gameModel->DestroySafetyNet();

                    // EVENT: The ball just destroyed the safety net
                    GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*currBall);

                    this->DoBallCollision(*currBall, n, collisionLine, seconds, timeUntilCollision, GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS);
                    ballChangedByCollision[ballIdx].posChanged = true;
		        }
            }

		    // Ball - tesla lightning arc collisions:
		    didCollideWithTeslaLightning = currLevel->TeslaLightningCollisionCheck(*currBall, seconds, n, collisionLine, timeUntilCollision);
		    if (didCollideWithTeslaLightning) {
			    // Clear the last piece collided with since the ball is now colliding with a tesla lightning arc
			    currBall->SetLastPieceCollidedWith(NULL);
    			
			    // Calculate the ball position/velocity after collision
			    this->DoBallCollision(*currBall, n, collisionLine, seconds, timeUntilCollision, 
                    GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS);
                ballChangedByCollision[ballIdx].posChanged = true;
    			
			    // Make for damn sure that the ball is no longer colliding!
			    while (currLevel->TeslaLightningCollisionCheck(*currBall, 0.0, n, collisionLine, timeUntilCollision)) {
				    this->DoBallCollision(*currBall, n, collisionLine, seconds, timeUntilCollision,
                        GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS);
			    }
		    }

		    // Make sure the ball can collide with level pieces (blocks) before running the block collision simulation
		    if (currBall->CanCollideWithBlocksAndBosses()) {

                // Now do boss collisions with the ball...
                if (currLevel->GetHasBoss()) {
                    Boss* boss = currLevel->GetBoss();
                    assert(boss != NULL);
                    
                    // Collide with the boss until there are no more collisions
                    BossBodyPart* collisionBossPart = NULL;
                    double currBallDt = seconds;
                    //while (currBallDt >= 0) {

                        // NOTE: For bosses, ghostballs never pass through.
                        // NOTE: For bosses we don't worry about issues with the paddle having a ball on it.
                        collisionBossPart = boss->CollisionCheck(*currBall, currBallDt, n, collisionLine, timeUntilCollision);
                        if (collisionBossPart != NULL) {
                            // Make the ball react to the collision...
                            this->DoBallCollision(*currBall, n, collisionLine, currBallDt, timeUntilCollision, 
                                GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS, collisionBossPart->GetCollisionVelocity());
                            ballChangedByCollision[ballIdx].posChanged = true;

                            // Now make the boss react to the collision...
                            this->gameModel->CollisionOccurred(*currBall, boss, collisionBossPart);

                            // Update the time delta for the ball, up to the time right after the collision
                            //currBallDt = currBallDt - timeUntilCollision;
                        }
                        else {
                            //break;
                        }
                    //}
                }

			    // Check for ball collision with level pieces
			    // Get the small set of level pieces based on the position of the ball...
			    std::vector<LevelPiece*> collisionPieces = 
                    currLevel->GetLevelPieceCollisionCandidates(seconds, currBall->GetBounds().Center(),
                    currBall->GetBounds().Radius(), currBall->GetSpeed());

			    for (std::vector<LevelPiece*>::iterator pieceIter = collisionPieces.begin(); 
                    pieceIter != collisionPieces.end(); ++pieceIter) {
    				
				    LevelPiece *currPiece = *pieceIter;
                    assert(currPiece != NULL);

                    didCollideWithBlock = currPiece->CollisionCheck(*currBall, seconds,
                        n, collisionLine, timeUntilCollision);

			        if (didCollideWithBlock) {
				        // Check to see if the ball is a ghost ball, if so there's a chance the ball will 
				        // lose its ability to collide for 1 second, also check to see if we're already in ghost mode
				        // if so we won't collide with anything (except solid blocks)...
				        if ((currBall->GetBallType() & GameBall::GhostBall) == GameBall::GhostBall && 
                             currPiece->GhostballPassesThrough()) {
    						
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
				        if (!currPiece->BallBouncesOffWhenHit() ||
                           (((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall) && 
                           currPiece->BallBlastsThrough(*currBall))) {
					        // Ignore collision...
				        }
				        else if (paddle->GetAttachedBall() == currBall) {
                            // Ignore this, we deal with it later on in DoUpdateToPaddleBoundriesAndCollisions
				        }
				        else {
					        // Make the ball react to the collision
					        this->DoBallCollision(*currBall, n, collisionLine, seconds, timeUntilCollision,
                                GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS);
                            ballChangedByCollision[ballIdx].posChanged = true;
				        }
    					
                        bool currPieceCanChangeSelfOrPiecesAroundIt = 
                            currPiece->CanChangeSelfOrOtherPiecesWhenHitByBall() || currPiece->CanBeDestroyedByBall();

				        // Tell the model that a ball collision occurred with currPiece
                        this->gameModel->CollisionOccurred(*currBall, currPiece);
                        
                        // Recalculate the blocks we're dealing with in this loop since 
                        // some blocks may no longer exist after a collision - of course this would only happen if the piece
                        // that collided could be destroyed during this loop by the ball
                        if (currPieceCanChangeSelfOrPiecesAroundIt) {
			                collisionPieces = currLevel->GetLevelPieceCollisionCandidates(seconds, currBall->GetBounds().Center(), 
                                std::max<float>(GameBall::DEFAULT_BALL_RADIUS, currBall->GetBounds().Radius()), currBall->GetSpeed());
                            pieceIter = collisionPieces.begin();
                        }
                    }
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
            if (currCollisionInfo.impulseApplied) {
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
	if (!paddle->HasBeenPausedAndRemovedFromGame(this->gameModel->GetPauseState()) && this->gameModel->IsSafetyNetActive()) {   
        if (this->gameModel->safetyNet->PaddleCollisionCheck(*paddle)) {
            this->gameModel->DestroySafetyNet();
            
            // EVENT: The paddle just destroyed the safety net
            GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*paddle);
        }
    }

	// Paddle-block collisions / boundry update (so that the paddle crashes into potential blocks at its sides).
    this->DoUpdateToPaddleBoundriesAndCollisions(seconds, true);
	
    // Tick/update any level pieces that require it...
	this->gameModel->DoPieceStatusUpdates(seconds);

    // Update the boost model for the ball(s)
    this->gameModel->boostModel->Tick(seconds);
}

// n must be normalized
// d is the distance from the center of the ball to the line that was collided with
// when d is negative the ball is inside the line, when positive it is outside
void BallInPlayState::DoBallCollision(GameBall& b, const Vector2D& n, 
                                      Collision::LineSeg2D& collisionLine, 
                                      double dT, double timeUntilCollision, 
                                      float minAngleInDegs, const Vector2D& lineVelocity,
                                      bool paddleReflection) {
	b.BallCollided();

	// Calculate the time of collision and then the difference up to this point
	// based on the velocity and then move it to that position...
	double timeToMoveInReflectionDir = std::max<double>(0.0, dT - timeUntilCollision);

	// Make sure the ball is on the correct side of the collision line (i.e., the side that the normal is pointing in)
	Vector2D fromLineToBall = b.GetCenterPosition2D() - collisionLine.P1();
	if (fromLineToBall == Vector2D(0, 0)) {
		fromLineToBall = b.GetCenterPosition2D() - collisionLine.P2();
		assert(fromLineToBall != Vector2D(0, 0));
	}

	// Make sure that the direction of the ball is against that of the normal, otherwise we adjust it to be so
	Vector2D reflVecHat;
    bool ignoreSmallReflectionAngle = false;

    // Position the ball so that it is at the location it was at when it collided...
    b.SetCenterPosition(b.GetCenterPosition2D() + timeUntilCollision * b.GetVelocity());

	if (Vector2D::Dot(b.GetDirection(), n) >= 0) {
		// Somehow the ball is traveling away from the normal but is hitting the line...

        // The ball will reflect off its own direction vector...
        reflVecHat = b.GetDirection();
        ignoreSmallReflectionAngle = true; // We don't want the ball to be effected by the MIN_BALL_ANGLE_*
	}
	else {
        // Typical bounce off the normal: figure out the reflection vector
        reflVecHat = Vector2D::Normalize(Reflect(b.GetDirection(), n));

        // In the case of gravity we decrease the reflection
        if ((b.GetBallType() & GameBall::GraviBall) == GameBall::GraviBall) {
            reflVecHat = Vector2D::Normalize(0.5f * (reflVecHat + n));
        }
	}

	float reflSpd = b.GetSpeed();
	
	// This should NEVER happen
	if (reflSpd == GameBall::GetZeroSpeed()) {
		assert(false);
		return;
	}

    if (!ignoreSmallReflectionAngle && !paddleReflection) {
        // Figure out the angle between the normal and the reflection...
        float angleOfReflInDegs = Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(n, reflVecHat)))));
        float diffAngleInDegs   = minAngleInDegs - fabs(angleOfReflInDegs);

        // Make sure the reflection is big enough to not cause an annoying slow down in the game
        // or to make a ridiculous gracing angle
        if (diffAngleInDegs > EPSILON) {

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
	        // Do the same with 90 degrees to the normal
	        Vector2D tangentVec(n[1], -n[0]);
	        float gracingAngle = Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(tangentVec, reflVecHat)))));
	        if (gracingAngle > 90) {
		        diffAngleInDegs = minAngleInDegs - fabs(gracingAngle - 180);
	        }
	        else {
		        diffAngleInDegs = minAngleInDegs - fabs(gracingAngle);
	        }

	        if (diffAngleInDegs > EPSILON) {
		        // We need to figure out which way to rotate the velocity
		        // to ensure it's at least the MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_RADS
		        Vector2D newReflVelHat = Vector2D::Rotate(diffAngleInDegs, reflVecHat);

		        // Check to see if it's still the case, if it is then we rotated the wrong way
		        float newAngleInDegs =  Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(tangentVec, newReflVelHat)))));
		        if (newAngleInDegs < minAngleInDegs) {
			        newReflVelHat = Vector2D::Rotate(-diffAngleInDegs, reflVecHat);
		        }

		        reflVecHat = newReflVelHat;
	        }
        }
    }

    // Reflect the ball off the normal... this will have some dependence on whether there is a velocity for the line...
    Vector2D moveVel(0,0);

    if (!lineVelocity.IsZero()) {

        Vector2D reflectionVel = reflSpd * reflVecHat;
        Vector2D augmentedReflectionVecHat = Vector2D::Normalize(reflectionVel + lineVelocity);
        
        if (Vector2D::Dot(reflVecHat, lineVelocity) < 0) {
            moveVel += lineVelocity;
        }
        reflVecHat = augmentedReflectionVecHat;
    }
    
    if (paddleReflection) {
        // Only have to check whether the ball is acceptably far enough away from the +/-x directions
        float angleAwayFromX =  Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(Vector2D(1,0), reflVecHat)))));
        debug_output("ANGLE FROM X: " << angleAwayFromX);
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
void BallInPlayState::DoBallCollision(GameBall& ball1, GameBall& ball2) {
	ball1.BallCollided();
	ball2.BallCollided();

	Collision::Circle2D ball1Bounds = ball1.GetBounds();
	Collision::Circle2D ball2Bounds = ball2.GetBounds();
	
	Vector2D s = ball2Bounds.Center() - ball1Bounds.Center();
	Vector2D v = ball2.GetVelocity() - ball1.GetVelocity();
	float r = ball2Bounds.Radius() + ball1Bounds.Radius();
	float c = Vector2D::Dot(s, s) - (r * r);

	assert(c < 0.0f);	// It should always be the case that there is a collision
	
	float a = Vector2D::Dot(v, v);
	if (a < EPSILON) { 
		return; 
	}

	float b = Vector2D::Dot(v, s);
	float d = b * b - a * c;
	if (d < 0.0f) { 
		return; 
	}

	float t = (-b - sqrt(d)) / a;
	assert(t <= 0.0f);	// Since the collision has already happened, t must be negative

	// Figure out what the centers of the two balls were at collision
	Point2D ball1CollisionCenter = ball1Bounds.Center() + t * ball1.GetVelocity();
	Point2D ball2CollisionCenter = ball2Bounds.Center() + t * ball2.GetVelocity();

	// Figure out the new velocity after the collision
	Vector2D ball1CorrectionVec = ball1CollisionCenter - ball2CollisionCenter;
	if (ball1CorrectionVec == Vector2D(0,0)) {
		return;
	}
	ball1CorrectionVec.Normalize();
	Vector2D ball2CorrectionVec = -ball1CorrectionVec;

	Vector2D reflectBall1Vec = Vector2D::Normalize(Reflect(ball1.GetDirection(), ball1CorrectionVec));
	Vector2D reflectBall2Vec = Vector2D::Normalize(Reflect(ball2.GetDirection(), ball2CorrectionVec));
	
	// Check to see how close the two velocities are to one another, if they're within
	// some small threshold then make them dramatically different...
	static const float CLOSE_TRAGECTORY_ANGLE = Trig::degreesToRadians(5.0f);
	if (fabs(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(reflectBall1Vec, reflectBall2Vec))))) < CLOSE_TRAGECTORY_ANGLE) {
		// Rotate them both 90 degrees away from each other
		reflectBall1Vec = Vector2D(reflectBall1Vec[1], -reflectBall1Vec[0]);
		reflectBall2Vec = Vector2D(-reflectBall2Vec[1], reflectBall2Vec[0]);
	}

	//float positiveT = -NumberFuncs::SignOf(t) * (fabs(t) + 2*EPSILON);

	// NOTE: we need to make sure the new velocities are not zero
	// Set the new velocities
	if (reflectBall1Vec != Vector2D(0, 0)) {
		ball1.SetVelocity(ball1.GetSpeed(), reflectBall1Vec);
	}

	if (reflectBall2Vec != Vector2D(0, 0)) {
		ball2.SetVelocity(ball2.GetSpeed(), reflectBall2Vec);
	}

    ball1.SetBallBallCollisionsDisabled(std::min<double>(0.2, ball1.GetBounds().Radius() / ball1.GetSpeed()));
    ball2.SetBallBallCollisionsDisabled(std::min<double>(0.2, ball2.GetBounds().Radius() / ball2.GetSpeed()));
}

/**
 * Private helper function for checking to see if an item collided with 
 * the player paddle and carrying out the necessary activity if one did.
 */
void BallInPlayState::DoItemCollision() {

    PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();

    // Ignore item collisions if the paddle has been removed from play
    if (paddle->HasBeenPausedAndRemovedFromGame(this->gameModel->GetPauseState())) {
        return;
    }

	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::list<GameItem*>& currLiveItems     = this->gameModel->GetLiveItems();
	std::vector<GameItem*> removeItems;

	// Check to see if the paddle hit any items, if so, activate those items
	for (std::list<GameItem*>::iterator iter = currLiveItems.begin(); iter != currLiveItems.end(); ++iter) {
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

// Makes sure that the ball's direction is upwards and changes it to be upwards - when it gets
// hit by the paddle this is called.
void BallInPlayState::AugmentBallDirectionToBeNotTooDownwards(GameBall& b, const PlayerPaddle& p, const Vector2D& collisionNormal) {
    // We are unforgiving if the collision normal was pointing very downwards as well
    if (collisionNormal[1] < -0.9f) {
        return;
    }
    // We are also unforgiving if the ball is below the paddle...
    if (b.GetCenterPosition2D()[1] < (p.GetCenterPosition()[1] - p.GetHalfHeight() - 0.9f*b.GetBounds().Radius())) {
        return;
    }

	Vector2D ballDirection = b.GetDirection();

	if (ballDirection[1] < 0) {
		ballDirection[1] = fabs(ballDirection[0] * tanf(GameBall::MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_RADS));
		ballDirection.Normalize();
		b.SetDirection(ballDirection);
	}
}