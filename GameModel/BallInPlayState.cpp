/**
 * BallInPlayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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

void BallInPlayState::BallBoostDirectionPressed(int x, int y) {
    assert(this->gameModel->boostModel != NULL);
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
	double timeSinceCollision;
	bool didCollideWithPaddle = false;
	bool didCollideWithBlock = false;
	bool didCollideWithTeslaLightning = false;
	bool didCollideWithballSafetyNet  = false;
	
	GameBall* ballToMoveToFront = NULL;																	// The last ball to hit the paddle is the one with priority for item effects
	std::list<std::list<GameBall*>::iterator> ballsToRemove;						// The balls that are no longer in play and will be removed
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();	// The current set of balls in play

#ifdef _DEBUG
	// Pause the ball from moving
	if ((this->gameModel->GetPauseState() & GameModel::PauseBall) == NULL) {
#endif

	// Gravity vector in world space
	Vector3D worldGravityDir = this->gameModel->GetTransformInfo()->GetGameTransform() * Vector3D(0, -1, 0);
	worldGravityDir.Normalize();
	Vector2D worldGravity2D(worldGravityDir[0], worldGravityDir[1]);

	for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
		GameBall *currBall = *iter;

		// Update the current ball
		//currBall->Tick(seconds, worldGravity2D);

		// Check for death (ball went out of bounds)
		if (this->gameModel->IsOutOfGameBounds(currBall->GetBounds().Center())) {
			if (gameBalls.size() == 1) {
                this->gameModel->SetNextState(new BallDeathState(currBall, this->gameModel));
				return;
			}
			else {
				// The ball is now dead and needs to be removed from the game
				ballsToRemove.push_back(iter);
				continue;
			}
		}

		// Check to see if the ball is allowed to collide with paddles, if so run the ball-paddle
		// collision simulation
		if (currBall->CanCollideWithPaddles()) {

			// Check for ball collision with the player's paddle
			didCollideWithPaddle = paddle->CollisionCheck(*currBall, seconds, n, collisionLine, timeSinceCollision);
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
				if ((paddle->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle &&
					(paddle->GetPaddleType() & PlayerPaddle::ShieldPaddle) == 0x0) {
					bool couldAttach = this->gameModel->GetPlayerPaddle()->AttachBall(currBall);
					if (couldAttach) {
                        // Reset the multiplier
                        this->gameModel->SetNumInterimBlocksDestroyed(0);
						continue;
					}
				}

                // Grab a point amount based on the collision of the ball with the paddle - 
                // only occurs if the paddle is not shielded or sticky
                // NOTE: Be sure to call this before telling the model about the collision occuring, we
                // want the score to be multiplied by any multiplier that exists before being reset by the collision!
                //std::list<PointAward> pointPairsList = paddle->GetPointsForHittingBall(*currBall);
                //this->gameModel->IncrementScore(pointPairsList);

				// Do ball-paddle collision
				this->DoBallCollision(*currBall, n, collisionLine, seconds, timeSinceCollision);
				// Tell the model that a ball collision occurred with the paddle
				this->gameModel->BallPaddleCollisionOccurred(*currBall);

                int count = 0;
				if ((paddle->GetPaddleType() & PlayerPaddle::ShieldPaddle) != PlayerPaddle::ShieldPaddle) {
				    // Make sure the ball's velocity direction is not downward - it's annoying to hit the ball with a paddle and
				    // still see it fly into the void - of course, if the shield is active then no help is provided
                    this->AugmentBallDirectionToBeNotDownwards(*currBall);
				}
                else {
                    // The shield paddle has some wonky collision detection so fix it so that it doesn't
                    // iterate for too long
                    count = 4;
                }

				// If there are multiple balls and the one that just hit the paddle is not
				// the first one in the list, then we need to move this one to the front
				if (gameBalls.size() > 1 && currBall != (*gameBalls.begin())) {
					ballToMoveToFront = currBall;
				}

                // Check to see whether the ball is hitting both a wall AND the paddle - if so the paddle should
                // be repelled by the opposing force of the wall against the ball...
                if (!paddle->UpdateForOpposingForceBallCollision(*currBall, seconds)) {
				    // Make sure that the ball is no longer colliding!
			        static Vector2D tempN;
			        static Collision::LineSeg2D tempLine;
			        static double tempTime;
			        while (paddle->CollisionCheck(*currBall, 0.0, tempN, tempLine, tempTime) && count < 5) {
				        currBall->SetCenterPosition(currBall->GetCenterPosition2D() + currBall->GetBounds().Radius() * n);
                        count++;
			        }
                }
			}
		}

		// Make sure the ball can collide with level pieces (blocks) before running the block collision simulation
		if (currBall->CanCollideWithBlocks()) {
			// Check for ball collision with level pieces
			// Get the small set of levelpieces based on the position of the ball...
			std::set<LevelPiece*> collisionPieces = 
                currLevel->GetLevelPieceCollisionCandidates(currBall->GetBounds().Center(), 
                                                            currBall->GetBounds().Radius());

			for (std::set<LevelPiece*>::iterator pieceIter = collisionPieces.begin(); 
                pieceIter != collisionPieces.end(); ++pieceIter) {
				
				LevelPiece *currPiece = *pieceIter;
                
				didCollideWithBlock = currPiece->CollisionCheck(*currBall, seconds, n, collisionLine, timeSinceCollision);
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

					// If the piece doesnt have bounds to bounce off of then don't bounce the ball OR
					// In the case that the ball is uber then we only reflect if the ball is not green OR
					// The ball is attached to the paddle (special case)
					if (!currPiece->BallBouncesOffWhenHit() ||
                       (((currBall->GetBallType() & GameBall::UberBall) == GameBall::UberBall) && 
                       currPiece->BallBlastsThrough(*currBall))) {
						// Ignore collision...
					}
					else if (paddle->GetAttachedBall() == currBall) {
						// Ignore the collison FOR THE BALL ONLY - the paddle should recoil since the ball attached to it just
						// hit a block and balls tend to be bouncy in the bbb universe
						
						float xRecoilDir = n[0];
						// Figure out the recoil direction by projecting the normal of the collision onto the x-axis
						if (fabs(xRecoilDir) < EPSILON) {
							// Annoying case: seems, somehow, that the ball collided with a side that is completely parallel to the x-axis

							// First see what the vector from the center of the collided block to the ball is and use it's x-component...
							xRecoilDir = (currBall->GetCenterPosition2D() - currPiece->GetCenter())[0];
							if (fabs(xRecoilDir) < EPSILON) {
								// Really annoying case: somehow the ball is almost at the center line of the block, wtf??, anyway, just pick the opposite
								// of the paddle moving direction
								xRecoilDir = paddle->GetLastMovingDirection();

								if (fabs(xRecoilDir) < EPSILON) {
									// This should just never happen, but in case it somehow does, pick a freaking random number (-1 or 1), geez
									xRecoilDir = Randomizer::GetInstance()->RandomNegativeOrPositive();
								}
							}
						}
						
						// Make sure it's normalized (just use signof function to get either -1 or 1)
						xRecoilDir = NumberFuncs::SignOf(xRecoilDir);
						// Now we need to apply an impulse to the paddle in the direction of the recoil...
                        paddle->ApplyImpulseForce(std::max<float>(PlayerPaddle::DEFAULT_MAX_SPEED/2.0f * xRecoilDir, fabs(paddle->GetSpeed()) * xRecoilDir));
					}
					else {
						// Make the ball react to the collision
						this->DoBallCollision(*currBall, n, collisionLine, seconds, timeSinceCollision);
					}
					
                    bool currPieceIsBreakableByBall = currPiece->CanBeDestroyedByBall();
					// Tell the model that a ball collision occurred with currPiece
                    this->gameModel->CollisionOccurred(*currBall, currPiece);
                    
                    // Recalculate the blocks we're dealing with in this loop since 
                    // some blocks may no longer exist after a collision - of course this would only happen if the piece
                    // that collided could be destroyed during this loop by the ball
                    if (currPieceIsBreakableByBall) {
				        collisionPieces = currLevel->GetLevelPieceCollisionCandidates(currBall->GetBounds().Center(), 
                                                                                      currBall->GetBounds().Radius());
                        pieceIter = collisionPieces.begin();
                    }
                }
			}
		}

		// Ball Safety Net Collisions:
		didCollideWithballSafetyNet = currLevel->BallSafetyNetCollisionCheck(*currBall, seconds, n, collisionLine, timeSinceCollision);
		if (didCollideWithballSafetyNet) {
			this->DoBallCollision(*currBall, n, collisionLine, seconds, timeSinceCollision);
		}

		// Ball - tesla lightning arc collisions:
		didCollideWithTeslaLightning = currLevel->TeslaLightningCollisionCheck(*currBall, seconds, n, collisionLine, timeSinceCollision);
		if (didCollideWithTeslaLightning) {
			// Clear the last piece collided with since the ball is now colliding with a tesla lightning arc
			currBall->SetLastPieceCollidedWith(NULL);
			
			// Calculate the ball position/velocity after collision
			this->DoBallCollision(*currBall, n, collisionLine, seconds, timeSinceCollision);
			
			// Make for damn sure that the ball is no longer colliding!
			Vector2D tempN;
			Collision::LineSeg2D tempLine;
			double tempTime;
			while (currLevel->TeslaLightningCollisionCheck(*currBall, 0.0, tempN, tempLine, tempTime)) {
				currBall->SetCenterPosition(currBall->GetCenterPosition2D() + currBall->GetBounds().Radius() * n);
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
	}
	
	// Get rid of all the balls that went out of bounds / are now dead
	for (std::list<std::list<GameBall*>::iterator>::iterator iter = ballsToRemove.begin(); iter != ballsToRemove.end(); ++iter) {
		GameBall* ballToDestroy = (**iter);

		// EVENT: Ball died
		GameEventManager::Instance()->ActionBallDied(*ballToDestroy);

		// SPECIAL CASE:
		// We need to be very careful - if the player is in ball camera mode but there are multiple balls and the one
		// that the camera is in just died then we need to deactivate ball camera mode but keep the player alive!
		if (ballToDestroy == GameBall::GetBallCameraBall()) {
			// Deactivate the ball camera item!!!
			bool wasRemoved = this->gameModel->RemoveActiveGameItemsOfGivenType(GameItem::BallCamItem);
			assert(wasRemoved);
		}

        this->gameModel->RemoveActiveGameItemsForThisBallOnly(ballToDestroy);

		gameBalls.erase(*iter);
		delete ballToDestroy;
		ballToDestroy = NULL;
	}

	// Move the last ball that hit the paddle to the front of the list of balls
	if (ballToMoveToFront != NULL) {
		gameBalls.remove(ballToMoveToFront);
		gameBalls.push_front(ballToMoveToFront);
	}

	// Tick the balls
	for (std::list<GameBall*>::iterator iter = gameBalls.begin(); iter != gameBalls.end(); ++iter) {
		GameBall *currBall = *iter;
		// Update the current ball
        currBall->Tick(seconds, worldGravity2D, this->gameModel);
	}

#ifdef _DEBUG
	} // Pause ball
#endif

	// Quick check to see if the paddle collided with the safety net - this will just register necessary
	// events and destroy the net if it exists and there is a collision
	currLevel->PaddleSafetyNetCollisionCheck(*paddle);

	// Paddle-block collisions / boundry update (so that the paddle crashes into potential blocks at its sides):
	// This may cause the level to end since the paddle shield can destroy blocks, in such a case we exit immediately
	// since this state is now destroyed.
	//bool stateChanged = 
    this->DoUpdateToPaddleBoundriesAndCollisions(seconds, false);
	//if (stateChanged) {
	//	return;
	//}

	// Projectile Collisions:
	this->gameModel->DoProjectileCollisions();
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
                                      double dT, double timeSinceCollision) {
	b.BallCollided();

	// Calculate the time of collision and then the difference up to this point
	// based on the velocity and then move it to that position...
	double timeToMoveInReflectionDir = std::max<double>(0.0, dT - timeSinceCollision);

	// Make sure the ball is on the correct side of the collision line (i.e., the side that the normal is pointing in)
	Vector2D fromLineToBall = b.GetCenterPosition2D() - collisionLine.P1();
	if (fromLineToBall == Vector2D(0, 0)) {
		fromLineToBall = b.GetCenterPosition2D() - collisionLine.P2();
		assert(fromLineToBall != Vector2D(0, 0));
	}

	// Make sure that the direction of the ball is against that of the normal, otherwise we adjust it to be so
	Vector2D reflVecHat;
	if (Vector2D::Dot(b.GetDirection(), n) >= 0) {
		// Somehow the ball is travelling away from the normal but is hitting the line, keep its direction
		reflVecHat = b.GetDirection();
	}
	else {
		// Position the ball so that it is at the location it was at when it collided...
		const float BALL_RADIUS  = b.GetBounds().Radius();
		const float BALL_EPSILON = 0.001f * BALL_RADIUS;
		b.SetCenterPosition(b.GetCenterPosition2D() + (BALL_EPSILON + timeSinceCollision) * -b.GetVelocity());

		// Typical bounce off the normal: figure out the reflection vector
		reflVecHat	= Vector2D::Normalize(Reflect(b.GetDirection(), n));
	}

	float reflSpd = b.GetSpeed();
	
	// This should NEVER happen
	if (reflSpd == GameBall::ZeroSpeed) {
		assert(false);
		return;
	}

	// Figure out the angle between the normal and the reflection...
	float angleOfReflInDegs = Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(n, reflVecHat)))));
	float diffAngleInDegs		= GameBall::MIN_BALL_ANGLE_IN_DEGS - fabs(angleOfReflInDegs);

	// Make sure the reflection is big enough to not cause an annoying slow down in the game
	// or to make a ridiculous gracing angle
	if (diffAngleInDegs > EPSILON) {

		// We need to figure out which way to rotate the velocity
		// to ensure it's at least the MIN_BALL_ANGLE_IN_RADS
		Vector2D newReflVelHat = Vector2D::Rotate(diffAngleInDegs, reflVecHat);

		// Check to see if it's still the case, if it is then we rotated the wrong way
		float newAngleInDegs =  Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(n, newReflVelHat)))));
		if (newAngleInDegs < GameBall::MIN_BALL_ANGLE_IN_DEGS) {
			newReflVelHat = Vector2D::Rotate(-diffAngleInDegs, reflVecHat);
		}
		
		reflVecHat = newReflVelHat;
	}
	else {
		// Do the same with 90 degrees to the normal
		Vector2D tangentVec(n[1], -n[0]);
		float gracingAngle = Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(tangentVec, reflVecHat)))));
		if (gracingAngle > 90) {
			diffAngleInDegs = GameBall::MIN_BALL_ANGLE_IN_DEGS - fabs(gracingAngle - 180);
		}
		else {
			diffAngleInDegs = GameBall::MIN_BALL_ANGLE_IN_DEGS - fabs(gracingAngle);
		}

		if (diffAngleInDegs > EPSILON) {
			// We need to figure out which way to rotate the velocity
			// to ensure it's at least the MIN_BALL_ANGLE_IN_RADS
			Vector2D newReflVelHat = Vector2D::Rotate(diffAngleInDegs, reflVecHat);

			// Check to see if it's still the case, if it is then we rotated the wrong way
			float newAngleInDegs =  Trig::radiansToDegrees(acosf(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(tangentVec, newReflVelHat)))));
			if (newAngleInDegs < GameBall::MIN_BALL_ANGLE_IN_DEGS) {
				newReflVelHat = Vector2D::Rotate(-diffAngleInDegs, reflVecHat);
			}

			reflVecHat = newReflVelHat;
		}
	}

	// Reflect the ball off the normal
	b.SetVelocity(reflSpd, reflVecHat);

	// Now move the ball in that direction over how ever much time was lost during the collision
	b.SetCenterPosition(b.GetCenterPosition2D() + timeToMoveInReflectionDir * b.GetVelocity());
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

	float positiveT = -NumberFuncs::SignOf(t) * (fabs(t) + 2*EPSILON);

	// NOTE: we need to make sure the new velocities are not zero

	// Set the new velocities and the correct center positions so that they 
	// still remain at the current time, but it is as if the balls had originally collided
	if (reflectBall1Vec != Vector2D(0, 0)) {
		ball1.SetVelocity(ball1.GetSpeed(), reflectBall1Vec);
		ball1.SetCenterPosition(ball1CollisionCenter + positiveT * reflectBall1Vec);
	}

	if (reflectBall2Vec != Vector2D(0, 0)) {
		ball2.SetVelocity(ball2.GetSpeed(), reflectBall2Vec);
		ball2.SetCenterPosition(ball2CollisionCenter + positiveT * reflectBall2Vec);
	}	
}

/**
 * Private helper function for checking to see if an item collided with 
 * the player paddle and carrying out the necessary activity if one did.
 */
void BallInPlayState::DoItemCollision() {
	std::list<GameItemTimer*>& activeTimers = this->gameModel->GetActiveTimers();
	std::list<GameItem*>& currLiveItems     = this->gameModel->GetLiveItems();
	std::vector<GameItem*> removeItems;

	// Check to see if the paddle hit any items, if so, activate those items
	for (std::list<GameItem*>::iterator iter = currLiveItems.begin(); iter != currLiveItems.end(); ++iter) {
		GameItem *currItem = *iter;
		
		if (currItem->CollisionCheck(*this->gameModel->GetPlayerPaddle())) {
			// EVENT: Item was obtained by the player paddle
			GameEventManager::Instance()->ActionItemPaddleCollision(*currItem, *this->gameModel->GetPlayerPaddle());
            
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

