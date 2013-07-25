/**
 * GameTransformMgr.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

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
class GameBall;
class PlayerPaddle;
class Camera;
class PaddleRemoteControlRocketProjectile;

/**
 * Class for delegating interpolation and transformation work and data
 * from the game model - this will handle cases of transforming the level
 * by flipping it, rotating it, moving it around, etc.
 */
class GameTransformMgr {

public:
	GameTransformMgr();
	~GameTransformMgr();

	void Reset();

	// Effect functions for distorting the level's transform and the camera's transform
	void FlipGameUpsideDown();
	void SetPaddleCamera(bool putCamInsidePaddle);
	void SetBallCamera(bool putCamInsideBall);
	void SetBallDeathCamera(bool turnOnBallDeathCam);
    void SetBulletTimeCamera(bool turnOnBulletTimeCam);

    void SetRemoteControlRocketCamera(bool turnOnRocketCam, PaddleRemoteControlRocketProjectile* rocket);

	// Setup functions for telling the camera/level where it should be by default
	void SetupLevelCameraDefaultPosition(const GameLevel& level);

	void Tick(double dT, GameModel& gameModel);

	Matrix4x4 GetGameTransform() const;
	Matrix4x4 GetCameraTransform() const;
	float GetCameraFOVAngle() const;

    bool GetIsLevelFlipAnimationActive() const;
    bool GetIsPaddleCameraOn() const;
    bool GetIsBallCameraOn() const;
    bool GetIsRemoteControlRocketCameraOn() const;

private:
	// Each animation operates atomically, so we queue up animations as they are required
	// and execute each in the order they come in, only
	enum TransformAnimationType { LevelFlipAnimation, ToPaddleCamAnimation, 
                                  FromPaddleCamAnimation, ToBallCamAnimation,
                                  FromBallCamAnimation, ToBallDeathAnimation,
                                  FromBallDeathAnimation, ToBulletTimeCamAnimation,
                                  FromBulletTimeCamAnimation, ToRemoteCtrlRocketCamAnimation,
                                  FromRemoteCtrlRocketCamAnimation};

	struct TransformAnimation {
    public:
        TransformAnimationType type;
		bool hasStarted;
        void* data;

		TransformAnimation(TransformAnimationType type) : type(type), hasStarted(false), data(NULL) {}
	};

	std::list<TransformAnimation> animationQueue;

	// Level-flip related variables
	static const double SECONDS_TO_FLIP;
	bool isFlipped;
	float currGameDegRotX, currGameDegRotY;

	// Paddle and ball camera related variables
	static const double SECONDS_PER_UNIT_PADDLECAM;
	static const double SECONDS_PER_UNIT_BALLCAM;
    static const double SECONDS_PER_UNIT_REMOTE_CTRL_ROCKETCAM;
	static const double SECONDS_PER_UNIT_DEATHCAM;
	
	static const float BALL_DEATH_CAM_DIST_TO_BALL;

	bool isBallDeathCamIsOn;

	PlayerPaddle* paddleWithCamera;	// Will be the paddle where the camera is in paddle cam mode, NULL otherwise
	GameBall* ballWithCamera;       // Will be the ball where the camera is in ball cam mode, NULL otherwise

	PaddleRemoteControlRocketProjectile* remoteControlRocketWithCamera; // The rocket where the camera is following it or inside it, NULL otherwise.
    Orientation3D storedCamOriBeforeRemoteControlRocketCam;
    float storedCamFOVBeforeRemoteControlRocketCam;

	// Camera transformations
	float cameraFOVAngle;
	Orientation3D defaultCamOrientation;
	Orientation3D currCamOrientation;

	// Active Animations
	std::list<AnimationLerp<float> > levelFlipAnimations;
	std::list<AnimationMultiLerp<Orientation3D> > paddleCamAnimations;
	std::list<AnimationMultiLerp<Orientation3D> > ballCamAnimations;
	std::list<AnimationMultiLerp<Orientation3D> > ballDeathAnimations;
    AnimationMultiLerp<Orientation3D> bulletTimeCamAnimation;
	std::list<AnimationMultiLerp<float> > camFOVAnimations;

    std::list<AnimationMultiLerp<Orientation3D> > remoteControlRocketCamAnimations;

	bool TickLevelFlipAnimation(double dT);
	void StartLevelFlipAnimation(double dT, GameModel& gameModel);
	void FinishLevelFlipAnimaiton(double dT, GameModel& gameModel);

	bool TickPaddleCamAnimation(double dT, GameModel& gameModel);
	void StartPaddleCamAnimation(double dT, GameModel& gameModel);
	void FinishPaddleCamAnimation(double dT, GameModel& gameModel);
	
	bool TickBallCamAnimation(double dT, GameModel& gameModel);
	void StartBallCamAnimation(double dT, GameModel& gameModel);
	void FinishBallCamAnimation(double dT, GameModel& gameModel);

	bool TickBallDeathAnimation(double dT, GameModel& gameModel);
	void StartBallDeathAnimation(double dT, GameModel& gameModel);
	void FinishBallDeathAnimation(double dT, GameModel& gameModel);

	bool TickBulletTimeCamAnimation(double dT, GameModel& gameModel);
	void StartBulletTimeCamAnimation(double dT, GameModel& gameModel);
	void FinishBulletTimeCamAnimation(double dT, GameModel& gameModel);

    bool TickRemoteControlRocketCamAnimation(double dT, GameModel& gameModel);
    void StartRemoteControlRocketCamAnimation(double dT, GameModel& gameModel);
    void FinishRemoteControlRocketCamAnimation(double dT, GameModel& gameModel);

	void GetPaddleCamPositionAndFOV(const PlayerPaddle& paddle, float levelWidth, float levelHeight, Vector3D& paddleCamPos, float& fov);
	void GetBallCamPositionAndFOV(const GameBall& ball, float levelWidth, float levelHeight, Vector3D& ballCamPos, float& fov); 

    void GetRemoteCtrlRocketPositionAndFOV(const PaddleRemoteControlRocketProjectile& rocket, float levelWidth, float levelHeight, Vector3D& rocketCamPos, float& fov);

	void ClearSpecialCamEffects();

    DISALLOW_COPY_AND_ASSIGN(GameTransformMgr);
};

inline bool GameTransformMgr::GetIsLevelFlipAnimationActive() const {
    return !this->levelFlipAnimations.empty();
}

inline bool GameTransformMgr::GetIsPaddleCameraOn() const {
    return (this->paddleWithCamera != NULL && this->remoteControlRocketWithCamera == NULL);
}
inline bool GameTransformMgr::GetIsBallCameraOn() const {
    return (this->ballWithCamera != NULL && this->remoteControlRocketWithCamera == NULL);
}
inline bool GameTransformMgr::GetIsRemoteControlRocketCameraOn() const {
    return (this->remoteControlRocketWithCamera != NULL);
}

#endif