#include "BallInPlayState.h"
#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "GameModel.h"
#include "GameEventManager.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"
#include "../Utils/Point.h"

const float BallInPlayState::MIN_BALL_ANGLE_IN_DEGS = 20.0f;
const float BallInPlayState::MIN_BALL_ANGLE_IN_RADS = Trig::degreesToRadians(MIN_BALL_ANGLE_IN_DEGS);

BallInPlayState::BallInPlayState(GameModel* gm) : GameState(gm) {
}

BallInPlayState::~BallInPlayState() {
}

/**
 * Since the ball is now in play this function will be doing A LOT, including:
 * - updating the ball's location and checking for collisions/death
 * - ... TBA
 */
void BallInPlayState::Tick(double seconds) {
	// The paddle can move while the ball is in play, of course
	PlayerPaddle& paddle = this->gameModel->playerPaddle;
	paddle.Tick(seconds);

	// Update the ball's current position
	GameBall& ball = this->gameModel->ball;
	ball.Tick(seconds);

	// Check for ball collisions with the pieces of the level and the paddle
	std::vector<std::vector<LevelPiece*>> &levelPieces = this->gameModel->GetCurrentLevelPieces();
	
	// First check for ball-paddle collision
	Vector2D n;
	float d;
	bool didCollide = paddle.CollisionCheck(ball.GetBounds(), n, d);

	if (didCollide) {
		// Do ball-paddle collision
		this->DoBallCollision(ball, n, d);

		// Modify the ball using the current paddle's movement
		this->ModifyBallBasedOnPaddle(paddle, ball);

		// EVENT: Ball-paddle collision
		GameEventManager::Instance()->ActionBallPaddleCollision(ball, paddle);
	}
	else {
		// There was no paddle collision check for block collisions
		for (size_t h = 0; h < levelPieces.size(); h++) {
			for (size_t w = 0; w < levelPieces[h].size(); w++) {
				
				LevelPiece* currPiece = levelPieces[h][w];
				didCollide = currPiece->CollisionCheck(ball.GetBounds(), n, d);
				if (didCollide) {
					this->DoBallCollision(ball, n, d);
					
					// Tell the model that a ball collision occurred with currPiece
					this->gameModel->BallCollisionOccurred(currPiece);
					
					// TODO: move this into the model
					this->CheckForEndOfLevelAndWorldAndGame();

					break;
				}
			}
			// Get out of the loop in case of a collision
			if (didCollide) { 
				break;
			}
		}
	
		// Check for death
		if (ball.GetBounds().Center()[1] <= GameLevel::Y_COORD_OF_DEATH) {
			// EVENT: Ball death
			GameEventManager::Instance()->ActionBallDeath(ball, 0);	// TODO: fix lives left...
			
			// TODO: Have a death state?
			this->gameModel->SetCurrentState(new BallOnPaddleState(this->gameModel));
		}
	}

}

// Checks to see if the game/world/level have ended, and does necessary stuffs
// to change things up
void BallInPlayState::CheckForEndOfLevelAndWorldAndGame() {
	GameLevel* currLevel = this->gameModel->GetCurrentWorld()->GetCurrentLevel();
	
	// Check to see if the level is done
	if (currLevel->IsLevelComplete()) {
		// EVENT: Level is complete
		GameEventManager::Instance()->ActionLevelCompleted(*currLevel);
		
		// Check to see if the world is done
		GameWorld* currWorld = this->gameModel->GetCurrentWorld();
		if (currWorld->IsLastLevel()) {
			// EVENT: World is complete
			GameEventManager::Instance()->ActionWorldCompleted(*currWorld);

			// Check to see if the game is done
			if (this->gameModel->IsOnLastLevelOfGame()) {
				// EVENT: Game is complete
				GameEventManager::Instance()->ActionGameCompleted();
			}
		}

		// The level was completed, increment the level - note this function
		// is robust and will increment level, world and determine if we have finished
		// the game all in one
		this->gameModel->IncrementLevel();
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
			b.SetCenterPosition(b.GetBounds().Center() + (-d + b.GetBounds().Radius()) * n);
		}
		else {
			// The ball is still on the out facing normal side of the line, but fix its position so that
			// it is exactly against the line
			b.SetCenterPosition(b.GetBounds().Center() + (b.GetBounds().Radius() - d) * n);
		}
	}

	// Reflect the ball off the normal
	b.SetVelocity(Reflect(b.GetVelocity(), n));
}

void BallInPlayState::ModifyBallBasedOnPaddle(const PlayerPaddle& paddle, GameBall& ball) {
	// Modify the ball velocity using the current paddle velocity
	Vector2D paddleVel = paddle.GetAvgVelocity();
	
	if (fabs(paddleVel[0]) > EPSILON) {
		// The paddle has a considerable velocity, we should augment the ball's
		// tragectory based on this (transfer of momentum)
		int angleDecSgn = -NumberFuncs::SignOf(paddleVel[0]);
		float fractionOfSpeed = fabs(paddleVel[0]) / PlayerPaddle::DEFAULT_SPEED;
		float angleChange = angleDecSgn * fractionOfSpeed * PlayerPaddle::RAND_DEG_ANG;

		// Set the ball to be exactly on the paddle, then rotate its velocity vector
		// to reflect the momentum of the paddle
		ball.SetCenterPosition(Point2D(ball.GetBounds().Center()[0], paddle.GetCenterPosition()[1] + paddle.GetHalfHeight() + ball.GetBounds().Radius()));
		ball.SetVelocity(Rotate(angleChange, ball.GetVelocity()));
		
		// Make sure the ball goes upwards (it can't reflect downwards off the paddle or the game would suck)
		Vector2D ballVel    = ball.GetVelocity();
		Vector2D ballVelHat = Vector2D::Normalize(ballVel);
		if (acosf(Vector2D::Dot(ballVelHat, Vector2D(0, 1))) > ((M_PI / 2.0f) - MIN_BALL_ANGLE_IN_RADS)) {
			// Inline: The ball either at a very sharp angle w.r.t. the paddle or it is aimed downwards
			// even though the paddle has deflected it, adjust the angle to be suitable for the game
			float ballMag = Vector2D::Magnitude(ballVel);
			if (ballVel[0] < 0) {
				Vector2D temp = Vector2D(-ballMag, 0);
				ball.SetVelocity(Rotate(-MIN_BALL_ANGLE_IN_DEGS, temp));
			}
			else {
				Vector2D temp = Vector2D(ballMag, 0);
				ball.SetVelocity(Rotate(MIN_BALL_ANGLE_IN_DEGS, temp));
			}
		}
	}
}