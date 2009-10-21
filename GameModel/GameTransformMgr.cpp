#include "GameTransformMgr.h"
#include "GameModel.h"
#include "PlayerPaddle.h"

#include "../BlammoEngine/Camera.h"

// Time of level flip transform lerp
const double GameTransformMgr::SECONDS_TO_FLIP	= 0.8;

// Time of camera entering or exiting the paddle camera in seconds per unit distance travelled
// This should be a fairly small value or else it will be extremely hard for the player to keep track of the ball
const double GameTransformMgr::SECONDS_PER_UNIT_PADDLECAM = 0.025;

GameTransformMgr::GameTransformMgr() {
	this->Reset();
}

GameTransformMgr::~GameTransformMgr() {
	this->levelFlipAnimations.clear();
	this->paddleCamAnimations.clear();
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
	this->cameraInPaddle = false;
	this->cameraFOVAngle = Camera::FOV_ANGLE_IN_DEGS;

	this->levelFlipAnimations.clear();
	this->paddleCamAnimations.clear();
	this->camFOVAnimations.clear();
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
		animType = GameTransformMgr::FromPaddleCamAnimation;
	}

	// Add the paddle cam transform to the queue
	TransformAnimation transformAnim(animType);
	// Make sure we compress the last transform in the animation queue if it is
	// also a paddle camera transform
	if (this->animationQueue.size() > 0) {
		GameTransformMgr::TransformAnimationType previousType = this->animationQueue.back().type;
		if (previousType == GameTransformMgr::ToPaddleCamAnimation || previousType == GameTransformMgr::FromPaddleCamAnimation) {
			this->animationQueue.pop_back();
		}
	}
	this->animationQueue.push_back(transformAnim);
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
					bool finished = this->TickPaddleCamAnimation(dT);
					if (finished) {
						this->FinishPaddleCamAnimation(dT, gameModel);
					}
				}
				else {
					this->StartPaddleCamAnimation(dT, gameModel);
				}
				break;

			default:
				assert(false);
				break;
		}
	}

	// We need to intelligently update where the camera is in relation to the paddle,
	// if we're in paddle cam mode...
	if (this->cameraInPaddle) {

		// Calculate the location of the paddle in world space and get the FOV angle as well
		PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
		GameLevel* currLevel = gameModel.GetCurrentLevel();
		assert(paddle != NULL);
		assert(currLevel != NULL);
		
		Vector3D worldSpaceTranslation;
		this->GetPaddleCamPositionAndFOV(*paddle, currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight(), worldSpaceTranslation, this->cameraFOVAngle);
		Vector3D worldRotation(90, 0, 0);
		worldRotation = this->GetGameTransform() * worldRotation;

		// Update the camera position and rotation based on the movement of the paddle and the level transform
		this->currCamOrientation.SetTranslation(worldSpaceTranslation);
		this->currCamOrientation.SetRotation(worldRotation);
	}
}

/**
 * Private helper function to get the exact location of the paddle camera
 * in world space.
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
			iter++;
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
		paddle->AnimatePaddleFade(true, timeToMoveAnimate);
		
		// Make the items become partially translucent
		for (std::list<GameItem*>::iterator iter = items.begin(); iter != items.end(); iter++) {
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
		paddle->AnimatePaddleFade(false, timeToAnimate);

		// Make the items become opaque again
		for (std::list<GameItem*>::iterator iter = items.begin(); iter != items.end(); iter++) {
			(*iter)->AnimateItemFade(1.0f, timeToAnimate);
		}
		this->cameraInPaddle = false;
	}

	// The animation has now started
	paddleCamAnim.hasStarted = true;
}

/**
 * Private helper function for ticking the paddle camera - related animations.
 * Returns: true when all camera animations are complete, false otherwise.
 */
bool GameTransformMgr::TickPaddleCamAnimation(double dT) {
	assert(this->paddleCamAnimations.size() > 0);

	for (std::list<AnimationMultiLerp<Orientation3D>>::iterator iter = this->paddleCamAnimations.begin(); iter != this->paddleCamAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->paddleCamAnimations.erase(iter);
		}
		else {
			iter++;
		}
	}

	for (std::list<AnimationMultiLerp<float>>::iterator iter = this->camFOVAnimations.begin(); iter != this->camFOVAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->camFOVAnimations.erase(iter);
		}
		else {
			iter++;
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
		gameModel.GetPlayerPaddle()->SetPaddleCamera(true);
		this->cameraInPaddle = true;
	}
	else {
		gameModel.GetPlayerPaddle()->SetPaddleCamera(false);
		this->cameraInPaddle = false;
	}

	// Pop the animation off the queue
	this->animationQueue.pop_front();
}