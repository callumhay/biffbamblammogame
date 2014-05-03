/**
 * GameTransformMgr.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GAMETRANSFORMMGR_H__
#define __GAMETRANSFORMMGR_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Orientation.h"
#include "../BlammoEngine/Collision.h"

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

    void SetGameZRotation(float rotInDegs, const GameLevel& level);
    static float GetCameraZDistance(float rotInDegs, const GameLevel& level);
    static float GetCameraZDistanceToFitAABB(const Collision::AABB2D& aabb, float addToWidth, float addToHeight);
    static float GetLevelCameraSetupAddToWidth(const GameLevel& level);
    static float GetLevelCameraSetupAddToHeight(const GameLevel& level);

	// Setup functions for telling the camera/level where it should be by default
	void SetupLevelCameraDefaultPosition(const GameLevel& level);

	void Tick(double dT, GameModel& gameModel);

	Matrix4x4 GetGameXYZTransform() const;
    Matrix4x4 GetGameXYTransform() const;
	Matrix4x4 GetCameraInvTransform() const;
	float GetCameraFOVAngle() const;

    float GetGameZRotationInDegs() const;

    bool GetIsLevelFlipAnimationActive() const;
    bool GetIsBallCamAnimationActive() const;
    bool GetIsPaddleCamAnimationActive() const;

    bool GetIsPaddleCameraOn() const;
    bool GetIsBallCameraOn() const;
    bool GetIsRemoteControlRocketCameraOn() const;
    bool GetIsBallDeathCameraOn() const;

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
	float currGameDegRotX, currGameDegRotY, currGameDegRotZ;

	// Paddle and ball camera related variables
	static const double SECONDS_PER_UNIT_PADDLECAM;
	static const double SECONDS_PER_UNIT_BALLCAM;
    static const double SECONDS_PER_UNIT_REMOTE_CTRL_ROCKETCAM;
	static const double SECONDS_PER_UNIT_DEATHCAM;
	static const double SECONDS_TO_FOLLOW_BALL_OUT_OF_BOUNDS;

	static const float BALL_DEATH_CAM_DIST_TO_BALL;

	bool isBallDeathCamIsOn;

	PlayerPaddle* paddleWithCamera;	// Will be the paddle where the camera is in paddle cam mode, NULL otherwise
	
    GameBall* ballWithCamera;       // Will be the ball where the camera is in ball cam mode, NULL otherwise
    float currBallCamZRot;
    float lastBallCamZRot;

    double followBallTimeCounter;
    Vector3D camTAtStartOfFollow;
    double unfollowBallTimeCounter;
    Vector3D camTAtStartOfUnfollow;

    Orientation3D storedOriBeforeBulletTimeCam;

	PaddleRemoteControlRocketProjectile* remoteControlRocketWithCamera; // The rocket where the camera is following it or inside it, NULL otherwise.
    Orientation3D storedCamOriBeforeRemoteControlRocketCam;
    float storedCamFOVBeforeRemoteControlRocketCam;

	// Camera transformations
	float cameraFOVAngle;
	Orientation3D defaultCamOrientation;
	Orientation3D currCamOrientation;

	// Active Animations
	std::list<AnimationLerp<float> > levelFlipAnimations;
	std::list<OrientationMultiLerp> paddleCamAnimations;
	std::list<OrientationMultiLerp> ballCamAnimations;
	std::list<OrientationMultiLerp> ballDeathAnimations;
    OrientationMultiLerp bulletTimeCamAnimation;
	std::list<AnimationMultiLerp<float> > camFOVAnimations;

    std::list<OrientationMultiLerp> remoteControlRocketCamAnimations;

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
	void GetBallCamPositionRotationAndFOV(const GameBall& ball, float levelWidth, float levelHeight, Orientation3D& ballCamOri, float& fov); 

    void GetRemoteCtrlRocketPositionRotationAndFOV(const PaddleRemoteControlRocketProjectile& rocket, 
        float levelWidth, float levelHeight, Vector3D& rocketCamPos, Vector3D& rocketCamRotation, float& fov);

	void ClearSpecialCamEffects();

    float GetXRotationForBallCam() const { return this->isFlipped ? 90.0f : -90.0f; }
    float GetXRotationForPaddleCam() const { return this->isFlipped ? -90.0f : 90.0f; }

    DISALLOW_COPY_AND_ASSIGN(GameTransformMgr);
};

/**
 * Grab the current transform for the game - this is a composite matrix
 * of all the transforms that are currently being applied - the transforms
 * are concatenated in the most sane and consistent way possible.
 */
inline Matrix4x4 GameTransformMgr::GetGameXYZTransform() const {
    return Matrix4x4::rotationXYZMatrix(this->currGameDegRotX, this->currGameDegRotY, this->currGameDegRotZ);
}

inline Matrix4x4 GameTransformMgr::GetGameXYTransform() const {
    return Matrix4x4::rotationXYZMatrix(this->currGameDegRotX, this->currGameDegRotY, 0);
}

/**
 * Grab the current transform for the camera.
 */
inline Matrix4x4 GameTransformMgr::GetCameraInvTransform() const {
	// The inverse is returned because the camera transform is applied to the world matrix
	return this->currCamOrientation.GetTransform();
}

/**
 * Get the current camera Field of View angle.
 */
inline float GameTransformMgr::GetCameraFOVAngle() const {
	return this->cameraFOVAngle;
}

inline float GameTransformMgr::GetGameZRotationInDegs() const {
    return this->currGameDegRotZ;
}

inline bool GameTransformMgr::GetIsLevelFlipAnimationActive() const {
    return !this->levelFlipAnimations.empty();
}

inline bool GameTransformMgr::GetIsBallCamAnimationActive() const {
    for (std::list<TransformAnimation>::const_iterator iter = this->animationQueue.begin();
         iter != this->animationQueue.end(); ++iter) {
        
        if (iter->type == ToBallCamAnimation || iter->type == FromBallCamAnimation) {
            return true;
        }
    }
    return false;
}
inline bool GameTransformMgr::GetIsPaddleCamAnimationActive() const {
    for (std::list<TransformAnimation>::const_iterator iter = this->animationQueue.begin();
        iter != this->animationQueue.end(); ++iter) {

            if (iter->type == ToPaddleCamAnimation || iter->type == FromPaddleCamAnimation) {
                return true;
            }
    }
    return false;
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
inline bool GameTransformMgr::GetIsBallDeathCameraOn() const {
    return (this->isBallDeathCamIsOn);
}

#endif