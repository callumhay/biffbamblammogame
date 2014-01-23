/**
 * BallCamHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

    static const char* BOOST_MALFUNCTION_TEXT;
    static const char* CANNON_OBSTRUCTION_MALFUNCTION_TEXT;

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