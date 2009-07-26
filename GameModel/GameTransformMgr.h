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
	enum TransformAnimationType { UpsideDownFlip, ToPaddleCam, FromPaddleCam };

	// Level-flip related variables
	static const double SECONDS_TO_FLIP;
	bool isFlipped;
	float currGameDegRotX, currGameDegRotY;

	// Paddle camera related variables
	static const double SECONDS_TO_ENTEROREXIT_PADDLECAM;

	// Camera transformations
	Orientation3D defaultCamOrientation;
	Orientation3D currCamOrientation;

	// Active Animations
	std::map<TransformAnimationType, std::list<AnimationLerp<float>>> animations;	// TODO: change to orientation animation

public:
	GameTransformMgr();
	~GameTransformMgr();

	// Effect functions for distorting the level's transform and the camera's transform
	void FlipGameUpsideDown();
	void SetPaddleCamera(bool putCamInsidePaddle, const PlayerPaddle& paddle);

	// Setup functions for telling the camera/level where it should be by default
	void SetupLevelCameraDefaultPosition(const GameLevel& level);

	void Tick(double dT, const GameModel& gameModel);

	Matrix4x4 GetGameTransform() const;
	Matrix4x4 GetCameraTransform() const;

};
#endif