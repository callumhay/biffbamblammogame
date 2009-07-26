#include "GameTransformMgr.h"
#include "GameModel.h"
#include "PlayerPaddle.h"

#include "../BlammoEngine/Camera.h"

// Time of level flip transform lerp
const double GameTransformMgr::SECONDS_TO_FLIP	= 1.0;	
// Time of camera entering or exiting the paddle camera mode
const double GameTransformMgr::SECONDS_TO_ENTEROREXIT_PADDLECAM = 0.75;

GameTransformMgr::GameTransformMgr() : 
currGameDegRotX(0.0f), currGameDegRotY(0.0f), isFlipped(false) {
}

GameTransformMgr::~GameTransformMgr() {
}

/**
 * Called to flip the game upside down (along the x-axis) - this will cause a 
 * quick, animated flipping of the level.
 */
void GameTransformMgr::FlipGameUpsideDown() {
	// Find any existing animations and hault them elegantly
	this->animations.erase(UpsideDownFlip);

	// Setup the animations for flipping
	int finalDegRotX = this->isFlipped ? 0 : 180;
	double percentFlippedX = abs(finalDegRotX - this->currGameDegRotX) / 180.0;
	AnimationLerp<float> flipAnimationRotX(&this->currGameDegRotX);
	flipAnimationRotX.SetLerp(percentFlippedX * GameTransformMgr::SECONDS_TO_FLIP, finalDegRotX);
	
	int finalDegRotY = this->isFlipped ? 0 : 180;
	double percentFlippedY = abs(finalDegRotY - this->currGameDegRotY) / 180.0;
	AnimationLerp<float> flipAnimationRotY(&this->currGameDegRotY);
	flipAnimationRotY.SetLerp(percentFlippedY * GameTransformMgr::SECONDS_TO_FLIP, finalDegRotY);
	
	// Add the new animations to the animation list for the flip and save the
	// flip state that we're in now
	this->animations[UpsideDownFlip].push_back(flipAnimationRotX);
	this->animations[UpsideDownFlip].push_back(flipAnimationRotY);
	this->isFlipped = !this->isFlipped;
}

/**
 * This will either enable the paddle camera or stop the paddle camera.
 * Based on the given parameter this will make sure the transforms are made
 * to either place the camera in the paddle or take it out and put it in
 * its default, normal place.
 */
void GameTransformMgr::SetPaddleCamera(bool putCamInsidePaddle, const PlayerPaddle& paddle) {

	if (putCamInsidePaddle) {
		// We are going into paddle camera mode... animate the camera from whereever
		// it currently is, into the paddle and animate its orientation appropriately
		const Point2D currPaddlePos = paddle.GetCenterPosition();

		// We want to be in the paddle, looking up at the level
		Orientation3D paddleOrientation(Vector3D(currPaddlePos[0], currPaddlePos[1], 0.0f), Vector3D(90.0f, 0, 0));
		
		// TODO
	}
	else {
		// We are going out of paddle camera mode back to default - take the camera
		// from whereever it is currently and move it back to the default position

		

	}
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
void GameTransformMgr::Tick(double dT, const GameModel& gameModel) {

	// Holder for any animations that are completed and will need to be deleted and removed
	std::list<std::map<TransformAnimationType, std::list<AnimationLerp<float>>>::iterator> toRemove;

	// Tick all the animations
	for (std::map<TransformAnimationType, std::list<AnimationLerp<float>>>::iterator animationIter = this->animations.begin(); 
		animationIter != this->animations.end(); animationIter++) {
		std::list<AnimationLerp<float>>& currAnimations = animationIter->second; 
		
		// Go through the current animation type's animations, tick them
		// and check to see if they're all finished
		bool allTypeAnimationsAreFinished = true;
		for (std::list<AnimationLerp<float>>::iterator iter = currAnimations.begin(); iter != currAnimations.end(); iter++) {
			bool currAnimationFinished = iter->Tick(dT);
			if (!currAnimationFinished) {
				allTypeAnimationsAreFinished = false;
			}
		}

		if (allTypeAnimationsAreFinished) {
			toRemove.push_back(animationIter);
		}
	}

	// Remove any finished animations
	for (std::list<std::map<TransformAnimationType, std::list<AnimationLerp<float>>>::iterator>::iterator iter = toRemove.begin(); 
		iter != toRemove.end(); iter++) {
		this->animations.erase(*iter);
	}
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