/**
 * GameTransformMgr.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameTransformMgr.h"
#include "GameModel.h"
#include "PlayerPaddle.h"
#include "GameBall.h"
#include "CannonBlock.h"

#include "../BlammoEngine/Camera.h"

// Time of level flip transform lerp
const double GameTransformMgr::SECONDS_TO_FLIP						= 0.8;

// Time of camera entering or exiting the paddle camera in seconds per unit distance travelled
// This should be a fairly small value or else it will be extremely hard for the player to keep track of the ball
const double GameTransformMgr::SECONDS_PER_UNIT_PADDLECAM = 0.025;
// See above, except this applies to the ball camera
const double GameTransformMgr::SECONDS_PER_UNIT_BALLCAM		= 0.025;
// .. applies to the ball death cam
const double GameTransformMgr::SECONDS_PER_UNIT_DEATHCAM	= 0.015;

// Distance between the camera and ball when in ball death camera mode
const float GameTransformMgr::BALL_DEATH_CAM_DIST_TO_BALL = 20.0f;

GameTransformMgr::GameTransformMgr() : paddleWithCamera(NULL), ballWithCamera(NULL) {
	this->Reset();
}

GameTransformMgr::~GameTransformMgr() {
	this->Reset();
}

void GameTransformMgr::ClearSpecialCamEffects() {
	if (this->ballWithCamera != NULL) {
		this->ballWithCamera = NULL;
		GameBall::SetBallCamera(NULL);
	}

	if (this->paddleWithCamera != NULL) {
		this->paddleWithCamera->SetPaddleCamera(false, 0);
		this->paddleWithCamera = NULL;
	}

	this->cameraFOVAngle = Camera::FOV_ANGLE_IN_DEGS;
	this->paddleCamAnimations.clear();
	this->ballCamAnimations.clear();
	this->camFOVAnimations.clear();
}

/** 
 * Reset the entire game transform manager - eliminate all animations on go back to defaults
 * for all relevant values.
 */
void GameTransformMgr::Reset() {
	this->currGameDegRotX = 0.0f;
	this->currGameDegRotY = 0.0f;
	this->isFlipped = false;
	this->isBallDeathCamIsOn = false;

	this->ClearSpecialCamEffects();
	this->levelFlipAnimations.clear();
	this->ballDeathAnimations.clear();
	this->animationQueue.clear();
}

/**
 * Called to flip the game upside down (along the x-axis) - this will cause a 
 * quick, animated flipping of the level.
 */
void GameTransformMgr::FlipGameUpsideDown() {

	// Add the flip animation to the queue
	TransformAnimation transformAnim(GameTransformMgr::LevelFlipAnimation);
	// Make sure we compress the last transform in the animation queue if it is
	// also a paddle camera transform
	if (this->animationQueue.size() > 0) {
		if (this->animationQueue.back().type == GameTransformMgr::LevelFlipAnimation) {
			this->animationQueue.pop_back();
		}
	}
	this->animationQueue.push_back(transformAnim);
}

/**
 * This will either enable the paddle camera or stop the paddle camera.
 * Based on the given parameter this will make sure the transforms are made
 * to either place the camera in the paddle or take it out and put it in
 * its default, normal place.
 */
void GameTransformMgr::SetPaddleCamera(bool putCamInsidePaddle) {
	GameTransformMgr::TransformAnimationType animType;
	if (putCamInsidePaddle) {
		animType = GameTransformMgr::ToPaddleCamAnimation;
	}
	else {
		if (this->paddleWithCamera == NULL) {
			return;
		}
		animType = GameTransformMgr::FromPaddleCamAnimation;
	}

	// Add the paddle cam transform to the queue
	TransformAnimation transformAnim(animType);
	// Make sure we compress the last transform in the animation queue if it is also a paddle camera transform
	if (this->animationQueue.size() > 0) {
		GameTransformMgr::TransformAnimationType previousType = this->animationQueue.back().type;
		if (previousType == GameTransformMgr::ToPaddleCamAnimation || previousType == GameTransformMgr::FromPaddleCamAnimation) {
			this->animationQueue.pop_back();
		}
	}
	this->animationQueue.push_back(transformAnim);
}

/**
 * This will either enable the ball camera or stop the ball camera.
 * Based on the given parameter this will make sure the transforms are made
 * to either place the camera in the ball or take it out and put it in
 * its default, normal place.
 */
void GameTransformMgr::SetBallCamera(bool putCamInsideBall) {
	GameTransformMgr::TransformAnimationType animType;
	if (putCamInsideBall) {
		animType = GameTransformMgr::ToBallCamAnimation;	
	}
	else {
		if (this->ballWithCamera == NULL) {
			return;
		}
		animType = GameTransformMgr::FromBallCamAnimation;
	}

	// Add the ball cam transform to the queue
	TransformAnimation transformAnim(animType);
	// Make sure we compress the last transform in the animation queue if it is also a ball camera transform
	if (this->animationQueue.size() > 0) {
		GameTransformMgr::TransformAnimationType previousType = this->animationQueue.back().type;
		if (previousType == GameTransformMgr::ToBallCamAnimation || previousType == GameTransformMgr::FromBallCamAnimation) {
			this->animationQueue.pop_back();
		}
	}
	this->animationQueue.push_back(transformAnim);
}

/**
 * This will either enable the camera that focuses on the last ball as it dies or
 * takes the focus off the last dieing ball.
 * This will obliterate all other transforms occuring in the transform manager
 * and ONLY play the animation for the camera at ball death.
 */
void GameTransformMgr::SetBallDeathCamera(bool turnOnBallDeathCam) {
	GameTransformMgr::TransformAnimationType animType;
	if (turnOnBallDeathCam) {
		animType = GameTransformMgr::ToBallDeathAnimation;
		// Clear up all other transforms that may manipulate the camera...
		this->ClearSpecialCamEffects();
	}
	else {
		animType = GameTransformMgr::FromBallDeathAnimation;
	}

	TransformAnimation transformAnim(animType);
	this->animationQueue.push_front(transformAnim);
}

/**
 * This needs to be called whenever a new level is being loaded - this will make sure
 * that the default camera position is calculated and made available to the view.
 */
void GameTransformMgr::SetupLevelCameraDefaultPosition(const GameLevel& level) {
	// Calculate the distance along the z axis that the camera needs to be from the origin in order
	// to see the entire level - this will be the default translation for the camera
	float distance = std::max<float>(level.GetLevelUnitHeight(), level.GetLevelUnitWidth()) / (2.0f * tanf(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS * 0.5f))) + 5.0f;
	
	// Telling the world to translate by (0, 0, -distance), telling the camera to translate by (0, 0, distance)
	this->defaultCamOrientation = Orientation3D(Vector3D(0, 0, distance), Vector3D(0,0,0));
	this->currCamOrientation		= this->defaultCamOrientation;
}

/**
 * This will cause interpolation among any transforms that are taking
 * place within this class.
 */
void GameTransformMgr::Tick(double dT, GameModel& gameModel) {

	// Figure out the next animation we should be animating, and animate it (if there are any)
	if (this->animationQueue.size() > 0) {
		TransformAnimation currAnimation = this->animationQueue.front();
		switch (currAnimation.type) {
			
			case GameTransformMgr::LevelFlipAnimation:
				if (currAnimation.hasStarted) {
					bool finished = this->TickLevelFlipAnimation(dT);
					if (finished) {
						this->FinishLevelFlipAnimaiton(dT, gameModel);
					}
				}
				else {
					this->StartLevelFlipAnimation(dT, gameModel);
				}
				break;

			case GameTransformMgr::ToPaddleCamAnimation:
			case GameTransformMgr::FromPaddleCamAnimation:
				if (currAnimation.hasStarted) {
					bool finished = this->TickPaddleCamAnimation(dT, gameModel);
					if (finished) {
						this->FinishPaddleCamAnimation(dT, gameModel);
					}
				}
				else {
					this->StartPaddleCamAnimation(dT, gameModel);
				}
				break;

			case GameTransformMgr::ToBallCamAnimation:
			case GameTransformMgr::FromBallCamAnimation:
				if (currAnimation.hasStarted) {
					bool finished = this->TickBallCamAnimation(dT, gameModel);
					if (finished) {
						this->FinishBallCamAnimation(dT, gameModel);
					}
				}
				else {
					this->StartBallCamAnimation(dT, gameModel);
				}
				break;

			case GameTransformMgr::ToBallDeathAnimation:
			case GameTransformMgr::FromBallDeathAnimation:
				if (currAnimation.hasStarted) {
					bool finished = this->TickBallDeathAnimation(dT, gameModel);
					if (finished) {
						this->FinishBallDeathAnimation(dT, gameModel);
					}
				}
				else {
					this->StartBallDeathAnimation(dT, gameModel);
				}
				break;

			default:
				assert(false);
				break;
		}
	}

	// We need to intelligently update where the camera is in relation to the paddle/ball/...,
	if (this->isBallDeathCamIsOn) {
		// Ball death cam takes priority over all other camera effects...
		// Follow the ball with the camera...
		assert(gameModel.GetGameBalls().size() == 1);
		const GameBall* deathBall = *(gameModel.GetGameBalls().begin());
	
		// Get the ball velocity and current camera worldspace position/translation
		Vector2D ballVelocity2D = 0.8f * deathBall->GetVelocity();
		Vector3D currCamTranslation = this->currCamOrientation.GetTranslation();
		
		// Calculate the distance the ball has moved so far (if it were travelling in a straight line), in worldspace
		Point3D currCamPosition   = Point3D(currCamTranslation[0], currCamTranslation[1], currCamTranslation[2]);
		Vector3D ballMoveDistance	= dT * Vector3D(ballVelocity2D[0], ballVelocity2D[1], 0.0f);
		ballMoveDistance = this->GetGameTransform() * ballMoveDistance; 

		currCamPosition  = currCamPosition + ballMoveDistance;
		this->currCamOrientation.SetTranslation(Vector3D(currCamPosition[0], currCamPosition[1], currCamPosition[2]));
	}
	else if (this->paddleWithCamera != NULL) {
		// Calculate the location of the paddle in world space and get the FOV angle as well
		GameLevel* currLevel = gameModel.GetCurrentLevel();
		assert(currLevel != NULL);
		
		Vector3D worldSpaceTranslation;
		this->GetPaddleCamPositionAndFOV(*this->paddleWithCamera, currLevel->GetLevelUnitWidth(), 
																			currLevel->GetLevelUnitHeight(), worldSpaceTranslation, 
																			this->cameraFOVAngle);

		Vector3D worldRotation(90, 0, 0);
		worldRotation = this->GetGameTransform() * worldRotation;	// TODO: Make this more versatile based on whether the paddle is being hit...
																															// rotate the y-axis to move the view point when the paddle is discombobulated!

		// Update the camera position and rotation based on the movement of the paddle and the level transform
		this->currCamOrientation.SetTranslation(worldSpaceTranslation);
		this->currCamOrientation.SetRotation(worldRotation);

	}
	else if (this->ballWithCamera != NULL) {
		// Calculate the location of the ball in world space
		GameLevel* currLevel = gameModel.GetCurrentLevel();
		assert(currLevel != NULL);

		Vector3D worldSpaceTranslation;
		this->GetBallCamPositionAndFOV(*this->ballWithCamera, currLevel->GetLevelUnitWidth(), 
																	 currLevel->GetLevelUnitHeight(), worldSpaceTranslation,
																	 this->cameraFOVAngle);

		// TODO: make the rotation a little more creative...
		Vector3D worldRotation(-90, 0, 0);
		worldRotation = this->GetGameTransform() * worldRotation;

		// Update the camera position and rotation based on the movement of the ball and the level transform
		this->currCamOrientation.SetTranslation(worldSpaceTranslation);
		this->currCamOrientation.SetRotation(worldRotation);
	}

}

/**
 * Private helper function to get the exact location of the paddle camera in world space.
 */
void GameTransformMgr::GetPaddleCamPositionAndFOV(const PlayerPaddle& paddle, float levelWidth, float levelHeight, Vector3D& paddleCamPos, float& fov) {
	const float DIST_FROM_PADDLE = paddle.GetHalfWidthTotal();

	Point2D paddleCamPosition = paddle.GetCenterPosition() - Vector2D(0, paddle.GetHalfHeight() + DIST_FROM_PADDLE);
	Vector2D halfLevelDim	= 0.5 * Vector2D(levelWidth, levelHeight);
	Point2D  worldSpacePaddlePos = paddleCamPosition - halfLevelDim;
	Vector3D worldSpaceTranslation(worldSpacePaddlePos[0], worldSpacePaddlePos[1], 0.0f);
	
	paddleCamPos = this->GetGameTransform() * worldSpaceTranslation;
	fov = 2.0f * Trig::radiansToDegrees(atan(paddle.GetHalfWidthTotal() / DIST_FROM_PADDLE));
}

/**
 * Private helper function to get the exact location of the ball camera in world space.
 */
void GameTransformMgr::GetBallCamPositionAndFOV(const GameBall& ball, float levelWidth, float levelHeight, Vector3D& ballCamPos, float& fov) {
	Collision::Circle2D ballBounds = ball.GetBounds();

	// Position the ball camera almost exactly at the bottom of the ball (we make sure we're at least a bit above that
	// so that we don't get wierdo clipping issues)
	Point2D  ballCamPosition = ballBounds.Center() - Vector2D(0, 0.8f*ballBounds.Radius());
	Vector2D halfLevelDim	= 0.5 * Vector2D(levelWidth, levelHeight);
	Point2D  worldSpaceBallPos = ballCamPosition - halfLevelDim;
	Vector3D worldSpaceTranslation(worldSpaceBallPos[0], worldSpaceBallPos[1], 0.0f);

	ballCamPos = this->GetGameTransform() * worldSpaceTranslation;
	fov = 2.0f * Trig::radiansToDegrees(atan(ballBounds.Radius() / GameBall::DEFAULT_BALL_RADIUS));
}

/**
 * Grab the current transform for the game - this is a composite matrix
 * of all the transforms that are currently being applied - the transforms
 * are contactenated in the most sane and consistent way possible.
 */
Matrix4x4 GameTransformMgr::GetGameTransform() const {
	Matrix4x4 rotX = Matrix4x4::rotationMatrix('x', this->currGameDegRotX, true);
	Matrix4x4 rotY = Matrix4x4::rotationMatrix('y', this->currGameDegRotY, true);

	return rotY * rotX;
}

/**
 * Grab the current transform for the camera.
 */
Matrix4x4 GameTransformMgr::GetCameraTransform() const {

	// If we're in ball camera mode and the ball is in a cannon block then we rotate
	// the camera with the cannon block...
	if (this->ballWithCamera != NULL) {
		const CannonBlock* cannon = this->ballWithCamera->GetCannonBlock();
		if (cannon != NULL) {
			const Vector3D& currCamRot = this->currCamOrientation.GetRotation();
			return (Matrix4x4::translationMatrix(this->currCamOrientation.GetTranslation()) *
							Matrix4x4::rotationMatrix('z', 90 + cannon->GetCurrentCannonAngleInDegs(), true) *
							Matrix4x4::rotationMatrix('x', currCamRot[0], true)).inverse();
		}
	}

	// The inverse is returned because the camera transform is applied to the world matrix
	return this->currCamOrientation.GetTransform().inverse();
}

/**
 * Get the current camera Field of View angle.
 */
float GameTransformMgr::GetCameraFOVAngle() const {
	return this->cameraFOVAngle;
}

void GameTransformMgr::StartLevelFlipAnimation(double dT, GameModel& gameModel) {
	// Grab the current transform animation information from the front of the queue
	TransformAnimation& levelFlipAnim = this->animationQueue.front();
	assert(levelFlipAnim.type == GameTransformMgr::LevelFlipAnimation);

	// Find any existing animations and hault them elegantly
	this->levelFlipAnimations.clear();

	// Setup the animations for flipping
	int finalDegRotX = this->isFlipped ? 0 : 180;
	double percentFlippedX = abs(finalDegRotX - this->currGameDegRotX) / 180.0;
	double timeToFlipX = percentFlippedX * GameTransformMgr::SECONDS_TO_FLIP;
	AnimationLerp<float> flipAnimationRotX(&this->currGameDegRotX);
	flipAnimationRotX.SetLerp(timeToFlipX, finalDegRotX);
	
	int finalDegRotY = finalDegRotX;
	double percentFlippedY = abs(finalDegRotY - this->currGameDegRotY) / 180.0;
	double timeToFlipY = percentFlippedY * GameTransformMgr::SECONDS_TO_FLIP;
	AnimationLerp<float> flipAnimationRotY(&this->currGameDegRotY);
	flipAnimationRotY.SetLerp(timeToFlipY, finalDegRotY);

	// Add the new animations to the animation list for the flip and save the
	// flip state that we're in now
	this->levelFlipAnimations.push_back(flipAnimationRotX);
	this->levelFlipAnimations.push_back(flipAnimationRotY);
	this->isFlipped = !this->isFlipped;

	// The animation has now started
	levelFlipAnim.hasStarted = true;
}

/**
 * Private helper function for ticking the level flip animations.
 * Returns: true once the animation is complete, false otherwise.
 */
bool GameTransformMgr::TickLevelFlipAnimation(double dT) {
	for (std::list<AnimationLerp<float>>::iterator iter = this->levelFlipAnimations.begin(); iter != this->levelFlipAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->levelFlipAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}

	return this->levelFlipAnimations.size() == 0;
}

/**
 * Call this to stop level flip animations. This will eliminate
 * the current level flip animation from the animation queue as well.
 */
void GameTransformMgr::FinishLevelFlipAnimaiton(double dT, GameModel& gameModel) {
	// Pop the animation off the queue
	this->animationQueue.pop_front();
}

/**
 * Starts the paddle camera animation. This will kick off transforms required to
 * get into or come out of paddle camera mode.
 */
void GameTransformMgr::StartPaddleCamAnimation(double dT, GameModel& gameModel) {
	// Grab the current transform animation information from the front of the queue
	TransformAnimation& paddleCamAnim = this->animationQueue.front();
	assert(paddleCamAnim.type == GameTransformMgr::ToPaddleCamAnimation || paddleCamAnim.type == GameTransformMgr::FromPaddleCamAnimation);
	
	// Make sure the game state is paused while we perform the paddle camera animations
	gameModel.SetPause(GameModel::PauseState);

	// Clear any previous paddle camera animations
	this->paddleCamAnimations.clear();

	// Calculate the paddle's world space position
	PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	GameLevel* currLevel = gameModel.GetCurrentLevel();
	std::list<GameItem*>& items = gameModel.GetLiveItems();
	assert(paddle != NULL);
	assert(currLevel != NULL);

	// Need to figure out where the paddle is in world space...
	Vector2D halfLevelDim	= 0.5 * Vector2D(currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight());
	Point2D paddleWorldSpacePos = paddle->GetCenterPosition() - halfLevelDim;
	Vector3D worldSpacePaddlePos(paddleWorldSpacePos[0], paddleWorldSpacePos[1], 0.0f);
	worldSpacePaddlePos = this->GetGameTransform() * worldSpacePaddlePos;
	
	if (paddleCamAnim.type == GameTransformMgr::ToPaddleCamAnimation) {
		// We are going into paddle camera mode... animate the camera from whereever
		// it currently is, into the paddle and animate its orientation appropriately

		// We want to be in the paddle, looking up at the level
		float finalCamRotationXDegs = this->isFlipped ? -90.0f : 90.0f;
		Orientation3D intermediateOrient(worldSpacePaddlePos, Vector3D(0, 0, 0));

		// The values for the camera orientation during the paddle camera animation...
		// We want the camera to go into the paddle and then turn towards the rest of the
		// level while moving down to encompass the paddle in the screen perfectly
		Vector3D finalPos;
		float finalFOVAngle;
		this->GetPaddleCamPositionAndFOV(*paddle, currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight(), finalPos, finalFOVAngle);
		Orientation3D finalOrientation(finalPos, Vector3D(finalCamRotationXDegs, 0, 0));

		std::vector<Orientation3D> orientationValues;
		orientationValues.reserve(3);
		orientationValues.push_back(this->currCamOrientation);
		orientationValues.push_back(intermediateOrient);
		orientationValues.push_back(finalOrientation);
		
		// Calculate the distance that will be travelled by the camera
		Vector3D travelVec = finalOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
		float distToTravel = travelVec.length();
		double timeToMoveAnimate   = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_PADDLECAM;
		double timeToRotateAnimate = timeToMoveAnimate / 2.0;
		double totalTimeToAnimate  = timeToMoveAnimate + timeToRotateAnimate;

		// Create the animation to get from whereever the camera currently is to the paddle
		// looking at the level
		std::vector<double> timeValues;
		timeValues.reserve(3);
		timeValues.push_back(0.0);
		timeValues.push_back(timeToMoveAnimate);
		timeValues.push_back(totalTimeToAnimate);

		AnimationMultiLerp<Orientation3D> toPaddleCamAnim(&this->currCamOrientation);
		toPaddleCamAnim.SetRepeat(false);
		toPaddleCamAnim.SetLerp(timeValues, orientationValues);
		
		this->paddleCamAnimations.push_back(toPaddleCamAnim);
		
		// The field of view angle of the camera must change to make sure it encompases the full size of the paddle
		std::vector<float> fovValues;
		fovValues.push_back(this->cameraFOVAngle);
		fovValues.push_back(this->cameraFOVAngle);
		fovValues.push_back(finalFOVAngle);
		AnimationMultiLerp<float> camFOVChangeAnim(&this->cameraFOVAngle);
		camFOVChangeAnim.SetLerp(timeValues, fovValues);
		this->camFOVAnimations.push_back(camFOVChangeAnim);

		// Make the paddle disappear
		paddle->AnimateFade(true, timeToMoveAnimate);
		
		// Make the items become partially translucent
		for (std::list<GameItem*>::iterator iter = items.begin(); iter != items.end(); ++iter) {
			(*iter)->AnimateItemFade(GameItem::ALPHA_ON_PADDLE_CAM, totalTimeToAnimate);
		}
	}
	else {
		// We are going out of paddle camera mode back to default - take the camera
		// from whereever it is currently and move it back to the default position
		AnimationMultiLerp<Orientation3D> toDefaultCamAnim(&this->currCamOrientation);

		// Calculate the distance that will be travelled by the camera
		Vector3D travelVec = this->defaultCamOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
		float distToTravel = travelVec.length();
		double timeToAnimate = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_PADDLECAM;
		
		toDefaultCamAnim.SetLerp(timeToAnimate, this->defaultCamOrientation);
		this->paddleCamAnimations.push_back(toDefaultCamAnim);

		// Make the camera's field of view angle normal again
		AnimationMultiLerp<float> camFOVChangeAnim(&this->cameraFOVAngle);
		camFOVChangeAnim.SetLerp(timeToAnimate, Camera::FOV_ANGLE_IN_DEGS);
		this->camFOVAnimations.push_back(camFOVChangeAnim);

		// Make the paddle reappear
		paddle->AnimateFade(false, timeToAnimate);

		// Make the items become opaque again
		for (std::list<GameItem*>::iterator iter = items.begin(); iter != items.end(); ++iter) {
			(*iter)->AnimateItemFade(1.0f, timeToAnimate);
		}
		this->paddleWithCamera = NULL;
	}

	// The animation has now started
	paddleCamAnim.hasStarted = true;
}

/**
 * Private helper function for ticking the paddle camera - related animations.
 * Returns: true when all camera animations are complete, false otherwise.
 */
bool GameTransformMgr::TickPaddleCamAnimation(double dT, GameModel& gameModel) {
	assert(this->paddleCamAnimations.size() > 0);

	// Animate the paddle itself
	PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	assert(paddle != NULL);
	paddle->Animate(dT);

	// Animate any camera-related animations for the paddle cam
	for (std::list<AnimationMultiLerp<Orientation3D>>::iterator iter = this->paddleCamAnimations.begin(); iter != this->paddleCamAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->paddleCamAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}

	for (std::list<AnimationMultiLerp<float>>::iterator iter = this->camFOVAnimations.begin(); iter != this->camFOVAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->camFOVAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}

	return this->paddleCamAnimations.size() == 0 && this->camFOVAnimations.size() == 0;
}

/**
 * Call this to stop paddle camera animations. This will eliminate
 * the current paddle camera animation from the animation queue as well.
 */
void GameTransformMgr::FinishPaddleCamAnimation(double dT, GameModel& gameModel) {
	// Grab the current transform animation information from the front of the queue
	TransformAnimation& paddleCamAnim = this->animationQueue.front();
	assert(paddleCamAnim.type == GameTransformMgr::ToPaddleCamAnimation || paddleCamAnim.type == GameTransformMgr::FromPaddleCamAnimation);
		
	// Unpause the game state since the camera should now be in the paddle
	gameModel.UnsetPause(GameModel::PauseState);

	// Tell the paddle whether it's in or out of paddle camera mode
	if (paddleCamAnim.type == GameTransformMgr::ToPaddleCamAnimation) {
		this->paddleWithCamera = gameModel.GetPlayerPaddle();
		this->paddleWithCamera->SetPaddleCamera(true, dT);
	}
	else {
		gameModel.GetPlayerPaddle()->SetPaddleCamera(false, dT);
		this->paddleWithCamera = NULL;
	}

	// Pop the animation off the queue
	this->animationQueue.pop_front();
}

void GameTransformMgr::StartBallCamAnimation(double dT, GameModel& gameModel) {
	// Grab the current transform animation information from the front of the queue
	TransformAnimation& ballCamAnim = this->animationQueue.front();
	assert(ballCamAnim.type == GameTransformMgr::ToBallCamAnimation || ballCamAnim.type == GameTransformMgr::FromBallCamAnimation);
	
	// Make sure the game state is paused while we perform the ball camera animations
	gameModel.SetPause(GameModel::PauseState);

	// Clear any previous ball camera animations
	this->ballCamAnimations.clear();

	// Grab the affected ball position and other info, make sure the relevant values are in world coordinates
	GameBall* ball = *(gameModel.GetGameBalls().begin());
	const GameLevel* currLevel = gameModel.GetCurrentLevel();
	std::list<GameItem*>& items = gameModel.GetLiveItems();
	assert(ball != NULL && currLevel != NULL);

	Point2D ballPos2D = ball->GetBounds().Center();
	Vector2D halfLevelDim	= 0.5 * Vector2D(currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight());
	Point2D ballWorldSpacePos = ballPos2D - halfLevelDim;

	// This will store the actual world space position that we need to transform the camera to...
	Vector3D worldSpaceBallPos(ballWorldSpacePos[0], ballWorldSpacePos[1], 0.0f);
	worldSpaceBallPos = this->GetGameTransform() * worldSpaceBallPos;

	if (ballCamAnim.type == GameTransformMgr::ToBallCamAnimation) {
		// We are going into ball camera mode... animate the camera from whereever
		// it currently is, into the ball and animate its orientation appropriately

		// We want to be in the ball, looking down at the paddle
		float finalCamRotationXDegs = this->isFlipped ? 90.0f : -90.0f;

		// The values for the camera orientation during the ball camera animation...
		// We want the camera to go into the ball and then turn towards the paddle
		Vector3D finalPos;
		float finalFOVAngle;
		this->GetBallCamPositionAndFOV(*ball, currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight(), finalPos, finalFOVAngle);
		
		Orientation3D intermediateOrientation(worldSpaceBallPos, Vector3D(0, 0, 0));
		Orientation3D finalOrientation(finalPos, Vector3D(finalCamRotationXDegs, 0, 0));

		std::vector<Orientation3D> orientationValues;
		orientationValues.reserve(3);
		orientationValues.push_back(this->currCamOrientation);
		orientationValues.push_back(intermediateOrientation);
		orientationValues.push_back(finalOrientation);
		
		// Calculate the distance that will be travelled by the camera
		Vector3D travelVec = finalOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
		float distToTravel = travelVec.length();
		double timeToMoveAnimate   = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_BALLCAM;
		double timeToRotateAnimate = timeToMoveAnimate / 2.0;
		double totalTimeToAnimate  = timeToMoveAnimate + timeToRotateAnimate;

		// Create the animation to get from whereever the camera currently is to the paddle
		// looking at the level
		std::vector<double> timeValues;
		timeValues.reserve(3);
		timeValues.push_back(0.0);
		timeValues.push_back(timeToMoveAnimate);
		timeValues.push_back(totalTimeToAnimate);

		AnimationMultiLerp<Orientation3D> toBallCamAnim(&this->currCamOrientation);
		toBallCamAnim.SetRepeat(false);
		toBallCamAnim.SetLerp(timeValues, orientationValues);
		
		this->ballCamAnimations.push_back(toBallCamAnim);

		// The field of view angle of the camera must change to make sure it suits the ball camera
		std::vector<float> fovValues;
		fovValues.push_back(this->cameraFOVAngle);
		fovValues.push_back(this->cameraFOVAngle);
		fovValues.push_back(finalFOVAngle);
		AnimationMultiLerp<float> camFOVChangeAnim(&this->cameraFOVAngle);
		camFOVChangeAnim.SetLerp(timeValues, fovValues);
		this->camFOVAnimations.push_back(camFOVChangeAnim);

		// Make the ball disappear
		ball->AnimateFade(true, timeToMoveAnimate);

		// Make the items become partially translucent
		for (std::list<GameItem*>::iterator iter = items.begin(); iter != items.end(); ++iter) {
			(*iter)->AnimateItemFade(GameItem::ALPHA_ON_BALL_CAM, totalTimeToAnimate);
		}
	}
	else {
		// We are going out of ball camera mode back to default - take the camera
		// from whereever it is currently and move it back to the default position
		AnimationMultiLerp<Orientation3D> toDefaultCamAnim(&this->currCamOrientation);

		// Calculate the distance that will be travelled by the camera
		Vector3D travelVec = this->defaultCamOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
		float distToTravel = travelVec.length();
		double timeToAnimate = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_BALLCAM;
		
		toDefaultCamAnim.SetLerp(timeToAnimate, this->defaultCamOrientation);
		this->ballCamAnimations.push_back(toDefaultCamAnim);

		// Make the camera's field of view angle normal again
		AnimationMultiLerp<float> camFOVChangeAnim(&this->cameraFOVAngle);
		camFOVChangeAnim.SetLerp(timeToAnimate, Camera::FOV_ANGLE_IN_DEGS);
		this->camFOVAnimations.push_back(camFOVChangeAnim);

		// Make the ball reappear
		ball->AnimateFade(false, timeToAnimate);

		// Make the items become opaque again
		for (std::list<GameItem*>::iterator iter = items.begin(); iter != items.end(); ++iter) {
			(*iter)->AnimateItemFade(1.0f, timeToAnimate);
		}

		this->ballWithCamera = NULL;
	}

	// The animation has now started
	ballCamAnim.hasStarted = true;
}

/**
 * Private helper function that ticks all the animations associated with the ball camera.
 * Returns: true if all animations are complete, false otherwise.
 */
bool GameTransformMgr::TickBallCamAnimation(double dT, GameModel& gameModel) {
	assert(this->ballCamAnimations.size() > 0);
	
	// Animate the ball itself
	GameBall* ball = *(gameModel.GetGameBalls().begin());
	assert(ball != NULL);
	ball->Animate(dT);

	// Animate the camera
	for (std::list<AnimationMultiLerp<Orientation3D>>::iterator iter = this->ballCamAnimations.begin(); iter != this->ballCamAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->ballCamAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}

	// Animate the field of view angle
	for (std::list<AnimationMultiLerp<float>>::iterator iter = this->camFOVAnimations.begin(); iter != this->camFOVAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->camFOVAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}
	
	return (this->ballCamAnimations.size() == 0) && (this->camFOVAnimations.size() == 0);
}

/**
 * Private helper function that makes sure all is well at the end of the animation for going into/out of the
 * ball camera mode.
 */
void GameTransformMgr::FinishBallCamAnimation(double dT, GameModel& gameModel) {
	// Grab the current transform animation information from the front of the queue
	TransformAnimation& ballCamAnim = this->animationQueue.front();
	assert(ballCamAnim.type == GameTransformMgr::ToBallCamAnimation || ballCamAnim.type == GameTransformMgr::FromBallCamAnimation);
		
	// Unpause the game state since the camera should now be in the ball
	gameModel.UnsetPause(GameModel::PauseState);

	if (ballCamAnim.type == GameTransformMgr::ToBallCamAnimation) {
		this->ballWithCamera = *(gameModel.GetGameBalls().begin());
		GameBall::SetBallCamera(this->ballWithCamera);
	}
	else {
		this->ballWithCamera = NULL;
		GameBall::SetBallCamera(NULL);
	}

	// Pop the animation off the queue
	this->animationQueue.pop_front();
}

bool GameTransformMgr::TickBallDeathAnimation(double dT, GameModel& gameModel) {
	assert(this->ballDeathAnimations.size() > 0);

	// Animate the camera
	for (std::list<AnimationMultiLerp<Orientation3D>>::iterator iter = this->ballDeathAnimations.begin(); iter != this->ballDeathAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->ballDeathAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}

	// Animate the field of view angle
	for (std::list<AnimationMultiLerp<float>>::iterator iter = this->camFOVAnimations.begin(); iter != this->camFOVAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->camFOVAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}
	
	return (this->ballDeathAnimations.size() == 0 && this->camFOVAnimations.size() == 0);
}

void GameTransformMgr::StartBallDeathAnimation(double dT, GameModel& gameModel) {
	// Grab the current transform animation information from the front of the queue
	TransformAnimation& ballDeathAnim = this->animationQueue.front();
	assert(ballDeathAnim.type == GameTransformMgr::ToBallDeathAnimation || ballDeathAnim.type == GameTransformMgr::FromBallDeathAnimation);
	assert(gameModel.GetGameBalls().size() == 1);

	// Clear any previous ball death animations
	this->ballDeathAnimations.clear();

	// Based on the animation, set the animations for the camera's orientation
	if (ballDeathAnim.type == GameTransformMgr::ToBallDeathAnimation) {
		const GameBall* ball = *(gameModel.GetGameBalls().begin());
		const GameLevel* currLevel = gameModel.GetCurrentLevel();

		// Make sure the game state is paused while we move the camera
		// into position to properly capture the ball death animation
		gameModel.SetPause(GameModel::PauseState);

		// Move the camera down to keep the ball in the center of the viewport
		Point2D ballPos2D = ball->GetBounds().Center();
		Vector2D halfLevelDim	= 0.5 * Vector2D(currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight());
		Point2D ballLevelSpacePos = ballPos2D - halfLevelDim;

		// This will store the actual world space position that we need to transform the camera to...
		Vector3D finalCamPosition(ballLevelSpacePos[0], ballLevelSpacePos[1], GameTransformMgr::BALL_DEATH_CAM_DIST_TO_BALL);
		finalCamPosition = this->GetGameTransform() * finalCamPosition;
		Orientation3D finalOrientation(finalCamPosition, Vector3D(0, 0, 0));

		// Figure out the time to travel
		Vector3D travelVec = this->currCamOrientation.GetTranslation() - finalCamPosition;
		float distToTravel = travelVec.length();
		double timeToAnimate = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_DEATHCAM;
			
		// Center the ball on the screen
		std::vector<Orientation3D> orients;
		orients.reserve(2);
		orients.push_back(this->currCamOrientation);
		orients.push_back(finalOrientation);
		std::vector<double> timeVals;
		timeVals.reserve(2);
		timeVals.push_back(0);
		timeVals.push_back(timeToAnimate);

		AnimationMultiLerp<Orientation3D> toBallDeathAnim(&this->currCamOrientation);
		toBallDeathAnim.SetLerp(timeVals, orients);
		toBallDeathAnim.SetRepeat(false);
		this->ballDeathAnimations.push_back(toBallDeathAnim);


		// Make the camera's field of view angle normal again
		AnimationMultiLerp<float> camFOVChangeAnim(&this->cameraFOVAngle);
		camFOVChangeAnim.SetLerp(timeToAnimate, Camera::FOV_ANGLE_IN_DEGS);
		this->camFOVAnimations.push_back(camFOVChangeAnim);
	}
	else {
		// Leaving the death cam back to the default camera
		AnimationMultiLerp<Orientation3D> toDefaultCamAnim(&this->currCamOrientation);

		// Calculate the distance that will be travelled by the camera
		Vector3D travelVec = this->defaultCamOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
		float distToTravel = travelVec.length();
		double timeToAnimate = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_DEATHCAM;
		
		toDefaultCamAnim.SetLerp(timeToAnimate, this->defaultCamOrientation);
		this->ballDeathAnimations.push_back(toDefaultCamAnim);
	}

	ballDeathAnim.hasStarted = true;
}

void GameTransformMgr::FinishBallDeathAnimation(double dT, GameModel& gameModel) {
	// Grab the current transform animation information from the front of the queue
	TransformAnimation& ballDeathAnim = this->animationQueue.front();
	assert(ballDeathAnim.type == GameTransformMgr::ToBallDeathAnimation || ballDeathAnim.type == GameTransformMgr::FromBallDeathAnimation);
	
	if (ballDeathAnim.type == GameTransformMgr::ToBallDeathAnimation) {
		// Unpause the game state to watch the ball spiral to its doom
		gameModel.UnsetPause(GameModel::PauseState);
		this->isBallDeathCamIsOn = true;
	}
	else {
		this->isBallDeathCamIsOn = false;
	}

	// Pop the animation off the queue
	this->animationQueue.pop_front();
}