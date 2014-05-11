/**
 * BallCamHUD.h
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

#ifndef __BALLCAMHUD_H__
#define __BALLCAMHUD_H__

#include "ButtonTutorialHint.h"
#include "CountdownHUD.h"
#include "MalfunctionTextHUD.h"

class GameAssets;
class FBObj;

class BallCamHUD {
public:
    static const float BALL_CAM_FG_KEY_LIGHT_Z_POS;
    static const float BALL_CAM_FG_FILL_LIGHT_Z_POS;

    explicit BallCamHUD(GameAssets& assets);
    ~BallCamHUD();

    bool GetIsBoostMalfunctionHUDActive() const;
    bool GetIsCannonObstrctionHUDActive() const;

    void Draw(double dT, const Camera& camera, const GameModel& gameModel);

    void SetCanShootCannon(bool canShoot);

    void ActivateBoostMalfunctionHUD();
    void ActivateCannonObstructionHUD();
    void ToggleCannonHUD(bool activate, const CannonBlock* cannon);
    void Activate();
    void Deactivate();
    void Reinitialize();

private:
    static const float ROTATE_HINT_BOTTOM_FROM_SCREEN_BOTTOM;
    static const double LEVEL_DISPLAY_FADE_IN_ANIMATE_TIME;
    static const double LEVEL_DISPLAY_FADE_OUT_ANIMATE_TIME;

    MalfunctionTextHUD* boostMalfunctionHUD;
    MalfunctionTextHUD* cannonObstructionHUD; 
   
    const CannonBlock* cannon;
    bool cannonHUDActive;
    CountdownHUD cannonCountdown;
    ButtonTutorialHint cannonRotateHint;
    ButtonTutorialHint cannonFireHint;

    int levelOverlayHUDWidth;
    int levelOverlayHUDHeight;

    // Misc. Textures and overlays
    Texture* barrelOverlayTex;	                         // Texture for overlay of the cannon barrel
    AnimationLerp<float> cannonOverlayFadeAnim;          // Fade-in/out animation for the cannon barrel texture overlay/HUD
    AnimationMultiLerp<ColourRGBA> arrowColourAnim;      // Colour animation for arrows in the cannon HUD

    bool canShootCannon;
    AnimationMultiLerp<Colour> canShootCannonColourAnim; // Colour animation for colouring the ring around the cannon barrel HUD

    AnimationLerp<float> levelDisplayFadeAnim;           // Fade-in/out animation for the level display HUD
    
    void DrawCannonHUD(double dT, const Camera& camera);
    void DrawCannonBarrelOverlay(double dT, float alpha);
    
    void DrawLevelDisplayHUD(double dT, const GameModel& gameModel);

    DISALLOW_COPY_AND_ASSIGN(BallCamHUD);
};

inline bool BallCamHUD::GetIsBoostMalfunctionHUDActive() const {
    return this->boostMalfunctionHUD->GetIsActive();
}

inline bool BallCamHUD::GetIsCannonObstrctionHUDActive() const {
    return this->cannonObstructionHUD->GetIsActive();
}

inline void BallCamHUD::Draw(double dT, const Camera& camera, const GameModel& gameModel) {

    // Cannon HUD drawing
    this->DrawCannonHUD(dT, camera);

    // Level Display HUD drawing
    this->DrawLevelDisplayHUD(dT, gameModel);

    // Boost and cannon malfunction HUD drawing
    this->boostMalfunctionHUD->Draw(dT, camera);
    this->cannonObstructionHUD->Draw(dT, camera);
}

inline void BallCamHUD::ActivateBoostMalfunctionHUD() {
    if (!this->cannonHUDActive) {
        this->boostMalfunctionHUD->Activate();
    }
}

inline void BallCamHUD::ActivateCannonObstructionHUD() {
    if (this->cannonHUDActive) {
        this->cannonObstructionHUD->Activate();
    }
}

#endif // __BALLCAMHUD_H__