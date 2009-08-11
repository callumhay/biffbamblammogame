#ifndef __GAMETRANSFORMMGR_H__
#define __GAMETRANSFORMMGR_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Orientation.h"

class GameModel;
class GameLevel;
class PlayerPaddle;
class Camera;

/**
 * Class for delegating interpolation and transformation work and data
 * from the game model - this will handle cases of transforming the level
 * by flipping it, rotating it, moving it around, etc.
 */
class GameTransformMgr {

private:
	// Each animation operates atomically, so we queue up animations as they are required
	// and execute each in the order they come in, only
	enum TransformAnimationType { LevelFlipAnimation, ToPaddleCamAnimation, FromPaddleCamAnimation};

	struct TransformAnimation {
		TransformAnimationType type;
		bool hasStarted;

		TransformAnimation(TransformAnimationType type) : type(type), hasStarted(false) {}
	};

	std::list<TransformAnimation> animationQueue;

	// Level-flip related variables
	static const double SECONDS_TO_FLIP;
	bool isFlipped;
	float currGameDegRotX, currGameDegRotY;

	// Paddle camera related variables
	static const double SECONDS_PER_UNIT_PADDLECAM;
	bool cameraInPaddle;
	float cameraFOVAngle;

	// Camera transformations
	Orientation3D defaultCamOrientation;
	Orientation3D currCamOrientation;

	// Active Animations
	std::list<AnimationLerp<float>> levelFlipAnimations;
	std::list<AnimationMultiLerp<Orientation3D>> paddleCamAnimations;
	std::list<AnimationMultiLerp<float>> camFOVAnimations;

	bool TickLevelFlipAnimation(double dT);
	void StartLevelFlipAnimation(double dT, GameModel& gameModel);
	void FinishLevelFlipAnimaiton(double dT, GameModel& gameModel);

	bool TickPaddleCamAnimation(double dT);
	void StartPaddleCamAnimation(double dT, GameModel& gameModel);
	void FinishPaddleCamAnimation(double dT, GameModel& gameModel);
	
	void GetPaddleCamPositionAndFOV(const PlayerPaddle& paddle, float levelWidth, float levelHeight, Vector3D& paddleCamPos, float& fov);
public:
	GameTransformMgr();
	~GameTransformMgr();

	void Reset();

	// Effect functions for distorting the level's transform and the camera's transform
	void FlipGameUpsideDown();
	void SetPaddleCamera(bool putCamInsidePaddle);

	// Setup functions for telling the camera/level where it should be by default
	void SetupLevelCameraDefaultPosition(const GameLevel& level);

	void Tick(double dT, GameModel& gameModel);

	Matrix4x4 GetGameTransform() const;
	Matrix4x4 GetCameraTransform() const;
	float GetCameraFOVAngle() const;

};
#endif