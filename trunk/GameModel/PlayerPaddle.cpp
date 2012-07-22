/**
 * PlayerPaddle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PlayerPaddle.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "GameModelConstants.h"
#include "PaddleLaserProjectile.h"
#include "PaddleMineProjectile.h"
#include "Beam.h"
#include "FireGlobProjectile.h"

// Default values for the size of the paddle
const float PlayerPaddle::PADDLE_WIDTH_TOTAL = 3.5f;
const float PlayerPaddle::PADDLE_WIDTH_FLAT_TOP = 2.0f;
const float PlayerPaddle::PADDLE_WIDTH_ANGLED_SIDE = (PADDLE_WIDTH_TOTAL - PADDLE_WIDTH_FLAT_TOP) / 2.0f;
const float PlayerPaddle::PADDLE_HEIGHT_TOTAL = 0.80f;
const float PlayerPaddle::PADDLE_DEPTH_TOTAL = 1.0f;
const float PlayerPaddle::PADDLE_HALF_WIDTH = PADDLE_WIDTH_TOTAL / 2.0f;
const float PlayerPaddle::PADDLE_HALF_HEIGHT = PADDLE_HEIGHT_TOTAL / 2.0f;
const float PlayerPaddle::PADDLE_HALF_DEPTH = PADDLE_DEPTH_TOTAL / 2.0f;

const Vector2D PlayerPaddle::DEFAULT_PADDLE_UP_VECTOR(0, 1);
const Vector2D PlayerPaddle::DEFAULT_PADDLE_RIGHT_VECTOR(1, 0);

// The difference in width per size change of the paddle
const float PlayerPaddle::WIDTH_DIFF_PER_SIZE = 0.65f;

// Number of seconds it takes for the paddle to change between sizes
// (bigger is slower, smaller is faster)
const float PlayerPaddle::SECONDS_TO_CHANGE_SIZE = 0.5f;

// Default speed of the paddle (units/sec)
const float PlayerPaddle::DEFAULT_MAX_SPEED = 26.0f;
// Default acceleration/decceleration of the paddle (units/sec^2)
const float PlayerPaddle::DEFAULT_ACCELERATION  = 138.0f;
const float PlayerPaddle::DEFAULT_DECCELERATION = -150.0f;

// Speed amount to diminish from the max speed when the paddle is poisoned
const float PlayerPaddle::POISON_SPEED_DIMINISH = PlayerPaddle::DEFAULT_MAX_SPEED / 3.0f;

// The coefficent angle change of the ball when deflected by a moving paddle
const int PlayerPaddle::DEFLECTION_DEGREE_ANGLE = 20; // TODO: Fix this...

// Delay between shots of the laser
const double PlayerPaddle::PADDLE_LASER_BULLET_DELAY = 0.3;
const double PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY  = 0.75;

// The default amount of damage the the paddle shield does to a block, when colliding with the block, per second
const int PlayerPaddle::DEFAULT_SHIELD_DMG_PER_SECOND = 90;

bool PlayerPaddle::paddleBallReleaseTimerEnabled = true;
bool PlayerPaddle::paddleBallReleaseEnabled      = true;

PlayerPaddle::PlayerPaddle() : 
centerPos(0.0f, 0.0f), minBound(0.0f), maxBound(0.0f), currSpeed(0.0f), lastDirection(0.0f), 
maxSpeed(PlayerPaddle::DEFAULT_MAX_SPEED), acceleration(PlayerPaddle::DEFAULT_ACCELERATION), 
decceleration(PlayerPaddle::DEFAULT_DECCELERATION), timeSinceLastLaserBlast(PADDLE_LASER_BULLET_DELAY), 
timeSinceLastMineLaunch(PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY),
moveButtonDown(false), hitWall(false), currType(NormalPaddle), currSize(PlayerPaddle::NormalSize), 
attachedBall(NULL), isPaddleCamActive(false), colour(1,1,1,1), isFiringBeam(false), impulse(0.0f) {
	this->ResetPaddle();
}

PlayerPaddle::PlayerPaddle(float minBound, float maxBound) : 
centerPos(0.0f, 0.0f), currSpeed(0.0f), lastDirection(0.0f), moveButtonDown(false), 
acceleration(PlayerPaddle::DEFAULT_ACCELERATION), decceleration(PlayerPaddle::DEFAULT_DECCELERATION),
currType(NormalPaddle), currSize(PlayerPaddle::NormalSize), attachedBall(NULL),
hitWall(false), isPaddleCamActive(false), colour(1,1,1,1), isFiringBeam(false), impulse(0.0f) {

	this->SetupAnimations();
	this->SetNewMinMaxLevelBound(minBound, maxBound); // resets the paddle
}

PlayerPaddle::~PlayerPaddle() {
}

// Reset the dimensions and position of this paddle (e.g., after death, start of a level).
void PlayerPaddle::ResetPaddle() {
	this->timeSinceLastLaserBlast = 0.0;
    this->timeSinceLastMineLaunch = 0.0;
	this->laserBeamTimer = 0.0;
	this->currSize = PlayerPaddle::NormalSize;
	this->centerPos = this->GetDefaultCenterPosition();
	this->currType = PlayerPaddle::NormalPaddle;
	this->colour = ColourRGBA(1, 1, 1, 1);
	this->isFiringBeam = false;
	this->hitWall = false;
	this->lastDirection = 0.0f;
	this->currSpeed = 0.0f;
	this->maxSpeed = PlayerPaddle::DEFAULT_MAX_SPEED;
	this->acceleration  = PlayerPaddle::DEFAULT_ACCELERATION; 
	this->decceleration = PlayerPaddle::DEFAULT_DECCELERATION;
	this->impulse = 0.0f;

	this->SetupAnimations();
	// This will set the dimensions the the default and regenerate the bounds of the paddle
	this->SetDimensions(PlayerPaddle::NormalSize);
}

/**
 * Private helper/initializer function for the paddle animations.
 */
void PlayerPaddle::SetupAnimations() {
	// The colour animation is used to change the colour or fade the paddle during 
	// certain animation sequences
	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);
	
	// The beam force downwards animation forces the paddle to move off the level/downwards
	// the amount of units equal to its current interpolated value when the paddle is firing its
	// laser beam power-up
	this->moveDownAnimation.ClearLerp();
	this->moveDownAnimation.SetInterpolantValue(0.0f);
	this->moveDownAnimation.SetRepeat(false);

	// Set the rotation animation - this sets the paddle at a certain rotation about the z-axis
	this->rotAngleZAnimation.ClearLerp();
	this->rotAngleZAnimation.SetInterpolantValue(0.0f);
	this->rotAngleZAnimation.SetRepeat(false);
}

// Regenerate the bounding lines of the player paddle so that they adhere to the size, position and rotation
// of the paddle as it currently lies.
void PlayerPaddle::RegenerateBounds() {
	std::vector<Collision::LineSeg2D> lineBounds;
	std::vector<Vector2D> lineNorms;

	if (this->isPaddleCamActive) {
		// When the paddle camera is active we compress the collision boundries down to the bottom of the paddle
		// this help make for a better game experience since the collision seems more natural when it's on the camera's view plane

		lineBounds.reserve(3);
		lineNorms.reserve(3);

		// 'Top' (flat) boundry
		Collision::LineSeg2D l1(Point2D(this->currHalfWidthFlat, -this->currHalfHeight), Point2D(-this->currHalfWidthFlat, -this->currHalfHeight));
		Vector2D n1(0, 1);
		lineBounds.push_back(l1);
		lineNorms.push_back(n1);

		// 'Side' (oblique) boundries
		Collision::LineSeg2D sideLine1(Point2D(this->currHalfWidthFlat, -this->currHalfHeight), Point2D(this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal1 = Vector2D(1, 1) / SQRT_2;
		lineBounds.push_back(sideLine1);
		lineNorms.push_back(sideNormal1);
		
		Collision::LineSeg2D sideLine2(Point2D(-this->currHalfWidthFlat, -this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal2 = Vector2D(-1, 1) / SQRT_2;
		lineBounds.push_back(sideLine2);
		lineNorms.push_back(sideNormal2);
	}
	else {
		lineBounds.reserve(4);
		lineNorms.reserve(4);

		// Top boundry
		Collision::LineSeg2D l1(Point2D(this->currHalfWidthFlat, this->currHalfHeight), Point2D(-this->currHalfWidthFlat, this->currHalfHeight));
		Vector2D n1(0, 1);
		lineBounds.push_back(l1);
		lineNorms.push_back(n1);

		// Side boundries
		Collision::LineSeg2D sideLine1(Point2D(this->currHalfWidthFlat, this->currHalfHeight), Point2D(this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal1 = Vector2D(1, 1) / SQRT_2;
		lineBounds.push_back(sideLine1);
		lineNorms.push_back(sideNormal1);
		
		Collision::LineSeg2D sideLine2(Point2D(-this->currHalfWidthFlat, this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal2 = Vector2D(-1, 1) / SQRT_2;
		lineBounds.push_back(sideLine2);
		lineNorms.push_back(sideNormal2);

		// Bottom boundry
		Collision::LineSeg2D bottomLine(Point2D(this->currHalfWidthTotal, -this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D bottomNormal(0, -1);
		lineBounds.push_back(bottomLine);
		lineNorms.push_back(bottomNormal);
	}

	this->bounds = BoundingLines(lineBounds, lineNorms);
	// Rotate the bounds (in the case where the paddle rotation has changed)
	this->bounds.RotateLinesAndNormals(this->rotAngleZAnimation.GetInterpolantValue(), Point2D(0,0));
	// Translate the bounds into game space
	this->bounds.TranslateBounds(Vector2D(this->GetCenterPosition()[0], this->GetCenterPosition()[1]));
}

/**
 * Set the dimensions of the paddle based on an enumerated paddle size given.
 * This will change the scale factor and bounds of the paddle.
 */
void PlayerPaddle::SetDimensions(PlayerPaddle::PaddleSize size) {
	int diffFromNormalSize = static_cast<int>(size) - static_cast<int>(PlayerPaddle::NormalSize);
	this->SetDimensions((PADDLE_WIDTH_TOTAL + diffFromNormalSize * PlayerPaddle::WIDTH_DIFF_PER_SIZE) / PADDLE_WIDTH_TOTAL);
}

/**
 * Set the dimensions of the paddle based on a scale factor given.
 * This will change the scale factor and bounds of the paddle.
 */
void PlayerPaddle::SetDimensions(float newScaleFactor) {
	this->currScaleFactor = newScaleFactor;
	assert(this->currScaleFactor > 0.0f);

	// Update all of the dimension values for the new scale factor
	this->currHalfHeight			= this->currScaleFactor * PlayerPaddle::PADDLE_HALF_HEIGHT;
	this->currHalfWidthTotal	= this->currScaleFactor * PlayerPaddle::PADDLE_HALF_WIDTH;
	this->currHalfWidthFlat		= this->currScaleFactor * PlayerPaddle::PADDLE_WIDTH_FLAT_TOP * 0.5f;
	this->currHalfDepthTotal  = this->currScaleFactor * PlayerPaddle::PADDLE_HALF_DEPTH;
	
	// The momentum of the paddle will change as well - we do a physics hack here where the acceleration/decceleration
	// are effected directly by the inverse scale factor of the paddle
	float intensifier = 1.0f;
	if (this->currScaleFactor != 1.0f) {
		intensifier = 1.25f;
	}
	float invCurrScaleFactor = 1.0f / (intensifier * this->currScaleFactor);
	this->acceleration  = invCurrScaleFactor * PlayerPaddle::DEFAULT_ACCELERATION;
	this->decceleration = invCurrScaleFactor * PlayerPaddle::DEFAULT_DECCELERATION;

	this->RegenerateBounds();
}

/**
 * Sets what the 'future' paddle size will be - this function does not immediately
 * change the size of the paddle but will cause the paddle to 'grow'/'shrink' to that
 * size as the tick function is called.
 */
void PlayerPaddle::SetPaddleSize(PlayerPaddle::PaddleSize size) {
	// If the current size is already the size being set then just exit
	if (this->currSize == size) {
		return;
	}
	this->currSize = size;
}

/**
 * Private helper function that shoots the ball attached to the player paddle.
 */
void PlayerPaddle::FireAttachedBall() {
	assert(this->attachedBall != NULL);

	// Set the ball's new trajectory in order for it to leave the paddle
	Vector2D ballReleaseDir = GameBall::STD_INIT_VEL_DIR;

	// Get the paddle's velocity
	Vector2D avgPaddleVel = this->GetVelocity();

	// Check for the sticky paddle - in the case of the sticky paddle we want the ball to not get stuck again..
	if ((this->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
		std::vector<int> indices = this->bounds.ClosestCollisionIndices(this->attachedBall->GetCenterPosition2D(), 0.01f);
		assert(indices.size() > 0);
		const Vector2D& launchNormal = this->bounds.GetNormal(indices[0]);

		// Move the ball so it's no longer colliding...
		this->attachedBall->SetCenterPosition(this->attachedBall->GetCenterPosition2D() + this->attachedBall->GetBounds().Radius() * launchNormal);
		this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), launchNormal);
	}
	else {
		// Check to see if the paddle is moving, if not just use a random angle
        if (fabs(avgPaddleVel[0]) <= EPSILON || this->centerPos[0] >= (this->maxBound - this->GetHalfWidthTotal() - EPSILON) ||
            this->centerPos[0] <= (this->minBound + this->GetHalfWidthTotal() + EPSILON)) {
			// Add some randomness to the velocity by deviating a straight-up shot by some random angle
			float randomAngleInDegs = static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne()) * GameBall::STILL_RAND_RELEASE_DEG;		
			ballReleaseDir = Vector2D::Rotate(randomAngleInDegs, ballReleaseDir);
		}
		else {
			// The paddle appears to be moving, modify the ball's release velocity
			// to reflect some of this movement
			float multiplier = PlayerPaddle::DEFAULT_MAX_SPEED / static_cast<float>(GameBall::GetNormalSpeed());
			Vector2D newBallDir = Vector2D::Normalize(ballReleaseDir + multiplier * avgPaddleVel);
			
			// and, of course, add some randomness...
			float randomAngleInDegs = static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne()) * GameBall::MOVING_RAND_RELEASE_DEG;		
			ballReleaseDir = Vector2D::Rotate(randomAngleInDegs, newBallDir);
		}

		ballReleaseDir.Normalize();

		// Set the ball velocity (tragectory it will leave the paddle on)
		this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), ballReleaseDir);
	}

	// Re-enable the ball's collisions
	this->attachedBall->SetBallBallCollisionsEnabled();
	this->attachedBall->SetBallPaddleCollisionsEnabled();

	// EVENT: Ball Shot
	GameEventManager::Instance()->ActionBallShot(*this->attachedBall);

	// Remove the ball from the paddle
	this->attachedBall = NULL;
}

/**
 * Private helper function used to solve the issue where a ball that is attached
 * to the paddle when the paddle boundries change doesn't get moved to the new boundries.
 * e.g., when in paddle cam mode and then changing back while the sticky paddle is active.
 */
void PlayerPaddle::MoveAttachedBallToNewBounds(double dT) {
	UNUSED_PARAMETER(dT);

	if (!this->HasBallAttached()) {
		return;
	}

	// We need to be careful when the collision boundries get changed the ball position could
	// be compromised; we need to make sure the ball also moves to be on the 
	// outside of the new paddle boundries
	assert(this->attachedBall != NULL);

	// Make sure the attached ball is sitting on the bounding lines and not inside the paddle...
	// Move the ball along the up vector until its no longer hitting...
	Point2D closestPtOnBounds = Collision::ClosestPoint(this->attachedBall->GetCenterPosition2D(), this->bounds.GetLine(0));
	this->attachedBall->SetCenterPosition(closestPtOnBounds + this->attachedBall->GetBounds().Radius() * this->GetUpVector());
}

void PlayerPaddle::Tick(double seconds, bool pausePaddleMovement, GameModel& gameModel) {
	Point2D startingCenterPos = this->centerPos;
	Point2D defaultCenterPos = this->GetDefaultCenterPosition();

	// If the beam is firing its laser then we want to animate it being pushed down
	this->centerPos[1] = defaultCenterPos[1] - this->moveDownAnimation.GetInterpolantValue();
	this->moveDownAnimation.Tick(seconds);

	// If the paddle is rotating (might have been hit or knocked around) we want to animate that rotation
	float startingZRotation = this->rotAngleZAnimation.GetInterpolantValue();
	this->rotAngleZAnimation.Tick(seconds);

	// Check to see if we need to increment seconds since the previous laser shot
	// This makes sure the user can't consecutively fire lasers like crazy
	if (this->timeSinceLastLaserBlast < PADDLE_LASER_BULLET_DELAY) {
		this->timeSinceLastLaserBlast += seconds;
	}
    if (this->timeSinceLastMineLaunch < PADDLE_MINE_LAUNCH_DELAY) {
        this->timeSinceLastMineLaunch += seconds;
    }

	// Figure out what the current acceleration is based on whether the player
	// is currently telling the paddle to move or not
	float distanceTravelled;
	if (pausePaddleMovement) {
		distanceTravelled = 0.0f;
	}
	else {
		float currAcceleration;
		if (this->moveButtonDown) {
			currAcceleration = this->acceleration;
		}
		else {
			currAcceleration = this->decceleration;
		}
		this->currSpeed = std::max<float>(0.0f, std::min<float>(this->maxSpeed, this->currSpeed + currAcceleration * seconds + this->impulse * seconds));
		distanceTravelled = this->GetSpeed() * seconds;
	}

	float newCenterX = this->centerPos[0] + distanceTravelled /*+ this->impulse*/;
	this->impulse = 0.0f;

	float minNewXPos = newCenterX - this->currHalfWidthTotal;
	float maxNewXPos = newCenterX + this->currHalfWidthTotal;
	Point2D oldPaddleCenter = this->centerPos;
	
	float halfWidthTotalWithBallMin = this->currHalfWidthTotal;
	float halfWidthTotalWithBallMax = this->currHalfWidthTotal;

	// If a ball is attached, it could affect the min and max x boundries of the paddle so 
	// adjust the boundries based on the position of the ball on the paddle
	bool ballIsAttached = this->attachedBall != NULL;
	if (ballIsAttached) {	
		float ballX = this->attachedBall->GetBounds().Center()[0] + distanceTravelled;
		float slightlyBiggerRadius = 1.05f * this->attachedBall->GetBounds().Radius();
		float ballMinX = ballX - slightlyBiggerRadius;
		float ballMaxX = ballX + slightlyBiggerRadius;
		
		// If it's the case that the ball increases the size of the paddle on the -x dir
		// then accomodate this, same goes for +x direction
		if (ballMinX < minNewXPos) {
			halfWidthTotalWithBallMin += (minNewXPos - ballMinX);
			minNewXPos = ballMinX;
		}
		else if (ballMaxX > maxNewXPos) {
			halfWidthTotalWithBallMax += (ballMaxX - maxNewXPos);
			maxNewXPos = ballMaxX;
		}
	}

	if (minNewXPos - EPSILON <= this->minBound) {
		// The paddle bumped into the left wall
		this->centerPos[0] = this->minBound + halfWidthTotalWithBallMin;
		
		// Only do the event if the paddle hit the wall for the 'first' time and is on the normal plane of movement
		if (!this->hitWall && this->moveDownAnimation.GetInterpolantValue() == 0.0) {
            
            if (gameModel.GetCurrentStateType() == GameState::BallInPlayStateType) {
                LevelPiece* piece = gameModel.GetCurrentLevel()->GetMinPaddleBoundPiece();
                LevelPiece* resultingPiece = piece->CollisionOccurred(&gameModel, *this);
                UNUSED_VARIABLE(resultingPiece);
                assert(resultingPiece == piece);
            }

			// EVENT: paddle hit left wall for first time
			GameEventManager::Instance()->ActionPaddleHitWall(*this, this->centerPos + Vector2D(-halfWidthTotalWithBallMin, 0));
		}

		this->hitWall = true;
	}
	else if (maxNewXPos + EPSILON > this->maxBound) {
		// The paddle bumped into the right wall
		this->centerPos[0] = this->maxBound - halfWidthTotalWithBallMax;
		
		// Only do the event if the paddle hit the wall for the 'first' time and is on the normal plane of movement
		if (!this->hitWall && this->moveDownAnimation.GetInterpolantValue() == 0.0) {

            if (gameModel.GetCurrentStateType() == GameState::BallInPlayStateType) {
                LevelPiece* piece = gameModel.GetCurrentLevel()->GetMaxPaddleBoundPiece();
                
                // Make sure the piece is actually being collided with
                if (fabs(this->maxBound - gameModel.GetCurrentLevel()->GetPaddleMaxBound()) < EPSILON) {
                    LevelPiece* resultingPiece = piece->CollisionOccurred(&gameModel, *this);
                    UNUSED_VARIABLE(resultingPiece);
                    assert(resultingPiece == piece);
                }
            }

			// EVENT: paddle hit right wall for first time
			GameEventManager::Instance()->ActionPaddleHitWall(*this, this->centerPos + Vector2D(halfWidthTotalWithBallMax, 0));
		}
		this->hitWall = true;
	}
	else {
		this->centerPos[0] = newCenterX;
		this->hitWall = false;
	}

	/*
	// Figure out the average speed over some number of ticks ... don't know if I actually need this...
	if (this->ticksSinceAvg == 0) {
		this->avgVel = this->currSpeed;
	}
	else {
		this->avgVel = (this->avgVel + this->currSpeed) * 0.5f;
	}
	this->ticksSinceAvg = (this->ticksSinceAvg + 1) % AVG_OVER_TICKS;
	*/

	// Change the size gradually (lerp based on some constant time) if need be...
	int diffFromNormalSize = static_cast<int>(this->currSize) - static_cast<int>(PlayerPaddle::NormalSize);
	float targetScaleFactor = (PADDLE_WIDTH_TOTAL + diffFromNormalSize * PlayerPaddle::WIDTH_DIFF_PER_SIZE) / PADDLE_WIDTH_TOTAL;
	float scaleFactorDiff = targetScaleFactor - this->currScaleFactor;
	if (scaleFactorDiff != 0.0f) {
		this->SetDimensions(this->currScaleFactor + ((scaleFactorDiff * seconds) / SECONDS_TO_CHANGE_SIZE));
	}

	// If there is a ball attached then we need to move it around with the paddle
	if (ballIsAttached) {
		// We need to figure out what the distance is from the paddle center-top to the ball center
		Vector2D paddleTopCenterToBallCenter = this->attachedBall->GetBounds().Center() - (startingCenterPos + Vector2D(0, this->GetHalfHeight()));
		// Maintain the vector obtained above in the new position of the paddle
		this->attachedBall->SetCenterPosition(this->centerPos + Vector2D(0, this->GetHalfHeight()) + paddleTopCenterToBallCenter);
	}

	// Check to see if the center position changed or its rotation changed, if so then regenerate the bounds
	if (startingCenterPos != this->centerPos || startingZRotation != this->rotAngleZAnimation.GetInterpolantValue()) {
		this->RegenerateBounds();
	}
}

/**
 * Increases the paddle size if it can.
 * Returns: true if there was an increase in size, false otherwise.
 */
bool PlayerPaddle::IncreasePaddleSize() {
	if (this->currSize == BiggestSize) { 
		return false; 
	}
	
	this->SetPaddleSize(static_cast<PaddleSize>(this->currSize + 1));
	return true;
}

/**
 * Decreases the paddle size if it can.
 * Returns: true if there was an decrease in size, false otherwise.
 */
bool PlayerPaddle::DecreasePaddleSize() {
	if (this->currSize == SmallestSize) { 
		return false; 
	}

	this->SetPaddleSize(static_cast<PaddleSize>(this->currSize - 1));
	return true;
}

 void PlayerPaddle::AddPaddleType(const PaddleType& type) {
		this->currType = this->currType | type;
		switch (type) {
			case PlayerPaddle::PoisonPaddle:
				this->maxSpeed -= PlayerPaddle::POISON_SPEED_DIMINISH;
				break;
			default:
				break;
		}
	}

void PlayerPaddle::RemovePaddleType(const PaddleType& type) {
	this->currType = this->currType & ~type;
	switch (type) {
			case PlayerPaddle::PoisonPaddle:
				this->maxSpeed += PlayerPaddle::POISON_SPEED_DIMINISH;
				break;
			default:
				break;
	}
}

// Tells the paddle that it has started to fire the laser beam (or has stopped firing the laser beam)
void PlayerPaddle::SetIsLaserBeamFiring(bool isFiring) {
	/*
	// We don't push the paddle down while firing the laser any more cause it's annoying and
	// it breaks the safety net, which is annoying and not fun.
	if (isFiring) {

		// Start up the animation that pushes the paddle down (as if reacting to the sheer
		// force of the beam pressing down on it)
		std::vector<double> times;
		times.reserve(2);
		times.push_back(0.0f);
		times.push_back(0.05f);

		std::vector<float> values;
		values.reserve(2);
		values.push_back(this->moveDownAnimation.GetInterpolantValue());
		values.push_back(PlayerPaddle::PADDLE_HEIGHT_TOTAL);

		this->moveDownAnimation.SetLerp(times, values);	// Blended lerp??
	}
	else {
		this->moveDownAnimation.AppendLerp(0.1, 0.0f);
	}
	*/
	this->isFiringBeam = isFiring;
}

void PlayerPaddle::ShootBall() {
	if (this->HasBallAttached()) {
		this->FireAttachedBall();
	}
}

/**
 * This will fire any weapons or abilities that paddle currently has - if none
 * exist then this function does nothing.
 */
void PlayerPaddle::Shoot(GameModel* gameModel) {
	// Check for the various paddle types and react appropriately for each
	
	// If there's a ball attached to the paddle then we release it and exit
	if (this->HasBallAttached()) {
		this->FireAttachedBall();
		return;
	}
	
	// Check for the rocket paddle (this has top priority)
	if ((this->GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle) {
		// The rocket immediately is fired from the paddle - create a projectile for it and add it to the model (i.e., fire it!)
		Point2D rocketSpawnPos;
		float rocketHeight, rocketWidth;
        this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		PaddleRocketProjectile* rocketProjectile = new PaddleRocketProjectile(rocketSpawnPos, 
            Vector2D::Normalize(this->GetUpVector()), rocketWidth, rocketHeight);

		// Make sure the rocket doesn't explode if it's lying up against a block when launched...
        bool foundPiece = false;
		const GameLevel* currLevel = gameModel->GetCurrentLevel();
		std::set<LevelPiece*> levelPieces = currLevel->GetLevelPieceCollisionCandidates(*rocketProjectile);
		for (std::set<LevelPiece*>::const_iterator iter = levelPieces.begin(); iter != levelPieces.end(); ++iter) {
			const LevelPiece* currPiece = *iter;
			if (!currPiece->ProjectilePassesThrough(rocketProjectile)) {
				rocketProjectile->SetLastThingCollidedWith(currPiece);
                foundPiece = true;
				break;
			}
		}

        // Make the last thing the rocket collided with the paddle - if there are no pieces to do this with
        if (!foundPiece) {
            rocketProjectile->SetLastThingCollidedWith(this);
        }

		gameModel->AddProjectile(rocketProjectile);
		this->RemovePaddleType(PlayerPaddle::RocketPaddle);

        // EVENT: Paddle just fired a rocket
        GameEventManager::Instance()->ActionPaddleWeaponFired();
	}
	// Check for laser beam paddle - this has secondary priority
	else if ((this->GetPaddleType() & PlayerPaddle::LaserBeamPaddle) == PlayerPaddle::LaserBeamPaddle && !this->isFiringBeam) {
		// We add the beam to the game model, the rest will be taken care of by the beam and model
		gameModel->AddBeam(Beam::PaddleLaserBeam);

        // EVENT: Paddle just fired a beam
        GameEventManager::Instance()->ActionPaddleWeaponFired();
	}
	
    else {
        // All of the rest are basic, multiple shot projectiles that should all execute simultaneously

        // Check for laser bullet paddle (shoots little laser bullets from the paddle)
        if ((this->GetPaddleType() & PlayerPaddle::LaserBulletPaddle) == PlayerPaddle::LaserBulletPaddle) {
		    // Make sure we are allowed to fire a new laser bullet
		    if (this->timeSinceLastLaserBlast >= PADDLE_LASER_BULLET_DELAY) {

			    // Calculate the width and height of the laser bullet based on the size of the paddle...
			    float projectileWidth  = this->GetPaddleScaleFactor() * PaddleLaserProjectile::WIDTH_DEFAULT;
			    float projectileHeight = this->GetPaddleScaleFactor() * PaddleLaserProjectile::HEIGHT_DEFAULT;

			    // Create the right type of projectile in the right place
			    Projectile* newProjectile = new PaddleLaserProjectile(
				    this->GetCenterPosition() + Vector2D(0, this->currHalfHeight + 0.5f * projectileHeight));
                newProjectile->SetLastThingCollidedWith(this);

			    // Modify the fired bullet based on the current paddle's properties...
			    newProjectile->SetWidth(projectileWidth);
			    newProjectile->SetHeight(projectileHeight);

			    // Fire ze laser bullet! - tell the model about it
			    gameModel->AddProjectile(newProjectile);

			    // Reset the timer for the next laser blast
			    this->timeSinceLastLaserBlast = 0.0;

                // EVENT: Paddle just fired a laser bullet
                GameEventManager::Instance()->ActionPaddleWeaponFired();
		    }
        }

        // Check for mine launcher paddle (shoots explosive mines from the paddle)
        if ((this->GetPaddleType() & PlayerPaddle::MineLauncherPaddle) == PlayerPaddle::MineLauncherPaddle) {
            if (this->timeSinceLastMineLaunch >= PADDLE_MINE_LAUNCH_DELAY) {

			    // Calculate the width and height of the mine based on the size of the paddle...
			    float projectileWidth  = this->GetPaddleScaleFactor() * PaddleMineProjectile::PADDLEMINE_WIDTH_DEFAULT;
			    float projectileHeight = this->GetPaddleScaleFactor() * PaddleMineProjectile::PADDLEMINE_HEIGHT_DEFAULT;

			    // Create the right type of projectile in the right place
			    Projectile* newProjectile = new PaddleMineProjectile(
				    this->GetCenterPosition() + Vector2D(0, this->currHalfHeight + 0.5f * projectileHeight),
                    Vector2D::Normalize(this->GetUpVector()), projectileWidth, projectileHeight);
               
                newProjectile->SetLastThingCollidedWith(this);

			    // Launch ze mine! - tell the model about it
			    gameModel->AddProjectile(newProjectile);

                this->timeSinceLastMineLaunch = 0.0;

                // EVENT: Paddle just launched a mine
                GameEventManager::Instance()->ActionPaddleWeaponFired();
            }
        }

	}

	// TODO: other paddle shooting abilities go here...
}

/**
 * Attaches the given ball to the paddle if there is no ball already attached
 * to the paddle.
 * Returns: true if the ball was successfully attached, false otherwise.
 */
bool PlayerPaddle::AttachBall(GameBall* ball) {
	assert(ball != NULL);

	// If the paddle already has a ball attached then we do nothing
	if (this->HasBallAttached()) {
		return false;
	}

	// Attach the ball
	this->attachedBall = ball;
	// Reset the ball so that it has no previous record of the level piece it last hit
	ball->SetLastPieceCollidedWith(NULL);

	// Make sure the position of the ball is sitting on-top of the paddle
	Vector2D normal;
	Collision::LineSeg2D collisionLine;
	double timeSinceCollision;
	bool onPaddle = this->CollisionCheck(*this->attachedBall, 0.0, normal, collisionLine, timeSinceCollision);
	if (onPaddle) {
		// Position the ball so that it is against the collision line
		const Collision::Circle2D& ballBounds = this->attachedBall->GetBounds();
		//this->attachedBall->SetCenterPosition(ballBounds.Center() + (ballBounds.Radius() - distance - 5 * EPSILON) * - this->attachedBall->GetDirection());
		this->attachedBall->SetCenterPosition(ballBounds.Center() + (EPSILON + timeSinceCollision) * -this->attachedBall->GetVelocity());
	}

	// Remove the ball's direction but not its speed
	this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), Vector2D(0, 0));

	// Disable collisions for the ball (reenable them when the ball is detached)
	this->attachedBall->SetBallBallCollisionsDisabled();
	this->attachedBall->SetBallPaddleCollisionsDisabled();
	// Tell the ball not to collide with this paddle again when it's released until it hits something else...
	this->attachedBall->SetLastThingCollidedWith(this);

	return true;
}

// Called when the paddle is hit by a projectile
void PlayerPaddle::HitByProjectile(GameModel* gameModel, const Projectile& projectile) {
	// The paddle is unaffected if it has a shield active...
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		// EVENT: Paddle shield was just hit by a projectile
		GameEventManager::Instance()->ActionPaddleShieldHitByProjectile(*this, projectile);
		return;
	}

	// Different projectiles have different effects on the paddle...
	switch (projectile.GetType()) {

		case Projectile::CollateralBlockProjectile:
			// Causes the paddle to be violently thrown off course for a short period of time...
			this->CollateralBlockProjectileCollision(projectile);
			break;

        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			this->LaserBulletProjectileCollision(projectile);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
            assert(dynamic_cast<const RocketProjectile*>(&projectile) != NULL);
			this->RocketProjectileCollision(gameModel, *static_cast<const RocketProjectile*>(&projectile));
			break;

        case Projectile::PaddleMineBulletProjectile:
            assert(dynamic_cast<const PaddleMineProjectile*>(&projectile) != NULL);
            this->MineProjectileCollision(gameModel, *static_cast<const PaddleMineProjectile*>(&projectile));
            break;

		case Projectile::FireGlobProjectile:
			this->FireGlobProjectileCollision(projectile);
			break;

		default:
			assert(false);
			break;
	}

	// EVENT: Paddle was just hit by a projectile
	GameEventManager::Instance()->ActionPaddleHitByProjectile(*this, projectile);
}

// Modify the projectile trajectory in certain special cases when the projectile is colliding with the paddle
// (or the paddle shield or some component of the paddle)
void PlayerPaddle::ModifyProjectileTrajectory(Projectile& projectile) {

	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
        switch (projectile.GetType()) {

            case Projectile::BallLaserBulletProjectile:
            case Projectile::PaddleLaserBulletProjectile:
            case Projectile::PaddleRocketBulletProjectile:
            case Projectile::RocketTurretBulletProjectile:
            case Projectile::LaserTurretBulletProjectile:
            case Projectile::PaddleMineBulletProjectile: {

		        // If the projectile is moving generally upwards and away from the paddle then we ignore this entirely...
		        if (acos(std::max<float>(-1.0f, std::min<float>(1.0f, 
			          Vector2D::Dot(projectile.GetVelocityDirection(), 
				        Vector2D::Normalize(this->GetUpVector()))))) > static_cast<float>(M_PI / 3.0f)) {
    				
			        Vector2D fromShieldCenterToProjectile = projectile.GetPosition() - this->GetCenterPosition();
			        fromShieldCenterToProjectile.Normalize();
			        projectile.SetVelocity(fromShieldCenterToProjectile, projectile.GetVelocityMagnitude());
			        projectile.SetPosition(projectile.GetPosition() + projectile.GetHeight() * fromShieldCenterToProjectile);

			        // Be sure to reset the last piece the projectile collided with since it technically just collided
			        // off of the paddle's shield
			        projectile.SetLastThingCollidedWith(NULL);

			        // EVENT: Projectile deflected by shield
			        GameEventManager::Instance()->ActionProjectileDeflectedByPaddleShield(projectile, *this);
		        }
                break;
            }

            default:
                break;
		}
	}
}

/**
 * It is assumed that at this point, the paddle collides with the given level piece.
 * Based on this collision, we need to update how far to the left/right the paddle can move.
 * If doAttachedBallCollision is true then a collision check will also take place for the attached ball -
 * this is fine if we don't want the ball to break blocks but if the game state is BallInPlayState then just
 * set this to false.
 */
void PlayerPaddle::UpdateBoundsByPieceCollision(const LevelPiece& p, bool doAttachedBallCollision) {

	bool didCollide = false;
	std::list<Point2D> collisionPts;

    if (p.GetType() != LevelPiece::NoEntry) {

	    // We need to find the location of the collision so we can make the paddle stop (by updating its bounds):
	    // Grab all of the collision points between the paddle and the level piece
	    if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		    didCollide = p.GetBounds().GetCollisionPoints(this->CreatePaddleShieldBounds(), collisionPts);
	    }
	    else {
		    didCollide = p.GetBounds().GetCollisionPoints(this->bounds, collisionPts);
	    }

	    // If there's a rocket attached we need to check its bounds too...
	    if ((this->GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle) {
		    Point2D rocketSpawnPos;
		    float rocketHeight, rocketWidth;
		    this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		    Vector2D widthHeightVec(rocketWidth/1.5f, rocketHeight/1.5f);

		    //didCollide |= Collision::GetCollisionPoint(p.GetAABB(), Collision::AABB2D(rocketSpawnPos - widthHeightVec, rocketSpawnPos + widthHeightVec), collisionPts);
		    didCollide |= p.GetBounds().GetCollisionPoints(Collision::AABB2D(rocketSpawnPos - widthHeightVec, rocketSpawnPos + widthHeightVec), collisionPts);
	    }

    }
	
	// If there's a ball attached we need to check its bounds as well...
	if (doAttachedBallCollision && this->HasBallAttached()) {
		didCollide |= p.GetBounds().GetCollisionPoints(this->GetAttachedBall()->GetBounds(), collisionPts);
	}

	if (!didCollide) {
		return;
	}
	
	// The trick with the points we've obtained is to make sure we properly update the min/max bounds
	// along the x-axis of the paddle...
	
	// Seperate points into those to the left and right of the paddle center, closest to the paddle center...
	bool collisionOnLeft  = false;
	bool collisionOnRight = false;

	for (std::list<Point2D>::iterator iter = collisionPts.begin(); iter != collisionPts.end(); ++iter) {
		const Point2D& currPt = *iter;
		// Check to see where the point is relative to the center of the paddle...
		if (currPt[0] < this->GetCenterPosition()[0]) {
			// Point is on the left of the paddle...
			collisionOnLeft = true;
		}
		else {
			// Point is on the right of the paddle (or centerish)...
			collisionOnRight = true;
		}
	}

	if (collisionOnLeft) {
		// If both the closest right and left coordinates were changed then the paddle is somehow
		// centered on a block, just ignore any changes and exit if this is the case (we don't want the
		// player to get stuck).
		if (collisionOnRight) {
			return;
		}

		this->minBound = std::max<float>(this->minBound, this->GetCenterPosition()[0] - this->GetHalfWidthTotal());
	}
	else {
		// There must bee a collision on the right then...
		assert(collisionOnRight);
		this->maxBound = std::min<float>(this->maxBound, this->GetCenterPosition()[0] + this->GetHalfWidthTotal());
	}	
}

/**
 * Adds an animation to the paddle that fades it in or out based on the
 * given parameter over the given amount of time.
 */
void PlayerPaddle::AnimateFade(bool fadeOut, double duration) {
	ColourRGBA finalColour = this->colour;
	if (fadeOut) {
		finalColour[3] = 0.0f;
	}
	else {
		finalColour[3] = 1.0f;
	}

	this->colourAnimation.SetLerp(duration, finalColour);
}

void PlayerPaddle::DebugDraw() const {
	glPushMatrix();
	this->bounds.DebugDraw();
	glPopMatrix();
}

/**
 * Do the paddle collision with the collateral block projectile - this will knock the
 * the paddle off course and rotate it a bit.
 */
void PlayerPaddle::CollateralBlockProjectileCollision(const Projectile& projectile) {
	float distFromCenter = 0.0f;
	// Find percent distance from edge to center of the paddle
	float percentNearCenter = this->GetPercentNearPaddleCenter(projectile.GetPosition(), distFromCenter);
	float percentNearEdge = 1.0 - percentNearCenter;

	static const float MAX_HIT_ROTATION = 80.0f;
	
	float rotationAmount = percentNearEdge * MAX_HIT_ROTATION;
	if (distFromCenter > 0.0) {
		// The projectile hit the 'right' side of the paddle (typically along the positive x-axis)
		rotationAmount *= -1.0f;
	}
	//else {
		// The projectile hit the 'left' side of the paddle (typically along the negative x-axis)
	//}

	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	static const double HIT_EFFECT_TIME = 2.5;
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(HIT_EFFECT_TIME);

	static const float MIN_MOVE_DOWN_AMT = 3.0f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;
	float totalMaxMoveDown = MIN_MOVE_DOWN_AMT + percentNearCenter * 2.25f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);

	times.clear();
	times.reserve(5);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(0.5f);
	times.push_back(1.0f);
	times.push_back(HIT_EFFECT_TIME);

	std::vector<float> rotationValues;
	rotationValues.reserve(5);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() - 0.25f * rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + 0.75f * rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);
}

// A laser bullet just collided with the paddle...
void PlayerPaddle::LaserBulletProjectileCollision(const Projectile& projectile) {
	float currHeight = 2.0f * this->GetHalfHeight();
	this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 1.5f, currHeight, currHeight, 15.0f);
}

// Rocket projectile just collided with the paddle - causes the paddle to fly wildly off course
void PlayerPaddle::RocketProjectileCollision(GameModel* gameModel, const RocketProjectile& projectile) {
	float distFromCenter = 0.0f;
	// Find percent distance from edge to center of the paddle
	float percentNearCenter = this->GetPercentNearPaddleCenter(projectile.GetPosition(), distFromCenter);
	float percentNearEdge = 1.0 - percentNearCenter;

    const float MAX_HIT_ROTATION = 170.0f * projectile.GetForcePercentageFactor();
	
	float rotationAmount = percentNearEdge * MAX_HIT_ROTATION;
	if (distFromCenter > 0.0) {
		// The projectile hit the 'right' side of the paddle (typically along the positive x-axis)
		rotationAmount *= -1.0f;
	}
	//else {
		// The projectile hit the 'left' side of the paddle (typically along the negative x-axis)
	//}

	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	const double HIT_EFFECT_TIME = 5.0 * projectile.GetForcePercentageFactor();
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(HIT_EFFECT_TIME);

	static const float MIN_MOVE_DOWN_AMT = 4.0f * PlayerPaddle::PADDLE_HEIGHT_TOTAL * projectile.GetForcePercentageFactor();
	float totalMaxMoveDown = MIN_MOVE_DOWN_AMT + percentNearCenter * 2.5f * PlayerPaddle::PADDLE_HEIGHT_TOTAL * projectile.GetForcePercentageFactor();

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);

	times.clear();
	times.reserve(5);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(1.0f);
	times.push_back(2.5f);
	times.push_back(HIT_EFFECT_TIME);

	std::vector<float> rotationValues;
	rotationValues.reserve(5);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() - 0.75f * rotationAmount);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + 0.50f * rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);

	// Rocket explosion!!
	GameLevel* currentLevel = gameModel->GetCurrentLevel();
	assert(currentLevel != NULL);

	// Figure out the position in space where the rocket hit and find what block is at that position...
	std::set<LevelPiece*> levelPieces = currentLevel->GetLevelPieceCollisionCandidates(projectile.GetPosition(), EPSILON);
	if (!levelPieces.empty()) {
		currentLevel->RocketExplosion(gameModel, &projectile, *levelPieces.begin());
	}
	else {
		currentLevel->RocketExplosionNoPieces(&projectile);
	}
}

void PlayerPaddle::MineProjectileCollision(GameModel* gameModel, const PaddleMineProjectile& projectile) {
    float currHeight = 2.0f * this->GetHalfHeight();
    this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 3.5f, 3.0f * currHeight, 2.0f * currHeight, 70.0f);
    
    // Mine explosion!
    GameLevel* currentLevel = gameModel->GetCurrentLevel();
	assert(currentLevel != NULL);
	currentLevel->MineExplosion(gameModel, &projectile);
}

void PlayerPaddle::FireGlobProjectileCollision(const Projectile& projectile) {
	assert(projectile.GetType() == Projectile::FireGlobProjectile);
	const FireGlobProjectile* fireGlobProjectile = static_cast<const FireGlobProjectile*>(&projectile);

	float currHeight = 2.0f * this->GetHalfHeight();
	switch (fireGlobProjectile->GetRelativeSize()) {
		case FireGlobProjectile::Small:
			this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 2.0f, currHeight, currHeight, 20.0f);
			break;

		case FireGlobProjectile::Medium:
			this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 2.75f, 2.0f * currHeight, 1.5f * currHeight, 35.0f);
			break;

		case FireGlobProjectile::Large:
			this->SetPaddleHitByProjectileAnimation(projectile.GetPosition(), 3.25f, 2.5f * currHeight, 2.0f * currHeight, 70.0f);
			break;

		default:
			assert(false);
			break;
	}
}

// Get an idea of how close the given projectile is to the center of the paddle as a percentage
// where 0 is a far away from the center as possible and 1 is at the center. Also returns the value
// of distFromCenter which is assigned a negative/positive value based on the distance from the center
// of the paddle on the left/right side respectively.
float PlayerPaddle::GetPercentNearPaddleCenter(const Point2D& projectileCenter, float& distFromCenter) {
	// Figure out what side of the paddle the projectile is hitting...
	// Use a line test...
	Vector2D vecToProjectile = projectileCenter - this->GetCenterPosition();
	Vector2D positivePaddleAxis = Vector2D::Rotate(this->rotAngleZAnimation.GetInterpolantValue(), PlayerPaddle::DEFAULT_PADDLE_RIGHT_VECTOR);

	// Use a dot product to determine whether the projectile is on the positive side or not
	distFromCenter = Vector2D::Dot(vecToProjectile, positivePaddleAxis);

	// Find percent distance from edge to center of the paddle
	float percentNearCenter = std::max<float>(0.0f, this->currHalfWidthTotal - fabs(distFromCenter)) / this->currHalfWidthTotal;
	assert(percentNearCenter >= 0.0f && percentNearCenter <= 1.0);
	return percentNearCenter;
}

// Helper function for setting the animation for when a projectile hits the paddle
// projectileCenter    - position where the projectile is
// totalHitEffectTime  - total time of the animation
// minMoveDown         - minimum distance the paddle will be pushed down
// closeToCenterCoeff  - added maximum effect of the paddle being hit right in the center
// maxRotationInDegs   - maximum rotation amount (when the paddle is hit on its outer edges)
void PlayerPaddle::SetPaddleHitByProjectileAnimation(const Point2D& projectileCenter, 
                                                     double totalHitEffectTime, 
                                                     float minMoveDown, 
                                                     float closeToCenterCoeff, float maxRotationInDegs) {
	
	// Find percent distance from edge to center of the paddle
	float distFromCenter = 0.0;
	float percentNearCenter = this->GetPercentNearPaddleCenter(projectileCenter, distFromCenter);
	float percentNearEdge = 1.0 - percentNearCenter;
	float rotationAmount = percentNearEdge * maxRotationInDegs;
	if (distFromCenter > 0.0) {
		// The projectile hit the 'right' side of the paddle (typically along the positive x-axis)
		rotationAmount *= -1.0f;
	}

	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(totalHitEffectTime);

	float totalMaxMoveDown = minMoveDown + percentNearCenter * closeToCenterCoeff;

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);

	std::vector<float> rotationValues;
	rotationValues.reserve(3);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);
}

/**
 * Calculates the number of points the player will get when the paddle hits the ball.
 * Each point amount is added to a list with a possible description of what the points were for.
 */
/*
std::list<PointAward> PlayerPaddle::GetPointsForHittingBall(const GameBall& ball) const {
    std::list<PointAward> pointPairs;

    // Make sure we only award points for a single paddle ball contact - multiple can occur when the player
    // is moving the paddle into the ball
    static long timeSinceLastPtsAwarded = 0;
	long currSystemTime = BlammoTime::GetSystemTimeInMillisecs();
    if ((currSystemTime - timeSinceLastPtsAwarded) < 100) {
        return pointPairs;
    }
    if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle ||
        (this->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
        // No points are awarded if there's a shield on the paddle or if it's sticky
        return pointPairs;
    }
    
    // Two major things factor into this: 
    // 1. How far the ball is from the paddle center (further = more points); exception: bullseye bonus (very close to center)
    // 2. How fast the paddle is moving when it hits the ball (faster = more points)
    float centerDiff = fabs(this->GetCenterPosition()[0] - ball.GetCenterPosition2D()[0]);
    float paddleSpdDiff = (this->maxSpeed - this->currSpeed);
    assert(centerDiff >= 0.0f);
    assert(paddleSpdDiff >= 0.0f);

    // Award the points for distance from the paddle center
    static const int DAREDEVIL_BONUS_POINTS = 200;
    //static const int BULLSEYE_BONUS_POINTS  = 100;
    //float oneFifthHalfWidth = this->GetHalfDepthTotal() / 5.0f;

    // Check for the bullseye bonus
    //if (centerDiff < oneFifthHalfWidth) {
    //    pointPairs.push_back(PointAward(BULLSEYE_BONUS_POINTS, ScoreTypes::PaddleBullseyeBonus, ball.GetCenterPosition2D()));
    //}
    if (centerDiff > (0.5f * ball.GetBounds().Radius() + this->GetHalfWidthTotal())) {
        // Bonus points are awarded if the player JUST hit the ball on the edge of the paddle
        pointPairs.push_back(PointAward(DAREDEVIL_BONUS_POINTS, ScoreTypes::PaddleDaredevilBonus, ball.GetCenterPosition2D()));
    }

    // Award points for paddle speed
    static const int SPEED_BONUS_POINTS = 50;
    if (paddleSpdDiff <= EPSILON) {
        pointPairs.push_back(PointAward(SPEED_BONUS_POINTS, ScoreTypes::SpeedyPaddleBonus, ball.GetCenterPosition2D()));
    }

    timeSinceLastPtsAwarded = currSystemTime;
    return pointPairs;
}
*/

/**
 * Used to detect and react to situations where the ball is forced up against a wall by the paddle.
 */
bool PlayerPaddle::UpdateForOpposingForceBallCollision(const GameBall& ball, double dT) {
    static Vector2D normal;
    static Collision::LineSeg2D collisionLine;
    static double timeSinceCollision;
    
    // Check to see if the ball is hitting the bounds of the level...
    float ballMinX = ball.GetCenterPosition2D()[0] - ball.GetBounds().Radius();
    float ballMaxX = ball.GetCenterPosition2D()[0] + ball.GetBounds().Radius();

    if (ballMinX <= this->minBound) {
        // The ball is currently intersecting the line that defines the 
        // minimum X position for the paddle to move to

        // Check for the paddle collision...
        if (this->CollisionCheck(ball, dT, normal, collisionLine, timeSinceCollision)) {
            // The paddle is also colliding - we want to push the paddle back so that it is no longer colliding...
            this->ApplyImpulseForce(std::max<float>(DEFAULT_MAX_SPEED/2, this->GetSpeed()));
            return true;
        }
        
    }
    else if (ballMaxX >= this->maxBound) {
        // The ball is currently intersecting the line that defines the
        // maximum X position for the paddle to move to

        // Check for the paddle collision...
        if (this->CollisionCheck(ball, dT, normal, collisionLine, timeSinceCollision)) {
            // The paddle is also colliding - we want to push the paddle back so that it is no longer colliding...
            this->ApplyImpulseForce(std::max<float>(DEFAULT_MAX_SPEED/2, this->GetSpeed()));
            return true;
        }
    }

    return false;
}

/**
 * When the paddle magnet is active this routine will potentitally modify the given vector so that the object associated
 * with it (as its velocity) and the given center position will start to migrate towards the paddle
 * as if being attracted by it.
 */
void PlayerPaddle::AugmentDirectionOnPaddleMagnet(double seconds, float degreesChangePerSec,
                                                  const Point2D& currCenter, Vector2D& vectorToAugment) const {
    // If the paddle has the magnet item active and the projectile is moving towards the paddle, then we need to
    // modify the velocity to make it move towards the paddle...
    if ((this->GetPaddleType() & PlayerPaddle::MagnetPaddle) != PlayerPaddle::MagnetPaddle) {
        return;
    }
        
    // Also, don't keep augmenting the direction if the projectile has passed the paddle going out of bounds...
    if (currCenter[1] < this->GetCenterPosition()[1]) {
        return;
    }

    // Figure out the vector from the projectile to the paddle...
    Vector2D projectileToPaddleVec = this->GetCenterPosition() - currCenter;
    projectileToPaddleVec.Normalize();

    if (Vector2D::Dot(vectorToAugment, projectileToPaddleVec) <= 0) {
        // The projectile is not within a 90 degree angle of the vector from the projectile to the paddle
        return;
    }
    
    float currRotAngle = atan2(vectorToAugment[1], vectorToAugment[0]);
    
    // Don't let the angle get too sharp or the projectile will collide with the paddle no matter what
    bool wasInRange = true;
    if (currRotAngle > Trig::degreesToRadians(-30) || currRotAngle < Trig::degreesToRadians(-150)) {
        wasInRange = false;
    }

    float targetRotAngle = atan2(projectileToPaddleVec[1],  projectileToPaddleVec[0]);

    if (currRotAngle > M_PI_DIV2) { currRotAngle -= M_PI_MULT2; }


    if (fabs(targetRotAngle - currRotAngle) <= 0.01) {
        // If the direction is already pointing at the paddle then exit immediately
        return;
    }

    // Figure out the shortest way to get there...
    float rotSgn;
    if (targetRotAngle < currRotAngle) {
        rotSgn = -1;
    }
    else {
        rotSgn = 1;
    }

    float rotationAmt = rotSgn * seconds * degreesChangePerSec;

    Vector2D newVector(vectorToAugment);
    newVector.Rotate(rotationAmt);
    newVector.Normalize();

    if (wasInRange) {
        // Make sure the new vector is in range as well...
        float newRotAngle = atan2(newVector[1], newVector[0]);
        if (newRotAngle > Trig::degreesToRadians(-30) || newRotAngle < Trig::degreesToRadians(-150)) {
            // We've gone out of range, don't change anything and exit
            return;
        }

    }

    vectorToAugment = newVector;
}

// Obtain an AABB encompassing the entire paddle's current collision area
Collision::AABB2D PlayerPaddle::GetPaddleAABB(bool includeAttachedBall) const {
	Point2D minPt, maxPt;
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		Vector2D sphereRadiusVec(this->GetHalfWidthTotal(), this->GetHalfWidthTotal());
		sphereRadiusVec = 1.2f * sphereRadiusVec;
		minPt = this->GetCenterPosition() - sphereRadiusVec;
		maxPt = this->GetCenterPosition() + sphereRadiusVec;
	}
	else {
		Vector2D halfWidthHeight(this->GetHalfWidthTotal(), this->GetHalfHeight());
		halfWidthHeight = 1.2f * halfWidthHeight;
		minPt = this->GetCenterPosition() - halfWidthHeight;
		maxPt = this->GetCenterPosition() + halfWidthHeight;
	}

	Collision::AABB2D paddleAABB(minPt, maxPt);
	if (includeAttachedBall && this->HasBallAttached()) {
		const Collision::Circle2D& ballBounds = this->GetAttachedBall()->GetBounds();
		Vector2D ballRadiusVec(ballBounds.Radius(), ballBounds.Radius());
		ballRadiusVec = 1.2f * ballRadiusVec;
		paddleAABB.AddPoint(ballBounds.Center() + ballRadiusVec);
		paddleAABB.AddPoint(ballBounds.Center() - ballRadiusVec);
	}

	if ((this->GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle) {
		Point2D rocketSpawnPos;
		float rocketHeight, rocketWidth;
		this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		Vector2D widthHeightVec(rocketWidth/1.5f, rocketHeight/1.5f);

		paddleAABB.AddPoint(rocketSpawnPos - widthHeightVec);
		paddleAABB.AddPoint(rocketSpawnPos + widthHeightVec);
	}

	return paddleAABB;
}

/**
 * Check to see if the given set of bounding lines collides with this paddle.
 */
bool PlayerPaddle::CollisionCheck(const BoundingLines& bounds,
                                         bool includeAttachedBallCheck) const {
	bool didCollide = false;
	// If the paddle has a shield around it do the collision with the shield
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		didCollide = bounds.CollisionCheck(this->CreatePaddleShieldBounds());
	}
	else {
		didCollide = this->bounds.CollisionCheck(bounds);
	}

	if (includeAttachedBallCheck && !didCollide && this->HasBallAttached()) {
		didCollide = bounds.CollisionCheck(this->GetAttachedBall()->GetBounds());
	}

	// If there's a rocket attached we need to check for collisions with it!
    if (!didCollide) {
        if ((this->GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle) {
		    Point2D rocketSpawnPos;
		    float rocketHeight, rocketWidth;
		    this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		    Vector2D widthHeightVec(rocketWidth/1.5f, rocketHeight/1.5f);

		    Collision::AABB2D rocketAABB(rocketSpawnPos - widthHeightVec, rocketSpawnPos + widthHeightVec);
		    didCollide = bounds.CollisionCheck(rocketAABB);
        }
	}
   
	return didCollide;
}