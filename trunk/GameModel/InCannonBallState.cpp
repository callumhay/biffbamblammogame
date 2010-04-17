
#include "InCannonBallState.h"
#include "GameBall.h"
#include "CannonBlock.h"

InCannonBallState::InCannonBallState(GameBall* ball, CannonBlock* cannonBlock, BallState* prevState) : 
BallState(ball), cannonBlock(cannonBlock), previousState(prevState) {
	assert(cannonBlock != NULL);
	assert(prevState != NULL);

	// Set the ball to be perfectly centered inside the cannon
	this->gameBall->SetCenterPosition(cannonBlock->GetCenter());

	// Make sure the ball is invisible (it's supposed to be "inside" the cannon now)
	// TODO: BE CAREFUL HERE!!!

	// The ball also should be ignoring collisions until it's fired back out of the cannon
	this->gameBall->SetBallBallCollisionsDisabled();
	this->gameBall->SetBallBlockCollisionsDisabled();
}

InCannonBallState::~InCannonBallState() {	
	// Special case: The game was closed while a ball was still inside a cannon block,
	// we need to delete the previous state as well...
	if (this->previousState != NULL) {
		delete this->previousState;
		this->previousState = NULL;
	}

}

BallState* InCannonBallState::Clone(GameBall* newBall) const {
	return new InCannonBallState(newBall, this->cannonBlock, this->previousState->Clone(newBall));
}

void InCannonBallState::Tick(double seconds, const Vector2D& worldSpaceGravityDir) {
	// 'Tick' the cannon to spin the ball around inside it... eventually the function will say
	// it has 'fired' the ball
	bool cannonHasFired = this->cannonBlock->RotateAndEventuallyFire(seconds);

	if (cannonHasFired) {		
		// Set the velocity in the direction the cannon has fired in
		Vector2D cannonFireDir = this->cannonBlock->GetCurrentCannonDirection();
		this->gameBall->SetVelocity(this->gameBall->GetSpeed(), cannonFireDir);

		// Move the ball so that it's outside the barrel of the cannon entirely - it will
		// currently be perfectly centered inside the cannon, so just move it along the cannon
		// firing vector by the correct amount...
		Collision::Circle2D ballBounds = this->gameBall->GetBounds();
		float moveBallAmt = CannonBlock::HALF_CANNON_BARREL_LENGTH + ballBounds.Radius();
		this->gameBall->SetCenterPosition(ballBounds.Center() + moveBallAmt * cannonFireDir);

		// Restore ball state information that was changed during this state's operations
		this->gameBall->SetBallBallCollisionsEnabled();
		this->gameBall->SetBallBlockCollisionsEnabled();

		// We use this to ensure that the destructor knows not to delete the previous state as well
		BallState* prevStateTemp = this->previousState;
		this->previousState = NULL;

		// Change the state of the ball back to what it was before this state...
		this->gameBall->SetBallState(prevStateTemp, true);	// WARNING: THIS WILL DELETE THIS OBJECT, SO GET OUT NOW

		return;
	}
}