/**
 * KinectController.h
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

#ifndef __KINECTCONTROLLER_H__
#define __KINECTCONTROLLER_H__

#ifdef USE_KINECT_CONTROLLER

// Windows Header (has to come first, because oh yay microsoft)
#include <Windows.h>
#include <Ole2.h>
// Microsoft Kinect Natural User Interface (NUI) Headers
#include <NuiApi.h>

#include "BBBGameController.h"
#include "GameControl.h"

class KinectController : public BBBGameController {
public:
    KinectController(GameModel* model, GameDisplay* display);
    ~KinectController();

    static bool CheckForConnection();

    // Inherited functions from BBBGameController --------------------

    // No vibration/haptic feedback for this device
    void Vibrate(double, const VibrateAmount&, const VibrateAmount&) {};
    void ClearVibration() {};

    bool ProcessState(double dT);
    void Sync(size_t frameID, double dT);

    bool IsConnected() const;
    // ----------------------------------------------------------------

private:
    bool isConnected;
    NUI_SKELETON_FRAME skeletonFrame;

    bool leftActionOn;
    bool rightActionOn;
    float directionMagnitudePercentLeftRight;


    void InGameOnProcessStateSpecificActions(double dT, const NUI_SKELETON_DATA& skeleton);
    void NotInGameOnProcessStateSpecificActions();

    int GetFirstTrackedSkeletonIdx() const;
    bool PollForSkeletonFrameEvent();
    //void ProcessCurrentSkeletonFrame();

    static GameControl::ActionMagnitude GetMagnitudeFromFract(float fract, float startingOffset, float maxOffset);

    DISALLOW_COPY_AND_ASSIGN(KinectController);
};

inline bool KinectController::CheckForConnection() {
    return !FAILED(NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON));
}

inline bool KinectController::IsConnected() const {
    return this->isConnected;
}

inline bool KinectController::PollForSkeletonFrameEvent() {
    if (FAILED(NuiSkeletonTrackingEnable(NULL, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT))) {
        return false;
    }

    // This may or may not update the skeleton data (based on whether a new frame is available)
    NuiSkeletonGetNextFrame(0, &this->skeletonFrame);
    // Try to smooth the skeleton data between frames
    //NuiTransformSmooth(&this->skeletonFrame, NULL);

    return true;
}

#endif // #ifdef USE_KINECT_CONTROLLER
#endif // __KINECTCONTROLLER_H__