/**
 * KinectController.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifdef USE_KINECT_CONTROLLER

#include "KinectController.h"
#include "../GameView/GameDisplay.h"
#include "../GameModel/GameModel.h"

KinectController::KinectController(GameModel* model, GameDisplay* display) : 
BBBGameController(model, display), isConnected(true), leftActionOn(false), rightActionOn(false),
directionMagnitudePercentLeftRight(0.0f) {
    // If this is being instanced then it must be connected to start with
}

KinectController::~KinectController() {
}

bool KinectController::ProcessState(double dT) {
    if (this->isConnected) {

        // Grab the current input state from the Kinect by polling the device for the data we need
        if (!this->PollForSkeletonFrameEvent()) {
            this->isConnected = false;
        }
        else {
            // Figure out if any skeletons are being tracked and get the index of the first one...
            int trackedSkeletonIdx = this->GetFirstTrackedSkeletonIdx();
            if (trackedSkeletonIdx != -1) {
                // A skeleton exists and is being tracked...


                // Functionality depends on whether we're in-game or not...
                if (this->display->GetCurrentDisplayState() == DisplayState::InGame ||
                    this->display->GetCurrentDisplayState() == DisplayState::InGameBossLevel ||
                    this->display->GetCurrentDisplayState() == DisplayState::InTutorialGame) {

                    this->InGameOnProcessStateSpecificActions(dT, this->skeletonFrame.SkeletonData[trackedSkeletonIdx]);
                }
                else {
                    this->NotInGameOnProcessStateSpecificActions();
                }

                // IMPORTANT NOTES:
                // Joints are measured in meters...

                // We use ...
                //
                // NUI_SKELETON_POSITION_SHOULDER_CENTER -- To map the paddle position
                //
                // NUI_SKELETON_POSITION_HAND_RIGHT, NUI_SKELETON_POSITION_HAND_LEFT -- 
                // To map selection and execution of options in menus
                
                // NUI_SKELETON_POSITION_HAND_RIGHT, NUI_SKELETON_POSITION_HAND_LEFT
                // To execute ball boosts
            }
        }

    }
    else {
        // Attempt to connect with a Kinect...
        this->isConnected = KinectController::CheckForConnection();
    }

    return false;
}

void KinectController::Sync(size_t frameID, double dT) {
    UNUSED_PARAMETER(dT);

    if (this->leftActionOn) {
        int leftDir = this->model->AreControlsFlippedForPaddle() ? 1 : -1;
        this->model->MovePaddle(frameID, leftDir, this->directionMagnitudePercentLeftRight);
        leftDir = this->model->AreControlsFlippedForOther() ? 1 : -1;
        this->model->MoveOther(frameID, leftDir, this->directionMagnitudePercentLeftRight);
    }
    else if (this->rightActionOn) {
        int rightDir = this->model->AreControlsFlippedForPaddle() ? -1 : 1;
        this->model->MovePaddle(frameID, rightDir, this->directionMagnitudePercentLeftRight);
        rightDir = this->model->AreControlsFlippedForOther() ? -1 : 1;
        this->model->MoveOther(frameID, rightDir, this->directionMagnitudePercentLeftRight);
    }
    else {
        this->model->MovePaddle(frameID, 0, 0.0);
        this->model->MoveOther(frameID, 0, 0.0);
    }
}

void KinectController::InGameOnProcessStateSpecificActions(double dT, const NUI_SKELETON_DATA& skeleton) {
    

    // Paddle positioning and velocity is dependent on the player's center-of-body/mass
    Vector4 skeletonPaddleMovePos = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
    Vector4 skeletonCorePos       = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HEAD];

    // Vector from the core to the paddle position (all measured in meters)...
    float coreToPaddleXVec = skeletonPaddleMovePos.x - skeletonCorePos.x;
    debug_output("X Diff: " << coreToPaddleXVec);

    // Using the x-coords...
    float paddleXAmt = NumberFuncs::Clamp(coreToPaddleXVec, -0.20f, 0.20f);
    paddleXAmt *= 5;

    /*
    // Figure out where the center is in the image 
    float paddleXAmt = 0;
    float paddleYAmt = 0; 

    NuiTransformSkeletonToDepthImage(skeletonCenterPos, &paddleXAmt, &paddleYAmt, NUI_IMAGE_RESOLUTION_320x240);
    paddleXAmt /= 320.0f;
    

    
    // Map to [-1, 1]
    paddleXAmt = ((paddleXAmt * 2.0f) - 1.0f);
    */

    static const float DEAD_ZONE_FRACT = 0.10f;
    static const float MAX_ZONE_FRACT  = 1.0f;

    // Use only the x value, + maps to the player's right and - maps to the player's left...
    float absPaddleXAmt = fabs(paddleXAmt);
    if (absPaddleXAmt > DEAD_ZONE_FRACT) {

        GameControl::ActionMagnitude magnitude;
        if (paddleXAmt < 0) {

            // Moving left
            if (this->rightActionOn) {
                this->display->ButtonReleased(GameControl::RightButtonAction);
                this->rightActionOn = false;
                this->directionMagnitudePercentLeftRight = 0.0f;
            }


            if (!this->rightActionOn && !this->leftActionOn) {
                magnitude = KinectController::GetMagnitudeFromFract(absPaddleXAmt, DEAD_ZONE_FRACT, MAX_ZONE_FRACT);
                this->display->ButtonPressed(GameControl::LeftButtonAction, magnitude);
                this->leftActionOn = true;
            }
            
            if (this->leftActionOn) {
                if (absPaddleXAmt <= DEAD_ZONE_FRACT) {
                    this->directionMagnitudePercentLeftRight = 0.0f;
                }
                else if (absPaddleXAmt >= MAX_ZONE_FRACT) {
                    this->directionMagnitudePercentLeftRight = 1.0f;
                }
                else {
                    this->directionMagnitudePercentLeftRight = NumberFuncs::LerpOverFloat(DEAD_ZONE_FRACT, 1.0f, 0.5f, 1.0f, absPaddleXAmt);
                }
            }
        }
        else {

            // Moving Right
            if (this->leftActionOn) {
                this->display->ButtonReleased(GameControl::LeftButtonAction);
                this->leftActionOn = false;
                this->directionMagnitudePercentLeftRight = 0.0f;
            }

            if (!this->rightActionOn && !this->leftActionOn) {
                magnitude = KinectController::GetMagnitudeFromFract(absPaddleXAmt, DEAD_ZONE_FRACT, MAX_ZONE_FRACT);
                this->display->ButtonPressed(GameControl::RightButtonAction, magnitude);
                this->rightActionOn = true;
            }
            
            if (this->rightActionOn) {
                if (absPaddleXAmt <= DEAD_ZONE_FRACT) {
                    this->directionMagnitudePercentLeftRight = 0.0f;
                }
                else if (absPaddleXAmt >= MAX_ZONE_FRACT) {
                    this->directionMagnitudePercentLeftRight = 1.0f;
                }
                else {
                    this->directionMagnitudePercentLeftRight = NumberFuncs::LerpOverFloat(DEAD_ZONE_FRACT, 1.0f, 0.5f, 1.0f, absPaddleXAmt);
                }
            }
        }
    }
    else {
        // Not moving in either direction, stop both
        if (this->leftActionOn) {
            this->display->ButtonReleased(GameControl::LeftButtonAction);
            this->leftActionOn = false;
            this->directionMagnitudePercentLeftRight = 0.0f;
        }
        if (this->rightActionOn) {
            this->display->ButtonReleased(GameControl::RightButtonAction);
            this->rightActionOn = false;
            this->directionMagnitudePercentLeftRight = 0.0f;
        }
    }

}

void KinectController::NotInGameOnProcessStateSpecificActions() {

}

/// <summary> Gets the first tracked skeleton index. </summary>
/// <returns> The first tracked skeleton index, -1 if no skeletons are being tracked. </returns>
int KinectController::GetFirstTrackedSkeletonIdx() const {
    for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
        if (this->skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
            return i;
        }
    }

    return -1;
}


/// <summary> Gets a game control magnitude from a fractional number in [0,1]. </summary>
/// <param name="fract"> A fraction in [0,1]. </param>
/// <param name="startingOffset">
/// The starting offset [0,maxOffset], anything equal or less than this
/// is zero.
/// </param>
/// <param name="maxOffset"> The maximum offset [startingOffset, 1]. </param>
/// <returns> The magnitude. </returns>
GameControl::ActionMagnitude KinectController::GetMagnitudeFromFract(float fract, float startingOffset, float maxOffset) {
    assert(fract >= 0 && fract <= 1);
    if (fract <= startingOffset) {
        return GameControl::ZeroMagnitude;
    }
    else if (fract >= maxOffset) {
        return GameControl::FullMagnitude;
    }

    float adjustedFract = NumberFuncs::LerpOverFloat(startingOffset, 1.0f, 0.0f, 1.0f, fract);
    if (adjustedFract < 0.5) {
        return GameControl::NormalMagnitude;
    }
    else if (adjustedFract < 0.75) {
        return GameControl::LargeMagnitude;
    }

    return GameControl::FullMagnitude;
}

#endif // #ifdef USE_KINECT_CONTROLLER