/**
 * GameTransformMgr.cpp
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

#include "GameTransformMgr.h"
#include "GameModel.h"
#include "PlayerPaddle.h"
#include "GameBall.h"
#include "CannonBlock.h"
#include "BallBoostModel.h"
#include "PaddleRemoteControlRocketProjectile.h"

#include "../BlammoEngine/Camera.h"
#include "../GameSound/GameSound.h"

// Time of level flip transform linear interpolation
const double GameTransformMgr::SECONDS_TO_FLIP = 0.9;

// Time of camera entering or exiting the paddle camera in seconds per unit distance traveled
// This should be a fairly small value or else it will be extremely hard for the player to keep track of the ball
const double GameTransformMgr::SECONDS_PER_UNIT_PADDLECAM = 0.025;
// See above, except this applies to the ball camera
const double GameTransformMgr::SECONDS_PER_UNIT_BALLCAM = 0.025;
// .. for the remote control rocket cam
const double GameTransformMgr::SECONDS_PER_UNIT_REMOTE_CTRL_ROCKETCAM = 0.033;
// .. applies to the ball death cam
const double GameTransformMgr::SECONDS_PER_UNIT_DEATHCAM = 0.015;

// Interpolation time to go from the current camera position to the outer edges of the level in
// order to follow the ball on the outskirts of a level
const double GameTransformMgr::SECONDS_TO_FOLLOW_BALL_OUT_OF_BOUNDS = 1.0;

// Distance between the camera and ball when in ball death camera mode
const float GameTransformMgr::BALL_DEATH_CAM_DIST_TO_BALL = 20.0f;

GameTransformMgr::GameTransformMgr() : paddleWithCamera(NULL), ballWithCamera(NULL), 
remoteControlRocketWithCamera(NULL), currGameDegRotX(0), currGameDegRotY(0), currGameDegRotZ(0), 
lastBallCamZRot(0), currBallCamZRot(0), followBallTimeCounter(0), unfollowBallTimeCounter(0) {
	this->Reset();
}

GameTransformMgr::~GameTransformMgr() {
	this->Reset();
}

void GameTransformMgr::ClearSpecialCamEffects() {
	if (this->ballWithCamera != NULL) {
		this->ballWithCamera = NULL;
		GameBall::SetBallCamera(NULL, NULL);
	}

	if (this->paddleWithCamera != NULL) {
		this->paddleWithCamera->SetPaddleCamera(false);
		this->paddleWithCamera = NULL;
	}

    if (this->remoteControlRocketWithCamera != NULL) {
        // TODO?
        this->remoteControlRocketWithCamera = NULL;
    }

	this->cameraFOVAngle = Camera::FOV_ANGLE_IN_DEGS;
    this->storedCamFOVBeforeRemoteControlRocketCam = Camera::FOV_ANGLE_IN_DEGS;
    this->storedCamOriBeforeRemoteControlRocketCam = this->defaultCamOrientation;
    
    this->storedOriBeforeBulletTimeCam = this->defaultCamOrientation;
    
	this->paddleCamAnimations.clear();
	this->ballCamAnimations.clear();
	this->camFOVAnimations.clear();
    this->bulletTimeCamAnimation.ClearLerp();
    this->remoteControlRocketCamAnimations.clear();
}

/** 
 * Reset the entire game transform manager - eliminate all animations on go back to defaults
 * for all relevant values.
 */
void GameTransformMgr::Reset() {
	this->currGameDegRotX = 0.0f;
	this->currGameDegRotY = 0.0f;
    this->currGameDegRotZ = 0.0f;
	this->isFlipped = false;
	this->isBallDeathCamIsOn = false;
    this->lastBallCamZRot = 0.0f;
    this->currBallCamZRot = 0.0f;
    this->followBallTimeCounter = 0.0f;
    this->unfollowBallTimeCounter = 0.0f;
    this->camTAtStartOfFollow = this->defaultCamOrientation.GetTranslation();
    this->camTAtStartOfUnfollow = this->defaultCamOrientation.GetTranslation();

	this->ClearSpecialCamEffects();
	this->levelFlipAnimations.clear();
	this->ballDeathAnimations.clear();
	this->animationQueue.clear();

    this->currCamOrientation = this->defaultCamOrientation;
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
	while (this->animationQueue.size() > 0) {
		GameTransformMgr::TransformAnimationType previousType = this->animationQueue.back().type;
		
        if (previousType == GameTransformMgr::ToPaddleCamAnimation || 
            previousType == GameTransformMgr::FromPaddleCamAnimation) {

			this->animationQueue.pop_back();
		}
        else if (previousType == GameTransformMgr::ToBulletTimeCamAnimation || 
            previousType == GameTransformMgr::FromBulletTimeCamAnimation) {

                // Kill any bullet time animations and reset the camera orientation -- this shouldn't happen,
                // but just in case...
                this->bulletTimeCamAnimation.ClearLerp();
                this->animationQueue.pop_back();
        }
        else {
            break;
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
    while (!this->animationQueue.empty()) {

        GameTransformMgr::TransformAnimationType previousType = this->animationQueue.back().type;
        
        if (previousType == GameTransformMgr::ToBallCamAnimation || 
            previousType == GameTransformMgr::FromBallCamAnimation) {

                this->animationQueue.pop_back();
        }
        else if (previousType == GameTransformMgr::ToBulletTimeCamAnimation || 
                 previousType == GameTransformMgr::FromBulletTimeCamAnimation) {

                // Kill any bullet time animations and reset the camera orientation -- this shouldn't happen,
                // but just in case...
                this->bulletTimeCamAnimation.ClearLerp();
                this->animationQueue.pop_back();
        }
        else {
            break;
        } 
    }
	this->animationQueue.push_back(transformAnim);
}

/**
 * This will either enable the camera that focuses on the last ball as it dies or
 * takes the focus off the last dieing ball.
 * This will obliterate all other transforms occurring in the transform manager
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

void GameTransformMgr::SetBulletTimeCamera(bool turnOnBulletTimeCam) {
    GameTransformMgr::TransformAnimationType animType;
    if (turnOnBulletTimeCam) {
        animType = GameTransformMgr::ToBulletTimeCamAnimation;
		// Clear up all other transforms that may manipulate the camera...
		this->ClearSpecialCamEffects();
    }
    else {
        animType = GameTransformMgr::FromBulletTimeCamAnimation;
    }

    TransformAnimation transformAnim(animType);
    transformAnim.data = &this->currCamOrientation;

    // Remove any existing bullet time animations from the animation queue
    for (std::list<TransformAnimation>::iterator iter = this->animationQueue.begin(); iter != this->animationQueue.end();) {
        GameTransformMgr::TransformAnimationType type = iter->type;
        if (type == GameTransformMgr::ToBulletTimeCamAnimation ||
            type == GameTransformMgr::FromBulletTimeCamAnimation) {

            transformAnim.data = &this->storedOriBeforeBulletTimeCam;
            iter = this->animationQueue.erase(iter);
        }
        else {
             ++iter;
        }
    }

	this->animationQueue.push_front(transformAnim);
}

void GameTransformMgr::SetRemoteControlRocketCamera(bool turnOnRocketCam, PaddleRemoteControlRocketProjectile* rocket) {

    GameTransformMgr::TransformAnimationType animType;
    PaddleRemoteControlRocketProjectile* rocketData = NULL;
    
    if (turnOnRocketCam) {
        assert(rocket != NULL);
        rocketData = rocket;
        animType = GameTransformMgr::ToRemoteCtrlRocketCamAnimation;
    }
    else {
        // If we're not in the remote control rocket camera or going into it then we don't do anything
        // since we can't come out of a camera mode that we aren't in
        if (this->remoteControlRocketWithCamera == NULL && 
            (this->animationQueue.empty() || this->animationQueue.front().type != GameTransformMgr::ToRemoteCtrlRocketCamAnimation)) {
            return;
        }
        animType = GameTransformMgr::FromRemoteCtrlRocketCamAnimation;
    }

    TransformAnimation transformAnim(animType);
    transformAnim.data = rocketData;

    // Make sure we compress the last transform in the animation queue if it is also a remote-control rocket animation
    while (!this->animationQueue.empty()) {

        GameTransformMgr::TransformAnimationType previousType = this->animationQueue.back().type;
        if (previousType == GameTransformMgr::ToRemoteCtrlRocketCamAnimation || 
            previousType == GameTransformMgr::FromRemoteCtrlRocketCamAnimation) {

            if (!this->animationQueue.back().hasStarted) {
                // Make sure the stored orientation and FOV are initialized properly
                this->storedCamFOVBeforeRemoteControlRocketCam = this->cameraFOVAngle;
                this->storedCamOriBeforeRemoteControlRocketCam = this->currCamOrientation;
            }

            this->animationQueue.pop_back();
        }
        else if (previousType == GameTransformMgr::ToBulletTimeCamAnimation || 
                 previousType == GameTransformMgr::FromBulletTimeCamAnimation) {

            // Kill any bullet time animations and reset the camera orientation -- this shouldn't happen,
            // but just in case...
            this->bulletTimeCamAnimation.ClearLerp();
            this->currCamOrientation = this->defaultCamOrientation;
            this->animationQueue.pop_back();
        }
        else {
            break;
        } 
    }
    this->animationQueue.push_front(transformAnim);
}

void GameTransformMgr::SetGameZRotation(float rotInDegs, const GameLevel& level) {
    this->currGameDegRotZ = fmod(rotInDegs, 360.0f);

    float camZDist = GameTransformMgr::GetCameraZDistance(this->currGameDegRotZ, level);
    this->defaultCamOrientation = Orientation3D(Vector3D(0, 0, camZDist), Vector3D(0,0,0));
    
    // Don't mess with the current camera orientation unless we're sure that there are no other
    // important camera animations going on
    if (!this->GetIsRemoteControlRocketCameraOn() && !this->GetIsBallCameraOn() && 
        !this->GetIsPaddleCameraOn() && !this->GetIsBallDeathCameraOn() &&
        this->animationQueue.empty() && this->remoteControlRocketCamAnimations.empty() &&
        this->ballDeathAnimations.empty()) {

        this->currCamOrientation.SetTZ(camZDist);
    }
}

float GameTransformMgr::GetCameraZDistance(float rotInDegs, const GameLevel& level) {
    static const float FADE_IN_FRACT_0_180_360 = 1.0f;
    static const float FADE_IN_FRACT_90_270    = 0.7f;

    float fractAmt   = 1.0f;
    float absModRotZ = fmod(fabs(rotInDegs), 360.0f);
    if (absModRotZ <= 90.0f) {
        fractAmt = NumberFuncs::Lerp(0.0f, 90.0f, FADE_IN_FRACT_0_180_360, FADE_IN_FRACT_90_270, absModRotZ);
    }
    else if (absModRotZ > 90.0f && absModRotZ <= 180.0f) {
        fractAmt = NumberFuncs::Lerp(90.0f, 180.0f, FADE_IN_FRACT_90_270, FADE_IN_FRACT_0_180_360, absModRotZ);
    }
    else if (absModRotZ > 180.0f && absModRotZ <= 270.0f) {
        fractAmt = NumberFuncs::Lerp(180.0f, 270.0f, FADE_IN_FRACT_0_180_360, FADE_IN_FRACT_90_270, absModRotZ);
    }
    else {
        fractAmt = NumberFuncs::Lerp(270.0f, 360.0f, FADE_IN_FRACT_90_270, FADE_IN_FRACT_0_180_360, absModRotZ);
    }

    // Recalculate the camera transform so that it fits the whole level...
    const float levelWidth  = level.GetLevelUnitWidth();
    const float levelHeight = level.GetLevelUnitHeight();
    const float levelHalfWidthFract  = fractAmt * (levelWidth / 2.0f);
    const float levelHalfHeightFract = fractAmt * (levelHeight / 2.0f);

    // Create a set of vectors that represent points at the corners of a bounding box 
    // around the level with origin at the center of the box
    Vector2D pt0(-levelHalfWidthFract, -levelHalfHeightFract);
    Vector2D pt1(-levelHalfWidthFract, levelHalfHeightFract);
    Vector2D pt2(levelHalfWidthFract, levelHalfHeightFract);
    Vector2D pt3(levelHalfWidthFract, -levelHalfHeightFract);

    // Rotate the box by the current z rotation...
    pt0.Rotate(rotInDegs);
    pt1.Rotate(rotInDegs);
    pt2.Rotate(rotInDegs);
    pt3.Rotate(rotInDegs);

    // Now create a new AABB with the rotated points
    Collision::AABB2D viewAABB;
    viewAABB.AddPoint(Point2D(pt0[0], pt0[1]));
    viewAABB.AddPoint(Point2D(pt1[0], pt1[1]));
    viewAABB.AddPoint(Point2D(pt2[0], pt2[1]));
    viewAABB.AddPoint(Point2D(pt3[0], pt3[1]));

    // Using the AABB, figure out the distance that the camera needs to be in order to fit it all into the view port...
    float addToWidth  = GameTransformMgr::GetLevelCameraSetupAddToWidth(level);
    float addToHeight = GameTransformMgr::GetLevelCameraSetupAddToHeight(level);
    return GameTransformMgr::GetCameraZDistanceToFitAABB(viewAABB, addToWidth, addToHeight);
}

/// <summary>
/// Gets the distance along the z-axis that the camera must be in to fit the given AABB in the view port.
/// </summary>
/// <returns> The required z distance. </returns>
float GameTransformMgr::GetCameraZDistanceToFitAABB(const Collision::AABB2D& aabb, 
                                                    float addToWidth, float addToHeight) {
    float width  = aabb.GetWidth();
    float height = aabb.GetHeight();

    float horizontalFOV = Camera::FOV_ANGLE_IN_DEGS * Camera::GetAspectRatio();
    float distance1 = (addToHeight + height) / (2.0f * tanf(Trig::degreesToRadians(0.5f * Camera::FOV_ANGLE_IN_DEGS)));
    float distance2 = (addToWidth + width)   / (2.0f * tanf(Trig::degreesToRadians(0.5f * horizontalFOV)));

    return std::max<float>(distance1, distance2);
}

/**
 * This needs to be called whenever a new level is being loaded - this will make sure
 * that the default camera position is calculated and made available to the view.
 */
void GameTransformMgr::SetupLevelCameraDefaultPosition(const GameLevel& level) {
	// Calculate the distance along the z axis that the camera needs to be from the origin in order
	// to see the entire level - this will be the default translation for the camera
    float levelWidth  = level.GetLevelUnitWidth();
    float levelHeight = level.GetLevelUnitHeight();
    
    float addToWidth  = GameTransformMgr::GetLevelCameraSetupAddToWidth(level); 
    float addToHeight = GameTransformMgr::GetLevelCameraSetupAddToHeight(level); 

    float horizontalFOV = Camera::FOV_ANGLE_IN_DEGS * Camera::GetAspectRatio();
    float distance1 = (levelHeight + addToHeight) / (2.0f * tanf(Trig::degreesToRadians(0.5f * Camera::FOV_ANGLE_IN_DEGS)));
    float distance2 = (levelWidth  + addToWidth)  / (2.0f * tanf(Trig::degreesToRadians(0.5f * horizontalFOV)));

    float distance = std::max<float>(distance1, distance2);

	// Telling the world to translate by (0, 0, -distance), telling the camera to translate by (0, 0, distance)
	this->defaultCamOrientation = Orientation3D(Vector3D(0, 0, distance), Vector3D(0,0,0));
	this->currCamOrientation	= this->defaultCamOrientation;
}

float GameTransformMgr::GetLevelCameraSetupAddToWidth(const GameLevel& level) {
    float levelWidth  = level.GetLevelUnitWidth();

    static const float MIN_WIDTH_FOR_LERP = LevelPiece::PIECE_WIDTH * 14;
    static const float MAX_WIDTH_FOR_LERP = LevelPiece::PIECE_WIDTH * 23;

    static const float MIN_ADD_AMT = 6.0f  * LevelPiece::PIECE_WIDTH;
    static const float MAX_ADD_AMT = 8.25f * LevelPiece::PIECE_WIDTH;

    static const float WIDTH_RATIO_COEFF  = Camera::GetWindowWidth()  / 1600.0f;

    float addToWidth  = MIN_ADD_AMT;
    if (levelWidth > MIN_WIDTH_FOR_LERP) {
        if (levelWidth > MAX_WIDTH_FOR_LERP) {
            addToWidth = MAX_ADD_AMT;
        }
        else {
            addToWidth = NumberFuncs::LerpOverFloat(MIN_WIDTH_FOR_LERP, MAX_WIDTH_FOR_LERP, MIN_ADD_AMT, MAX_ADD_AMT, levelWidth);
        }
    }

    addToWidth  *= WIDTH_RATIO_COEFF;
    return addToWidth;
}

float GameTransformMgr::GetLevelCameraSetupAddToHeight(const GameLevel& level) {
    UNUSED_PARAMETER(level);
    static const float HEIGHT_RATIO_COEFF = Camera::GetWindowHeight() / 900.0f;
    float addToHeight = 4 * LevelPiece::PIECE_HEIGHT;
    addToHeight *= HEIGHT_RATIO_COEFF;
    return addToHeight;
}

/**
 * This will cause interpolation among any transforms that are taking
 * place within this class.
 */
void GameTransformMgr::Tick(double dT, GameModel& gameModel) {

	// Figure out the next animation and animate it (if there are any)
	if (this->animationQueue.size() > 0) {
		TransformAnimation currAnimation = this->animationQueue.front();
		switch (currAnimation.type) {
			
			case GameTransformMgr::LevelFlipAnimation:
				if (currAnimation.hasStarted) {
					if (this->TickLevelFlipAnimation(dT)) {
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
					if (this->TickPaddleCamAnimation(dT, gameModel)) {
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
					if (this->TickBallCamAnimation(dT, gameModel)) {
						this->FinishBallCamAnimation(dT, gameModel);
					}
				}
				else {
					this->StartBallCamAnimation(dT, gameModel);
				}
				break;

            case GameTransformMgr::ToRemoteCtrlRocketCamAnimation:
            case GameTransformMgr::FromRemoteCtrlRocketCamAnimation:
                if (currAnimation.hasStarted) {
                    if (this->TickRemoteControlRocketCamAnimation(dT, gameModel)) {
                        this->FinishRemoteControlRocketCamAnimation(dT, gameModel);
                    }
                }
                else {
                    this->StartRemoteControlRocketCamAnimation(dT, gameModel);
                }
                break;

			case GameTransformMgr::ToBallDeathAnimation:
                
                // If there's a level flip animation currently executing then we wait for it to finish...
                if (!this->levelFlipAnimations.empty()) {
                    break;
                }

			case GameTransformMgr::FromBallDeathAnimation:
				if (currAnimation.hasStarted) {
					if (this->TickBallDeathAnimation(dT, gameModel)) {
						this->FinishBallDeathAnimation(dT, gameModel);
					}
				}
				else {
					this->StartBallDeathAnimation(dT, gameModel);
				}
				break;

            case GameTransformMgr::ToBulletTimeCamAnimation:
            case GameTransformMgr::FromBulletTimeCamAnimation:
				if (currAnimation.hasStarted) {
					if (this->TickBulletTimeCamAnimation(dT, gameModel)) {
						this->FinishBulletTimeCamAnimation(dT, gameModel);
					}
				}
				else {
					this->StartBulletTimeCamAnimation(dT, gameModel);
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
	
		// Get the ball velocity and current camera world-space position/translation
		Vector3D ballVelocity(deathBall->GetVelocity(), 0.0f);
        ballVelocity = 0.75f * (this->GetGameXYZTransform() * ballVelocity);
		const Vector3D& currCamTranslation = this->currCamOrientation.GetTranslation();
		
		// Calculate the distance the ball has moved so far (if it were traveling in a straight line), in world-space
		Point3D currCamPosition   = Point3D(currCamTranslation[0], currCamTranslation[1], currCamTranslation[2]);
		Vector3D ballMoveDistance	= dT * Vector3D(ballVelocity[0], ballVelocity[1], ballVelocity[2]);

		currCamPosition  = currCamPosition + ballMoveDistance;
		this->currCamOrientation.SetTranslation(Vector3D(currCamPosition[0], currCamPosition[1], currCamPosition[2]));
	}
    else {

        if (this->remoteControlRocketWithCamera != NULL) {
            // Calculate the location of the rocket camera in world space...
            GameLevel* currLevel = gameModel.GetCurrentLevel();
            assert(currLevel != NULL);

            Vector3D worldSpaceTranslation;
            Vector3D worldSpaceRotation;
            this->GetRemoteCtrlRocketPositionRotationAndFOV(*this->remoteControlRocketWithCamera, currLevel->GetLevelUnitWidth(), 
                currLevel->GetLevelUnitHeight(), worldSpaceTranslation, worldSpaceRotation, this->cameraFOVAngle);

            this->currCamOrientation.SetTranslation(worldSpaceTranslation);
            this->currCamOrientation.SetXYZRotation(worldSpaceRotation);
        }
        else {
            if (this->paddleWithCamera != NULL) {
		        // Calculate the location of the paddle in world space and get the FOV angle as well
		        GameLevel* currLevel = gameModel.GetCurrentLevel();
		        assert(currLevel != NULL);
        		
		        Vector3D worldSpaceTranslation;
		        this->GetPaddleCamPositionAndFOV(*this->paddleWithCamera, currLevel->GetLevelUnitWidth(), 
			        currLevel->GetLevelUnitHeight(), worldSpaceTranslation, this->cameraFOVAngle);

                Vector3D worldRotation(90, this->paddleWithCamera->GetZRotation(), this->paddleWithCamera->GetIsPaddleFlipped() ? 180 : 0);
		        worldRotation = this->GetGameXYZTransform() * worldRotation;	
																								    
                // Rotate the y-axis to move the view point when the paddle is discombobulated!
		        // Update the camera position and rotation based on the movement of the paddle and the level transform
		        this->currCamOrientation.SetTranslation(worldSpaceTranslation);
		        this->currCamOrientation.SetXYZRotation(worldRotation);
	        }
	        else if (this->ballWithCamera != NULL) {

		        // Calculate the location of the ball in world space
		        GameLevel* currLevel = gameModel.GetCurrentLevel();
		        assert(currLevel != NULL);

                // Update the camera position and rotation based on the movement of the ball and the level transform
		        this->GetBallCamPositionRotationAndFOV(*this->ballWithCamera, currLevel->GetLevelUnitWidth(), 
                                                       currLevel->GetLevelUnitHeight(), this->currCamOrientation, this->cameraFOVAngle);

                // Exponential decay towards the last z rotation from the current one...
                this->currBallCamZRot = NumberFuncs::LerpOverTime(0.0, 0.1, this->currBallCamZRot, this->lastBallCamZRot, dT);
	        }
            else {
                // Check to see if the ball is currently out of bounds of the level, if it is then the camera follows it a bit,
                // in order to keep it on the screen...
                
                // Check to see if there's only one ball and that there are no active animations...
                const std::list<GameBall*>& balls = gameModel.GetGameBalls();
                if (balls.size() == 1 && this->animationQueue.empty()) {
                    
                    const GameBall* ball = balls.front();
                    assert(ball != NULL);

                    // Check whether the one ball is close to the edge of the screen/level...
                    if (gameModel.GetCurrentStateType() == GameState::BallInPlayStateType &&
                        gameModel.IsOutOfPaddedLevelBounds(ball->GetCenterPosition2D(), LevelPiece::PIECE_WIDTH/2.0f, 3.0f*PlayerPaddle::PADDLE_HEIGHT_TOTAL, 
                        LevelPiece::PIECE_WIDTH/2.0f, LevelPiece::PIECE_HEIGHT)) {
                        
                        const GameLevel* currLevel = gameModel.GetCurrentLevel();
                        assert(currLevel != NULL);

                        // Calculate a point on the edge of the boundaries of the level where the camera will move to
                        // in the XY plane in order to follow the ball
                        Collision::AABB2D levelAABB;
                        currLevel->GetLevelAABB(levelAABB);
                        Point2D closestPt;
                        Collision::ClosestPoint(ball->GetCenterPosition2D(), levelAABB, closestPt);
                        closestPt = Point2D::GetMidPoint(closestPt, ball->GetCenterPosition2D());
                        
                        // Transform the point into the level space...
                        closestPt += currLevel->GetTranslationToMiddle2D();
                        closestPt = this->GetGameXYZTransform()* closestPt;
                        
                        // Start following...
                        if (this->followBallTimeCounter == 0.0) {
                            this->camTAtStartOfFollow = this->currCamOrientation.GetTranslation();
                        }
                        this->followBallTimeCounter = std::min<double>(
                            this->followBallTimeCounter + std::max<float>(ball->GetSpeed()/GameBall::GetNormalSpeed(), 1.0)*dT, 
                            GameTransformMgr::SECONDS_TO_FOLLOW_BALL_OUT_OF_BOUNDS);
                        Vector2D newTranslation = NumberFuncs::LerpOverTime<Vector2D>(0.0, 
                            GameTransformMgr::SECONDS_TO_FOLLOW_BALL_OUT_OF_BOUNDS, this->camTAtStartOfFollow.ToVector2D(), 
                            Vector2D(closestPt[0], closestPt[1]), this->followBallTimeCounter);

                        this->currCamOrientation.SetTranslation(Vector3D(newTranslation, this->currCamOrientation.GetTZ()));
                        this->unfollowBallTimeCounter = 0.0;
                    }
                    else {
                        // Reset the position of the camera to the default

                        if (this->unfollowBallTimeCounter < GameTransformMgr::SECONDS_TO_FOLLOW_BALL_OUT_OF_BOUNDS) {
                            if (this->unfollowBallTimeCounter == 0.0) {
                                this->camTAtStartOfUnfollow = this->currCamOrientation.GetTranslation();
                            }

                            this->unfollowBallTimeCounter = std::min<double>(
                                this->unfollowBallTimeCounter + std::max<float>(ball->GetSpeed()/GameBall::GetNormalSpeed(), 1.0)*dT, 
                                GameTransformMgr::SECONDS_TO_FOLLOW_BALL_OUT_OF_BOUNDS);
                            Vector3D newTranslation = NumberFuncs::LerpOverTime<Vector3D>(0.0, 
                                GameTransformMgr::SECONDS_TO_FOLLOW_BALL_OUT_OF_BOUNDS, this->camTAtStartOfUnfollow, 
                                this->defaultCamOrientation.GetTranslation(), this->unfollowBallTimeCounter);
                            
                            this->currCamOrientation.SetTranslation(newTranslation);
                        }
                        else {
                            this->currCamOrientation = this->defaultCamOrientation;
                        }

                        this->followBallTimeCounter = 0.0;
                    }
                }
            }
        }
    }
}

/**
 * Private helper function to get the exact location of the paddle camera in world space.
 */
void GameTransformMgr::GetPaddleCamPositionAndFOV(const PlayerPaddle& paddle, float levelWidth, float levelHeight, 
                                                  Vector3D& paddleCamPos, float& fov) {

	const float DIST_FROM_PADDLE = paddle.GetHalfWidthTotal();

	Point2D paddleCamPosition = paddle.GetCenterPosition() - Vector2D(0, paddle.GetHalfHeight() + DIST_FROM_PADDLE);
	Vector2D halfLevelDim	= 0.5 * Vector2D(levelWidth, levelHeight);
	Point2D  worldSpacePaddlePos = paddleCamPosition - halfLevelDim;
	Vector3D worldSpaceTranslation(worldSpacePaddlePos[0], worldSpacePaddlePos[1], 0.0f);
	
	paddleCamPos = this->GetGameXYZTransform() * worldSpaceTranslation;

    // N.B., We take a fraction of the half width so that the FOV covers a little bit less than the
    // total width of the paddle, this gives the player a bit of leeway if the ball is just on or slightly off the edge
    // of the screen when it's coming down to the paddle in paddle camera mode
	fov = 2.0f * Trig::radiansToDegrees(atan(0.85f * paddle.GetHalfWidthTotal() / DIST_FROM_PADDLE));
}

/**
 * Private helper function to get the exact location of the ball camera in world space.
 */
void GameTransformMgr::GetBallCamPositionRotationAndFOV(const GameBall& ball, float levelWidth, float levelHeight, 
                                                        Orientation3D& ballCamOri, float& fov) {

	Collision::Circle2D ballBounds = ball.GetBounds();

	// Position the ball camera almost exactly at the bottom of the ball (we make sure we're at least a bit above that
	// so that we don't get weirdo clipping issues)
	const Point2D& ballCamPosition = ballBounds.Center();
	Vector2D halfLevelDim	= 0.5 * Vector2D(levelWidth, levelHeight);
	Point2D  worldSpaceBallPos = ballCamPosition - halfLevelDim;
	Vector3D worldSpaceTranslation(worldSpaceBallPos[0], worldSpaceBallPos[1], 0.0f);

	ballCamOri.SetTranslation(this->GetGameXYZTransform() * worldSpaceTranslation);

    // If we're in ball camera mode and the ball is in a cannon block then we rotate
    // the camera with the cannon block...
    const CannonBlock* cannon = ball.GetCannonBlock();
    if (cannon != NULL) {

        this->currBallCamZRot = cannon->GetCurrentCannonAngleInDegs() + 90;
        this->lastBallCamZRot = this->currBallCamZRot;

        ballCamOri.SetZYXRotation(this->GetGameXYZTransform() * 
            Vector3D(this->currBallCamZRot, 0.0f, this->GetXRotationForBallCam()));
        fov = 45.0f;
    }
    else {
        this->lastBallCamZRot = 0.0f;

        // We'll be a bit more creative than just point the camera downwards:
        // Check to see if the ball is traveling in the direction of a reasonably downwards-pointed
        // trajectory, if so then we use that to modify the lock-at direction of the camera...
        static const float MODIFY_LOOKAT_CUTOFF_ANGLE_IN_DEGS = 60.0f;
        static const float MODIFY_LOOKAT_CUTOFF_ANGLE_IN_RADS = Trig::degreesToRadians(MODIFY_LOOKAT_CUTOFF_ANGLE_IN_DEGS);
        const Vector2D& ballDir = ball.GetDirection();
        if (!ballDir.IsZero()) {

            // Get the angle of the ball's direction in the space of the downwards vector
            float angle = atan2(ballDir[1], ballDir[0]);
            angle += M_PI_DIV2;

            if (fabs(angle) <= MODIFY_LOOKAT_CUTOFF_ANGLE_IN_RADS) {
                this->lastBallCamZRot = Trig::radiansToDegrees(angle);
            }
        }
        
        ballCamOri.SetZYXRotation(this->GetGameXYZTransform() * 
            Vector3D(this->currBallCamZRot, 0.0f, this->GetXRotationForBallCam()));
        fov = std::min<float>(75.0f, 50.0f + 15.0f * (ballBounds.Radius() / GameBall::DEFAULT_BALL_RADIUS));
    }
}

void GameTransformMgr::GetRemoteCtrlRocketPositionRotationAndFOV(const PaddleRemoteControlRocketProjectile& rocket, 
                                                                 float levelWidth, float levelHeight,
                                                                 Vector3D& rocketCamPos, Vector3D& rocketCamRotation, float& fov) {

    // Calculate the final camera distance away from the rocket...
    static const float ROCKET_VIEW_BORDER = 6.0f * LevelPiece::PIECE_WIDTH;
    float zDistanceFromRocket = (std::max<float>(rocket.GetHeight(), rocket.GetWidth()) + ROCKET_VIEW_BORDER) / 
       (2.0f * tanf(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS * 0.5f)));

    Point2D rocketWorldSpacePos  = rocket.GetPosition() - 0.5 * Vector2D(levelWidth, levelHeight);
    const Vector2D& rocketVelDir = rocket.GetVelocityDirection();

    rocketCamPos = this->GetGameXYZTransform() * Vector3D(rocketWorldSpacePos[0], rocketWorldSpacePos[1], zDistanceFromRocket);
    
    // Camera rotates with the rocket...
    rocketCamRotation[0] = 0.0f;
    rocketCamRotation[1] = 0.0f;
    rocketCamRotation[2] = fmod(Trig::radiansToDegrees(atan2(rocketVelDir[1], rocketVelDir[0])) - 90 - this->currGameDegRotZ, 360.0f);

    fov = Camera::FOV_ANGLE_IN_DEGS;
}

void GameTransformMgr::StartLevelFlipAnimation(double dT, GameModel& gameModel) {
	UNUSED_PARAMETER(dT);

	// Grab the current transform animation information from the front of the queue
	TransformAnimation& levelFlipAnim = this->animationQueue.front();
	assert(levelFlipAnim.type == GameTransformMgr::LevelFlipAnimation);

	// Find any existing animations and halt them elegantly
	this->levelFlipAnimations.clear();

    // Pause the game while we flip the level
    gameModel.SetPause(GameModel::PauseState);

    GameSound* sound = gameModel.GetSound();
    bool canPlaySound = GameState::IsGameInPlayState(gameModel) || 
        gameModel.GetCurrentStateType() == GameState::BallDeathStateType;
    
    int finalDegRotX = 0;
    if (this->isFlipped) {
        finalDegRotX = 0;
        if (canPlaySound) {
            sound->PlaySound(GameSound::LevelUnflipEvent, false);
        }
    }
    else {
        finalDegRotX = 180;
        if (canPlaySound) {
            sound->PlaySound(GameSound::LevelFlipEvent, false);
        }
    }

	// Setup the animations for flipping
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
	for (std::list<AnimationLerp<float> >::iterator iter = this->levelFlipAnimations.begin();
         iter != this->levelFlipAnimations.end();) {

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
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);

	// Pop the animation off the queue
	this->animationQueue.pop_front();

    // Unpause the game now that the flip is complete
    gameModel.UnsetPause(GameModel::PauseState);
}

/**
 * Starts the paddle camera animation. This will kick off transforms required to
 * get into or come out of paddle camera mode.
 */
void GameTransformMgr::StartPaddleCamAnimation(double dT, GameModel& gameModel) {
	UNUSED_PARAMETER(dT);

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
	worldSpacePaddlePos = this->GetGameXYZTransform() * worldSpacePaddlePos;
	
	if (paddleCamAnim.type == GameTransformMgr::ToPaddleCamAnimation) {
		// We are going into paddle camera mode... animate the camera from wherever
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
		
		// Calculate the distance that will be traveled by the camera
		Vector3D travelVec = finalOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
		float distToTravel = travelVec.length();
		double timeToMoveAnimate   = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_PADDLECAM;
		double timeToRotateAnimate = timeToMoveAnimate / 2.0;
		double totalTimeToAnimate  = timeToMoveAnimate + timeToRotateAnimate;

		// Create the animation to get from wherever the camera currently is to the paddle
		// looking at the level
		std::vector<double> timeValues;
		timeValues.reserve(3);
		timeValues.push_back(0.0);
		timeValues.push_back(timeToMoveAnimate);
		timeValues.push_back(totalTimeToAnimate);

		OrientationMultiLerp toPaddleCamAnim(&this->currCamOrientation);
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
		// from wherever it is currently and move it back to the default position
		OrientationMultiLerp toDefaultCamAnim(&this->currCamOrientation);

		// Calculate the distance that will be traveled by the camera
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
	for (std::list<OrientationMultiLerp >::iterator iter = this->paddleCamAnimations.begin(); iter != this->paddleCamAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->paddleCamAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}

	for (std::list<AnimationMultiLerp<float> >::iterator iter = this->camFOVAnimations.begin(); 
         iter != this->camFOVAnimations.end();) {

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
    UNUSED_PARAMETER(dT);

	// Grab the current transform animation information from the front of the queue
	TransformAnimation& paddleCamAnim = this->animationQueue.front();
	assert(paddleCamAnim.type == GameTransformMgr::ToPaddleCamAnimation || paddleCamAnim.type == GameTransformMgr::FromPaddleCamAnimation);
		
	// Unpause the game state since the camera should now be in the paddle
	gameModel.UnsetPause(GameModel::PauseState);

	// Tell the paddle whether it's in or out of paddle camera mode
	if (paddleCamAnim.type == GameTransformMgr::ToPaddleCamAnimation) {
		this->paddleWithCamera = gameModel.GetPlayerPaddle();
		this->paddleWithCamera->SetPaddleCamera(true);
	}
	else {
		gameModel.GetPlayerPaddle()->SetPaddleCamera(false);
		this->paddleWithCamera = NULL;
	}

	// Pop the animation off the queue
	this->animationQueue.pop_front();
}

void GameTransformMgr::StartBallCamAnimation(double dT, GameModel& gameModel) {
	UNUSED_PARAMETER(dT);

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
	worldSpaceBallPos = this->GetGameXYZTransform() * worldSpaceBallPos;

	if (ballCamAnim.type == GameTransformMgr::ToBallCamAnimation) {
		// We are going into ball camera mode... animate the camera from wherever
		// it currently is, into the ball and animate its orientation appropriately

        this->currBallCamZRot = 0.0f;
        this->lastBallCamZRot = 0.0f;


		// The values for the camera orientation during the ball camera animation...
		// We want the camera to go into the ball and then turn towards the paddle
		float finalFOVAngle;
		Orientation3D finalOrientation;
        this->GetBallCamPositionRotationAndFOV(*ball, currLevel->GetLevelUnitWidth(), 
            currLevel->GetLevelUnitHeight(), finalOrientation, finalFOVAngle);

		std::vector<Orientation3D> orientationValues;
		orientationValues.reserve(3);
		orientationValues.push_back(this->currCamOrientation);
		orientationValues.push_back(finalOrientation);
        orientationValues.push_back(finalOrientation);
		
		// Calculate the distance that will be traveled by the camera
		Vector3D travelVec = finalOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
		float distToTravel = travelVec.length();
		double timeToMoveAnimate   = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_BALLCAM;
		double timeToRotateAnimate = timeToMoveAnimate / 2.0;
		double totalTimeToAnimate  = timeToMoveAnimate + timeToRotateAnimate;

		// Create the animation to get from wherever the camera currently is to the paddle
		// looking at the level
		std::vector<double> timeValues;
		timeValues.reserve(3);
		timeValues.push_back(0.0);
		timeValues.push_back(timeToMoveAnimate);
		timeValues.push_back(totalTimeToAnimate);

		OrientationMultiLerp toBallCamAnim(&this->currCamOrientation);
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
		// from wherever it is currently and move it back to the default position
		OrientationMultiLerp toDefaultCamAnim(&this->currCamOrientation);

		// Calculate the distance that will be traveled by the camera
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
	for (std::list<OrientationMultiLerp >::iterator iter = this->ballCamAnimations.begin(); iter != this->ballCamAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->ballCamAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}

	// Animate the field of view angle
	for (std::list<AnimationMultiLerp<float> >::iterator iter = this->camFOVAnimations.begin(); iter != this->camFOVAnimations.end();) {
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
	UNUSED_PARAMETER(dT);

	// Grab the current transform animation information from the front of the queue
	TransformAnimation& ballCamAnim = this->animationQueue.front();
	assert(ballCamAnim.type == GameTransformMgr::ToBallCamAnimation ||
        ballCamAnim.type == GameTransformMgr::FromBallCamAnimation);
		
	// Unpause the game state since the camera should now be in the ball
	gameModel.UnsetPause(GameModel::PauseState);

	if (ballCamAnim.type == GameTransformMgr::ToBallCamAnimation) {
		this->ballWithCamera = gameModel.GetBallChosenForBallCamera();
		GameBall::SetBallCamera(this->ballWithCamera, gameModel.GetCurrentLevel());
	}
	else {
		this->ballWithCamera = NULL;
		GameBall::SetBallCamera(NULL, gameModel.GetCurrentLevel());
	}

	// Pop the animation off the queue
	this->animationQueue.pop_front();
}

bool GameTransformMgr::TickBallDeathAnimation(double dT, GameModel& gameModel) {
	UNUSED_PARAMETER(gameModel);

	assert(this->ballDeathAnimations.size() > 0);

	// Animate the camera
	for (std::list<OrientationMultiLerp >::iterator iter = this->ballDeathAnimations.begin(); iter != this->ballDeathAnimations.end();) {
		bool currAnimationFinished = iter->Tick(dT);
		if (currAnimationFinished) {
			iter = this->ballDeathAnimations.erase(iter);
		}
		else {
			++iter;
		}
	}

	// Animate the field of view angle
	for (std::list<AnimationMultiLerp<float> >::iterator iter = this->camFOVAnimations.begin(); iter != this->camFOVAnimations.end();) {
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
	UNUSED_PARAMETER(dT);

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
		finalCamPosition = this->GetGameXYZTransform() * finalCamPosition;
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

		OrientationMultiLerp toBallDeathAnim(&this->currCamOrientation);
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
		OrientationMultiLerp toDefaultCamAnim(&this->currCamOrientation);

		// Calculate the distance that will be traveled by the camera
		Vector3D travelVec = this->defaultCamOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
		float distToTravel = travelVec.length();
		double timeToAnimate = distToTravel * GameTransformMgr::SECONDS_PER_UNIT_DEATHCAM;
		
		toDefaultCamAnim.SetLerp(timeToAnimate, this->defaultCamOrientation);
		this->ballDeathAnimations.push_back(toDefaultCamAnim);
	}

	ballDeathAnim.hasStarted = true;
}

void GameTransformMgr::FinishBallDeathAnimation(double dT, GameModel& gameModel) {
	UNUSED_PARAMETER(dT);

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

bool GameTransformMgr::TickBulletTimeCamAnimation(double dT, GameModel& gameModel) {
    UNUSED_PARAMETER(gameModel);

    // Grab the current transform animation information from the front of the queue
    TransformAnimation& bulletTimeAnim = this->animationQueue.front();
    assert(bulletTimeAnim.type == GameTransformMgr::ToBulletTimeCamAnimation || 
           bulletTimeAnim.type == GameTransformMgr::FromBulletTimeCamAnimation);

    const BallBoostModel* currBoostModel = gameModel.GetBallBoostModel();

    // Stop the animation if no balls are available anymore
    if (bulletTimeAnim.type == GameTransformMgr::ToBulletTimeCamAnimation) {
        if (currBoostModel == NULL || currBoostModel->GetNumBallsAllowedToBoost() == 0) {
            this->SetBulletTimeCamera(false);
            return false;
        }
    }

    float invTimeDialation = 1.0f;   
    if (currBoostModel != NULL && currBoostModel->GetBulletTimeState() != BallBoostModel::NotInBulletTime) {

        invTimeDialation = currBoostModel->GetInverseTimeDialation();

	    if (bulletTimeAnim.type == GameTransformMgr::ToBulletTimeCamAnimation) {
            const Collision::AABB2D& ballBounds = currBoostModel->GetBallZoomBounds();
            const GameLevel* currLevel = gameModel.GetCurrentLevel();
	        Vector2D halfLevelDim	   = 0.5f * Vector2D(currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight());
            Point2D ballsCenter        = ballBounds.GetCenter() - halfLevelDim;

            Vector3D translation1 = this->GetGameXYZTransform() * Vector3D(ballsCenter[0], ballsCenter[1], 
                this->bulletTimeCamAnimation.GetInterpolationValue(1).GetTranslation()[2]);
            Vector3D translation2 = this->GetGameXYZTransform() * Vector3D(ballsCenter[0], ballsCenter[1], 
                this->bulletTimeCamAnimation.GetFinalInterpolationValue().GetTranslation()[2]);

            this->bulletTimeCamAnimation.SetInterpolationValue(1, Orientation3D(translation1, 
                this->bulletTimeCamAnimation.GetInterpolationValue(1).GetXYZRotation()));
            this->bulletTimeCamAnimation.SetFinalInterpolationValue(Orientation3D(translation2, 
                this->bulletTimeCamAnimation.GetFinalInterpolationValue().GetXYZRotation()));
        }
    }

    // We need to multiply the dT by the inverse time dilation since we're currently in bullet time
	return this->bulletTimeCamAnimation.Tick(invTimeDialation * dT);
}

void GameTransformMgr::StartBulletTimeCamAnimation(double dT, GameModel& gameModel) {
    UNUSED_PARAMETER(dT);

    // Grab the current transform animation information from the front of the queue
	TransformAnimation& bulletTimeAnim = this->animationQueue.front();
	assert(bulletTimeAnim.type == GameTransformMgr::ToBulletTimeCamAnimation || 
           bulletTimeAnim.type == GameTransformMgr::FromBulletTimeCamAnimation);

    // Don't do anything if there's no boost model ...
    const BallBoostModel* currBoostModel = gameModel.GetBallBoostModel();
    if (currBoostModel == NULL) {
        // Pop the animation off the queue and get out
        this->animationQueue.pop_front();
        return;
    }

	this->bulletTimeCamAnimation = OrientationMultiLerp(&this->currCamOrientation);
    
	// Based on the animation, set the animations for the camera's orientation
    if (bulletTimeAnim.type == GameTransformMgr::ToBulletTimeCamAnimation) {

        // Make sure there are balls available for boosting
        if (currBoostModel->GetNumBallsAllowedToBoost() == 0) {
            // Pop the animation off the queue and get out
            this->animationQueue.pop_front();
            return;
        }

        // Calculate the extent to which we should zoom in, this changes based
        // on the number of balls present - with more balls we make the border around them smaller
        // since it could be quite large, with a single ball we can make it larger... we also
        // make this depend on how close balls are to one another (in the case of multiple balls)
        static const float BORDER_AROUND_ONE_BALL_BOUNDS1 = 6.0f * LevelPiece::PIECE_WIDTH;
        static const float BORDER_AROUND_ONE_BALL_BOUNDS2 = 3.0f * LevelPiece::PIECE_WIDTH;
        static const float BORDER_AROUND_MULTI_BALL_BOUNDS1 = 4.5f * LevelPiece::PIECE_WIDTH;
        static const float BORDER_AROUND_MULTI_BALL_BOUNDS2 = 1.5f * LevelPiece::PIECE_WIDTH;

        float border1, border2;
        float maxBallBoundsSize = std::max<float>(currBoostModel->GetBallZoomBounds().GetWidth(), 
                                                  currBoostModel->GetBallZoomBounds().GetHeight());
        if (currBoostModel->GetCurrentNumBalls() > 1 && maxBallBoundsSize > 2*LevelPiece::PIECE_WIDTH) {
            border1 = BORDER_AROUND_MULTI_BALL_BOUNDS1;
            border2 = BORDER_AROUND_MULTI_BALL_BOUNDS2;
        }
        else {
            border1 = BORDER_AROUND_ONE_BALL_BOUNDS1;
            border2 = BORDER_AROUND_ONE_BALL_BOUNDS2;
        }

        const Collision::AABB2D& ballBounds = currBoostModel->GetBallZoomBounds();

        const GameLevel* currLevel = gameModel.GetCurrentLevel();
		Vector2D halfLevelDim	   = 0.5 * Vector2D(currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight());
        Point2D ballsCenter        = ballBounds.GetCenter() - halfLevelDim;

        float minCamDistance1 = (std::max<float>(ballBounds.GetHeight(), ballBounds.GetWidth()) + border1) / 
                               (2.0f * tanf(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS * 0.5f)));
        float minCamDistance2 = (std::max<float>(ballBounds.GetHeight(), ballBounds.GetWidth()) + border2) / 
                               (2.0f * tanf(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS * 0.5f)));
        Vector3D translation1 = this->GetGameXYZTransform() * Vector3D(ballsCenter[0], ballsCenter[1], minCamDistance1);
        Vector3D translation2 = this->GetGameXYZTransform() * Vector3D(ballsCenter[0], ballsCenter[1], minCamDistance2);

        std::vector<double> timeVals;
        timeVals.push_back(0.0);
        timeVals.push_back(BallBoostModel::BULLET_TIME_FADE_IN_SECONDS);
        timeVals.push_back(BallBoostModel::BULLET_TIME_FADE_IN_SECONDS +
                           BallBoostModel::GetMaxBulletTimeDuration());
        std::vector<Orientation3D> orientVals;
        orientVals.push_back(this->currCamOrientation);
        orientVals.push_back(Orientation3D(translation1, this->currCamOrientation.GetXYZRotation()));
        orientVals.push_back(Orientation3D(translation2, this->currCamOrientation.GetXYZRotation()));

        this->bulletTimeCamAnimation.SetLerp(timeVals, orientVals);
        this->bulletTimeCamAnimation.SetRepeat(false);
        this->storedOriBeforeBulletTimeCam = *static_cast<Orientation3D*>(bulletTimeAnim.data);
    }
    else {
        this->bulletTimeCamAnimation.SetLerp(BallBoostModel::BULLET_TIME_FADE_OUT_SECONDS, this->storedOriBeforeBulletTimeCam);
        this->bulletTimeCamAnimation.SetRepeat(false);
    }
    bulletTimeAnim.hasStarted = true;
}

void GameTransformMgr::FinishBulletTimeCamAnimation(double dT, GameModel& gameModel) {
	UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);

	// Grab the current transform animation information from the front of the queue
	TransformAnimation& bulletTimeAnim = this->animationQueue.front();
    UNUSED_VARIABLE(bulletTimeAnim);
	assert(bulletTimeAnim.type == GameTransformMgr::ToBulletTimeCamAnimation || 
           bulletTimeAnim.type == GameTransformMgr::FromBulletTimeCamAnimation);

	// Pop the animation off the queue
	this->animationQueue.pop_front();
}

void GameTransformMgr::StartRemoteControlRocketCamAnimation(double dT, GameModel& gameModel) {
    UNUSED_PARAMETER(dT);

    // Grab the current transform animation information from the front of the queue
    TransformAnimation& rocketCamAnim = this->animationQueue.front();
    assert(rocketCamAnim.type == GameTransformMgr::ToRemoteCtrlRocketCamAnimation || 
           rocketCamAnim.type == GameTransformMgr::FromRemoteCtrlRocketCamAnimation);

    // Make sure the game state is paused while we perform the animation...
    //gameModel.SetPause(GameModel::PauseState);

    // Clear any previous remote control rocket camera animations
    this->remoteControlRocketCamAnimations.clear();

    if (rocketCamAnim.type == GameTransformMgr::ToRemoteCtrlRocketCamAnimation) {
        
        // While the remote control rocket is activated, the player gains control of
        // it while the paddle and ball are frozen and removed from the game temporarily
        gameModel.SetPause(GameModel::PausePaddle);
        gameModel.SetPause(GameModel::PauseBall);
        gameModel.SetPause(GameModel::PauseTimers);

        // Grab the affected rocket position and other info, make sure the relevant values are in world coordinates
        assert(reinterpret_cast<PaddleRemoteControlRocketProjectile*>(rocketCamAnim.data) != NULL);
        PaddleRemoteControlRocketProjectile* remoteCtrlRocket = static_cast<PaddleRemoteControlRocketProjectile*>(rocketCamAnim.data);

        // We are going into remote control rocket camera mode... animate the camera from wherever
        // it currently is, transforming it to the location of the rocket and animate its orientation appropriately...

        // This will store the actual world-space position that we need to transform the camera to...
        Vector3D finalWorldspacePos;
        Vector3D finalWorldspaceRot;
        float finalFOV;
        const GameLevel* currLevel = gameModel.GetCurrentLevel();
        this->GetRemoteCtrlRocketPositionRotationAndFOV(*remoteCtrlRocket, currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight(), 
            finalWorldspacePos, finalWorldspaceRot, finalFOV);

        Orientation3D finalOrientation(finalWorldspacePos, finalWorldspaceRot);
        std::vector<Orientation3D> orientationValues;
        orientationValues.reserve(2);
        orientationValues.push_back(this->currCamOrientation);
        orientationValues.push_back(finalOrientation);

        // Calculate the distance that will be traveled by the camera
        Vector3D travelVec = finalOrientation.GetTranslation() - this->currCamOrientation.GetTranslation();
        float distToTravel = travelVec.length();
        double totalTimeToAnimate = std::max<double>(0.01, distToTravel * GameTransformMgr::SECONDS_PER_UNIT_REMOTE_CTRL_ROCKETCAM);

        // Create the animation to get from wherever the camera currently is to the rocket
        std::vector<double> timeValues;
        timeValues.reserve(orientationValues.size());
        timeValues.push_back(0.0);
        timeValues.push_back(totalTimeToAnimate);

        OrientationMultiLerp toRocketCamAnim(&this->currCamOrientation);
        toRocketCamAnim.SetRepeat(false);
        toRocketCamAnim.SetLerp(timeValues, orientationValues);

        this->remoteControlRocketCamAnimations.push_back(toRocketCamAnim);

        // Add a camera FOV animation so that it's normal...
        AnimationMultiLerp<float> camFOVChangeAnim(&this->cameraFOVAngle);
        camFOVChangeAnim.SetLerp(totalTimeToAnimate, finalFOV);
        this->camFOVAnimations.clear();
        this->camFOVAnimations.push_back(camFOVChangeAnim);

        // Fade out the balls since they will not be a part of the gameplay for as long as the
        // remote control rocket is in play...
        const double fadeTime = totalTimeToAnimate;

        gameModel.GetPlayerPaddle()->SetRemoveFromGameVisibility(fadeTime);
        const std::list<GameBall*>& balls = gameModel.GetGameBalls();
        for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
            GameBall* currBall = *iter;
            assert(currBall != NULL);
            currBall->AnimateFade(true, fadeTime);
            // Note: We don't have to "remove" the ball from the game since when paused it pretty much is removed from the game
        }

        // Store the current orientation and FOV so we can return back to it after the rocket is done
        this->storedCamOriBeforeRemoteControlRocketCam = this->currCamOrientation;
        this->storedCamFOVBeforeRemoteControlRocketCam = this->cameraFOVAngle;
    }
    else {
        // Re-enable the paddle ASAP
        gameModel.UnsetPause(GameModel::PausePaddle);

        // Make sure we update the z-distance of the stored camera orientation since it may
        // have changed since
        this->storedCamOriBeforeRemoteControlRocketCam.SetTZ(this->GetCameraZDistance(this->currGameDegRotZ, *gameModel.GetCurrentLevel()));

        // We are going out of ball camera mode back to the original - take the camera
        // from wherever it is currently and move it back to the default position
        OrientationMultiLerp toOriginalCamAnim(&this->currCamOrientation);

        // Calculate the distance that will be traveled by the camera
        Vector3D travelVec = this->storedCamOriBeforeRemoteControlRocketCam.GetTranslation() - this->currCamOrientation.GetTranslation();
        float distToTravel = travelVec.length();
        double timeToAnimate = std::max<double>(0.01, distToTravel * GameTransformMgr::SECONDS_PER_UNIT_REMOTE_CTRL_ROCKETCAM);

        std::vector<Orientation3D> orientationValues;
        orientationValues.reserve(3);
        orientationValues.push_back(this->currCamOrientation);
        orientationValues.push_back(this->storedCamOriBeforeRemoteControlRocketCam);
        orientationValues.push_back(this->storedCamOriBeforeRemoteControlRocketCam);

        std::vector<double> timeVals;
        timeVals.reserve(orientationValues.size());
        timeVals.push_back(0.0);
        timeVals.push_back(timeToAnimate);
        timeVals.push_back(timeToAnimate + 0.33); // We give the player a little bit of breathing time to re-orient themselves

        toOriginalCamAnim.SetLerp(timeVals, orientationValues);
        this->remoteControlRocketCamAnimations.push_back(toOriginalCamAnim);

        // Add a camera FOV animation so that it's restored...
        AnimationMultiLerp<float> camFOVChangeAnim(&this->cameraFOVAngle);
        camFOVChangeAnim.SetLerp(timeToAnimate, this->storedCamFOVBeforeRemoteControlRocketCam);
        this->camFOVAnimations.clear();
        this->camFOVAnimations.push_back(camFOVChangeAnim);

        // Make the paddle and balls reappear (unless ball or paddle camera is active...)
        const double reappearTime = timeToAnimate;
        PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
        assert(paddle != NULL);
        paddle->AnimateFade(this->paddleWithCamera != NULL, reappearTime);

        const std::list<GameBall*>& balls = gameModel.GetGameBalls();
        for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
            GameBall* currBall = *iter;
            assert(currBall != NULL);
            currBall->AnimateFade(this->ballWithCamera != NULL, reappearTime);
        }

        this->remoteControlRocketWithCamera = NULL;
    }

    // The animation has now started
    rocketCamAnim.hasStarted = true;
}

bool GameTransformMgr::TickRemoteControlRocketCamAnimation(double dT, GameModel& gameModel) {
   
    // Grab the current transform animation information from the front of the queue
    assert(this->animationQueue.front().type == GameTransformMgr::ToRemoteCtrlRocketCamAnimation || 
           this->animationQueue.front().type == GameTransformMgr::FromRemoteCtrlRocketCamAnimation);

    // Update the orientation so that it keeps up-to-date with the current z distance
    if (this->animationQueue.front().type == GameTransformMgr::FromRemoteCtrlRocketCamAnimation) {
        assert(this->remoteControlRocketCamAnimations.size() == 1);

        Orientation3D finalOrientation = this->remoteControlRocketCamAnimations.front().GetFinalInterpolationValue();
        finalOrientation.SetTZ(this->GetCameraZDistance(this->currGameDegRotZ, *gameModel.GetCurrentLevel()));
        this->remoteControlRocketCamAnimations.front().SetInterpolationValue(1, finalOrientation);
        this->remoteControlRocketCamAnimations.front().SetInterpolationValue(2, finalOrientation);
    }
    else {
        const GameLevel* currLevel = gameModel.GetCurrentLevel();
        TransformAnimation& rocketCamAnim = this->animationQueue.front();
        assert(reinterpret_cast<PaddleRemoteControlRocketProjectile*>(rocketCamAnim.data) != NULL);
        PaddleRemoteControlRocketProjectile* remoteCtrlRocket = 
            static_cast<PaddleRemoteControlRocketProjectile*>(rocketCamAnim.data);

        float temp;
        Vector3D rcRocketPos;
        Vector3D rcRocketRot;
        this->GetRemoteCtrlRocketPositionRotationAndFOV(*remoteCtrlRocket, currLevel->GetLevelUnitWidth(),
            currLevel->GetLevelUnitHeight(), rcRocketPos, rcRocketRot, temp);

        assert(this->remoteControlRocketCamAnimations.size() == 1);
        Orientation3D& finalOrientation = this->remoteControlRocketCamAnimations.front().GetEditableFinalInterpolationValue();
        finalOrientation.SetTranslation(rcRocketPos);
        finalOrientation.SetXYZRotation(rcRocketRot);
    }

    // Animate the camera for the animation to move to or away from the rocket when
    // going into or out of the remote control rocket camera mode
    for (std::list<OrientationMultiLerp >::iterator iter = this->remoteControlRocketCamAnimations.begin(); 
         iter != this->remoteControlRocketCamAnimations.end();) {

        bool currAnimationFinished = iter->Tick(dT);
        if (currAnimationFinished) {
            iter = this->remoteControlRocketCamAnimations.erase(iter);
        }
        else {
            ++iter;
        }
    }
    for (std::list<AnimationMultiLerp<float> >::iterator iter = this->camFOVAnimations.begin(); 
        iter != this->camFOVAnimations.end();) {

            bool currAnimationFinished = iter->Tick(dT);
            if (currAnimationFinished) {
                iter = this->camFOVAnimations.erase(iter);
            }
            else {
                ++iter;
            }
    }

    // Animate the balls and paddle...
    gameModel.GetPlayerPaddle()->Animate(dT);
    const std::list<GameBall*>& balls = gameModel.GetGameBalls();
    for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
        GameBall* currBall = *iter;
        currBall->Animate(dT);
    }

    return this->remoteControlRocketCamAnimations.empty() && this->camFOVAnimations.empty();
}

void GameTransformMgr::FinishRemoteControlRocketCamAnimation(double dT, GameModel& gameModel) {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);

    // Grab the current transform animation information from the front of the queue
    TransformAnimation& rocketCamAnim = this->animationQueue.front();
    assert(rocketCamAnim.type == GameTransformMgr::ToRemoteCtrlRocketCamAnimation || 
        rocketCamAnim.type == GameTransformMgr::FromRemoteCtrlRocketCamAnimation);

    // Unpause the game state since the camera should now be either following the rocket or back
    // to its original state before the remote control rocket camera was activated
    //gameModel.UnsetPause(GameModel::PauseState);

    if (rocketCamAnim.type == GameTransformMgr::ToRemoteCtrlRocketCamAnimation) {
        // Grab the affected rocket and store it...
        assert(reinterpret_cast<PaddleRemoteControlRocketProjectile*>(rocketCamAnim.data) != NULL);
        PaddleRemoteControlRocketProjectile* remoteCtrlRocket = 
            static_cast<PaddleRemoteControlRocketProjectile*>(rocketCamAnim.data);

        // Hide the paddle...
        PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
        assert(paddle != NULL);
        paddle->SetAlpha(0.0f);
        assert(paddle->HasBeenPausedAndRemovedFromGame(gameModel.GetPauseState()));

        this->remoteControlRocketWithCamera = remoteCtrlRocket;
    }
    else {
        // Re-enable the ball
        gameModel.UnsetPause(GameModel::PauseBall);
        gameModel.UnsetPause(GameModel::PauseTimers);

        this->remoteControlRocketWithCamera = NULL;
    }

    // Pop the animation off the queue
    this->animationQueue.pop_front();
}